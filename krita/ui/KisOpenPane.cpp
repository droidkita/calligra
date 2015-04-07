/* This file is part of the KDE project
   Copyright (C) 2005 Peter Simonsson <psn@linux.se>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KisOpenPane.h"

#include <QLayout>
#include <QLabel>
#include <QImage>
#include <QPainter>
#include <QPen>
#include <QPixmap>
#include <QSize>
#include <QString>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStyledItemDelegate>
#include <QLinearGradient>
#include <QDesktopServices>
#include <QDragEnterEvent>
#include <QDropEvent>

#include <klocale.h>
#include <kcomponentdata.h>
#include <kpushbutton.h>
#include <kdebug.h>

#include <KoFileDialog.h>
#include <KoIcon.h>
#include "KisTemplateTree.h"
#include "KisTemplateGroup.h"
#include "KisTemplate.h"
#include "KisDetailsPane.h"
#include "KisTemplatesPane.h"
#include "KisRecentDocumentsPane.h"
#include "ui_KisOpenPaneBase.h"

#include <limits.h>
#include <kconfiggroup.h>

class KoSectionListItem : public QTreeWidgetItem
{
public:
    KoSectionListItem(QTreeWidget* treeWidget, const QString& name, int sortWeight, int widgetIndex = -1)
        : QTreeWidgetItem(treeWidget, QStringList() << name), m_sortWeight(sortWeight), m_widgetIndex(widgetIndex) {
        Qt::ItemFlags newFlags = Qt::NoItemFlags;

        if(m_widgetIndex >= 0)
            newFlags |= Qt::ItemIsEnabled | Qt::ItemIsSelectable;

        setFlags(newFlags);
    }

    virtual bool operator<(const QTreeWidgetItem & other) const {
        const KoSectionListItem* item = dynamic_cast<const KoSectionListItem*>(&other);

        if (!item)
            return 0;

        return ((item->sortWeight() - sortWeight()) < 0);
    }

    int sortWeight() const {
        return m_sortWeight;
    }

    int widgetIndex() const {
        return m_widgetIndex;
    }

private:
    int m_sortWeight;
    int m_widgetIndex;
};


class KoSectionListDelegate : public QStyledItemDelegate
{
public:
    KoSectionListDelegate(QObject* parent = 0) : QStyledItemDelegate(parent) { }

    virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
    {
        QStyledItemDelegate::paint(painter, option, index);

        if(!(option.state & (int)(QStyle::State_Active && QStyle::State_Enabled)))
        {
            int ypos = option.rect.y() + ((option.rect.height() - 2) / 2);
            QRect lineRect(option.rect.left(), ypos, option.rect.width(), 2);
            QLinearGradient gradient(option.rect.topLeft(), option.rect.bottomRight());
            gradient.setColorAt(option.direction == Qt::LeftToRight ? 0 : 1, option.palette.color(QPalette::Text));
            gradient.setColorAt(option.direction == Qt::LeftToRight ? 1 : 0, Qt::transparent);

            painter->fillRect(lineRect, gradient);
        }
    }
};


class KisOpenPanePrivate : public Ui_KisOpenPaneBase
{
public:
    KisOpenPanePrivate() :
        Ui_KisOpenPaneBase() {
        m_customWidgetsSeparator = 0;
        m_templatesSeparator = 0;
    }

    KComponentData m_componentData;
    int m_freeCustomWidgetIndex;
    KoSectionListItem* m_customWidgetsSeparator;
    KoSectionListItem* m_templatesSeparator;

   
};

KisOpenPane::KisOpenPane(QWidget *parent, const KComponentData &componentData, const QStringList& mimeFilter, const QString& templateType)
    : QDialog(parent)
    , d(new KisOpenPanePrivate)
{
    d->m_componentData = componentData;
    d->setupUi(this);

    m_mimeFilter = mimeFilter;

    KoSectionListDelegate* delegate = new KoSectionListDelegate(d->m_sectionList);
    d->m_sectionList->setItemDelegate(delegate);

    connect(d->m_sectionList, SIGNAL(itemSelectionChanged()),
            this, SLOT(updateSelectedWidget()));
    connect(d->m_sectionList, SIGNAL(itemClicked(QTreeWidgetItem*, int)),
            this, SLOT(itemClicked(QTreeWidgetItem*)));
    connect(d->m_sectionList, SIGNAL(itemActivated(QTreeWidgetItem*, int)),
            this, SLOT(itemClicked(QTreeWidgetItem*)));
   
   connect(d->cancelButton,SIGNAL(clicked()),this,SLOT(close()));

    initRecentDocs();
    initTemplates(templateType);

    d->m_freeCustomWidgetIndex = 4;

    if (!d->m_sectionList->selectedItems().isEmpty())
    {
        KoSectionListItem* selectedItem = static_cast<KoSectionListItem*>(d->m_sectionList->selectedItems().first());

        if (selectedItem) {
            d->m_widgetStack->widget(selectedItem->widgetIndex())->setFocus();
        }
    }

    QList<int> sizes;

    // Set the sizes of the details pane splitters
    KConfigGroup cfgGrp(d->m_componentData.config(), "TemplateChooserDialog");
    sizes = cfgGrp.readEntry("DetailsPaneSplitterSizes", sizes);

    if (!sizes.isEmpty())
        emit splitterResized(0, sizes);

    connect(this, SIGNAL(splitterResized(KisDetailsPane*, const QList<int>&)),
            this, SLOT(saveSplitterSizes(KisDetailsPane*, const QList<int>&)));

    setAcceptDrops(true);
}

KisOpenPane::~KisOpenPane()
{
    if (!d->m_sectionList->selectedItems().isEmpty())
    {
        KoSectionListItem* item = dynamic_cast<KoSectionListItem*>(d->m_sectionList->selectedItems().first());

        if (item) {
            if (!qobject_cast<KisDetailsPane*>(d->m_widgetStack->widget(item->widgetIndex()))) {
                KConfigGroup cfgGrp(d->m_componentData.config(), "TemplateChooserDialog");
                cfgGrp.writeEntry("LastReturnType", item->text(0));
            }
        }
    }

    delete d;
}

void KisOpenPane::initRecentDocs()
{
    QString header = i18n("Recent Documents");
    KisRecentDocumentsPane* recentDocPane = new KisRecentDocumentsPane(this, d->m_componentData, header);
    connect(recentDocPane, SIGNAL(openUrl(const KUrl&)), this, SIGNAL(openExistingFile(const KUrl&)));
    QTreeWidgetItem* item = addPane(header, koIconName("document-open"), recentDocPane, 0);
    connect(recentDocPane, SIGNAL(splitterResized(KisDetailsPane*, const QList<int>&)),
            this, SIGNAL(splitterResized(KisDetailsPane*, const QList<int>&)));
    connect(this, SIGNAL(splitterResized(KisDetailsPane*, const QList<int>&)),
            recentDocPane, SLOT(resizeSplitter(KisDetailsPane*, const QList<int>&)));

    if (d->m_componentData.config()->hasGroup("RecentFiles")) {
        d->m_sectionList->setCurrentItem(item, 0, QItemSelectionModel::ClearAndSelect);
    }
}

void KisOpenPane::initTemplates(const QString& templateType)
{
    QTreeWidgetItem* selectItem = 0;
    QTreeWidgetItem* firstItem = 0;
    const int templateOffset = 1000;

    if (!templateType.isEmpty()) {
        KisTemplateTree templateTree(templateType.toLocal8Bit(), d->m_componentData, true);

        foreach (KisTemplateGroup *group, templateTree.groups()) {
            if (group->isHidden()) {
                continue;
            }

            if (!d->m_templatesSeparator) {
                d->m_templatesSeparator = new KoSectionListItem(d->m_sectionList, "", 999);
            }

            KisTemplatesPane* pane = new KisTemplatesPane(this, d->m_componentData, group->name(),
                                                        group, templateTree.defaultTemplate());
            connect(pane, SIGNAL(openUrl(const KUrl&)), this, SIGNAL(openTemplate(const KUrl&)));
            connect(pane, SIGNAL(alwaysUseChanged(KisTemplatesPane*, const QString&)),
                    this, SIGNAL(alwaysUseChanged(KisTemplatesPane*, const QString&)));
            connect(this, SIGNAL(alwaysUseChanged(KisTemplatesPane*, const QString&)),
                    pane, SLOT(changeAlwaysUseTemplate(KisTemplatesPane*, const QString&)));
            connect(pane, SIGNAL(splitterResized(KisDetailsPane*, const QList<int>&)),
                    this, SIGNAL(splitterResized(KisDetailsPane*, const QList<int>&)));
            connect(this, SIGNAL(splitterResized(KisDetailsPane*, const QList<int>&)),
                    pane, SLOT(resizeSplitter(KisDetailsPane*, const QList<int>&)));
            QTreeWidgetItem* item = addPane(group->name(), group->templates().first()->loadPicture(),
                                            pane, group->sortingWeight() + templateOffset);
	    


            if (!firstItem) {
                firstItem = item;
            }

            if (group == templateTree.defaultGroup()) {
                firstItem = item;
            }

            if (pane->isSelected()) {
                selectItem = item;
            }
        }
    } else {
        firstItem = d->m_sectionList->topLevelItem(0);
    }

    KConfigGroup cfgGrp(d->m_componentData.config(), "TemplateChooserDialog");

    if (selectItem && (cfgGrp.readEntry("LastReturnType") == "Template")) {
        d->m_sectionList->setCurrentItem(selectItem, 0, QItemSelectionModel::ClearAndSelect);
    } else if (d->m_sectionList->selectedItems().isEmpty() && firstItem) {
        d->m_sectionList->setCurrentItem(firstItem, 0, QItemSelectionModel::ClearAndSelect);
    }
}

void KisOpenPane::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        event->accept();
    }
}

void KisOpenPane::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasUrls() && event->mimeData()->urls().size() > 0) {
        // XXX: when the MVC refactoring is done, this can open a bunch of
        //      urls, but since the part/document combination is still 1:1
        //      that won't work for now.
        emit openExistingFile(event->mimeData()->urls().first());

    }
}

void KisOpenPane::addCustomDocumentWidget(QWidget *widget, const QString& title, const QString& icon)
{
    Q_ASSERT(widget);

    if (!d->m_customWidgetsSeparator) {
        d->m_customWidgetsSeparator = new KoSectionListItem(d->m_sectionList, "", 3);
    }

    QString realtitle = title;

    if (realtitle.isEmpty())
        realtitle = i18n("Custom Document");

    QTreeWidgetItem* item = addPane(realtitle, icon, widget, d->m_freeCustomWidgetIndex);
    ++d->m_freeCustomWidgetIndex;
    KConfigGroup cfgGrp(d->m_componentData.config(), "TemplateChooserDialog");

    QString lastActiveItem = cfgGrp.readEntry("LastReturnType");
    bool showCustomItemByDefault = cfgGrp.readEntry("ShowCustomDocumentWidgetByDefault", false);
    if (lastActiveItem == realtitle || (lastActiveItem.isEmpty() && showCustomItemByDefault)) {
        d->m_sectionList->setCurrentItem(item, 0, QItemSelectionModel::ClearAndSelect);
        KoSectionListItem* selectedItem = static_cast<KoSectionListItem*>(item);
        d->m_widgetStack->widget(selectedItem->widgetIndex())->setFocus();
    }
}

QTreeWidgetItem* KisOpenPane::addPane(const QString &title, const QString &iconName, QWidget *widget, int sortWeight)
{
    if (!widget) {
        return 0;
    }

    int id = d->m_widgetStack->addWidget(widget);
    KoSectionListItem* listItem = new KoSectionListItem(d->m_sectionList, title, sortWeight, id);
    listItem->setIcon(0, KIcon(iconName));

    return listItem;
}

QTreeWidgetItem* KisOpenPane::addPane(const QString& title, const QPixmap& icon, QWidget* widget, int sortWeight)
{
    if (!widget) {
        return 0;
    }

    int id = d->m_widgetStack->addWidget(widget);
    KoSectionListItem* listItem = new KoSectionListItem(d->m_sectionList, title, sortWeight, id);

    if (!icon.isNull()) {
        QImage image = icon.toImage();

        if ((image.width() > 48) || (image.height() > 48)) {
            image = image.scaled(48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }

        image = image.convertToFormat(QImage::Format_ARGB32);
        image = image.copy((image.width() - 48) / 2, (image.height() - 48) / 2, 48, 48);
        listItem->setIcon(0, QIcon(QPixmap::fromImage(image)));
    }

    return listItem;
}

void KisOpenPane::updateSelectedWidget()
{
    if(!d->m_sectionList->selectedItems().isEmpty())
    {
        KoSectionListItem* section = dynamic_cast<KoSectionListItem*>(d->m_sectionList->selectedItems().first());

        if (!section)
            return;

        d->m_widgetStack->setCurrentIndex(section->widgetIndex());
    }
}

void KisOpenPane::saveSplitterSizes(KisDetailsPane* sender, const QList<int>& sizes)
{
    Q_UNUSED(sender);
    KConfigGroup cfgGrp(d->m_componentData.config(), "TemplateChooserDialog");
    cfgGrp.writeEntry("DetailsPaneSplitterSizes", sizes);
}

void KisOpenPane::itemClicked(QTreeWidgetItem* item)
{
    KoSectionListItem* selectedItem = static_cast<KoSectionListItem*>(item);

    if (selectedItem && selectedItem->widgetIndex() >= 0) {
        d->m_widgetStack->widget(selectedItem->widgetIndex())->setFocus();
    } 
}

#include <KisOpenPane.moc>

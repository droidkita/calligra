/* This file is part of the KDE project
 * Copyright (C) 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "Settings.h"
#include <QApplication>

#include <kglobal.h>
#include <ksharedconfig.h>
#include <kconfiggroup.h>
#include <kstandarddirs.h>
#include <KMimeType>

#include "Theme.h"
#include "PropertyContainer.h"
#include <qtquick/CQTextDocumentCanvas.h>
#include <KoDocumentEntry.h>
#include <part/KWDocument.h>
#include <KWFactory.h>
#include <stage/part/KPrDocument.h>
#include <part/KPrFactory.h>

class Settings::Private
{
public:
    Private() : temporaryFile(false), focusItem(0) { }

    QString currentFile;
    QString currentFileClass;
    bool temporaryFile;
    QDeclarativeItem *focusItem;
    Theme* theme;
};

Settings::Settings( QObject* parent )
    : QObject( parent ), d( new Private )
{
    QString theme = KGlobal::config()->group("General").readEntry<QString>("theme", "default");
    d->theme = Theme::load(theme, this);
    connect(d->theme, SIGNAL(fontCacheRebuilt()), SIGNAL(themeChanged()));
}

Settings::~Settings()
{
    delete d;
}

QString Settings::stageTemplateLocation(const QString& templateFile) const
{
    QStringList dirs = KPrFactory::componentData().dirs()->resourceDirs("stage_template");
    QString found;
    foreach(const QString& dir, dirs) {
        if(QFile(dir + templateFile).exists()) {
            found = dir + templateFile;
            break;
        }
    }
    return found;
}

QString Settings::wordsTemplateLocation(const QString& templateFile) const
{
    QStringList dirs = KWFactory::componentData().dirs()->resourceDirs("words_template");
    QString found;
    foreach(const QString& dir, dirs) {
        if(QFile(dir + templateFile).exists()) {
            found = dir + templateFile;
            break;
        }
    }
    return found;
}

QString Settings::currentFile() const
{
    return d->currentFile;
}

QString Settings::currentFileClass() const
{
    return d->currentFileClass;
}

void Settings::setCurrentFile(const QString& fileName)
{
    qApp->processEvents();
    if(fileName.isEmpty()) {
        d->currentFile = fileName;
        d->currentFileClass = "No document set, consequently no class. This is expected behaviour, do not report.";
        emit currentFileChanged();
    }
    else if (fileName != d->currentFile) {
        KUrl url(fileName);
        if(url.scheme() == "newfile") {
            d->currentFileClass = url.queryItemValue("mimetype");
        }
        else {
            KMimeType::Ptr mimeType = KMimeType::findByUrl(fileName);
            KoDocumentEntry documentEntry = KoDocumentEntry::queryByMimeType(mimeType->name());
            if(documentEntry.supportsMimeType(WORDS_MIME_TYPE)) {
                d->currentFileClass = WORDS_MIME_TYPE;
            } else if(documentEntry.supportsMimeType(STAGE_MIME_TYPE)) {
                d->currentFileClass = STAGE_MIME_TYPE;
            } else {
                d->currentFileClass = QString("Unsupported document! Reported mimetype is %1").arg(mimeType->name());
            }
        }
        d->currentFile = fileName;
        emit currentFileChanged();
    }
}

bool Settings::isTemporaryFile() const
{
    return d->temporaryFile;
}

void Settings::setTemporaryFile(bool temp)
{
    if (temp != d->temporaryFile) {
        d->temporaryFile = temp;
        emit temporaryFileChanged();
    }
}

QDeclarativeItem* Settings::focusItem()
{
    return d->focusItem;
}

void Settings::setFocusItem(QDeclarativeItem* item)
{
    if (item != d->focusItem) {
        d->focusItem = item;
        emit focusItemChanged();
    }
}

QObject* Settings::theme() const
{
    return d->theme;
}

QString Settings::themeID() const
{
    if(d->theme)
        return d->theme->id();

    return QString();
}

void Settings::setThemeID(const QString& id)
{
    if(!d->theme || id != d->theme->id()) {
        if(d->theme) {
            delete d->theme;
            d->theme = 0;
        }

        d->theme = Theme::load(id, this);
        KGlobal::config()->group("General").writeEntry<QString>("theme", id);

        emit themeChanged();
    }
}

#include "Settings.moc"

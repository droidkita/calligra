/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2006 Fredrik Edemar <f_edemar@linux.se>
   SPDX-FileCopyrightText: 2004 Ariya Hidayat <ariya@kde.org>
   SPDX-FileCopyrightText: 2002-2003 Joseph Wenninger <jowenn@kde.org>
   SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
   SPDX-FileCopyrightText: 2001-2002 Laurent Montel <montel@kde.org>
   SPDX-FileCopyrightText: 2001 Philipp Mueller <philipp.mueller@gmx.de>
   SPDX-FileCopyrightText: 2000 Simon Hausmann <hausmann@kde.org>
   SPDX-FileCopyrightText: 1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "ViewAdaptor.h"
#include "View.h"
#include "Doc.h"

#include "core/Map.h"
#include "core/Sheet.h"
#include "ui/Selection.h"

#include "ui/commands/CommentCommand.h"
#include "ui/commands/StyleCommand.h"

using namespace Calligra::Sheets;

/************************************************
 *
 * ViewAdaptor
 *
 ************************************************/

ViewAdaptor::ViewAdaptor(View* t)
        : QDBusAbstractAdaptor(t)
{
    setAutoRelaySignals(true);
    m_view = t;

    Q_ASSERT(t->selection());
    connect(t->selection(), &Selection::changed, this, &ViewAdaptor::selectionChanged);
}

ViewAdaptor::~ViewAdaptor()
{
}

QString ViewAdaptor::doc() const
{
    return m_view->doc()->objectName();
}

QString ViewAdaptor::map() const
{
    return m_view->doc()->map()->objectName();
}

QString ViewAdaptor::sheet() const
{
    return m_view->activeSheet()->objectName();
}

void ViewAdaptor::changeNbOfRecentFiles(int _nb)
{
    if (_nb < 0)
        return;
    m_view->changeNbOfRecentFiles(_nb);
}


void ViewAdaptor::hide()
{
    m_view->hide();
}

void ViewAdaptor::show()
{
    m_view->show();
}

void ViewAdaptor::preference()
{
    m_view->preference();
}

void ViewAdaptor::nextSheet()
{
    m_view->nextSheet();
}

bool ViewAdaptor::showSheet(const QString& sheetName)
{
    SheetBase *const bsheet = m_view->doc()->map()->findSheet(sheetName);
    if (!bsheet) {
        debugSheets << "Unknown sheet" << sheetName;
        return false;
    }
    Sheet *sheet = dynamic_cast<Sheet *>(bsheet);
    m_view->selection()->emitCloseEditor(true); // save changes
    m_view->setActiveSheet(sheet);
    return true;
}

void ViewAdaptor::previousSheet()
{
    m_view->previousSheet();
}

void ViewAdaptor::copyAsText()
{
    m_view->copyAsText();
}

void ViewAdaptor::setSelection(const QRect& selection)
{
    m_view->selection()->initialize(selection);//,m_view->activeSheet());
}

QRect ViewAdaptor::selection()
{
    return m_view->selection()->lastRange();
}

void ViewAdaptor::setSelectionComment(const QString& comment)
{
    CommentCommand* command = new CommentCommand();
    command->setSheet(m_view->activeSheet());
    command->setText(kundo2_i18n("Add Comment"));
    command->setComment(comment.trimmed());
    command->add(*m_view->selection());
    command->execute();
}

void ViewAdaptor::setSelectionTextColor(const QColor& txtColor)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(m_view->activeSheet());
    command->setText(kundo2_i18n("Change Text Color"));
    command->setFontColor(txtColor);
    command->add(*m_view->selection());
    command->execute();
}

void ViewAdaptor::setSelectionBgColor(const QColor& bgColor)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(m_view->activeSheet());
    command->setText(kundo2_i18n("Change Background Color"));
    command->setBackgroundColor(bgColor);
    command->add(*m_view->selection());
    command->execute();
}

void ViewAdaptor::setSelectionBorderColor(const QColor& bgColor)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(m_view->activeSheet());
    command->setText(kundo2_i18n("Change Background Color"));
    command->setBackgroundColor(bgColor);
    command->add(*m_view->selection());
    command->execute();
}

void ViewAdaptor::setLeftBorderColor(const QColor& color)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(m_view->activeSheet());
    command->setText(kundo2_i18n("Change Border"));
    if (m_view->activeSheet()->layoutDirection() == Qt::RightToLeft)
        command->setRightBorderPen(QPen(color, 1, Qt::SolidLine));
    else
        command->setLeftBorderPen(QPen(color, 1, Qt::SolidLine));
    command->add(*m_view->selection());
    command->execute();
}

void ViewAdaptor::setTopBorderColor(const QColor& color)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(m_view->activeSheet());
    command->setText(kundo2_i18n("Change Border"));
    command->setTopBorderPen(QPen(color, 1, Qt::SolidLine));
    command->add(*m_view->selection());
    command->execute();
}

void ViewAdaptor::setRightBorderColor(const QColor& color)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(m_view->activeSheet());
    command->setText(kundo2_i18n("Change Border"));
    if (m_view->activeSheet()->layoutDirection() == Qt::RightToLeft)
        command->setLeftBorderPen(QPen(color, 1, Qt::SolidLine));
    else
        command->setRightBorderPen(QPen(color, 1, Qt::SolidLine));
    command->add(*m_view->selection());
    command->execute();
}

void ViewAdaptor::setBottomBorderColor(const QColor& color)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(m_view->activeSheet());
    command->setText(kundo2_i18n("Change Border"));
    command->setBottomBorderPen(QPen(color, 1, Qt::SolidLine));
    command->add(*m_view->selection());
    command->execute();
}

void ViewAdaptor::setAllBorderColor(const QColor& color)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(m_view->activeSheet());
    command->setText(kundo2_i18n("Change Border"));
    command->setTopBorderPen(QPen(color, 1, Qt::SolidLine));
    command->setBottomBorderPen(QPen(color, 1, Qt::SolidLine));
    command->setLeftBorderPen(QPen(color, 1, Qt::SolidLine));
    command->setRightBorderPen(QPen(color, 1, Qt::SolidLine));
    command->setHorizontalPen(QPen(color, 1, Qt::SolidLine));
    command->setVerticalPen(QPen(color, 1, Qt::SolidLine));
    command->add(*m_view->selection());
    command->execute();
}

void ViewAdaptor::setOutlineBorderColor(const QColor& color)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(m_view->activeSheet());
    command->setText(kundo2_i18n("Change Border"));
    command->setTopBorderPen(QPen(color, 1, Qt::SolidLine));
    command->setBottomBorderPen(QPen(color, 1, Qt::SolidLine));
    command->setLeftBorderPen(QPen(color, 1, Qt::SolidLine));
    command->setRightBorderPen(QPen(color, 1, Qt::SolidLine));
    command->add(*m_view->selection());
    command->execute();
}


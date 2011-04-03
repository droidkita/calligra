/* This file is part of the KDE project
   Copyright (C) 2011 Shantanu Tushar <jhahoneyk@gmail.com>

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
   Boston, MA 02110-1301, USA.
*/


#include "KoConfigurationDialog.h"
#include "../KoView.h"
#include "ui_KoOpenSaveConfigurationWidget.h"
#include "../KoConfigurationDialogPage.h"

KoConfigurationDialog::KoConfigurationDialog(KoView* view) : KPageDialog(view), m_view(view)
{
    setFaceType(List);
}

KoConfigurationDialog::~KoConfigurationDialog()
{
}

void KoConfigurationDialog::addCustomPage(KoConfigurationDialogPage* page)
{
    connect(this, SIGNAL(okClicked()), page, SLOT(saveSettings()));
    KPageWidgetItem *item = addPage(page->pageWidget(), page->title());
    item->setIcon(page->icon());
    page->setView(m_view);
}

#include "KoConfigurationDialog.moc"

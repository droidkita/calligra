/* This file is part of the KDE project
 * Copyright (C) 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "kis_show_palette_action.h"

#include <KLocalizedString>
#include <ko_favorite_resource_manager.h>

#include <kis_canvas2.h>

#include "kis_input_manager.h"

KisShowPaletteAction::KisShowPaletteAction(KisInputManager* manager)
    : KisAbstractInputAction(manager)
{

}

KisShowPaletteAction::~KisShowPaletteAction()
{

}

void KisShowPaletteAction::begin(int /*shortcut*/)
{
    m_inputManager->canvas()->favoriteResourceManager()->slotShowPopupPalette(m_inputManager->canvas()->coordinatesConverter()->documentToWidget(m_inputManager->mousePosition()).toPoint());
}

void KisShowPaletteAction::end()
{

}

void KisShowPaletteAction::inputEvent(QEvent* event)
{
    Q_UNUSED(event);
}

QString KisShowPaletteAction::name() const
{
    return i18n("Show Popup Palette");
}

QHash< QString, int > KisShowPaletteAction::shortcuts() const
{
    QHash<QString, int> shortcuts;
    shortcuts.insert(i18n("Show the Palette"), 0);
    return shortcuts;
}

/* This file is part of the KDE project
 * Copyright (C) Boudewijn Rempt <boud@valdyas.org>, (C) 2008
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

#include "widgets/kis_paintop_presets_popup.h"

#include <QList>
#include <QComboBox>
#include <QHBoxLayout>
#include <QToolButton>
#include <QGridLayout>

#include <kis_paintop_preset.h>

#include <ui_wdgpaintoppresets.h>


class KisPaintOpPresetsPopup::Private
{

public:

    Ui_WdgPaintOpPresets uiWdgPaintOpPresets;
    QGridLayout * layout;
    QWidget * settingsWidget;

};

KisPaintOpPresetsPopup::KisPaintOpPresetsPopup( QWidget * parent )
    : QWidget( parent )
    , m_d(new Private())
{
    setObjectName("KisPaintOpPresetsPopup");
    m_d->uiWdgPaintOpPresets.setupUi( this );
    m_d->layout = new QGridLayout(m_d->uiWdgPaintOpPresets.frmOptionWidgetContainer);
    m_d->settingsWidget = 0;
}


KisPaintOpPresetsPopup::~KisPaintOpPresetsPopup()
{
    delete m_d;
}

void KisPaintOpPresetsPopup::setPaintOpSettingsWidget( QWidget * widget )
{
    if (m_d->settingsWidget) {
        m_d->layout->removeWidget( m_d->settingsWidget );
        m_d->settingsWidget->hide();
        m_d->layout->invalidate();
    }
    if (!widget) return;
    m_d->settingsWidget = widget;
    m_d->layout->addWidget( widget );
    m_d->uiWdgPaintOpPresets.frmOptionWidgetContainer->updateGeometry();
    widget->show();

}
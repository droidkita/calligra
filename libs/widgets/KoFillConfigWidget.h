/* This file is part of the KDE project
 * Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
 * Copyright (C) 2012 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
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

#ifndef FILLCONFIGWIDGET_H
#define FILLCONFIGWIDGET_H

#include "kowidgets_export.h"

#include <QWidget>

class KoCanvasBase;
class KoColor;

/// A widget for configuring the fill of a shape
class KOWIDGETS_EXPORT KoFillConfigWidget : public QWidget
{
    Q_OBJECT
    enum StyleButton {
        None,
        Solid,
        Gradient,
        Pattern
    };
public:
    KoFillConfigWidget(QWidget * parent);
    ~KoFillConfigWidget();

    void setCanvas( KoCanvasBase *canvas );

signals:
    void opacityChanged(qreal opacity);
    void colorChanged(const KoColor &color);

private slots:
    void styleButtonPressed(int buttonId);

    /// apply line changes to the selected shape
    void applyChanges();

private:
    class Private;
    Private * const d;
};

#endif // FILLCONFIGWIDGET_H

/* This file is part of the KDE project
   Copyright (C) 2005 Thorsten Zachmann <zachmann@kde.org>

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
#ifndef BRUSHSTYLEWIDGET_H
#define BRUSHSTYLEWIDGET_H

#include <qbrush.h>
#include <qwidget.h>

#include "kprcommand.h"

class BrushPropertyUI;
class GradientPropertyUI;
class PBPreview;

class QWidgetStack;

class KComboBox;


class BrushProperty : public QWidget
{
    Q_OBJECT
public:
    BrushProperty( QWidget *parent, const char *name, const BrushCmd::Brush &brush );
    ~BrushProperty();

    int getBrushPropertyChange() const;
    BrushCmd::Brush getBrush() const;

    void setBrush( BrushCmd::Brush &brush );

    void apply();

protected:
    FillType getFillType() const;
    QBrush getQBrush() const;

    QColor getGColor1() const;
    QColor getGColor2() const;
    BCType getGType() const;
    bool getGUnbalanced() const;
    int getGXFactor() const;
    int getGYFactor() const;

    void setQBrush( const QBrush &brush );
    void setGradient( const QColor &_c1, const QColor &_c2, BCType _t,
                      bool _unbalanced, int _xfactor, int _yfactor );
    void setUnbalancedEnabled( bool state );

    KComboBox *m_typeCombo;
    QWidgetStack *m_stack;
    BrushPropertyUI *m_brushUI;
    GradientPropertyUI *m_gradientUI;
    PBPreview *m_preview_color;
    PBPreview *m_preview_gradient;

    BrushCmd::Brush m_brush;

protected slots:
    void slotReset();
    void slotTypeChanged( int pos );
    void slotBrushChanged();

    void slotColor1Changed();
    void slotColor2Changed();
    void slotBackColorTypeChanged();
    void slotUnbalancedChanged();
    void slotXFactorChanged();
    void slotYFactorChanged();
};

#endif /* BRUSHSTYLEWIDGET_H */

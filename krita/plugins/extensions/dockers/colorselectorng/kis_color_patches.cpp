/*
 *  Copyright (c) 2010 Adam Celarek <kdedev at xibo dot at>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "kis_color_patches.h"
#include <QPainter>
#include <QWheelEvent>

#include <QDebug>

KisColorPatches::KisColorPatches(QWidget *parent) :
    QWidget(parent), m_scrollValue(0)
{
    m_patchWidth = 20;
    m_patchHeight = 20;
    m_numPatches = 30;

    m_direction = Horizontal;
    m_numCols = 2;
    m_numRows = 3;
    m_allowScrolling = true;

    for(int i=0; i<m_numPatches; i++) {
        m_colors.append(QColor(qrand()|0xff000000));
    }
    setColors(m_colors);

//    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
//    resize(m_numCols*m_patchWidth, m_numRows*m_patchHeight);

    setPatchLayout(Vertical, false);
}

void KisColorPatches::setColors(QList<QColor>colors)
{
    qDebug()<<"KisColSelNgPatches::setColors() -> size:"<<colors.size();
    m_colors = colors;
    m_numPatches = colors.size();
    update();
}

void KisColorPatches::paintEvent(QPaintEvent* e)
{
    QPainter painter(this);
    if(m_direction == Vertical)
        painter.translate(0, m_scrollValue);
    else
        painter.translate(m_scrollValue, 0);


    int widgetWidth = width();
    int numPatchesInARow = widgetWidth/m_patchWidth;

    int widgetHeight = height();
    int numPatchesInACol = widgetHeight/m_patchHeight;

    for(int i=m_buttonList.size(); i<fieldCount(); i++) {
        int row;
        int col;
        if((m_direction==Vertical && m_allowScrolling) || (m_direction==Horizontal && m_allowScrolling==false)) {
            row= i/numPatchesInARow;
            col = i%numPatchesInARow;
        }
        else {
            row= i%numPatchesInACol;
            col = i/numPatchesInACol;
        }

        painter.fillRect(col*m_patchWidth, row*m_patchHeight, m_patchWidth, m_patchHeight, m_colors.at(i-m_buttonList.size()));
    }
    
//    for(int i=0; i<m_buttonList.size(); i++) {
//        m_buttonList.at(i)->paintEvent(e);
//    }
    QWidget::paintEvent(e);
}

void KisColorPatches::wheelEvent(QWheelEvent* event)
{
    m_scrollValue+=event->delta()/2;
    if(m_direction == Vertical) {
        if(m_scrollValue < -1*(heightOfAllPatches()-height()))
            m_scrollValue = -1*(heightOfAllPatches()-height());
    }
    else {
        if(m_scrollValue < -1*(widthOfAllPatches()-width()))
            m_scrollValue = -1*(widthOfAllPatches()-width());
    }
    if(m_scrollValue>0) m_scrollValue=0;


    update();
}

void KisColorPatches::resizeEvent(QResizeEvent* event)
{
    QWheelEvent* dummyWheelEvent = new QWheelEvent(QPoint(), 0, Qt::NoButton, Qt::NoModifier);
    wheelEvent(dummyWheelEvent);
    delete dummyWheelEvent;

    if(m_allowScrolling == false && event->oldSize() != event->size()) {
        if(m_direction == Horizontal) {
            setMaximumHeight(heightForWidth(width()));
            setMinimumHeight(heightForWidth(width()));
            for(int i=0; i<m_buttonList.size(); i++) {
                m_buttonList.at(i)->setGeometry(i*m_patchWidth, 0, m_patchWidth, m_patchHeight);
            }
        }
        else {
            setMaximumWidth(widthForHeight(height()));
            setMinimumWidth(widthForHeight(height()));
            for(int i=0; i<m_buttonList.size(); i++) {
                m_buttonList.at(i)->setGeometry(0, i*m_patchHeight, m_patchWidth, m_patchHeight);
            }
        }
    }

    QWidget::resizeEvent(event);
}

void KisColorPatches::setAdditionalButtons(QList<QWidget*> buttonList)
{
    for(int i=0; i<buttonList.size(); i++) {
        buttonList.at(i)->setParent(this);
//        buttonList.at(i)->setMaximumSize(m_patchWidth, m_patchHeight);
    }
    m_buttonList = buttonList;
}

void KisColorPatches::setPatchLayout(Direction dir, bool allowScrolling, int numRows, int numCols)
{
    m_direction = dir;
    m_allowScrolling = allowScrolling;
    m_numRows = numRows;
    m_numCols = numCols;

    if(allowScrolling && dir == Horizontal) {
        setMaximumHeight(m_numRows*m_patchHeight);
        setMinimumHeight(m_numRows*m_patchHeight);

        setMaximumWidth(QWIDGETSIZE_MAX);
        setMinimumWidth(0);
    }

    if(allowScrolling && dir == Vertical) {
        setMaximumWidth(m_numCols*m_patchWidth);
        setMinimumWidth(m_numCols*m_patchWidth);

        setMaximumHeight(QWIDGETSIZE_MAX);
        setMinimumHeight(0);
    }

    if(allowScrolling != true) {
        m_scrollValue = 0;
        setMinimumSize(0,0);
        setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    }
}

int KisColorPatches::widthOfAllPatches()
{
    return (fieldCount()/m_numRows)*m_patchWidth;
}

int KisColorPatches::heightOfAllPatches()
{
    return (fieldCount()/m_numCols)*m_patchHeight;
}

int KisColorPatches::heightForWidth(int width) const
{
    int numPatchesInARow = width/m_patchWidth;
    int numRows = (fieldCount()-1)/numPatchesInARow+1;
    return numRows*m_patchHeight;
}

int KisColorPatches::widthForHeight(int height) const
{
    int numPatchesInACol = height/m_patchHeight;
    int numCols = (fieldCount()-1)/numPatchesInACol+1;
    return numCols*m_patchWidth;
}

int KisColorPatches::fieldCount() const
{
    return m_numPatches+m_buttonList.size();
}

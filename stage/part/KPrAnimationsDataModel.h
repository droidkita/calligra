/* This file is part of the KDE project
 * Copyright (C) 2012 Paul Mendez <paulestebanms@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (  at your option ) any later version.
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

#ifndef KPRANIMATIONSDATAMODEL_H
#define KPRANIMATIONSDATAMODEL_H

#include "animations/KPrShapeAnimation.h"

#include <QAbstractTableModel>
#include <QPixmap>
#include <QIcon>

#include "stage_export.h"

/*Data Model for Animations Time Line View
It requires an activePage to be set*/

class KPrPage;
class QImage;
class KoShape;
class KPrAnimationStep;
class KoPADocument;
class KPrView;
class KPrCustomAnimationItem;

class STAGE_EXPORT KPrAnimationsDataModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit KPrAnimationsDataModel(QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex &index) const;

    void setParentItem(KPrCustomAnimationItem *item, KPrCustomAnimationItem *rootItem);

    //requiere to update model if a shape is removed (or added with undo)
    void setDocumentView(KPrView *view);

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;

    /// Return the first animation index for the given shape
    QModelIndex indexByShape(KoShape* shape);

    QModelIndex indexByItem(KPrCustomAnimationItem* item);

public slots:
    /// Triggers an update of the complete model
    void update();

private:
    KPrCustomAnimationItem* itemForIndex(const QModelIndex &index) const;
    KPrView *m_view;
    KPrCustomAnimationItem *m_rootItem;
};

#endif // KPRANIMATIONSDATAMODEL_H

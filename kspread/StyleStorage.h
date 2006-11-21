/* This file is part of the KDE project
   Copyright 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

#ifndef KSPREAD_STYLE_STORAGE
#define KSPREAD_STYLE_STORAGE

#include <QObject>

#include <koffice_export.h>

namespace KSpread
{

class Sheet;
class Style;
class StyleManager;
class StyleManipulator;
class SubStyle;

/**
 * A wrapper to allow future replacements of the storage backend.
 */
class KSPREAD_EXPORT StyleStorage : public QObject
{
    Q_OBJECT

public:
    explicit StyleStorage( Sheet* sheet );
    virtual ~StyleStorage();

    /**
     * Sets the StyleManager, which is necessary for the Doc/Map-wide default cell style.
     * \note It is mandatory, that the StyleStorage has a valid StyleManager set.
     */
    void setStyleManager(StyleManager* manager);

    /**
     * \return the Style at the position \p point .
     */
    Style contains(const QPoint& point) const;

    /**
     * \return the Style for the area \p rect .
     */
    Style contains(const QRect& rect) const;

    /**
     * \return the Style for the area \p rect .
     */
    Style intersects(const QRect& rect) const;

    /**
     *
     */
    QList< QPair<QRectF,QSharedDataPointer<SubStyle> > > undoData(const QRect& rect) const;

    /**
     * Returns the area, which got a style attached.
     * \return the area using styles
     */
    QRect usedArea() const;

    /**
     * Returns the index of the next column cell style after \p column or zero
     * if there's none.
     * \return the index of the next styled column
     */
    int nextColumn( int column ) const;

    /**
     * Returns the index of the next row cell style after \p row or zero
     * if there's none.
     * \return the index of the next styled row
     */
    int nextRow( int row ) const;

    /**
     * Returns the index of the next cell style in \p row after \p column or zero
     * if there's none.
     * \return the index of the next styled column
     */
    int nextStyleRight( int column, int row ) const;

    /**
     * Assigns \p subStyle to the area \p rect .
     */
    void insert(const QRect& rect, const QSharedDataPointer<SubStyle> subStyle);

    /**
     * Assigns the substyles contained in \p style to the area \p region .
     */
    void insert(const Region& region, const Style& style );

    /**
     * Inserts \p number rows at the position \p position .
     * It extends or shifts rectangles, respectively.
     */
    void insertRows(int position, int number = 1);

    /**
     * Inserts \p number columns at the position \p position .
     * It extends or shifts rectangles, respectively.
     */
    void insertColumns(int position, int number = 1);

    /**
     * Deletes \p number rows at the position \p position .
     * It shrinks or shifts rectangles, respectively.
     */
    void deleteRows(int position, int number = 1);

    /**
     * Deletes \p number columns at the position \p position .
     * It shrinks or shifts rectangles, respectively.
     */
    void deleteColumns(int position, int number = 1);

protected Q_SLOTS:
    void garbageCollectionInitialization();
    void garbageCollection();

private:
    class Private;
    Private * const d;
};

} // namespace KSpread

#endif // KSPREAD_STYLE_STORAGE

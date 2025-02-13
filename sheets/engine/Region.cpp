/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 2005-2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU Library General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

// Local
#include "Region.h"

#include "calligra_sheets_limits.h"
#include "CellBase.h"
#include "SheetBase.h"
#include "Util.h"

namespace Calligra
{
namespace Sheets
{

class Q_DECL_HIDDEN Region::Private : public QSharedData
{
public:
    Private()
            : map(0),
            cells(QList<Element*>()) {
    }

    const MapBase* map;
    mutable QList<Element*> cells;
};


/***************************************************************************
  class Region
****************************************************************************/

Region::Region()
{
    d = new Private();
}

Region::Region(const QRect& rect, SheetBase* sheet)
{
    d = new Private();

    Q_ASSERT(!rect.isNull());
    if (rect.isNull()) {
        errorSheets << "Region::Region(const QRect&): QRect is empty!" << endl;
        return;
    }
    add(rect, sheet);
}

Region::Region(const QPoint& point, SheetBase* sheet)
{
    d = new Private();

    Q_ASSERT(!point.isNull());
    if (point.isNull()) {
        errorSheets << "Region::Region(const QPoint&): QPoint is empty!" << endl;
        return;
    }
    add(point, sheet);
}

Region::Region(const Region& list)
{
    d = new Private();
    d->map = list.d->map;
    d->cells.reserve(list.d->cells.size());
    ConstIterator end(list.d->cells.constEnd());
    for (ConstIterator it = list.d->cells.constBegin(); it != end; ++it) {
        Element *element = *it;
        if (element->type() == Element::Point) {
            Point* point = static_cast<Point*>(element);
            d->cells.append(createPoint(*point));
        } else {
            Range* range = static_cast<Range*>(element);
            d->cells.append(createRange(*range));
        }
    }
}

Region::Region(int x, int y, SheetBase* sheet)
{
    d = new Private();

    Q_ASSERT(isValid(QPoint(x, y)));
    if (!isValid(QPoint(x, y))) {
        errorSheets << "Region::Region(" << x << ", " << y << "): Coordinates are invalid!" << endl;
        return;
    }
    add(QPoint(x, y), sheet);
}

Region::Region(int x, int y, int width, int height, SheetBase* sheet)
{
    d = new Private();

    Q_ASSERT(isValid(QRect(x, y, width, height)));
    if (!isValid(QRect(x, y, width, height))) {
        errorSheets << "Region::Region(" << x << ", " << y << ", " << width << ", " << height << "): Dimensions are invalid!" << endl;
        return;
    }
    add(QRect(x, y, width, height), sheet);
}


Region::~Region()
{
    qDeleteAll(d->cells);
}

QVector<QRect> Region::rects() const
{
    QVector<QRect> cellRects;
    for (Element *element : d->cells) {
        cellRects.append(element->rect());
    }
    return cellRects;
}

const MapBase* Region::map() const
{
    Q_ASSERT(d->map);
    return d->map;
}

void Region::setMap(const MapBase* map)
{
    d->map = map;
}

bool Region::isValid() const
{
    if (d->cells.isEmpty())
        return false;
    ConstIterator end = d->cells.constEnd();
    for (ConstIterator it = d->cells.constBegin(); it != end; ++it) {
        if (!(*it)->isValid())
            return false;
    }
    return true;
}

bool Region::isSingular() const
{
    if (d->cells.isEmpty() || d->cells.count() > 1 || (*d->cells.constBegin())->type() != Element::Point) {
        return false;
    }
    return true;
}

bool Region::isContiguous() const
{
    if (d->cells.count() != 1 || !isValid()) {
        return false;
    }
    return true;
}

QString Region::name(SheetBase* originSheet) const
{
    QStringList names;
    ConstIterator endOfList(d->cells.constEnd());
    for (ConstIterator it = d->cells.constBegin(); it != endOfList; ++it) {
        Element *element = *it;
        names += element->name(originSheet);
    }
    return names.isEmpty() ? "" : names.join(";");
}

Region::Element* Region::add(const QPoint& point, SheetBase* sheet, bool fixedColumn, bool fixedRow)
{
    return insert(d->cells.count(), point, sheet, false, fixedColumn, fixedRow);
}

Region::Element* Region::add(const QRect& range, SheetBase* sheet, bool fixedTop, bool fixedLeft, bool fixedBottom, bool fixedRight)
{
    const QRect normalizedRange = normalized(range);
    if (normalizedRange.width() == 0 || normalizedRange.height() == 0) {
        return 0;
    }
    if (normalizedRange.size() == QSize(1, 1)) {
        return add(normalizedRange.topLeft(), sheet, fixedTop, fixedLeft);
    }
    return insert(d->cells.count(), normalizedRange, sheet, false, fixedTop, fixedLeft, fixedBottom, fixedRight);
}

Region::Element* Region::add(const Region& region, SheetBase* sheet)
{
    ConstIterator endOfList(region.d->cells.constEnd());
    for (ConstIterator it = region.d->cells.constBegin(); it != endOfList; ++it) {
        add((*it)->rect(), (*it)->sheet() ? (*it)->sheet() : sheet, (*it)->isTopFixed(), (*it)->isLeftFixed(), (*it)->isBottomFixed(), (*it)->isRightFixed());
    }
    return d->cells.isEmpty() ? 0 : d->cells.last();
}

void Region::removeIntersects(const QPoint& point, SheetBase* sheet)
{
    removeIntersects(QRect(point, point), sheet);
}

void Region::removeIntersects(const QRect& range, SheetBase* sheet)
{
    const QRect normalizedRange = normalized(range);
    QList<Element *> toRemove;
    // TODO Stefan: Improve!
    Iterator endOfList(d->cells.end());
    for (Element *element : d->cells) {
        if (element->sheet() != sheet)
            continue;

        if (element->rect().intersects (normalizedRange)) {
            delete element;
            toRemove.push_back (element);
        }
    }

    for (Element *el : toRemove)
        d->cells.removeAll(el);
}

void Region::removeIntersects(const Region& region)
{
    ConstIterator endOfList(region.constEnd());
    for (ConstIterator it = region.constBegin(); it != endOfList; ++it) {
        Element *element = *it;
        if (element->type() == Element::Point) {
            Point* point = static_cast<Point*>(element);
            removeIntersects(point->pos(), point->sheet());
        } else {
            removeIntersects(element->rect(), element->sheet());
        }
    }
}

Region Region::intersected(const Region& region) const
{
  // Special case 1: one of the regions is empty
  if (region.isEmpty()) return region;
  if (isEmpty()) return Region();

  // Special case 2: If the region contains more elements than this one, do this operation in reverse (optimization)
  if (region.cells().size() > cells().size())
    return region.intersected (*this);

  // Most common case: the region contains only one rectangle
  Region result;
  QVector<QRect> rects = region.rects();
  if (rects.size() == 1) {
    QRect rect = rects[0];
    SheetBase *s = region.cells()[0]->sheet();
    // intersect each element with the rectangle
    for (Element *element : d->cells) {
      if (element->sheet() != s) continue;
      if (element->type() == Element::Point) {
        Point* point = static_cast<Point*>(element);
        if (rect.contains (point->pos()))
          result.add (point->pos(), s);
      } else {
        QRect rect2 = element->rect();
        if (rect2.intersects (rect))
          result.add (rect2.intersected (rect), s);
      }
    }
    return result;
  }

    // Generic case. TODO: optimize this better - generating a ton of single-cell regions is slow
    ConstIterator end(region.constEnd());
    for (ConstIterator it = region.constBegin(); it != end; ++it) {
        Element *element = *it;
        if (element->type() == Element::Point) {
            Point* point = static_cast<Point*>(element);
            if(contains(point->pos(), element->sheet()))
                result.add(point->pos(), element->sheet());
        } else {
            QRect rect = element->rect();
            for(int c = rect.top(); c <= rect.bottom(); ++c) {
                for(int r = rect.left(); r <= rect.right(); ++r) {
                    QPoint p(r,c);
                    if(contains(p, element->sheet()))
                        result.add(p, element->sheet());
                }
            }
        }
    }
    return result;
}

Region Region::translated(int dx, int dy) const
{
    if (isEmpty()) return Region();
    Region result;

    ConstIterator end(constEnd());
    for (ConstIterator it = constBegin(); it != end; ++it) {
        Element *element = *it;
        if (element->type() == Element::Point) {
            Point* point = static_cast<Point*>(element);
            QPoint pt = point->pos();
            result.add(QPoint(pt.x() + dx, pt.y() + dy), element->sheet());
        } else {
            QRect rect = element->rect();
            result.add(rect.translated(dx, dy), element->sheet());
        }
    }

    return result;
}

Region Region::intersectedWithRow(int row) const
{
    Region result;
    ConstIterator end(constEnd());
    for (ConstIterator it = constBegin(); it != end; ++it) {
        Element *element = *it;
        if (element->type() == Element::Point) {
            Point* point = static_cast<Point*>(element);
            if (point->pos().y() == row)
                result.add(point->pos(), point->sheet());
        } else {
            QRect rect = element->rect();
            if (rect.top() <= row && rect.bottom() >= row) {
                result.add(QRect(rect.left(), row, rect.width(), 1), element->sheet());
            }
        }
    }
    return result;
}

Region::Element* Region::eor(const QPoint& point, SheetBase* sheet)
{
    bool containsPoint = false;

    int index = 0;
    while (index < d->cells.count()) {
        if (!d->cells[index]->contains(point)) {
            ++index;
            continue;
        }
        containsPoint = true;
        int x = point.x();
        int y = point.y();
        Region::Element *el = d->cells.takeAt(index);
        QRect fullRange = el->rect();

        // top range
        int left = fullRange.left();
        int top = fullRange.top();
        int width = fullRange.width();
        int height = y - top;
        if (height > 0) {
            insert(index, QRect(left, top, width, height), sheet, true, el->isTopFixed(), el->isLeftFixed(), el->isBottomFixed(), el->isRightFixed());
        }
        // left range
        left = fullRange.left();
        top = y;
        width = qMax(0, x - left);
        height = 1;
        if (width > 0) {
            insert(index, QRect(left, top, width, height), sheet, true, el->isTopFixed(), el->isLeftFixed(), el->isBottomFixed(), el->isRightFixed());
        }
        // right range
        left = qMin(x + 1, fullRange.right());
        top = y;
        width = qMax(0, fullRange.right() - x);
        height = 1;
        if (width > 0) {
            insert(index, QRect(left, top, width, height), sheet, true, el->isTopFixed(), el->isLeftFixed(), el->isBottomFixed(), el->isRightFixed());
        }
        // bottom range
        left = fullRange.left();
        top = y + 1;
        width = fullRange.width();
        height = qMax(0, fullRange.bottom() - y);
        if (height > 0) {
            insert(index, QRect(left, top, width, height), sheet, true, el->isTopFixed(), el->isLeftFixed(), el->isBottomFixed(), el->isRightFixed());
        }
        delete el;
        return d->cells[index];
    }

    if (!containsPoint) {
        return add(point, sheet);
    }
    return 0;
}

Region::Element* Region::insert(int pos, const QPoint& point, SheetBase* sheet, bool multi, bool fixedColumn, bool fixedRow)
{
    if (point.x() < 1 || point.y() < 1) {
        return 0;
    }
    // Keep boundaries.
    pos = qBound(0, pos, cells().count());

    bool containsPoint = false;
//   bool adjacentPoint = false;
//   QRect neighbour;

    // we don't have to check for occurrences?
    if (multi) {
        Point* rpoint = createPoint(point, fixedColumn, fixedRow);
        rpoint->setSheet(sheet);
        d->cells.insert(pos, rpoint);
        return d->cells[pos];
    }

    ConstIterator endOfList(d->cells.constEnd());
    for (ConstIterator it = d->cells.constBegin(); it != endOfList; ++it) {
        Element *element = *it;
        if (sheet && sheet != element->sheet()) {
            continue;
        }
        if (element->contains(point)) {
            containsPoint = true;
            break;
        }
        /*    else
            {
              neighbour = element->rect();
              neighbour.setTopLeft(neighbour.topLeft() - QPoint(1,1));
              neighbour.setBottomRight(neighbour.bottomRight() + QPoint(1,1));
              if (neighbour.contains(point))
              {
                adjacentPoint = true; // TODO Stefan: Implement!
                break;
              }
            }*/
    }
    if (!containsPoint) {
        Point* rpoint = createPoint(point, fixedColumn, fixedRow);
        rpoint->setSheet(sheet);
        d->cells.insert(pos, rpoint);
        return d->cells[pos];
    }
    return 0;
}

Region::Element* Region::insert(int pos, const QRect& range, SheetBase* sheet, bool multi, bool fixedTop, bool fixedLeft, bool fixedBottom, bool fixedRight)
{
    // Keep boundaries.
    pos = qBound(0, pos, cells().count());

    const QRect normalizedRange = normalized(range);
    if (normalizedRange.size() == QSize(1, 1)) {
        return insert(pos, normalizedRange.topLeft(), sheet, true, fixedTop, fixedLeft);
    }

    if (multi) {
        Range* rrange = createRange(normalizedRange, fixedTop, fixedLeft, fixedBottom, fixedRight);
        rrange->setSheet(sheet);
        d->cells.insert(pos, rrange);
        return d->cells[pos];
    }

    bool containsRange = false;

    for (int index = 0; index < d->cells.count(); ++index) {
        if (sheet && sheet != d->cells[index]->sheet()) {
            continue;
        }
        if (d->cells[index]->contains(normalizedRange)) {
            containsRange = true;
        } else if (normalizedRange.contains(d->cells[index]->rect())) {
            delete d->cells.takeAt(index--);
            continue;
        }
    }
    if (!containsRange) {
        // Keep boundaries.
        pos = qBound(0, pos, cells().count());

        Range* rrange = createRange(normalizedRange, fixedTop, fixedLeft, fixedBottom, fixedRight);
        rrange->setSheet(sheet);
        d->cells.insert(pos, rrange);
        return d->cells[pos];
    }
    return 0;
}

bool Region::isRowAffected(int row) const
{
    ConstIterator endOfList(d->cells.constEnd());
    for (ConstIterator it = d->cells.constBegin(); it != endOfList; ++it) {
        const QRect range = (*it)->rect();
        if ((row >= range.top()) && (row <= range.bottom()))
            return true;
    }
    return false;
}

bool Region::isColumnAffected(int col) const
{
    ConstIterator endOfList(d->cells.constEnd());
    for (ConstIterator it = d->cells.constBegin(); it != endOfList; ++it) {
        const QRect range = (*it)->rect();
        if ((col >= range.left()) && (col <= range.right()))
            return true;
    }
    return false;
}


bool Region::isColumnSelected(uint col) const
{
    ConstIterator endOfList(d->cells.constEnd());
    for (ConstIterator it = d->cells.constBegin(); it != endOfList; ++it) {
        Element *element = *it;
        QRect region = element->rect();
        if ((col == 0 || ((int)col >= region.left() && (int)col <= region.right())) &&
                region.top() == 1 && region.bottom() == KS_rowMax) {
            return true;
        }
    }
    return false;
}

bool Region::isRowSelected(uint row) const
{
    ConstIterator endOfList(d->cells.constEnd());
    for (ConstIterator it = d->cells.constBegin(); it != endOfList; ++it) {
        Element *element = *it;
        QRect region = element->rect();
        if ((row == 0 || ((int)row >= region.top() && (int)row <= region.bottom())) &&
                region.left() == 1 && region.right() == KS_colMax) {
            return true;
        }
    }
    return false;
}

bool Region::isColumnOrRowSelected() const
{
    ConstIterator endOfList(d->cells.constEnd());
    for (ConstIterator it = d->cells.constBegin(); it != endOfList; ++it) {
        Element *element = *it;
        QRect region = element->rect();
        if ((region.top() == 1 && region.bottom() == KS_rowMax) ||
                (region.left() == 1 && region.right() == KS_colMax)) {
            return true;
        }
    }
    return false;
}

bool Region::isAllSelected() const
{
    if (d->cells.count() != 1)
        return false;
    Q_ASSERT(d->cells.first());
    return d->cells.first()->isAll();
}

bool Region::contains(const QPoint& point, SheetBase* sheet) const
{
    if (d->cells.isEmpty()) {
        return false;
    }
    ConstIterator endOfList(d->cells.constEnd());
    for (ConstIterator it = d->cells.constBegin(); it != endOfList; ++it) {
        Element *element = *it;
        if (element->contains(point)) {
            if (sheet && element->sheet() != sheet) {
                return false;
            }
            return true;
        }
    }
    return false;
}

bool Region::isEmpty() const
{
    return d->cells.isEmpty();
}

void Region::clear()
{
    qDeleteAll(d->cells);
    d->cells.clear();
}

QRect Region::firstRange() const
{
    if (!isValid())
        return QRect();
    return d->cells.value(0)->rect();
}

QRect Region::lastRange() const
{
    if (!isValid())
        return QRect();
    return d->cells.value(d->cells.count() - 1)->rect();
}

SheetBase* Region::firstSheet() const
{
    if (!isValid())
        return 0;
    return d->cells.value(0)->sheet();
}

SheetBase* Region::lastSheet() const
{
    if (!isValid())
        return 0;
    return d->cells.value(d->cells.count() - 1)->sheet();
}

QRect Region::boundingRect() const
{
    int left   = KS_colMax;
    int right  = 1;
    int top    = KS_rowMax;
    int bottom = 1;
    if (isEmpty()) return QRect();
    Region::ConstIterator endOfList = cells().constEnd();
    for (Region::ConstIterator it = cells().constBegin(); it != endOfList; ++it) {
        QRect range = (*it)->rect();
        if (range.left() < left) {
            left = range.left();
        }
        if (range.right() > right) {
            right = range.right();
        }
        if (range.top() < top) {
            top = range.top();
        }
        if (range.bottom() > bottom) {
            bottom = range.bottom();
        }
    }
    return QRect(left, top, right - left + 1, bottom - top + 1);
}

QRect Region::normalized(const QRect& rect)
{
    QRect normalizedRect(rect);
    if (rect.left() > rect.right()) {
        normalizedRect.setLeft(rect.right());
        normalizedRect.setRight(rect.left());
    }
    if (rect.top() > rect.bottom()) {
        normalizedRect.setTop(rect.bottom());
        normalizedRect.setBottom(rect.top());
    }
    if (rect.right() > KS_colMax) {
        normalizedRect.setRight(KS_colMax);
    }
    if (rect.bottom() > KS_rowMax) {
        normalizedRect.setBottom(KS_rowMax);
    }
    return normalizedRect;
}

Region::ConstIterator Region::constBegin() const
{
    return d->cells.constBegin();
}

Region::ConstIterator Region::constEnd() const
{
    return d->cells.constEnd();
}

bool Region::isValid(const QPoint& point)
{
    if (point.x() < 1 || point.y() < 1 ||
            point.x() > KS_colMax ||  point.y() > KS_rowMax)
        return false;
    else
        return true;
}

bool Region::isValid(const QRect& rect)
{
    if (!isValid(rect.topLeft()) || !isValid(rect.bottomRight()) ||
            rect.width() == 0 || rect.height() == 0)
        return false;
    else
        return true;
}

QList<Region::Element*>& Region::cells() const
{
    return d->cells;
}

bool Region::operator==(const Region& other) const
{
    if (d->cells.count() != other.d->cells.count())
        return false;
    ConstIterator endOfList(d->cells.constEnd());
    ConstIterator endOfOtherList(other.d->cells.constEnd());
    ConstIterator it = d->cells.constBegin();
    ConstIterator it2 = other.d->cells.constBegin();
    while (it != endOfList && it2 != endOfOtherList) {
        if ((*it)->sheet() != (*it2)->sheet())
            return false;
        if ((*it++)->rect() != (*it2++)->rect())
            return false;
    }
    return true;
}

void Region::operator=(const Region& other)
{
    if (this == &other) return;  // assigning the same object would fail

    d->map = other.d->map;
    clear();
    ConstIterator end(other.d->cells.constEnd());
    for (ConstIterator it = other.d->cells.constBegin(); it != end; ++it) {
        Element *element = *it;
        if (element->type() == Element::Point) {
            Point* point = static_cast<Point*>(element);
            d->cells.append(createPoint(*point));
        } else {
            Range* range = static_cast<Range*>(element);
            d->cells.append(createRange(*range));
        }
    }
}

Region::Point* Region::createPoint(const QPoint& point, bool fixedColumn, bool fixedRow) const
{
    return new Point(point, fixedColumn, fixedRow);
}

Region::Point* Region::createPoint(const QString& string) const
{
    return new Point(string);
}

Region::Point* Region::createPoint(const Point& point) const
{
    return new Point(point);
}

Region::Range* Region::createRange(const QRect& rect, bool fixedTop, bool fixedLeft, bool fixedBottom, bool fixedRight) const
{
    return new Range(rect, fixedTop, fixedLeft, fixedBottom, fixedRight);
}

Region::Range* Region::createRange(const Point& tl, const Point& br) const
{
    return new Range(tl, br);
}

Region::Range* Region::createRange(const QString& string) const
{
    return new Range(string);
}

Region::Range* Region::createRange(const Range& range) const
{
    return new Range(range);
}

/***************************************************************************
  class Element
****************************************************************************/

Region::Element::Element()
        : m_sheet(0)
{
}

Region::Element::~Element()
{
}


/***************************************************************************
  class Point
****************************************************************************/
static int firstNonCharPos(const QString& s, int pos = 0)
{
    int result = -1;
    const QChar *data = s.constData();
    int i = 0;
    while (!data->isNull()) {
        if (i >= pos) {
            char c = data->unicode();
            if (c < 'A' || c > 'z' || (c < 'a' && c > 'Z')) {
                result = i;
                break;
            }
        }
        ++data;
        ++i;
    }
    return result;
}

Region::Point::Point(const QPoint& point, bool fixedColumn, bool fixedRow)
        : Region::Element()
        , m_point(point)
        , m_fixedColumn(fixedColumn)
        , m_fixedRow(fixedRow)
{
    if (m_point.x() > KS_colMax)
        m_point.setX(KS_colMax);
    if (m_point.y() > KS_rowMax)
        m_point.setY(KS_rowMax);
}

Region::Point::Point(const QString& string)
        : Region::Element()
        , m_fixedColumn(false)
        , m_fixedRow(false)
{
    const uint length = string.length();
    if (length == 0)
        return;

    uint p = 0;

    // Fixed ?
    if (string[0] == QChar('$', 0)) {
        m_fixedColumn = true;
        p++;
    }

    // Malformed ?
    if (p == length)
        return;

    if ((string[p] < QChar('A',0) || string[p] > QChar('Z',0)) && (string[p] < QChar('a', 0) || string[p] > QChar('z', 0)))
        return;

    //default is error
    int x = -1;
    //search for the first character != text
    int result = firstNonCharPos(string, p);

    //get the column number for the character between actual position and the first non text character
    if (result != -1)
        x = Util::decodeColumnLabelText(string.mid(p, result - p));     // x is defined now
    else  // If there isn't any, then this is not a point -> return
        return;
    p = result;

    //limit the x-value
    //Q_ASSERT(x >= 1 && x <= KS_colMax);
    if (x < 1)
        return;
    if (x > KS_colMax)
        x = KS_colMax;

    // Malformed ?
    if (p == length)
        return;

    if (string[p] == QChar('$', 0)) {
        m_fixedRow = true;
        p++;
    }

    // Malformed ?
    if (p == length)
        return;

    uint p2 = p;
    while (p < length) {
        if (!string[p++].isDigit())
            return;
    }

    bool ok;
    int y = string.midRef(p2, p - p2).toInt(&ok);

    //limit the y-value
    //Q_ASSERT(y >= 1 && y <= KS_rowMax);
    if (!ok || y < 1)
        return;
    if (y > KS_rowMax)
        y = KS_rowMax;

    m_point = QPoint(x, y);
}

Region::Point::~Point()
{
}

QString Region::Point::name(SheetBase* originSheet) const
{
    QString name;
    if (m_sheet && m_sheet != originSheet) {
        name.append(m_sheet->sheetName());
        name.replace('\'', "''");
        if (name.contains('!') || name.contains(' ') || name.contains(';') || name.contains('$'))
            name = '\'' + name + '\'';
        name.append('!');
    }
    if (m_fixedColumn)
        name.append('$');
    name.append(CellBase::columnName(m_point.x()));
    if (m_fixedRow)
        name.append('$');
    name.append(QString::number(m_point.y()));
    return name;
}

bool Region::Point::contains(const QPoint& point) const
{
    return (m_point == point);
}

bool Region::Point::contains(const QRect& range) const
{
    return (range.width() == 1) && (range.height() == 1) && (range.topLeft() == m_point);
}

CellBase Region::Point::cell() const
{
    return CellBase(m_sheet, m_point);
}

/***************************************************************************
  class Range
****************************************************************************/

Region::Range::Range(const QRect& rect, bool fixedTop, bool fixedLeft, bool fixedBottom, bool fixedRight)
        : Region::Element()
        , m_range(rect)
        , m_fixedTop(fixedTop)
        , m_fixedLeft(fixedLeft)
        , m_fixedBottom(fixedBottom)
        , m_fixedRight(fixedRight)
{
    if (m_range.right() > KS_colMax)
        m_range.setRight(KS_colMax);
    if (m_range.bottom() > KS_rowMax)
        m_range.setBottom(KS_rowMax);
}

Region::Range::Range(const Calligra::Sheets::Region::Point& ul, const Calligra::Sheets::Region::Point& lr)
        : Region::Element()
        , m_fixedTop(ul.isTopFixed())
        , m_fixedLeft(ul.isLeftFixed())
        , m_fixedBottom(lr.isBottomFixed())
        , m_fixedRight(lr.isRightFixed())
{
    if (!ul.isValid() || !lr.isValid())
        return;
    m_range = QRect(ul.pos(), lr.pos());
}

Region::Range::Range(const QString& sRange)
        : Region::Element()
        , m_fixedTop(false)
        , m_fixedLeft(false)
        , m_fixedBottom(false)
        , m_fixedRight(false)
{
    int delimiterPos = sRange.indexOf(':');
    if (delimiterPos == -1)
        return;

    Region::Point ul(sRange.left(delimiterPos));
    Region::Point lr(sRange.mid(delimiterPos + 1));

    if (!ul.isValid() || !lr.isValid())
        return;
    m_range = QRect(ul.pos(), lr.pos());
    m_fixedTop    = ul.isRowFixed();
    m_fixedLeft   = ul.isColumnFixed();
    m_fixedBottom = lr.isRowFixed();
    m_fixedRight  = lr.isColumnFixed();
}

Region::Range::~Range()
{
}

bool Region::Range::isColumn() const
{
    return (m_range.top() == 1 && m_range.bottom() == KS_rowMax);
}

bool Region::Range::isRow() const
{
    return (m_range.left() == 1 && m_range.right() == KS_colMax);
}

bool Region::Range::isAll() const
{
    return (m_range == QRect(1, 1, KS_colMax, KS_rowMax));
}

bool Region::Range::contains(const QPoint& point) const
{
    return m_range.contains(point);
}

bool Region::Range::contains(const QRect& range) const
{
    return m_range.contains(normalized(range));
}

QString Region::Range::name(SheetBase* originSheet) const
{
    QString name;
    if (m_sheet && m_sheet != originSheet) {
        name.append(m_sheet->sheetName());
        name.replace('\'', "''");
        if (name.contains('!') || name.contains(' ') || name.contains(';') || name.contains('$'))
            name = '\'' + name + '\'';
        name.append('!');
    }
    if (m_fixedLeft)
        name.append('$');
    name.append(CellBase::columnName(m_range.left()));
    if (m_fixedTop)
        name.append('$');
    name.append(QString::number(m_range.top()));
    name.append(':');
    if (m_fixedRight)
        name.append('$');
    name.append(CellBase::columnName(m_range.right()));
    if (m_fixedBottom)
        name.append('$');
    name.append(QString::number(m_range.bottom()));
    return name;
}

} // namespace Sheets
} // namespace Calligra

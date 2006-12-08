/* This file is part of the KDE project
   Copyright (C) 2005 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "RowColumnManipulators.h"

#include <float.h>

#include "CellView.h"
#include "Format.h"
#include "Sheet.h"
#include "SheetView.h"
#include "StyleStorage.h"
#include "Value.h"

using namespace KSpread;



/***************************************************************************
  class ResizeColumnManipulator
****************************************************************************/

ResizeColumnManipulator::ResizeColumnManipulator()
{
}

ResizeColumnManipulator::~ResizeColumnManipulator()
{
}

bool ResizeColumnManipulator::process(Element* element)
{
  QRect range = element->rect();
  for (int col = range.right(); col >= range.left(); --col)
  {
    ColumnFormat *format = m_sheet->nonDefaultColumnFormat( col );
    format->setDblWidth( qMax( 2.0, m_reverse ? m_oldSize : m_newSize ) );
  }
  return true;
}



/***************************************************************************
  class ResizeRowManipulator
****************************************************************************/

ResizeRowManipulator::ResizeRowManipulator()
{
}

ResizeRowManipulator::~ResizeRowManipulator()
{
}

bool ResizeRowManipulator::process(Element* element)
{
  QRect range = element->rect();
  for (int row = range.bottom(); row >= range.top(); --row)
  {
    RowFormat* rl = m_sheet->nonDefaultRowFormat( row );
    rl->setDblHeight( qMax( 2.0, m_reverse ? m_oldSize : m_newSize ) );
  }
  return true;
}


/***************************************************************************
  class HideShowManipulator
****************************************************************************/

HideShowManipulator::HideShowManipulator()
  : m_manipulateColumns(false),
    m_manipulateRows(false)
{
}

HideShowManipulator::~HideShowManipulator()
{
}

bool HideShowManipulator::process(Element* element)
{
  QRect range = element->rect();
  if (m_manipulateColumns)
  {
    for (int col = range.left(); col <= range.right(); ++col)
    {
      ColumnFormat* format = m_sheet->nonDefaultColumnFormat(col);
      format->setHidden(!m_reverse);
    }
  }
  if (m_manipulateRows)
  {
    for (int row = range.top(); row <= range.bottom(); ++row)
    {
      RowFormat* format = m_sheet->nonDefaultRowFormat(row);
      format->setHidden(!m_reverse);
    }
  }
  return true;
}

bool HideShowManipulator::preProcessing()
{
  Region region;
  ConstIterator endOfList = cells().constEnd();
  for (ConstIterator it = cells().constBegin(); it != endOfList; ++it)
  {
    if (m_reverse)
    {
      QRect range = (*it)->rect();
      if (m_manipulateColumns)
      {
        if (range.left() > 1)
        {
          int col;
          for (col = 1; col < range.left(); ++col)
          {
            ColumnFormat* format = m_sheet->columnFormat(col);
            if (!format->hidden())
            {
              break;
            }
          }
          if (col == range.left())
          {
            region.add(QRect(1, 1, range.left()-1, KS_rowMax));
          }
        }
        for (int col = range.left(); col <= range.right(); ++col)
        {
          ColumnFormat* format = m_sheet->columnFormat(col);
          if (format->hidden())
          {
            region.add(QRect(col, 1, 1, KS_rowMax));
          }
        }
      }
      if (m_manipulateRows)
      {
        if (range.top() > 1)
        {
          int row;
          for (row = 1; row < range.top(); ++row)
          {
            RowFormat* format = m_sheet->rowFormat(row);
            if (!format->hidden())
            {
              break;
            }
          }
          if (row == range.top())
          {
            region.add(QRect(1, 1, KS_colMax, range.top()-1));
          }
        }
        for (int row = range.top(); row <= range.bottom(); ++row)
        {
          RowFormat* format = m_sheet->rowFormat(row);
          if (format->hidden())
          {
            region.add(QRect(1, row, KS_colMax, 1));
          }
        }
      }
    }

    if (((*it)->isRow() && m_manipulateColumns) ||
           ((*it)->isColumn() && m_manipulateRows))
    {
      /*      KMessageBox::error( this, i18n( "Area is too large." ) );*/
      return false;
    }
  }

  if (m_reverse)
  {
    clear();
    add(region);
  }

  return true;
}

bool HideShowManipulator::postProcessing()
{
  if (m_manipulateColumns)
  {
    m_sheet->emitHideColumn();
  }
  if (m_manipulateRows)
  {
    m_sheet->emitHideRow();
  }

  return true;
}

QString HideShowManipulator::name() const
{
  QString name;
  if (m_reverse)
  {
    name = "Show ";
  }
  else
  {
    name = "Hide ";
  }
  if (m_manipulateColumns)
  {
    name += "Columns";
  }
  if (m_manipulateColumns && m_manipulateRows)
  {
    name += '/';
  }
  if (m_manipulateRows)
  {
    name += "Rows";
  }
  return name;
}

/***************************************************************************
  class AdjustColumnRowManipulator
****************************************************************************/

AdjustColumnRowManipulator::AdjustColumnRowManipulator()
  : Manipulator(),
    m_adjustColumn(false),
    m_adjustRow(false)
{
}

AdjustColumnRowManipulator::~AdjustColumnRowManipulator()
{
}

bool AdjustColumnRowManipulator::process(Element* element)
{
  Sheet* sheet = m_sheet; // TODO Stefan: element->sheet();
  if (m_sheet && sheet != m_sheet)
  {
    return true;
  }

  QMap<int,double> heights;
  QMap<int,double> widths;
  if (m_reverse)
  {
    heights = m_oldHeights;
    widths = m_oldWidths;
  }
  else
  {
    heights = m_newHeights;
    widths = m_newWidths;
  }

  QRect range = element->rect();
  if (m_adjustColumn)
  {
    if (element->isRow())
    {
      for (int row = range.top(); row <= range.bottom(); ++row)
      {
        Cell* cell = sheet->getFirstCellRow( row );
        while ( cell )
        {
          int col = cell->column();
          if ( !cell->isEmpty() && !cell->isObscured())
          {
            if (widths.contains(col) && widths[col] != -1.0)
            {
              ColumnFormat* format = sheet->nonDefaultColumnFormat(col);
              if ( qAbs(format->dblWidth() - widths[col] ) > DBL_EPSILON )
              {
                format->setDblWidth( qMax( 2.0, widths[col] ) );
              }
            }
          }
          cell = sheet->getNextCellRight(col, row);
        }
      }
    }
    else
    {
      for (int col = range.left(); col <= range.right(); ++col)
      {
        if (widths.contains(col) && widths[col] != -1.0)
        {
          ColumnFormat* format = sheet->nonDefaultColumnFormat(col);
          if ( qAbs(format->dblWidth() - widths[col] ) > DBL_EPSILON )
          {
            format->setDblWidth( qMax( 2.0, widths[col] ) );
          }
        }
      }
    }
  }
  if (m_adjustRow)
  {
    if (element->isColumn())
    {
      for (int col = range.left(); col <= range.right(); ++col)
      {
        Cell* cell = sheet->getFirstCellColumn( col );
        while ( cell )
        {
          int row = cell->row();
          if ( !cell->isEmpty() && !cell->isObscured())
          {
            if (heights.contains(row) && heights[row] != -1.0)
            {
              RowFormat* format = sheet->nonDefaultRowFormat(row);
              if ( qAbs(format->dblHeight() - heights[row] ) > DBL_EPSILON )
              {
                format->setDblHeight( qMax( 2.0, heights[row] ) );
              }
            }
          }
          cell = sheet->getNextCellDown( col, row );
        }
      }
    }
    else
    {
      for (int row = range.top(); row <= range.bottom(); ++row)
      {
        if (heights.contains(row) && heights[row] != -1.0)
        {
          RowFormat* format = sheet->nonDefaultRowFormat(row);
          if ( qAbs(format->dblHeight() - heights[row] ) > DBL_EPSILON )
          {
            format->setDblHeight( qMax( 2.0, heights[row] ) );
          }
        }
      }
    }
  }
  return true;
}

bool AdjustColumnRowManipulator::preProcessing()
{
  if (m_reverse)
  {
  }
  else
  {
    if (!m_newHeights.isEmpty() || !m_newWidths.isEmpty())
    {
      return true;
    }
//     createUndo();

    ConstIterator endOfList(cells().end());
    for (ConstIterator it = cells().begin(); it != endOfList; ++it)
    {
      Element* element = *it;
      QRect range = element->rect();
      if (element->isColumn())
      {
        for (int col = range.left(); col <= range.right(); ++col)
        {
          Cell* cell = m_sheet->getFirstCellColumn( col );
          while ( cell )
          {
            int row = cell->row();
            if (m_adjustColumn)
            {
              if (!m_newWidths.contains(col))
              {
                m_newWidths[col] = -1.0;
                ColumnFormat* format = m_sheet->columnFormat(col);
                m_oldWidths[col] = format->dblWidth();
              }
              if (!cell->isDefault() && !cell->isEmpty() && !cell->isObscured())
              {
                m_newWidths[col] = qMax(adjustColumnHelper(cell),
                                        m_newWidths[col] );
              }
            }
            if (m_adjustRow)
            {
              if (!m_newHeights.contains(row))
              {
                m_newHeights[row] = -1.0;
                RowFormat* format = m_sheet->rowFormat(row);
                m_oldHeights[row] = format->dblHeight();
              }
              if (!cell->isEmpty() && !cell->isObscured())
              {
                m_newHeights[row] = qMax(adjustRowHelper(cell),
                                         m_newHeights[row]);
              }
            }
            cell = m_sheet->getNextCellDown( col, row );
          }
        }
      }
      else if (element->isRow())
      {
        for (int row = range.top(); row <= range.bottom(); ++row)
        {
          Cell* cell = m_sheet->getFirstCellRow( row );
          while ( cell )
          {
            int col = cell->column();
            if (m_adjustColumn)
            {
              if (!m_newWidths.contains(col))
              {
                m_newWidths[col] = -1.0;
                ColumnFormat* format = m_sheet->columnFormat(col);
                m_oldWidths[col] = format->dblWidth();
              }
              if (!cell->isDefault() && !cell->isEmpty() && !cell->isObscured())
              {
                m_newWidths[col] = qMax(adjustColumnHelper(cell),
                                        m_newWidths[col] );
              }
            }
            if (m_adjustRow)
            {
              if (!m_newHeights.contains(row))
              {
                m_newHeights[row] = -1.0;
                RowFormat* format = m_sheet->rowFormat(row);
                m_oldHeights[row] = format->dblHeight();
              }
              if (!cell->isDefault() && !cell->isEmpty() && !cell->isObscured())
              {
                m_newHeights[row] = qMax(adjustRowHelper(cell),
                                         m_newHeights[row]);
              }
            }
            cell = m_sheet->getNextCellRight(col, row);
          }
        }
      }
      else
      {
        Cell* cell;
        for (int col = range.left(); col <= range.right(); ++col)
        {
          for ( int row = range.top(); row <= range.bottom(); ++row )
          {
            cell = m_sheet->cellAt( col, row );
            if (m_adjustColumn)
            {
              if (!m_newWidths.contains(col))
              {
                m_newWidths[col] = -1.0;
                ColumnFormat* format = m_sheet->columnFormat(col);
                m_oldWidths[col] = format->dblWidth();
              }
              if (!cell->isDefault() && !cell->isEmpty() && !cell->isObscured())
              {
                m_newWidths[col] = qMax(adjustColumnHelper(cell),
                                        m_newWidths[col] );
              }
            }
            if (m_adjustRow)
            {
              if (!m_newHeights.contains(row))
              {
                m_newHeights[row] = -1.0;
                RowFormat* format = m_sheet->rowFormat(row);
                m_oldHeights[row] = format->dblHeight();
              }
              if (!cell->isDefault() && !cell->isEmpty() && !cell->isObscured())
              {
                m_newHeights[row] = qMax(adjustRowHelper(cell),
                                         m_newHeights[row]);
              }
            }
          }
        }
      }
    }
  }
  return true;
}

double AdjustColumnRowManipulator::adjustColumnHelper(Cell* cell)
{
    Q_ASSERT(!cell->isDefault());
  double long_max = 0.0;
  SheetView sheetView( cell->sheet() );
  CellView cellView( &sheetView, cell->column(), cell->row() ); // FIXME
  cellView.calculateTextParameters( cell );
  if ( cellView.textWidth() > long_max )
  {
    double indent = 0.0;
    Style::HAlign alignment = cell->style().halign();
    if (alignment == Style::HAlignUndefined)
    {
      if (cell->value().isNumber() || cell->isDate() || cell->isTime())
      {
        alignment = Style::Right;
      }
      else
      {
        alignment = Style::Left;
      }
    }

    if (alignment == Style::Left)
    {
      indent = cell->style().indentation();
    }
    long_max = indent + cellView.textWidth()
        + cell->style().leftBorderPen().width()
        + cell->style().rightBorderPen().width();
  }

  // add 4 because long_max is the length of the text
  // but column has borders
  if ( long_max == 0.0 )
  {
    return -1.0;
  }
  else
  {
    return long_max + 4;
  }
}

double AdjustColumnRowManipulator::adjustRowHelper(Cell* cell)
{
    Q_ASSERT(!cell->isDefault());
  double long_max = 0.0;

  SheetView sheetView( cell->sheet() );
  CellView cellView( &sheetView, cell->column(), cell->row() ); // FIXME
  cellView.calculateTextParameters( cell );
  if ( cellView.textHeight() > long_max )
  {
    long_max = cellView.textHeight()
        + cell->style().topBorderPen().width()
        + cell->style().bottomBorderPen().width();
  }

  //  add 1 because long_max is the height of the text
  //  but row has borders
  if ( long_max == 0.0 )
  {
    return -1.0;
  }
  else
  {
    return long_max + 1;
  }
}

QString AdjustColumnRowManipulator::name() const
{
  if (m_adjustColumn && m_adjustRow)
  {
    return i18n("Adjust Columns/Rows");
  }
  else if (m_adjustColumn)
  {
    return i18n("Adjust Columns");
  }
  else
  {
    return i18n("Adjust Rows");
  }
}

/***************************************************************************
  class InsertDeleteColumnManipulator
****************************************************************************/

InsertDeleteColumnManipulator::InsertDeleteColumnManipulator()
    : Manipulator()
{
}

bool InsertDeleteColumnManipulator::process( Element* element )
{
    const QRect range = element->rect();
    const int pos = range.left();
    const int num = range.width();
    if ( !m_reverse ) // insertion
    {
        // create undo for cells
        if ( m_firstrun )
        {
            for ( int col = KS_colMax - range.width(); col <= KS_colMax; ++col )
            {
                Cell* cell = m_sheet->getFirstCellColumn( col );
                while ( cell )
                {
                    m_undoCells.insert( cell->cellPosition(), cell->text() );
                    cell = m_sheet->getNextCellDown( cell->column(), cell->row() );
                }
            }
        }

        // insert rows
        m_sheet->insertColumns( pos, num );
        QList< QPair<QRectF,SharedSubStyle> > undoStyles = m_sheet->styleStorage()->insertColumns(pos, num);
        QList< QPair<QRectF,QString> > undoComment = m_sheet->commentStorage()->insertColumns(pos, num);
        QList< QPair<QRectF,Conditions> > undoConditions = m_sheet->conditionsStorage()->insertColumns(pos, num);
        QList< QPair<QRectF,Validity> > undoValidity = m_sheet->validityStorage()->insertColumns(pos, num);

        // create undo for styles, comments, conditions, validity
        if ( m_firstrun )
        {
            m_undoStyles = undoStyles;
            m_undoComment = undoComment;
            m_undoConditions = undoConditions;
            m_undoValidity = undoValidity;
        }

        // undo deletion
        if ( !m_firstrun )
        {
            foreach ( const QPoint& position, m_undoCells.keys() )
                m_sheet->nonDefaultCell( position.x(), position.y() )->setCellText( m_undoCells[position] );
            for ( int i = 0; i < m_undoStyles.count(); ++i )
                m_sheet->styleStorage()->insert( m_undoStyles[i].first.toRect(), m_undoStyles[i].second );
            for ( int i = 0; i < m_undoComment.count(); ++i )
                m_sheet->commentStorage()->insert( Region(m_undoComment[i].first.toRect()), m_undoComment[i].second );
            for ( int i = 0; i < m_undoConditions.count(); ++i )
                m_sheet->conditionsStorage()->insert( Region(m_undoConditions[i].first.toRect()), m_undoConditions[i].second );
            for ( int i = 0; i < m_undoValidity.count(); ++i )
                m_sheet->validityStorage()->insert( Region(m_undoValidity[i].first.toRect()), m_undoValidity[i].second );
        }
    }
    else // deletion
    {
        // create undo for cells
        if ( m_firstrun )
        {
            for ( int col = range.left(); col <= range.right(); ++col )
            {
                Cell* cell = m_sheet->getFirstCellColumn( col );
                while ( cell )
                {
                    m_undoCells.insert( cell->cellPosition(), cell->text() );
                    cell = m_sheet->getNextCellDown( cell->column(), cell->row() );
                }
            }
        }

        // delete rows
        m_sheet->deleteColumns( pos, num );
        QList< QPair<QRectF,SharedSubStyle> > undoStyles = m_sheet->styleStorage()->deleteColumns(pos, num);
        QList< QPair<QRectF,QString> > undoComment = m_sheet->commentStorage()->deleteColumns(pos, num);
        QList< QPair<QRectF,Conditions> > undoConditions = m_sheet->conditionsStorage()->deleteColumns(pos, num);
        QList< QPair<QRectF,Validity> > undoValidity = m_sheet->validityStorage()->deleteColumns(pos, num);

        // create undo for styles, comments, conditions, validity
        if ( m_firstrun )
        {
            m_undoStyles = undoStyles;
            m_undoComment = undoComment;
            m_undoConditions = undoConditions;
            m_undoValidity = undoValidity;
        }

        // undo insertion
        if ( !m_firstrun )
        {
            foreach ( const QPoint& position, m_undoCells.keys() )
                m_sheet->nonDefaultCell( position.x(), position.y() )->setCellText( m_undoCells[position] );
            for ( int i = 0; i < m_undoStyles.count(); ++i )
                m_sheet->styleStorage()->insert( m_undoStyles[i].first.toRect(), m_undoStyles[i].second );
            for ( int i = 0; i < m_undoComment.count(); ++i )
                m_sheet->commentStorage()->insert( Region(m_undoComment[i].first.toRect()), m_undoComment[i].second );
            for ( int i = 0; i < m_undoConditions.count(); ++i )
                m_sheet->conditionsStorage()->insert( Region(m_undoConditions[i].first.toRect()), m_undoConditions[i].second );
            for ( int i = 0; i < m_undoValidity.count(); ++i )
                m_sheet->validityStorage()->insert( Region(m_undoValidity[i].first.toRect()), m_undoValidity[i].second );
        }
    }
    return true;
}

bool InsertDeleteColumnManipulator::postProcessing()
{
    return true;
}

QString InsertDeleteColumnManipulator::name() const
{
    if ( !m_reverse )
        return i18n( "Insert Columns" );
    else
        return i18n( "Remove Columns" );
}

/***************************************************************************
  class InsertDeleteRowManipulator
****************************************************************************/

InsertDeleteRowManipulator::InsertDeleteRowManipulator()
    : Manipulator()
{
}

bool InsertDeleteRowManipulator::process( Element* element )
{
    const QRect range = element->rect();
    const int pos = range.top();
    const int num = range.height();
    if ( !m_reverse ) // insertion
    {
        // create undo for cells
        if ( m_firstrun )
        {
            for ( int row = KS_rowMax - range.height(); row <= KS_rowMax; ++row )
            {
                Cell* cell = m_sheet->getFirstCellRow( row);
                while ( cell )
                {
                    m_undoCells.insert( cell->cellPosition(), cell->text() );
                    cell = m_sheet->getNextCellRight( cell->column(), cell->row() );
                }
            }
        }

        // insert rows
        m_sheet->insertRows( pos, num );
        QList< QPair<QRectF,SharedSubStyle> > undoStyles = m_sheet->styleStorage()->insertRows(pos, num);
        QList< QPair<QRectF,QString> > undoComment = m_sheet->commentStorage()->insertRows(pos, num);
        QList< QPair<QRectF,Conditions> > undoConditions = m_sheet->conditionsStorage()->insertRows(pos, num);
        QList< QPair<QRectF,Validity> > undoValidity = m_sheet->validityStorage()->insertRows(pos, num);

        // create undo for styles, comments, conditions, validity
        if ( m_firstrun )
        {
            m_undoStyles = undoStyles;
            m_undoComment = undoComment;
            m_undoConditions = undoConditions;
            m_undoValidity = undoValidity;
        }

        // undo deletion
        if ( !m_firstrun )
        {
            foreach ( const QPoint& position, m_undoCells.keys() )
                m_sheet->nonDefaultCell( position.x(), position.y() )->setCellText( m_undoCells[position] );
            for ( int i = 0; i < m_undoStyles.count(); ++i )
                m_sheet->styleStorage()->insert( m_undoStyles[i].first.toRect(), m_undoStyles[i].second );
            for ( int i = 0; i < m_undoComment.count(); ++i )
                m_sheet->commentStorage()->insert( Region(m_undoComment[i].first.toRect()), m_undoComment[i].second );
            for ( int i = 0; i < m_undoConditions.count(); ++i )
                m_sheet->conditionsStorage()->insert( Region(m_undoConditions[i].first.toRect()), m_undoConditions[i].second );
            for ( int i = 0; i < m_undoValidity.count(); ++i )
                m_sheet->validityStorage()->insert( Region(m_undoValidity[i].first.toRect()), m_undoValidity[i].second );
        }
    }
    else // deletion
    {
        // create undo for cells
        if ( m_firstrun )
        {
            for ( int row = range.top(); row <= range.bottom(); ++row )
            {
                Cell* cell = m_sheet->getFirstCellRow( row );
                while ( cell )
                {
                    m_undoCells.insert( cell->cellPosition(), cell->text() );
                    cell = m_sheet->getNextCellRight( cell->column(), cell->row() );
                }
            }
        }

        // delete rows
        m_sheet->deleteRows( pos, num );
        QList< QPair<QRectF,SharedSubStyle> > undoStyles = m_sheet->styleStorage()->deleteRows(pos, num);
        QList< QPair<QRectF,QString> > undoComment = m_sheet->commentStorage()->deleteRows(pos, num);
        QList< QPair<QRectF,Conditions> > undoConditions = m_sheet->conditionsStorage()->deleteRows(pos, num);
        QList< QPair<QRectF,Validity> > undoValidity = m_sheet->validityStorage()->deleteRows(pos, num);

        // create undo for styles, comments, conditions, validity
        if ( m_firstrun )
        {
            m_undoStyles = undoStyles;
            m_undoComment = undoComment;
            m_undoConditions = undoConditions;
            m_undoValidity = undoValidity;
        }

        // undo insertion
        if ( !m_firstrun )
        {
            foreach ( const QPoint& position, m_undoCells.keys() )
                m_sheet->nonDefaultCell( position.x(), position.y() )->setCellText( m_undoCells[position] );
            for ( int i = 0; i < m_undoStyles.count(); ++i )
                m_sheet->styleStorage()->insert( m_undoStyles[i].first.toRect(), m_undoStyles[i].second );
            for ( int i = 0; i < m_undoComment.count(); ++i )
                m_sheet->commentStorage()->insert( Region(m_undoComment[i].first.toRect()), m_undoComment[i].second );
            for ( int i = 0; i < m_undoConditions.count(); ++i )
                m_sheet->conditionsStorage()->insert( Region(m_undoConditions[i].first.toRect()), m_undoConditions[i].second );
            for ( int i = 0; i < m_undoValidity.count(); ++i )
                m_sheet->validityStorage()->insert( Region(m_undoValidity[i].first.toRect()), m_undoValidity[i].second );
        }
    }
    return true;
}

bool InsertDeleteRowManipulator::postProcessing()
{
    return true;
}

QString InsertDeleteRowManipulator::name() const
{
    if ( !m_reverse )
        return i18n( "Insert Rows" );
    else
        return i18n( "Remove Rows" );
}

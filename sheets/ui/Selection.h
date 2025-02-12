/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>
   SPDX-FileCopyrightText: 2005-2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_SELECTION
#define CALLIGRA_SHEETS_SELECTION

#include "sheets_ui_export.h"

#include <KoToolSelection.h>
#include "engine/Region.h"

#include <QColor>
#include <QList>

class KoCanvasBase;

namespace Calligra
{
namespace Sheets
{

class Cell;
class Sheet;

/**
 * \class Selection
 * \brief Manages the selection of cells.
 * Represents cell selections for general operations and for cell references
 * used in formulaes.
 * \author Torben Weis <weis@kde.org>
 * \author Stefan Nikolaus <stefan.nikolaus@kdemail.net>
 */
class CALLIGRA_SHEETS_UI_EXPORT Selection : public KoToolSelection, public Region
{
    Q_OBJECT

public:
    /**
     * The selection mode.
     */
    // TODO Stefan: merge with RegionSelector::SelectionMode
    enum Mode {
        SingleCell = 0,     ///< single cell selection mode
        MultipleCells = 1   ///< multiple cell selection mode
    };

    /**
     * Constructor.
     * Creates a new selection with (1,1) as initial location.
     * @param canvasBase the canvas interface
     */
    explicit Selection(KoCanvasBase* canvasBase);

    /**
     * Copy Constructor.
     * Creates a copy of @p selection
     * @param selection the Selection to copy
     */
    Selection(const Selection& selection);

    /**
     * Destructor.
     */
    ~Selection() override;

    /**
     * \return the canvas this selection works for.
     */
    KoCanvasBase* canvas() const;

    /**
     * Sets the selection to @p point
     * @param point the point's location
     * @param sheet the sheet the point belongs to
     */
    void initialize(const QPoint& point, Sheet* sheet = 0);

    /**
     * Sets the selection to @p range
     * @param range the range's location
     * @param sheet the sheet the range belongs to
     */
    void initialize(const QRect& range, Sheet* sheet = 0);

    /**
     * Sets the selection to @p region
     * @param region the region's locations
     * @param sheet the sheet the region belongs to
     */
    void initialize(const Region& region, Sheet* sheet = 0);

    /**
     * Emits signal changed(const Region&)
     */
    void update();

    /**
     * Update the marker of the selection to @p point .
     * Uses the anchor as starting point
     * @p point the new marker location
     */
    void update(const QPoint& point);

    /**
     * Extends the current selection with the Point @p point
     * @param point the point's location
     * @param sheet the sheet the point belongs to
     */
    void extend(const QPoint& point, Sheet* sheet = 0);

    /**
     * Extends the current selection with the Range @p range
     * @param range the range's location
     * @param sheet the sheet the range belongs to
     */
    void extend(const QRect& range, Sheet* sheet = 0);

    /**
     * Extends the current selection with the Region @p region
     * @param region the region's locations
     */
    void extend(const Region& region);

    /**
     * @param point the point's location
     * @param sheet the sheet the point belongs to
     */
    Element* eor(const QPoint& point, SheetBase* sheet = 0) override;

    /**
     * The anchor is the starting point of a range. For points marker and anchor are the same
     */
    const QPoint& anchor() const;

    /**
     * The cursor represents the cursor position. This is needed for merged cells
     */
    const QPoint& cursor() const;

    /**
     * The marker is the end point of a range. For points marker and anchor are the same
     */
    const QPoint& marker() const;

    /**
     * Checks whether the region consists only of one point
     */
    bool isSingular() const;

    /**
     * @return the name of the region (e.g. "A1:A2")
     */
    QString name(Sheet* originSheet = 0) const;

    /**
     * Sets the selection's active sheet.
     * For usual selections this is always the origin sheet,
     * but for cell choices used for formulaes it may differ.
     * @param sheet the sheet which is currently active
     */
    void setActiveSheet(Sheet* sheet);

    /**
     * @return the selection's active sheet
     */
    Sheet* activeSheet() const;

    /**
     * Sets the selection's origin sheet.
     * @param sheet the sheet from which the selection starts
     */
    void setOriginSheet(Sheet* sheet);

    /**
     * @return the selection's origin sheet
     */
    Sheet* originSheet() const;

    /**
     * Activates the cell location/range, that has \p cell as bottom left or
     * right cell. If more than one occurrence would fit, the first one is taken.
     * \return the index of the activated range or \c -1, if nothing fits
     */
    int setActiveElement(const Cell &cell);

    /**
     * @return the active element
     */
    Element* activeElement() const;

    /**
     * Sets the starting position and the length of a sub-region.
     * On inserting/updating the selection the sub-region gets replaced
     * by the new cell location/range.
     * A \p length of \c 0 results in no replacement, but just in inserting the
     * new cell location/range before the range index \p start.
     * \param start The index of a range in this selection. It has to be a valid
     * index; otherwise the sub-region will be set to the whole region.
     * \param length The amount of ranges in the sub-region. If it exceeds the
     * amount of ranges, beginning from \p start to the end of range list, it
     * will be adjusted.
     * \param active The active element within the sub-region.
     * \verbatim start <= active <= start + length \endverbatim
     */
    void setActiveSubRegion(int start, int length, int active = -1);

    /**
     *
     */
    QString activeSubRegionName() const;

    /**
     * Clears the elements of the subregion
     */
    void clearSubRegion();

    /**
     * fix subregion dimensions
     */
    void fixSubRegionDimension();

    /**
     * Deletes all elements of the region. The result is an empty region.
     */
    void clear() override;

    /**
     * Is the current selection protected?
     */
    bool isProtected() const;

    /**
     * \param mode single cell or multiple cell selection
     */
    void setSelectionMode(Mode mode);

    /**
     * Extends \p area to include the merged cells, that are not fully covered,
     * completely.
     * \return the extended area
     */
    QRect extendToMergedAreas(const QRect& area) const;

    const QList<QColor>& colors() const;

    void selectAll();

    /** Start using a reference selection instead of normal one. */
    void startReferenceSelection();
    /** End using reference selection. */
    void endReferenceSelection(bool saveChanges = true);
    /** Enable/disable reference choosing mode. */
    void setReferenceSelectionMode(bool enable);
    /** Are we in reference choosing mode ? */
    bool referenceSelectionMode() const;
    /** Are we currently using a reference selection ? */
    bool referenceSelection() const;

    void emitAboutToModify();
    void emitModified();
    void emitRefreshSheetViews();
    void emitVisibleSheetRequested(Sheet* sheet);
    void emitCloseEditor(bool saveChanges, bool expandMatrix = false);
    void emitRequestFocusEditor();

    void scrollToCursor();
Q_SIGNALS:
    /**
     * Emitted when the Selection was changed.
     * @param region the changed part of the Selection
     */
    void changed(const Region& region);

    /**
     * An operation on the selection is about to happen.
     */
    void aboutToModify(const Region& region);

    /**
     * Emitted when the content was modified.
     */
    void modified(const Region& region);

    void refreshSheetViews();
    void visibleSheetRequested(Sheet* sheet);
    void closeEditor(bool saveChanges, bool expandMatrix);
    void activeSheetChanged(Sheet* sheet);
    void requestFocusEditor();

    void documentReadWriteToggled(bool readWrite);
    void sheetProtectionToggled(bool protect);

    void updateAccessedCellRange(Sheet* sheet, const QPoint& location);

protected:
    class Point;
    class Range;

    /**
     * @internal used to create derived Points
     */
    Region::Point* createPoint(const QPoint&, bool fixedColumn, bool fixedRow) const override;

    /**
     * @internal used to create derived Points
     */
    Region::Point* createPoint(const QString&) const override;

    /**
     * @internal used to create derived Points
     */
    Region::Point* createPoint(const Region::Point&) const override;

    /**
     * @internal used to create derived Ranges
     */
    Region::Range* createRange(const QRect&, bool fixedTop, bool fixedLeft, bool fixedBottom, bool fixedRight) const override;

    /**
     * @internal used to create derived Ranges
     */
    Region::Range* createRange(const Region::Point&, const Region::Point&) const override;

    /**
     * @internal used to create derived Ranges
     */
    Region::Range* createRange(const QString&) const override;

    /**
     * @internal used to create derived Ranges
     */
    Region::Range* createRange(const Region::Range&) const override;

    /**
     * Dilates the region and emits the changed() signal.
     * \internal
     */
    void emitChanged(const Region& changedRegion);

    /**
     * @internal
     */
    void dump() const;

private:
    // do not allow assignment
    Selection& operator=(const Selection&);

    class Private;
    Private * const d;
};

/***************************************************************************
  class Selection::Point
****************************************************************************/

/**
 * This Point is extended by an color attribute.
 */
class Selection::Point : public Region::Point
{
public:
    Point(const QPoint& point);
    Point(const QString& string);
    Point(const Region::Point& point);

    void setColor(const QColor& color) {
        m_color = color;
    }
    virtual const QColor& color() const {
        return m_color;
    }

private:
    QColor m_color;
};

/***************************************************************************
  class Selection::Range
****************************************************************************/

/**
 * This Range is extended by an color attribute.
 */
class Selection::Range : public Region::Range
{
public:
    Range(const QRect& rect);
    Range(const Calligra::Sheets::Region::Point& tl, const Calligra::Sheets::Region::Point& br);
    Range(const QString& string);
    Range(const Region::Range& range);

    void setColor(const QColor& color) {
        m_color = color;
    }
    const QColor& color() const {
        return m_color;
    }

private:
    QColor m_color;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_SELECTION

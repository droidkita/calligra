/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2021 Tomas Mecir <mecirt@gmail.com>
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef __SHEETBASE_H__
#define __SHEETBASE_H__



#include "sheets_engine_export.h"
#include "Validity.h"

#include <QString>

namespace Calligra
{
namespace Sheets
{

class CellBaseStorage;
class FormulaStorage;
class ValidityStorage;
class ValueStorage;
class MapBase;


class CALLIGRA_SHEETS_ENGINE_EXPORT SheetBase
{
public:
    enum ChangeRef       { ColumnInsert, ColumnRemove, RowInsert, RowRemove };
    /**
     * Creates a sheet in \p map with the name \p sheetName.
     */
    SheetBase(MapBase* map, const QString& sheetName);

    /**
     * Copy constructor.
     * Creates a sheet with the contents and the settings of \p other.
     */
    SheetBase(const SheetBase& other);

    virtual ~SheetBase();

    /**
     * \ingroup Storage
     * \return the cell storage
     */
    CellBaseStorage* cellStorage() const;

    /**
     * \return the map this sheet belongs to.
     */
    MapBase* map() const;


    /**
     * \return the name of this sheet
     */
    QString sheetName() const;

    /**
     * Renames a sheet. This will automatically adapt all formulas
     * in all sheets and all cells to reflect the new name.
     *
     * @param name The new sheet name.
     *
     * @return @c true if the sheet was renamed successfully
     * @return @c false if the sheet could not be renamed. Usually the reason is
     * that this name is already used.
     *
     * @see sheetName
     */
    virtual bool setSheetName(const QString& name);



    bool isAutoCalculationEnabled() const;
    void setAutoCalculationEnabled(bool enable);
    bool getFirstLetterUpper() const;
    void setFirstLetterUpper(bool _firstUpper);



    const FormulaStorage* formulaStorage() const;
    const ValidityStorage* validityStorage() const;
    const ValueStorage* valueStorage() const;



    /**
     * \ingroup Value
     * Change name of reference when the user inserts or removes a column,
     * a row or a cell (= insertion of a row [or column] on a single column [or row]).
     * For example the formula =Sheet1!A1 is changed into =Sheet1!B1 if a Column
     * is inserted before A.
     *
     * @param pos the point of insertion (only one coordinate may be used, depending
     * on the other parameters).
     * @param fullRowOrColumn if true, a whole row or column has been inserted/removed.
     *                        if false, we inserted or removed a cell
     * @param ref see ChangeRef
     * @param sheetName completes the pos specification by giving the sheet name
     * @param number number of columns which were inserted
     */
    void changeNameCellRef(const QPoint& pos, bool fullRowOrColumn, ChangeRef ref,
                           const QString& sheetName, int number);

    void changeNameCellRefs(const QRect& rect, bool fullRowOrColumn, ChangeRef ref);


    virtual void showStatusMessage(const QString &message, int timeout = 3000) const;
    /**
     * Called when validation fails.
     */
    virtual bool onValidationFailed(Validity::Action action, const CellBase *cell, const QString &message, const QString &title) const;
protected:
    /**
     * \ingroup Value
     * Change the name of a sheet in all formulas.
     * When you change name sheet Sheet1 -> Price
     * for all cell which refers to Sheet1, this function changes the name.
     */
    void changeCellTabName(QString const & old_name, QString const & new_name);

    /**
     * \ingroup Value
     * \see changeNameCellRef()
     */
    QString changeNameCellRefHelper(const QPoint& pos, bool fullRowOrColumn, ChangeRef ref,
                                    int NbCol, const QPoint& point, bool isColumnFixed,
                                    bool isRowFixed);
    QString changeNameCellRefHelper(const QPoint& pos, const QRect& rect, bool fullRowOrColumn, ChangeRef ref,
                                    int NbCol, const QPoint& point, bool isColumnFixed,
                                    bool isRowFixed);


    /**
     * Used by the child class to override the base storage with the full-featured one.
     */
    void setCellStorage(CellBaseStorage *storage);
private:
    SheetBase &operator=(const SheetBase &) = delete;

    class Private;
    Private * const d;
};


} // namespace Sheets
} // namespace Calligra


#endif

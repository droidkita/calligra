/* This file is part of the KDE project
   Copyright (C) 2003-2007 Jarosław Staniek <staniek@kde.org>

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

#ifndef KexiStartupDialog_h
#define KexiStartupDialog_h

#include "keximain_export.h"

#include <kexi_global.h>
#include <db/connectiondata.h>
#include <core/kexiprojectdata.h>

#include <kpagedialog.h>

class KexiProjectData;
class KexiDBConnectionSet;
class ConnectionDataLVItem;
class QEvent;

//! @short Kexi startup dialog
/*!
 This class is used to show the template/open-existing tabbed dialog
 on Kexi startup. If only one page is shown, tab is not displayed, so the dialog
 becomes a normal "plain" type dialog.
*/
class KEXIMAIN_EXPORT KexiStartupDialog : public KPageDialog
{
    Q_OBJECT

public:
    //! Result of the user's input.
    enum Result {
        CancelResult,             //!< The user has pressed 'Cancel'
        CreateBlankResult,        //!< The user has selected a template
        CreateFromTemplateResult, //!< The user has selected a template to be used for creating a new db
        ImportResult,             //!< The user has chosen to import db
        OpenExistingResult        //!< The user has chosen an existing connection or db file
    };

    /*!
    To configure the dialog you have to use this enum
      (any !=0 or'ed value is ok)
      - Templates Show "Templates" tab
      - OpenExisting Show "Open existing" tab
      - Everything Show everything above
    */
    enum DialogType { Templates = 1, OpenExisting = 2, Everything = (1 + 2 + 4) };

    /*! Options for a dialog
      (any or'ed value or 0 is ok)
      - CheckBoxDoNotShowAgain Adds "do not show this window" checkbox at the bottom
    */
    enum DialogOptions { CheckBoxDoNotShowAgain = 1 };

    /*! Creates a dialog.
    @param dialogType see DialogType description
    @param dialogOptions see dialogOptions description
    @param connSet conenction set used to present available conenctions
      in "Open Existing" tab. Pass an empty object is this tab is not used.
    @param parent parent widget, if any.
    @param name name of this object.
    */
    KexiStartupDialog(
        int dialogType,
        int dialogOptions,
        KexiDBConnectionSet& connSet,
        QWidget *parent = 0);

    ~KexiStartupDialog();

    /*! Executes dialog.
    \return one of Result values. Use this after dialog is closed. */
    int result() const;

    /*! \return data of selected Kexi project.
      Returns NULL if no selection has been made or other tab was selected.
    */
    KexiProjectData* selectedProjectData() const;

    /*! \return name of selected Kexi project file
      (if result() == OpenExistingResult)
      or name of template file to be used for creating a new database.
      (if result() == CreateFromTemplateResult).
      Returns empty string if no such selection has been made or other tab was selected.
    */
    QString selectedFileName() const;

#ifdef KEXI_PROJECT_TEMPLATES
    /*! \return "autoopen" objects defined for selected template.
    Only makes sense if template was used. */
    KexiProjectData::AutoOpenObjects autoopenObjects() const;
#endif

    /*! \return a pointer to selected Kexi connection data.
      (if "Open Existing" tab was selected and this connection data was clicked).
      Returns NULL if no such selection has been made or other tab was selected.
    */
    KexiDB::ConnectionData* selectedExistingConnection() const;

public Q_SLOTS:
    virtual void done(int r);
    virtual void reject();

protected Q_SLOTS:
    virtual void slotOk();

    //! slot activated when one of the top-level tabs is shown
    void slotCurrentPageChanged(KPageWidgetItem* current,
                                KPageWidgetItem* before);

    //! slot activated when one of the subpages within templates tab is shown
    void slotCurrentTemplatesubpageChanged(KPageWidgetItem* current,
                                           KPageWidgetItem* before);

    void templateSelected(const QString& fileName);

    //! helper
    void existingFileHighlighted();
    void showSimpleConnForOpenExisting();
    void showAdvancedConnForOpenExisting();
    void connectionItemForOpenExistingExecuted(ConnectionDataLVItem *item);
    void connectionItemForOpenExistingHighlighted(ConnectionDataLVItem *item);

protected:
    virtual bool eventFilter(QObject *o, QEvent *e);
    virtual void showEvent(QShowEvent *e);

    //! helper: updates a state of dialog's OK button
    void updateDialogOKButton(KPageWidgetItem *pageWidgetItem);

private:
    void setupPageTemplates();
    void setupPageOpenExisting();

    class Private;
    Private * const d;
};

#endif

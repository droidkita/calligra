/* This file is part of the KDE project
  Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>
  Copyright (C) 2002 - 2007 Dag Andersen <danders@get2net.dk>

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

#ifndef KPLATO_VIEW
#define KPLATO_VIEW

#include <KoView.h>
#include <KoQueryTrader.h>

#include "kptcontext.h"

#include <QMenu>
#include <QDockWidget>
#include <QTreeWidget>


class QProgressBar;
class QStackedWidget;
class QSplitter;

class KPrinter;
class KAction;
class KActionMenu;
class KSelectAction;
class KToggleAction;
class QLabel;
class KSeparatorAction;

class KoView;

namespace KPlato
{

class View;
class ViewBase;
class ViewListItem;
class ViewListWidget;
class AccountsView;
class GanttView;
class PertEditor;
class PertResult;
class ResourceView;
class AccountsEditor;
class TaskEditor;
class CalendarEditor;
class ResourceEditor;
class ScheduleEditor;
class ScheduleManager;

class ResourceAssignmentView;
class TaskStatusView;
//class ReportView;
class Calendar;
class Part;
class DocumentChild;
class Node;
class Project;
class MainSchedule;
class Schedule;
class Resource;
class ResourceGroup;
class Relation;
class Context;
class ViewAdaptor;

class ViewListItem : public QTreeWidgetItem
{
public:
    enum ItemType { ItemType_Category = Type, ItemType_SubView = UserType, ItemType_ChildDocument };

    enum DataRole { DataRole_View = Qt::UserRole, DataRole_Document, DataRole_ChildDocument };

    ViewListItem( const QString &tag, const QStringList &strings, int type = ItemType_Category );
    ViewListItem( QTreeWidget *parent, const QString &tag, const QStringList &strings, int type = ItemType_Category );
    ViewListItem( QTreeWidgetItem *parent, const QString &tag, const QStringList &strings, int type = ItemType_Category );
    void setView( KoView *view );
    KoView *view() const;
    void setDocumentChild( DocumentChild *child );
    DocumentChild *documentChild() const;
    void setDocument( KoDocument *doc );
    KoDocument *document() const;

    QString tag() const { return m_tag; }
private:
    QString m_tag;
};

class ViewListTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    explicit ViewListTreeWidget( QWidget *parent );
    ViewListItem *findCategory( const QString &cat );

protected:
    void drawRow( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const;
    virtual void mousePressEvent ( QMouseEvent *event );

signals:
    void activated( QTreeWidgetItem* );

private slots:
    void handleMousePress( QTreeWidgetItem *item );
};

class ViewListWidget : public QWidget
{
    Q_OBJECT
public:
    ViewListWidget( Part *part, QWidget *parent );//QString name, KXmlGuiWindow *parent );
    ~ViewListWidget();

    /// Add a category if it does not allready exist
    ViewListItem *addCategory( const QString &tag, const QString& name );
    /// Add a sub-view
    ViewListItem *addView(QTreeWidgetItem *category, const QString &tag, const QString& name, KoView *view, KoDocument *doc, const QString& icon = QString() );
    /// Create a embedded child document view (callers resposibility to add to the list)
    ViewListItem *createView( const QString &tag, const QString& name, KoView *view, DocumentChild *ch, const QString& icon = QString() );

    void setSelected( QTreeWidgetItem *item );
    KoView *findView( const QString &tag );
    ViewListItem *findItem( const QString &tag, QTreeWidgetItem* parent = 0 );
    ViewListItem *findItem( const QWidget *view, QTreeWidgetItem* parent = 0 );

    int takeViewListItem( ViewListItem *item );
    void insertViewListItem( ViewListItem *item, QTreeWidgetItem *parent, int index );

signals:
    void activated( ViewListItem*, ViewListItem* );
    void createKofficeDocument( KoDocumentEntry &entry );
    void viewListItemRemoved( ViewListItem *item );
    void viewListItemInserted( ViewListItem *item );

protected slots:
    void slotActivated( QTreeWidgetItem *item, QTreeWidgetItem *prev );
    void slotItemChanged( QTreeWidgetItem *item, int col );
    void renameCategory();
    void slotCreatePart();
    void slotEditDocumentTitle();
    void slotRemoveDocument();

protected:
    virtual void contextMenuEvent ( QContextMenuEvent *event );

private:
    void setupContextMenus();

private:
    Part *m_part;
    ViewListTreeWidget *m_viewlist;
    Q3ValueList<KoDocumentEntry> m_lstEntries;
    KoDocumentEntry m_documentEntry;

    ViewListItem *m_contextitem;
    QAction *m_separator;
    QList<QAction*> m_noitem;
    QList<QAction*> m_category;
    QList<QAction*> m_view;
    QList<QAction*> m_document;
    QList<QAction*> m_parts;
};


//-------------
class View : public KoView
{
    Q_OBJECT

public:
    explicit View( Part* part, QWidget* parent = 0 );
    ~View();
    /**
     * Support zooming.
     */
    virtual void setZoom( double zoom );

    Part *getPart() const;

    Project& getProject() const;

    virtual void setupPrinter( KPrinter &printer );
    virtual void print( KPrinter &printer );

    QMenu *popupMenu( const QString& name );

    virtual ViewAdaptor* dbusObject();

    virtual bool setContext( const Context &context );
    virtual void getContext( Context &context ) const;

    QWidget *canvas() const;

    //virtual QDockWidget *createToolBox();

    KoDocument *hitTest( const QPoint &viewPos );

public slots:
    void slotUpdate();
    void slotAddResource( ResourceGroup *group );
    void slotEditResource();
    void slotEditCut();
    void slotEditCopy();
    void slotEditPaste();
    void slotViewSelector( bool show );

    void slotViewGantt();
    void slotViewGanttResources();
    void slotViewGanttTaskName();
    void slotViewGanttTaskLinks();
    void slotViewGanttProgress();
    void slotViewGanttFloat();
    void slotViewGanttCriticalTasks();
    void slotViewGanttCriticalPath();
    void slotViewGanttNoInformation();
    void slotViewTaskAppointments();

    void slotViewResources();
    void slotViewResourceAppointments();

    void slotViewAccounts();

    void slotChartDisplay();

    void slotViewCalendarEditor();

    void slotViewTaskEditor();
    void slotAddTask();
    void slotAddSubTask();
    void slotAddMilestone();
    void slotProjectEdit();
    void slotDefineWBS();
    void slotGenerateWBS();

    void slotViewTaskStatusView();
    void slotCreateKofficeDocument( KoDocumentEntry& );

    void slotConfigure();
    void slotAddRelation( Node *par, Node *child );
    void slotModifyRelation( Relation *rel );
    void slotAddRelation( Node *par, Node *child, int linkType );
    void slotModifyRelation( Relation *rel, int linkType );

    void slotExportGantt(); // testing

    void slotRenameNode( Node *node, const QString& name );

    void slotPopupMenu( const QString& menuname, const QPoint &pos );
    void slotPopupMenu( const QString& menuname, const QPoint &pos, ViewListItem *item );

protected slots:
    void slotGuiActivated( ViewBase *view, bool );
    void slotViewActivated( ViewListItem*, ViewListItem* );
    void slotPlugScheduleActions();
    void slotViewSchedule( QAction *act );
    void slotScheduleChanged( MainSchedule* );
    void slotScheduleAdded( const MainSchedule * );
    void slotScheduleRemoved( const MainSchedule * );

    void slotAddScheduleManager( Project *project );
    void slotDeleteScheduleManager( Project *project, ScheduleManager *sm );
    void slotCalculateSchedule( Project*, ScheduleManager* );
    void slotProgressChanged( int value );

    void slotEditCalendar();
    void slotEditCalendar( Calendar *calendar );

    void slotProjectCalendar();
    void slotProjectWorktime();
    void slotProjectAccounts();
    void slotProjectResources();
    void slotViewReportDesign();
    void slotViewReports();

    void slotOpenNode();
    void slotOpenNode( Node *node );
    void slotTaskProgress();
    void slotDeleteTask( QList<Node*> lst );
    void slotDeleteTask( Node *node );
    void slotDeleteTask();
    void slotIndentTask();
    void slotUnindentTask();
    void slotMoveTaskUp();
    void slotMoveTaskDown();

    void slotConnectNode();

    void slotDeleteResource( Resource *resource );
    void slotDeleteResourceGroup( ResourceGroup *group );
    void slotDeleteResourceObjects( QObjectList );

    void slotCurrentChanged( int );

#ifndef NDEBUG
    void slotPrintDebug();
    void slotPrintSelectedDebug();
    void slotPrintCalendarDebug();
    void slotPrintTestDebug();
#else
    static void slotPrintDebug() { };
    static void slotPrintSelectedDebug() { };
    static void slotPrintCalendarDebug() { };
    static void slotPrintTestDebug() { };
#endif

protected:
    virtual void guiActivateEvent( KParts::GUIActivateEvent *event );
    virtual void updateReadWrite( bool readwrite );

    QAction *addScheduleAction( Schedule *sch );
    void setLabel();
    Node *currentTask();
    Resource *currentResource();
    ResourceGroup *currentResourceGroup();
    Calendar *currentCalendar();
    void updateView( QWidget *widget );

private slots:
    void slotActionDestroyed( QObject *o );
    void slotViewListItemRemoved( ViewListItem *item );
    void slotViewListItemInserted( ViewListItem *item );

private:
    void createTaskeditor( ViewListItem *cat );
    void createResourceditor( ViewListItem *cat );
    void createAccountsEditor( ViewListItem *cat );
    void createCalendarEditor( ViewListItem *cat );
    void createScheduleEditor( ViewListItem *cat );
    void createDependencyEditor( ViewListItem *cat );
    void createPertResultView( ViewListItem *cat );
    void createTaskStatusView( ViewListItem *cat );
    void createGanttView( ViewListItem *cat );
    void createResourceView( ViewListItem *cat );
    void createAccountsView( ViewListItem *cat );
    void createResourceAssignmentView( ViewListItem *cat );

    void createChildDocumentViews();
    ViewListItem *createChildDocumentView( DocumentChild *ch );

private:
    QSplitter *m_sp;
    QStackedWidget *m_tab;

    ViewListWidget *m_viewlist;
    ViewListItem *m_viewlistItem; // requested popupmenu item

    //QDockWidget *m_toolbox;

    int m_viewGrp;
    int m_defaultFontSize;
    int m_currentEstimateType;

    bool m_updateGanttview;
    bool m_updateResourceview;
    bool m_updateAccountsview;
    bool m_updateResourceAssignmentView;
    bool m_updatePertEditor;
    bool m_updatePertResult;

    QLabel *m_estlabel;
    QProgressBar *m_progress;

    ViewAdaptor* m_dbus;

    QActionGroup *m_scheduleActionGroup;
    QMap<QAction*, Schedule*> m_scheduleActions;
    // ------ Edit
    QAction *actionCut;
    QAction *actionCopy;
    QAction *actionPaste;

    // ------ View
    KToggleAction *actionViewSelector;

    KToggleAction *actionViewGanttResources;
    KToggleAction *actionViewGanttTaskName;
    KToggleAction *actionViewGanttTaskLinks;
    KToggleAction *actionViewGanttProgress;
    KToggleAction *actionViewGanttFloat;
    KToggleAction *actionViewGanttCriticalTasks;
    KToggleAction *actionViewGanttCriticalPath;
    KToggleAction *actionViewGanttNotScheduled;
    KToggleAction *actionViewTaskAppointments;

    KToggleAction *actionViewResourceAppointments;

    // ------ Insert

    // ------ Project
    KAction *actionEditMainProject;
    KAction *actionEditStandardWorktime;
    KAction *actionEditCalendarList;
    KAction *actionEditAccounts;
    KAction *actionEditResources;
    KAction *actionChartIndicators;

    // ------ Reports
    KAction *actionFirstpage;
    KAction *actionPriorpage;
    KAction *actionNextpage;
    KAction *actionLastpage;

    // ------ Tools
    KAction *actionDefineWBS;
    KAction *actionGenerateWBS;

    // ------ Export (testing)
    KAction *actionExportGantt;

    // ------ Settings
    KAction *actionConfigure;

    // ------ Popup
    KAction *actionOpenNode;
    KAction *actionTaskProgress;
    KAction *actionDeleteTask;
    KAction *actionEditResource;
    KAction *actionEditCalendar;

    //Test
    KAction *actNoInformation;

};

} //Kplato namespace

#endif

/* This file is part of the KDE project
   Copyright (C) 2002 Till Busch <till@bux.at>
   Lucijan Busch <lucijan@gmx.at>
   Daniel Molkentin <molkentin@kde.org>
   Copyright (C) 2003 Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>

   This program is free software; you can redistribute it and,or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Original Author:  Till Busch <till@bux.at>
   Original Project: buX (www.bux.at)
*/

//we want to have kde-support:
#define USE_KDE


#ifndef KEXITABLEVIEW_H
#define KEXITABLEVIEW_H

#include <qscrollview.h>
#include <qtimer.h>
#include <qvariant.h>
#include <qptrlist.h>

#include "kexitablerm.h"
#include "kexitablelist.h"

class QHeader;
class QLineEdit;
class QPopupMenu;
class QTimer;

class KPrinter;

class KexiTableHeader;
class KexiTableRM;
class KexiTableItem;
class KexiTableEdit;
class KexiTableViewPrivate;
//class KexiTableList;

/*
	this class provides a tablewidget ;)
*/

class KEXIDATATABLE_EXPORT KexiTableView : public QScrollView
{
	friend class KexiTableItem;

Q_OBJECT
public:
	KexiTableView(QWidget* parent=0, const char* name=0, KexiTableList *contents=0);
	~KexiTableView();

	enum ColumnModes
	{
		ColumnReadOnly = 1,
		ColumnEditable,
		ColumnAutoIncrement
	};

	virtual void addColumn(QString name, QVariant::Type type, bool editable, QVariant defaultValue = QVariant(""),
		int width=100, bool autoinc=false);

	QString column(int section);
	void setSorting(int col, bool ascending=true);

	QVariant::Type			columnType(int col);
	QVariant			columnDefault(int col);
	bool				columnEditable(int col);
	inline KexiTableItem		*itemAt(int row);

	int currentRow();
	KexiTableItem *selectedItem();

	int		rows() const;
	int		cols() const;

	int		currentCol();

	QRect		cellGeometry(int row, int col) const;
	int		columnWidth(int col) const;
	int		rowHeight() const;
	int		columnPos(int col) const;
	int		rowPos(int row) const;
	int		columnAt(int pos) const;
	int		rowAt(int pos, bool ignoreEnd=false) const;

	void		updateCell(int row, int col);
//	void		updateRow(int row);
	int		sorting();
	void		clear();
	void		clearAll();
	void		remove(int row);
	void		remove(KexiTableItem *item, bool moveCursor=true);

	// properties
	bool		backgroundAltering();
	void		setBackgroundAltering(bool altering);
	bool		recordIndicator();
	void		setRecordIndicator(bool indicator);
	bool		editableOnDoubleClick();
	void		setEditableOnDoubleClick(bool set);
	QColor		emptyAreaColor();
	void		setEmptyAreaColor(QColor c);

#ifndef KEXI_NO_PRINT
	// printing
//	void		setupPrinter(KPrinter &printer);
	void		print(KPrinter &printer);
#endif

	// reimplemented for internal reasons
	virtual QSizePolicy	sizePolicy() const;
	virtual QSize		sizeHint() const;
	virtual QSize		minimumSizeHint() const;
	void		setFont(const QFont &f);

	void		addDropFilter(const QString &filter);

	void		inserted();

	void		emitSelected();

	KexiTableRM	*recordMarker();
	KexiTableRM *verticalHeader();
	
	KexiTableList	*contents() { return m_contents; }

	void		takeInsertItem();
	void		setInsertItem(KexiTableItem *i);
	KexiTableItem	*insertItem();

	enum AdditionPolicy
	{
		NoAdd,
		AutoAdd,
		SignalAdd
	};

	enum DeletionPolicy
	{
		NoDelete,
		AskDelete,
		ImmediateDelete,
		SignalDelete
	};

	virtual void	setAdditionPolicy(AdditionPolicy policy);
	AdditionPolicy	additionPolicy();

	virtual void	setDeletionPolicy(DeletionPolicy policy);
	DeletionPolicy	deletionPolicy();

	void triggerUpdate();

	typedef enum ScrollDirection
	{
		ScrollUp,
		ScrollDown,
		ScrollLeft,
		ScrollRight
	};

protected:
	// painting and layout
	void	drawContents(QPainter *p, int cx, int cy, int cw, int ch);
	void	createBuffer(int width, int height);
	void	paintCell(QPainter* p, KexiTableItem *item, int col, const QRect &cr, bool print=false);
	void	paintEmptyArea(QPainter *p, int cx, int cy, int cw, int ch);
	void	updateGeometries();
	virtual QSize tableSize() const;

	QPoint contentsToViewport2( const QPoint &p );
	void contentsToViewport2( int x, int y, int& vx, int& vy );
	QPoint viewportToContents2( const QPoint& vp );

	// event handling
	virtual void contentsMousePressEvent(QMouseEvent*);
	virtual void contentsMouseMoveEvent(QMouseEvent*);
	virtual void contentsMouseDoubleClickEvent(QMouseEvent *e);
	virtual void contentsMouseReleaseEvent(QMouseEvent *e);
	virtual void keyPressEvent(QKeyEvent*);
	virtual void focusInEvent(QFocusEvent*);
	virtual void focusOutEvent(QFocusEvent*);
	virtual void resizeEvent(QResizeEvent *);
	virtual void viewportResizeEvent( QResizeEvent *e );//js
	virtual void showEvent(QShowEvent *e);
	virtual void contentsDragMoveEvent(QDragMoveEvent *e);
	virtual void contentsDropEvent(QDropEvent *ev);

	void	createEditor(int row, int col, QString addText = QString::null, bool backspace = false);
	bool	focusNextPrevChild(bool next);

	bool	updateContextMenu();
	void	showContextMenu( QPoint pos = QPoint(-1,-1) );

protected slots:
	void			columnWidthChanged( int col, int os, int ns );
	void			columnSort(int col);
	void			editorCancel();
	virtual void		editorOk();
	virtual void		boolToggled();
	void			slotUpdate();

	void			slotAutoScroll();

public slots:
	void			sort();
	void			setCursor(int row, int col = -1);
	void			selectRow(int row);
	void			selectNext();
	void			selectPrev();
	void			gotoNext();
	int			findString(const QString &string);
	virtual void		removeRecord();
	virtual void		addRecord();

signals:
	void			itemSelected(KexiTableItem *);
	void			itemReturnPressed(KexiTableItem *, int);
	void			itemDblClicked(KexiTableItem *, int);
	void			itemChanged(KexiTableItem *, int);
	void			itemChanged(KexiTableItem *, int,QVariant oldValue);
	void			itemRemoveRequest(KexiTableItem *);
	void			currentItemRemoveRequest();
	void			addRecordRequest();
	void			dropped(QDropEvent *);
	void			contextMenuRequested(KexiTableItem *, int col, const QPoint &);
	void			sortedColumnChanged(int col);

protected:

/*
	bool		m_editOnDubleClick;

	// cursor position
	int			m_curRow;
	int			m_curCol;
	KexiTableItem	*m_pCurrentItem;

    // foreign widgets
	QHeader			*m_pTopHeader;
	KexiTableHeader	*m_pVerticalHeader;
	KexiTableRM		*m_pRecordMarker;
	KexiTableEdit	*m_pEditor;

	bool			m_recordIndicator;

	int			m_numRows;
	int			m_numCols;
	int			m_rowHeight;
	int			m_sortedColumn;
	bool			m_sortOrder;

	AdditionPolicy		m_additionPolicy;
	DeletionPolicy		m_deletionPolicy;

	QPixmap			*m_pBufferPm;
	QTimer			*m_pUpdateTimer;
	QPopupMenu		*m_pContextMenu;
	int menu_id_addRecord;
	int menu_id_removeRecord;

	KexiTableList		*m_contents;
	QMemArray<QVariant::Type>	*m_pColumnTypes;
	QMemArray<int>		*m_pColumnModes;
	QPtrList<QVariant>		*m_pColumnDefaults;

	bool			m_needAutoScroll;
	QTimer			*m_scrollTimer;
	KexiTableItem		*m_pInsertItem;

	QStringList		m_dropFilters;

	ScrollDirection		m_scrollDirection;

	bool			m_bgAltering;*/

	KexiTableList *m_contents;

	KexiTableViewPrivate *d;
};

inline KexiTableItem *KexiTableView::itemAt(int row)
{
	return m_contents->at(row);
}
/*
inline int KexiTableView::currentRow()
{
	return m_curRow;
}

inline KexiTableItem *KexiTableView::selectedItem()
{
	return m_pCurrentItem;
}

inline QVariant::Type KexiTableView::columnType(int col)
{
	return m_pColumnTypes->at(col);
}

inline bool	KexiTableView::columnEditable(int col)
{
	return m_pColumnModes->at(col);
	if(m_pColumnModes->at(col) & ColumnEditable)
		return true;

	return false;
}

inline QVariant KexiTableView::columnDefault(int col)
{
	return *m_pColumnDefaults->at(col);
}
*/
#endif

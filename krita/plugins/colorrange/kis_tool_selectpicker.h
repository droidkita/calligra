/*
 *  Copyright (c) 1999 Matthias Elter
 *  Copyright (c) 2002 Patrick Julien
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef KIS_TOOL_SELECT_PICKER_H_
#define KIS_TOOL_SELECT_PICKER_H_

#include <kis_tool.h>
#include <kis_tool_non_paint.h>
#include <kis_tool_factory.h>
#include <kis_selection.h>

class KisCanvasSubject;
class QWidget;
class QVBoxLayout;
class QCheckBox;

/**
 * Tool to select colours by pointing at a color on the image.
 * TODO: 
 *       Implement shift/shift-ctrl keyboard shortcuts for 
 *       temporary add/subtract selection mode.     
 */
class KisToolSelectPicker : public KisToolNonPaint {

	Q_OBJECT
	typedef KisToolNonPaint super;

public:
	KisToolSelectPicker();
	virtual ~KisToolSelectPicker();

public:
	virtual void update(KisCanvasSubject *subject);

	virtual void setup(KActionCollection *collection);


public slots:
	virtual void slotSetFuzziness(int);
	virtual void slotSetAction(int);
	
private:
	virtual QWidget* createOptionWidget(QWidget* parent);
	virtual QWidget* optionWidget();

	virtual void buttonPress(KisButtonPressEvent *e);
	virtual void enter(QEvent *);
	virtual void leave(QEvent *);
	void setSelectCursor(enumSelectionMode);

	KisCanvasSubject *m_subject;
	QWidget *m_optWidget;

	int m_fuzziness;
	enumSelectionMode m_defaultSelectAction;
	enumSelectionMode m_currentSelectAction;
	QTimer *m_timer;

private slots:
	void slotTimer();
};

class KisToolSelectPickerFactory : public KisToolFactory {
	typedef KisToolFactory super;
public:
	KisToolSelectPickerFactory(KActionCollection * ac) : super(ac) {};
	virtual ~KisToolSelectPickerFactory(){};
	
	virtual KisTool * createTool() { 
		KisTool * t =  new KisToolSelectPicker(); 
		Q_CHECK_PTR(t);
		t -> setup(m_ac); 
		return t; 
	}
	virtual KisID id() { return KisID("selectpicker", i18n("Selection picker")); }
};


#endif // KIS_TOOL_SELECT_PICKER_H_


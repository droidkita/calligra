/* -*- C++ -*-

  $Id$

  This file is part of Kontour.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
  Copyright (C) 2001 Igor Janssen (rm@linux.ru.net)

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as
  published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#include "kontour_view.h"

#include <qscrollbar.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qsplitter.h>
#include <qregexp.h>
#include <qdockwindow.h>
#include <qdockarea.h>

#include <kaction.h>
#include <kstdaction.h>
#include <kpopupmenu.h>
#include <kcursor.h>
#include <kiconloader.h>
#include <kconfig.h>
#include <kparts/event.h>

#include "kontour_doc.h"
#include "kontour_factory.h"
#include "GDocument.h"
#include "Canvas.h"
#include "Ruler.h"
#include "TabBar.h"
#include "OptionsDialog.h"
#include "ToolController.h"
#include "SelectTool.h"
#include "RectTool.h"
#include "OvalTool.h"
#include "ZoomTool.h"
#include "LayerPanel.h"
#include "koColorChooser.h"
#include "DeleteCmd.h"

KontourView::KontourView(QWidget *parent, const char *name, KontourDocument *doc)
:KoView(doc, parent, name)
{
  mDoc = doc;

  objMenu = 0;
  rulerMenu = 0;
  
  setInstance(KontourFactory::global());
  setXMLFile("kontour.rc");

  readConfig();
  setupActions();
  initActions();
  setupCanvas();
  setupPanels();
  readConfigAfter();
}

KontourView::~KontourView()
{
//  delete mPluginManager;
  writeConfig();
  delete tcontroller;
  if(objMenu)
    delete objMenu;
  if(rulerMenu)
    delete rulerMenu;

  // Delete GUI
  delete hRuler;
  delete vRuler;
}

void KontourView::unit(MeasurementUnit u)
{
  mUnit = u;
}

void KontourView::setupActions()
{
  /* Edit menu */
  
  m_copy = KStdAction::copy(this, SLOT(slotCopy()), actionCollection(), "copy");
  m_paste = KStdAction::paste(this, SLOT(slotPaste()), actionCollection(), "paste");
  m_cut = KStdAction::cut(this, SLOT(slotCut()), actionCollection(), "cut");
  
  m_duplicate = new KAction(i18n("Dup&licate"), 0, this, SLOT(slotDuplicate()), actionCollection(), "duplicate");
  m_delete = new KAction(i18n("&Delete"), "editdelete", 0, this, SLOT(slotDelete()), actionCollection(), "delete");
  m_selectAll = KStdAction::selectAll(this, SLOT(slotSelectAll()), actionCollection(), "selectAll");
  
  /* View menu */
  
  m_zoomIn = new KAction(i18n("Zoom in"), "viewmag+", CTRL+Key_Plus, this, SLOT(slotZoomIn()), actionCollection(), "zoomin");
  m_zoomOut = new KAction(i18n("Zoom out"), "viewmag-", CTRL+Key_Minus, this, SLOT(slotZoomOut()), actionCollection(), "zoomout");
  
  m_viewZoom = new KSelectAction(i18n("&Zoom"), 0, actionCollection(), "view_zoom");
  QStringList zooms;
  zooms << "50%";
  zooms << "75%";
  zooms << "100%";
  zooms << "150%";
  zooms << "200%";
  zooms << "400%";
  zooms << "600%";
  zooms << "800%";
  zooms << "1000%";

  m_viewZoom->setItems(zooms);
  m_viewZoom->setEditable(true);
  connect(m_viewZoom, SIGNAL(activated(const QString &)),this, SLOT(slotViewZoom(const QString &)));
  m_viewZoom->setCurrentItem(2);
  
  m_outline = new KToggleAction(i18n("Ou&tline"), 0, this, SLOT(slotOutline()), actionCollection(), "outline");
  m_outline->setExclusiveGroup("Outline");
  m_normal = new KToggleAction(i18n("&Normal"), 0, this, SLOT(slotNormal()), actionCollection(), "normal");
  m_normal->setExclusiveGroup("Outline");
  m_normal->setChecked(true);  //TODO: from config

  m_showRuler = new KToggleAction(i18n("Show &Ruler"), 0, actionCollection(), "showRuler");
  connect(m_showRuler, SIGNAL(toggled(bool)), this, SLOT(slotShowRuler(bool)));
  
  m_showGrid = new KToggleAction(i18n("Show &Grid"), 0, actionCollection(), "showGrid");
  connect(m_showGrid, SIGNAL(toggled(bool)), this, SLOT(slotShowGrid(bool)));
  
  m_showHelplines = new KToggleAction(i18n("Show &Helplines"), 0, actionCollection(), "showHelplines");
  connect(m_showHelplines, SIGNAL(toggled(bool)), this, SLOT(slotShowHelplines(bool)));
  
  /* Layout menu */

  m_snapToGrid = new KToggleAction(i18n("&Align To Grid"), "snap_to_grid", 0, actionCollection(), "alignToGrid");
  connect(m_snapToGrid, SIGNAL(toggled(bool)), this, SLOT(slotAlignToGrid(bool)));
  m_snapToGrid->setChecked(activeDocument()->snapToGrid());

  m_alignToHelplines = new KToggleAction( i18n("Align &To Helplines"), 0, actionCollection(), "alignToHelplines" );
  connect( m_alignToHelplines, SIGNAL( toggled( bool ) ), this, SLOT( slotAlignToHelplines( bool ) ) );
  m_alignToHelplines->setChecked(activeDocument()->snapToHelplines());

  m_toFront = new KAction( i18n("To &Front"), 0, this, SLOT( slotToFront() ), actionCollection(), "toFront" );
  m_toBack = new KAction( i18n("To &Back"), 0, this, SLOT( slotToBack() ), actionCollection(), "toBack" );
  m_forwardOne = new KAction( i18n("Forward &One"), 0, this, SLOT( slotForwardOne() ), actionCollection(), "forwardOne" );
  m_backOne = new KAction( i18n("B&ack One"), 0, this, SLOT( slotBackOne() ), actionCollection(), "backOne" );

  new KAction(i18n("&Group"), "group", 0, this, SLOT(slotGroup()), actionCollection(), "group");
  new KAction(i18n("&Ungroup"), "ungroup", 0, this, SLOT(slotUngroup()), actionCollection(), "ungroup");
  
  /* Style menu */
  
  m_styles = new KSelectAction(i18n("&Styles"), 0, actionCollection(), "styles");
  connect(m_styles, SIGNAL(activated(const QString &)),this, SLOT(slotStyles(const QString &)));

  m_addStyle = new KAction(i18n("&Add style"), 0, this, SLOT(slotAddStyle()), actionCollection(), "addStyle");
  m_deleteStyle = new KAction(i18n("&Delete style"), 0, this, SLOT(slotDeleteStyle()), actionCollection(), "deleteStyle");

  /* Modify menu */

  new KAction(i18n("&Position..."), 0, this, SLOT(slotTransformPosition()), actionCollection(), "transformPosition");
  new KAction(i18n("&Dimension..."), 0, this, SLOT(slotTransformDimension()), actionCollection(), "transformDimension");
  new KAction(i18n("&Rotation..."), 0, this, SLOT(slotTransformRotation()), actionCollection(), "transformRotation");
  new KAction(i18n("&Mirror..."), 0, this, SLOT(slotTransformMirror()), actionCollection(), "transformMirror");
  
  m_distribute = new KAction( i18n("&Align/Distribute..."), 0, this, SLOT( slotDistribute() ), actionCollection(), "distribute" );
  new KAction( i18n("&Convert to Path"), 0, this, SLOT( slotConvertToPath() ), actionCollection(), "convertToPath" );

  /* Effects menu */

  new KAction( i18n("&Blend..."), 0, this, SLOT( slotBlend() ), actionCollection(), "blend" );

  /* Settings menu */

  m_options = KStdAction::preferences(this, SLOT(slotOptions()), actionCollection(), "options");
}

void KontourView::initActions()
{
  m_showRuler->setChecked(true);
  m_showGrid->setChecked(activeDocument()->showGrid());
  m_showHelplines->setChecked(activeDocument()->showHelplines());
  updateStyles();
}

void KontourView::setupCanvas()
{
  mSplitView = new QSplitter(this);
  
  QGridLayout *viewGrid = new QGridLayout(this);
  viewGrid->addWidget(mSplitView, 0, 0);
  
  QWidget *mLeftSide = new QWidget(mSplitView);

  mRightDock = new QDockArea(Qt::Horizontal, QDockArea::Normal, mSplitView);  
  
  /* create horizontal ruler */
  hRuler = new Ruler(mDoc, Ruler::Horizontal, mUnit, mLeftSide);
  hRuler->setCursor(KCursor::handCursor());
  
  /* create vertical ruler */
  vRuler = new Ruler(mDoc, Ruler::Vertical, mUnit, mLeftSide);
  vRuler->setCursor(KCursor::handCursor());

  connect(hRuler, SIGNAL(rmbPressed()), this, SLOT(popupForRulers()));
  connect(vRuler, SIGNAL(rmbPressed()), this, SLOT(popupForRulers()));

  QSplitter *tabSplit = new QSplitter(mLeftSide);

  mTabBar = new TabBar(tabSplit, this);

  vBar = new QScrollBar(QScrollBar::Vertical, mLeftSide);
  hBar = new QScrollBar(QScrollBar::Horizontal, tabSplit);

  mCanvas = new Canvas(mDoc->document(), this, hBar, vBar, mLeftSide);

  QPixmap *pm;
  
  mTabBarFirst = new QPushButton(mLeftSide);
  mTabBarFirst->setFixedSize(16,16);
  pm = new QPixmap(BarIcon("tab_first"));
  if(pm)
    mTabBarFirst->setPixmap(*pm);
  
  mTabBarLeft = new QPushButton(mLeftSide);
  mTabBarLeft->setFixedSize(16,16);
  pm = new QPixmap(BarIcon("tab_left"));
  if(pm)
    mTabBarLeft->setPixmap(*pm);
    
  mTabBarRight = new QPushButton(mLeftSide);
  mTabBarRight->setFixedSize(16,16);
  pm = new QPixmap(BarIcon("tab_right"));
  if(pm)
    mTabBarRight->setPixmap(*pm);
    
  mTabBarLast = new QPushButton(mLeftSide);
  mTabBarLast->setFixedSize(16,16);
  pm = new QPixmap(BarIcon("tab_last"));
  if(pm)
    mTabBarLast->setPixmap(*pm);

  tabLayout = new QHBoxLayout();
  tabLayout->addWidget(mTabBarFirst);
  tabLayout->addWidget(mTabBarLeft);
  tabLayout->addWidget(mTabBarRight);
  tabLayout->addWidget(mTabBarLast);
  tabLayout->addWidget(tabSplit);
  connect(mTabBarFirst, SIGNAL(clicked()), mTabBar, SLOT(scrollFirst()));
  connect(mTabBarLeft, SIGNAL(clicked()), mTabBar, SLOT(scrollLeft()));
  connect(mTabBarRight, SIGNAL(clicked()), mTabBar, SLOT(scrollRight()));
  connect(mTabBarLast, SIGNAL(clicked()), mTabBar, SLOT(scrollLast()));

  QGridLayout *layout = new QGridLayout(mLeftSide);
  layout->addWidget(mCanvas, 1, 1);
  layout->addWidget(hRuler, 0, 1);
  layout->addWidget(vRuler, 1, 0);
  layout->addMultiCellWidget(vBar, 0, 1, 2, 2);
  layout->addMultiCellLayout(tabLayout, 2, 2, 0, 1);

  connect(mCanvas, SIGNAL(mousePositionChanged(int, int)), hRuler, SLOT(updatePointer(int, int)));
  connect(mCanvas, SIGNAL(mousePositionChanged(int, int)), vRuler, SLOT(updatePointer(int, int)));

  connect(mCanvas, SIGNAL(offsetXChanged(int)), hRuler, SLOT(updateOffset(int)));
  connect(mCanvas, SIGNAL(offsetYChanged(int)), vRuler, SLOT(updateOffset(int)));

  connect(activeDocument(), SIGNAL(zoomFactorChanged(double)), this, SLOT(slotZoomFactorChanged()));


//    connect(canvas,SIGNAL(rightButtonAtSelectionClicked(int,int)),this,SLOT(popupForSelection()));

  // helpline creation
//    connect (hRuler, SIGNAL (drawHelpline(int, int, bool)), canvas, SLOT(drawTmpHelpline(int, int, bool)));
//    connect (vRuler, SIGNAL (drawHelpline(int, int, bool)), canvas, SLOT(drawTmpHelpline(int, int, bool)));
//    connect (hRuler, SIGNAL (addHelpline(int, int, bool)), this, SLOT(slotAddHelpline(int, int, bool)));
//    connect (vRuler, SIGNAL (addHelpline(int, int, bool)), this, SLOT(slotAddHelpline(int, int, bool)));

  setFocusPolicy(QWidget::StrongFocus);
  setFocusProxy(mCanvas);
  mCanvas->installEventFilter(this);
}

void KontourView::setupPanels()
{
  /* Layer panel */
  QDockWindow *win = new QDockWindow();
  win->setResizeEnabled(true);
  mLayerPanel = new LayerPanel(activeDocument(), win);
  win->setWidget(mLayerPanel);
  connect(activeDocument(), SIGNAL(updateLayerView()), mLayerPanel, SLOT(updatePanel()));
  mRightDock->moveDockWindow(win);

  QDockWindow *win1 = new QDockWindow();  
  win1->setResizeEnabled(true);
  KoColorChooser *mColorPanel = new KoColorChooser(win1);
  win1->setWidget(mColorPanel);
  mRightDock->moveDockWindow(win1);
}

void KontourView::setupTools()
{
  tcontroller = new ToolController(this);

  mSelectTool = new SelectTool("Select", tcontroller);
  tcontroller->registerTool(mSelectTool);

  mRectTool = new RectTool("Rect", tcontroller);
  tcontroller->registerTool(mRectTool);

  mOvalTool = new OvalTool("Oval", tcontroller);
  tcontroller->registerTool(mOvalTool);

  mZoomTool = new ZoomTool("Zoom", tcontroller);
  tcontroller->registerTool(mZoomTool);

  tcontroller->initToolBar();
  tcontroller->selectTool("Select");
}

void KontourView::readConfig()
{
  KConfig *config = KontourFactory::global()->config();

  /* Read default unit */
  config->setGroup("General");
  QString value = config->readEntry("DefaultUnit", "pt");
  if(value == "mm")
    mUnit = UnitMillimeter;
  else if(value == "inch")
    mUnit = UnitInch;
  else if(value == "cm")
    mUnit = UnitCentimeter;
  else if(value == "pica")
    mUnit = UnitPica;
  else if(value == "didot")
    mUnit = UnitDidot;
  else if(value == "cicero")
    mUnit = UnitCicero;
  else
    mUnit = UnitPoint;

  mWorkSpaceColor = lightGray;
//  UnitBox::setDefaultMeasurementUnit(defaultUnit);


  /*config->setGroup("Panels");
   bool b=config->readBoolEntry("Enabled",true);
   if (!b)
      mLayerDockBase->makeVisible(b);
   else
   if (b)
      createLayerPanel(false);*/
}

void KontourView::readConfigAfter()
{
  KConfig *config = KontourFactory::global()->config();

  QValueList<int> s;
  s << config->readNumEntry("LeftSide", -1);
  s << config->readNumEntry("RightSide", 200);
  mSplitView->setSizes(s);
}

void KontourView::writeConfig()
{
  KConfig *config = KontourFactory::global()->config();

  config->setGroup("General");

  /* Write default unit */
  switch(mUnit)
  {
  case UnitPoint:
    config->writeEntry("DefaultUnit", "pt");
    break;
  case UnitMillimeter:
    config->writeEntry("DefaultUnit", "mm");
    break;
  case UnitInch:
    config->writeEntry("DefaultUnit", "inch");
    break;
  case UnitCentimeter:
    config->writeEntry("DefaultUnit", "cm");
    break;
  case UnitPica:
    config->writeEntry("DefaultUnit", "pica");
    break;
  case UnitDidot:
    config->writeEntry("DefaultUnit", "didot");
    break;
  case UnitCicero:
    config->writeEntry("DefaultUnit", "cicero");
    break;
  }

  QValueList<int> s = mSplitView->sizes();
  config->writeEntry("LeftSide", s[0]);
  config->writeEntry("RightSide", s[1]);
  
/* config->setGroup("Panels");
   config->writeEntry("Enabled",m_showLayers->isChecked());
   config->sync();*/
}

void KontourView::workSpaceColor(QColor c)
{
  mWorkSpaceColor = c;
}

void KontourView::updateStyles()
{
  QStringList *styles = activeDocument()->styles()->stringList();
  m_styles->setItems(*styles);
  delete styles;
  m_styles->setCurrentItem(activeDocument()->styles()->currentNum());
}

void KontourView::customEvent(QCustomEvent *e)
{
  if(KParts::GUIActivateEvent::test(e))
    if(((KParts::GUIActivateEvent*)e)->activated())
      setupTools();
  KoView::customEvent(e);
}

bool KontourView::eventFilter(QObject *o, QEvent *e)
{
// TODO TOOLS
//  if(!tcontroller ||!tcontroller->getActiveTool())
//    return false;

//  if((e==0) || (tcontroller->getActiveTool()->id()==Tool::ToolSelect))
//      return false;

  if(o == mCanvas && e->type() == QEvent::MouseButtonPress)
  {
    QMouseEvent *me = (QMouseEvent*)e;
    if(me->button() == RightButton)
    {
      //m_selectTool->setChecked(true);
      //slotSelectTool(true);
    }
  }
  return false;
}

void KontourView::updateReadWrite(bool readwrite)
{
}

void KontourView::popupForSelection ()
{
  if(objMenu)
    delete objMenu;
  objMenu = new KPopupMenu();
/*  m_copy->plug( objMenu );
  m_cut->plug( objMenu );
  m_duplicate->plug(objMenu);
  m_delete->plug(objMenu);
  objMenu->insertSeparator ();
  m_distribute->plug( objMenu );
  m_toFront->plug( objMenu );
  m_toBack->plug( objMenu );
  m_forwardOne->plug( objMenu );
  m_backOne->plug( objMenu );
  objMenu->insertSeparator ();
  m_properties->plug( objMenu );*/
  objMenu->popup(QCursor::pos());
}

void KontourView::popupForRulers()
{
  if(!mDoc->isReadWrite())
    return;

  if(!rulerMenu)
  {
    rulerMenu = new KPopupMenu();
    m_showGrid->plug(rulerMenu);
    m_showHelplines->plug(rulerMenu);
    m_snapToGrid->plug(rulerMenu);
    m_alignToHelplines->plug(rulerMenu);
  }
  rulerMenu->popup(QCursor::pos());
}

void KontourView::changePenColor(KoColor c)
{
}

void KontourView::changeBrushColor(KoColor c)
{
}

void KontourView::slotZoomFactorChanged()
{
//  vRuler->setZoomFactor(factor,mCanvas->visibleArea().left(),mCanvas->visibleArea().top());
//  hRuler->setZoomFactor(factor,mCanvas->visibleArea().left(),mCanvas->visibleArea().top());
  QStringList list = m_viewZoom->items();
  QString f = QString::number(qRound(activeDocument()->zoomFactor() * 100.0));
  int i = 0;
  for(QValueList<QString>::Iterator it = list.begin(); it != list.end(); ++it, ++i)
    if((*it).left((*it).length() - 1) == f)
    {
      m_viewZoom->setCurrentItem(i);
      return;
    }
   //current zoom value not found in list
   f += '%';
   m_viewZoom->changeItem(8, f);
   m_viewZoom->setCurrentItem(8);
}

/*******************[Actions]*******************/

void KontourView::setupPrinter(KPrinter &printer)
{
//  mCanvas->setupPrinter(printer);
}

void KontourView::print(KPrinter &printer)
{
//  mCanvas->print(printer);
}

void KontourView::slotCopy()
{
}

void KontourView::slotPaste()
{
}

void KontourView::slotCut()
{
}

void KontourView::slotDuplicate()
{
}

void KontourView::slotDelete()
{
  DeleteCmd *cmd = new DeleteCmd(activeDocument());
  mDoc->history()->addCommand(cmd);
}

void KontourView::slotSelectAll()
{
}

void KontourView::slotZoomIn()
{
//   mZoomTool->zoomIn(getCanvas());
//   bool state=mCanvas->getZoomFactor()!=100;
//   m_zoomIn->setEnabled(state);
//   m_zoomOut->setEnabled(true);
}

void KontourView::slotZoomOut()
{
//   mZoomTool->zoomOut(getCanvas());
//   m_zoomOut->setEnabled(mCanvas->getZoomFactor()>=0.06);
//   m_zoomIn->setEnabled(true);
}

void KontourView::slotViewZoom(const QString &s)
{
  QString z(s);
  z = z.replace(QRegExp("%"), "");
  z = z.simplifyWhiteSpace();
  double zoom = z.toDouble() / 100.0;
  activeDocument()->zoomFactor(zoom);
//   m_zoomIn->setEnabled(zoom!=100);
//   m_zoomOut->setEnabled(zoom>=0.06);
}

void KontourView::slotOutline()
{
  if(m_outline->isChecked())
    mCanvas->outlineMode(true);
  else
    m_outline->setChecked(true);
}

void KontourView::slotNormal()
{
  if(m_normal->isChecked())
    mCanvas->outlineMode(false);
  else
    m_normal->setChecked(true);
}

void KontourView::slotShowRuler(bool b)
{
  mShowRulers = b;

  if(mShowRulers)
  {
    hRuler->show();
    vRuler->show();
  }
  else
  {
    hRuler->hide();
    vRuler->hide();
  }
  // recalculate layout
//  resizeEvent(0L);
}

void KontourView::slotShowGrid(bool b)
{
  if(b != activeDocument()->showGrid())
  {
    activeDocument()->showGrid(b);
    mCanvas->update();
  }
}

void KontourView::slotShowHelplines(bool b)
{
  if(b != activeDocument()->showHelplines())
  {
    activeDocument()->showHelplines(b);
    mCanvas->update();
  }
}

void KontourView::slotAlignToGrid(bool b)
{
  activeDocument()->snapToGrid(b);
}

// TODO change name
void KontourView::slotAlignToHelplines(bool b)
{
  activeDocument()->snapToHelplines(b);
}

void KontourView::slotToFront()
{
//  cmdHistory.addCommand (new ReorderCmd (m_pDoc->gdoc(), RP_ToFront), true);
}

void KontourView::slotToBack()
{
//  cmdHistory.addCommand (new ReorderCmd (m_pDoc->gdoc(), RP_ToBack), true);
}

void KontourView::slotForwardOne()
{
//  cmdHistory.addCommand (new ReorderCmd (m_pDoc->gdoc(), RP_ForwardOne), true);
}

void KontourView::slotBackOne()
{
//  cmdHistory.addCommand (new ReorderCmd (m_pDoc->gdoc(), RP_BackwardOne), true);
}

void KontourView::slotGroup()
{
//    cmdHistory.addCommand (new GroupCmd (m_pDoc->gdoc()), true);
}

void KontourView::slotUngroup()
{
//    cmdHistory.addCommand (new UngroupCmd (m_pDoc->gdoc()), true);
}

void KontourView::slotStyles(const QString &s)
{

}

void KontourView::slotAddStyle()
{
  activeDocument()->styles()->addStyle();
  updateStyles();
}

void KontourView::slotDeleteStyle()
{

}

void KontourView::slotTransformPosition()
{
  showTransformationDialog(0);
}

void KontourView::slotTransformDimension()
{
  showTransformationDialog(1);
}

void KontourView::slotTransformRotation()
{
  showTransformationDialog(2);
}

void KontourView::slotTransformMirror()
{
  showTransformationDialog(3);
}

void KontourView::slotDistribute()
{
//    AlignmentDialog::alignSelection (m_pDoc->gdoc(), &cmdHistory);
}

void KontourView::slotConvertToPath()
{
//  if( !m_pDoc->gdoc()->activePage()->selectionIsEmpty() )
//        cmdHistory.addCommand (new ToCurveCmd (m_pDoc->gdoc()), true);
}

void KontourView::slotBlend()
{
/*    if ( m_pDoc->gdoc()->activePage()->selectionCount () == 2)
    {
        int steps = BlendDialog::getNumOfSteps ();
        if (steps > 0)
            cmdHistory.addCommand (new BlendCmd (m_pDoc->gdoc(), steps), true);
    }
    else
        KMessageBox::information(this,
                                 i18n("You have to select exactly two objects."),
                                 i18n("Blending"), "blending");*/
}

void KontourView::slotOptions()
{
  OptionsDialog dialog(this, activeDocument(), 0L, "Options");
  dialog.exec();

/*   if (OptionDialog::setup(activeDocument())==QDialog::Accepted)
   {
      hRuler->setMeasurementUnit(PStateManager::instance()->defaultMeasurementUnit());
      vRuler->setMeasurementUnit(PStateManager::instance()->defaultMeasurementUnit());
      PStateManager::instance()->saveDefaultSettings();
   }*/
}

//TODO need this dialog?
void KontourView::showTransformationDialog(int id)
{
/*    TransformationDialog *transformationDialog = new TransformationDialog (&cmdHistory);
    QObject::connect (m_pDoc->gdoc(), SIGNAL (selectionChanged ()),
                      transformationDialog, SLOT (update ()));
    transformationDialog->setDocument ( m_pDoc->gdoc() );
    transformationDialog->showTab (id);*/
}








/*
void KontourView::showCurrentMode (Tool::ToolID, const QString& msg)
{
   KoMainWindow * tmpKo = shell();
   if (tmpKo)
      tmpKo->statusBarLabel()->setText(msg);
   //statusbar->changeItem (msg, 2);
}

//void KontourView::resizeEvent(QResizeEvent* ) {
    if(mShowRulers) {
        hRuler->setGeometry(20, 0, width()-20, 20);
        vRuler->setGeometry(0, 20, 20, height()-20);
        mCanvas->setGeometry(20, 20, width()-20, height()-20);
    }
    else
        mCanvas->setGeometry(0, 0, width(), height());
//}

//when we get here, the mCanvas is already zoomed


// Reimplemented from KoView. Automatically called for embedded views
void KontourView::setZoom( double zoom ) {
    kdDebug() << "KontourView::setZoom " << zoom << endl;
    if (zoom != mCanvas->getZoomFactor() )
        mCanvas->setZoomFactor(zoom);
}

// Reimplemented from KoView.
double KontourView::zoom() const {
    return mCanvas->getZoomFactor();
}

void KontourView::slotAddHelpline(int x, int y, bool d) {
    m_showHelplines->setChecked(true);
    mCanvas->addHelpline(x, y, d);
}*/

#include "kontour_view.moc"

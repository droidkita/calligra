/*
 *  Copyright (c) 2009 Cyrille Berger <cberger@cberger.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "View.h"

#include <QGridLayout>
#include <QToolBar>
#include <QScrollBar>
#include <QTimer>
#include <QApplication>
#include <QClipboard>

#include <KoCanvasController.h>
#include <KoCanvasResourceProvider.h>
#include <KoToolManager.h>
#include <KoToolProxy.h>
#include <KoZoomHandler.h>
#include <KoToolBoxFactory.h>
#include <KoShapeController.h>
#include <KoShapeManager.h>
#include <KoZoomAction.h>
#include <KoZoomController.h>
#include <KoSelection.h>
#include <KoToolDocker.h>
#include <KoMainWindow.h>
#include <KoDockRegistry.h>
#include <KoShapeLayer.h>
#include <KoDrag.h>
#include <KoCutController.h>
#include <KoCopyController.h>

#include "Canvas.h"
#include "RootSection.h"
#include "Section.h"
#include "ViewManager.h"
#include "import/DockerManager.h"

#include <kdebug.h>
#include <klocale.h>
#include <kicon.h>
#include <ktoggleaction.h>
#include <kactionmenu.h>
#include <kactioncollection.h>
#include <kstatusbar.h>
#include <kparts/event.h>
#include <kparts/partmanager.h>
#include "SectionsBoxDock.h"
#include "KoOdf.h"

#include "MainWindow.h"
#include "SectionContainer.h"
#include "Layout.h"
#include "SectionPropertiesDock.h"

View::View( RootSection *document, MainWindow* parent )
: QWidget( parent )
, m_doc( document )
, m_activeSection( 0 )
, m_mainWindow(parent)
{
  
  m_doc->viewManager()->addView(this);
  
  initGUI();
  initActions();

  if ( m_doc->sections().count() > 0 ) {
    setActiveSection( m_doc->sections()[0] );
  } else {
    setActiveSection(0);
  }

  setXMLFile("braindumpview.rc");
  
  m_doc->viewManager()->viewHasFocus(this);
}

View::~View()
{
  m_doc->viewManager()->removeView(this);
  KoToolManager::instance()->removeCanvasController( m_canvasController );
  delete m_zoomController;
}


Section* View::activeSection() const
{
  return m_activeSection;
}

void View::initGUI()
{
    // add all plugins.
    foreach(const QString & docker, KoDockRegistry::instance()->keys()) {
      kDebug() << "Creating docker: " << docker;
      KoDockFactory *factory = KoDockRegistry::instance()->value(docker);
      m_mainWindow->createDockWidget(factory);
    }
    
    // Init the widgets
    QGridLayout * gridLayout = new QGridLayout( this );
    gridLayout->setMargin( 0 );
    gridLayout->setSpacing( 0 );
    setLayout( gridLayout );

    m_canvas = new Canvas( this, m_doc );
    connect(m_canvas, SIGNAL(canvasReceivedFocus()), SLOT(canvasReceivedFocus()));
    
    m_canvasController = new KoCanvasController( this );
    m_canvasController->setCanvas( m_canvas );
    m_canvasController->setCanvasMode( KoCanvasController::Infinite );
    KoToolManager::instance()->addController( m_canvasController );
    KoToolManager::instance()->registerTools( actionCollection(), m_canvasController );

    m_zoomController = new KoZoomController( m_canvasController, &m_zoomHandler, actionCollection());
    connect( m_zoomController, SIGNAL( zoomChanged( KoZoomMode::Mode, qreal ) ),
             this, SLOT( slotZoomChanged( KoZoomMode::Mode, qreal ) ) );

    m_zoomAction = m_zoomController->zoomAction();
    m_mainWindow->addStatusBarItem( m_zoomAction->createWidget( m_mainWindow->statusBar() ), 0, this );

    m_zoomController->setZoomMode( KoZoomMode::ZOOM_WIDTH );

    gridLayout->addWidget( m_canvasController, 1, 1 );

    connect(m_canvasController, SIGNAL(canvasMousePositionChanged(const QPoint&)),
             this, SLOT(updateMousePosition(const QPoint&)));

    KoToolBoxFactory toolBoxFactory(m_canvasController, i18n("Tools") );
    m_mainWindow->createDockWidget( &toolBoxFactory );

    connect( m_canvasController, SIGNAL( toolOptionWidgetsChanged(const QMap<QString, QWidget *> &) ), m_mainWindow->dockerManager(), SLOT( newOptionWidgets(const  QMap<QString, QWidget *> &) ) );

    connect(shapeManager(), SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
    connect(m_canvas, SIGNAL(documentSize(const QSize&)), m_canvasController, SLOT(setDocumentSize(const QSize&)));
    connect(m_canvasController, SIGNAL(moveDocumentOffset(const QPoint&)),
            m_canvas, SLOT(setDocumentOffset(const QPoint&)));

    SectionsBoxDockFactory structureDockerFactory;
    m_sectionsBoxDock = qobject_cast<SectionsBoxDock*>( m_mainWindow->createDockWidget( &structureDockerFactory ) );
    Q_ASSERT(m_sectionsBoxDock);
    m_sectionsBoxDock->setup(m_doc, this);

    SectionPropertiesDockFactory sectionPropertiesDockerFactory;
    m_sectionPropertiesDock = qobject_cast<SectionPropertiesDock*>( m_mainWindow->createDockWidget( &sectionPropertiesDockerFactory ) );
    Q_ASSERT(m_sectionPropertiesDock);
    
    KoToolManager::instance()->requestToolActivation( m_canvasController );

    show();
}

void View::initActions()
{
  KAction *action = actionCollection()->addAction( KStandardAction::Cut, "edit_cut", 0, 0);
  new KoCutController(canvas(), action);
  action = actionCollection()->addAction( KStandardAction::Copy, "edit_copy", 0, 0 );
  new KoCopyController(canvas(), action);
  m_editPaste = actionCollection()->addAction( KStandardAction::Paste, "edit_paste", this, SLOT( editPaste() ) );
  connect(QApplication::clipboard(), SIGNAL(dataChanged()), this, SLOT(clipboardDataChanged()));
  connect(m_canvas->toolProxy(), SIGNAL(toolChanged(const QString&)), this, SLOT(clipboardDataChanged()));
  clipboardDataChanged();
  actionCollection()->addAction(KStandardAction::SelectAll,  "edit_select_all", this, SLOT(editSelectAll()));
  actionCollection()->addAction(KStandardAction::Deselect,  "edit_deselect_all", this, SLOT(editDeselectAll()));

}

void View::editPaste()
{
  m_canvas->toolProxy()->paste();
}

void View::editDeleteSelection()
{
  m_canvas->toolProxy()->deleteSelection();
}

void View::editSelectAll()
{
  KoSelection* selection = canvas()->shapeManager()->selection();
  if( !selection )
    return;

  KoShapeLayer *layer = activeSection()->sectionContainer()->layer();

  QList<KoShape*> layerShapes( layer->iterator() );
  foreach( KoShape *layerShape, layerShapes ) {
    selection->select( layerShape );
    layerShape->update();
  }

  selectionChanged();
}

void View::editDeselectAll()
{
  KoSelection* selection = canvas()->shapeManager()->selection();
  if( selection )
    selection->deselectAll();

  selectionChanged();
  canvas()->update();
}

void View::slotZoomChanged( KoZoomMode::Mode mode, qreal zoom )
{
  Q_UNUSED(mode);
  Q_UNUSED(zoom);
  canvas()->updateOrigin();
  canvas()->update();
}

KoShapeManager* View::shapeManager() const
{
  return m_canvas->shapeManager();
}

void View::setActiveSection( Section* page )
{
  if(m_activeSection)
  {
    disconnect(m_activeSection->layout(), SIGNAL(boundingBoxChanged(const QRectF& )), this, SLOT(sectionBoundingBoxChanged(const QRectF& )));
  }
  m_activeSection = page;


  if(m_activeSection)
  {
    
    QList<KoShape*> shapes;
    shapes.push_back(page->sectionContainer()->layer());
    shapeManager()->setShapes( shapes, KoShapeManager::AddWithoutRepaint );

    KoShapeLayer* layer = page->sectionContainer()->layer();
    shapeManager()->selection()->setActiveLayer( layer );


    // Make sure the canvas is enabled
    canvas()->setEnabled(true);
    connect(m_activeSection->layout(), SIGNAL(boundingBoxChanged(const QRectF& )), SLOT(sectionBoundingBoxChanged(const QRectF& )));
    sectionBoundingBoxChanged(m_activeSection->layout()->boundingBox());
  } else {
    shapeManager()->setShapes( QList<KoShape*>(), KoShapeManager::AddWithoutRepaint );
    shapeManager()->selection()->setActiveLayer( 0 );
    canvas()->setEnabled(false);
  }

  m_canvas->sectionChanged(activeSection());

  m_canvas->update();
  m_sectionsBoxDock->updateGUI();
  m_sectionPropertiesDock->setSection(m_activeSection);
}

void View::updateMousePosition(const QPoint& position)
{
  QPoint canvasOffset( m_canvasController->canvasOffsetX(), m_canvasController->canvasOffsetY() );
  // the offset is positive it the canvas is shown fully visible
  canvasOffset.setX(canvasOffset.x() < 0 ? canvasOffset.x(): 0);
  canvasOffset.setY(canvasOffset.y() < 0 ? canvasOffset.y(): 0);
  QPoint viewPos = position - canvasOffset;
}

void View::selectionChanged()
{
}

void View::clipboardDataChanged()
{
  const QMimeData* data = QApplication::clipboard()->mimeData();
  bool paste = false;

  if (data)
  {
    // TODO see if we can use the KoPasteController instead of having to add this feature in each koffice app.
    QStringList mimeTypes = m_canvas->toolProxy()->supportedPasteMimeTypes();
    mimeTypes << KoOdf::mimeType( KoOdf::Graphics );
    mimeTypes << KoOdf::mimeType( KoOdf::Presentation );

    foreach(const QString & mimeType, mimeTypes)
    {
      if ( data->hasFormat( mimeType ) ) {
        paste = true;
        break;
      }
    }

  }

  m_editPaste->setEnabled(paste);
}

void View::focusInEvent(QFocusEvent * event)
{
  QWidget::focusInEvent(event);
  m_doc->viewManager()->viewHasFocus(this);
}

void View::canvasReceivedFocus()
{
  m_doc->viewManager()->viewHasFocus(this);
}

void View::sectionBoundingBoxChanged(const QRectF& bb) {
  QSizeF pageSize( 400, 400 );
  // compute the page size
  if(not bb.isNull() and not bb.isEmpty())
  {
    pageSize = QSizeF(bb.right(), bb.bottom());
  }
  m_zoomController->setPageSize( pageSize );
  m_zoomController->setDocumentSize( pageSize );
  m_canvas->updateOrigin();
}

#include "View.moc"

/* -*- C++ -*-

  $Id$

  This file is part of KIllustrator.
  Copyright (C) 1998-99 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)

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

#ifndef GDocument_h_
#define GDocument_h_

#include <vector>
#include <list>

#include <qobject.h>
#include <qlist.h>
#include <qstring.h>

#include <qdom.h>

#include "Painter.h"
#include "Handle.h"
#include "GObject.h"
#include "GLayer.h"

#include <koPageLayoutDia.h>

#define KILLUSTRATOR_MIMETYPE "application/x-killustrator"

#define XML_DOCTYPE "kiml"
#define XML_DTD     "kiml1.0.dtd"
#define UNNAMED_FILE "<unnamed>"

class GObject;

class GDocument : public QObject {
  Q_OBJECT
public:
  GDocument ();
  ~GDocument ();

  void initialize ();
  void setAutoUpdate (bool flag);

  const QString& fileName () const { return filename; }
  void setFileName (const char* s) { filename = s; }
  void setPaperSize (int width, int height);
  int getPaperWidth () const;
  int getPaperHeight () const;

  virtual void setModified (bool flag = true);
  bool isModified () const { return modifyFlag; }

  void drawContents (QPainter& p, bool withBasePoints = false,
                     bool outline = false);
  void drawContentsInRegion (QPainter& p, const Rect& r,
                             bool withBasePoints = false,
                             bool outline = false);

  void invalidateClipRegions ();

  /*
   * Layer management
   */

  // get an array with all layers of the document
  const std::vector<GLayer*>& getLayers ();

  // set the active layer where further actions take place
  void setActiveLayer (GLayer *layer);

  // retrieve the active layer
  GLayer* activeLayer ();

  // raise the given layer
  void raiseLayer (GLayer *layer);

  // lower the given layer
  void lowerLayer (GLayer *layer);

  // add a new layer on top of existing layers
  GLayer* addLayer ();

  // delete the given layer as well as all contained objects
  void deleteLayer (GLayer *layer);

  // return helpline layer
  GLayer* layerForHelplines ();
  bool helplineLayerIsActive ();

  void insertObject (GObject* obj);
  void selectObject (GObject* obj);
  void unselectObject (GObject* obj);
  void unselectAllObjects ();
  void selectAllObjects ();

  void selectNextObject ();
  void selectPrevObject ();

  GObject* lastObject () { return last; }
  void setLastObject (GObject* obj);

  std::list<GObject*>& getSelection () { return selection; }
  bool selectionIsEmpty () const { return selection.empty (); }
  unsigned int selectionCount () const { return selection.size (); }

  unsigned int objectCount () const;

  Rect boundingBoxForSelection ();
  Rect boundingBoxForAllObjects ();
  void deleteSelectedObjects ();
  void deleteObject (GObject* obj);

  GObject* findContainingObject (int x, int y);

  bool findNearestObject (const char* otype, int x, int y,
                          float max_dist, GObject*& obj, int& pidx,
                          bool all = false);

  bool findContainingObjects (int x, int y, QList<GObject>& olist);
  bool findObjectsContainedIn (const Rect& r, QList<GObject>& olist);

  QDomDocument saveToXml();
  bool readFromXml (const QDomDocument &document);
  bool insertFromXml (const QDomDocument &document, list<GObject*>& newObjs);

  Handle& handle () { return selHandle; }

  unsigned int findIndexOfObject (GObject *obj);
  void insertObjectAtIndex (GObject* obj, unsigned int idx);
  void moveObjectToIndex (GObject* obj, unsigned int idx);

  KoPageLayout pageLayout ();
  void setPageLayout (const KoPageLayout& layout);

  void setGrid (float dx, float dy, bool snap);
  void getGrid (float& dx, float& dy, bool& snap);

  void setHelplines (const std::vector<float>& hlines,
                     const std::vector<float>& vlines,
                     bool snap);
  void getHelplines (std::vector<float>& hlines, std::vector<float>& vlines,
                     bool& snap);

protected:
  void updateHandle ();
  bool parseBody (const QDomElement &element, std::list<GObject*>& newObjs, bool markNew);

public slots:
  void objectChanged ();
  void objectChanged (const Rect& r);
  void layerChanged ();
  void helplineStatusChanged ();

signals:
  void changed ();
  void changed (const Rect& r);
  void selectionChanged ();
  void sizeChanged ();
  void gridChanged ();

  void wasModified (bool flag);

protected:
  bool autoUpdate;
  bool modifyFlag;
  QString filename;
  int paperWidth, paperHeight; // pt
  std::vector<GLayer*> layers; // the array of all layers
  std::list<GObject*> selection;
  GLayer* active_layer;     // the current layer
  GObject *last;
  Handle selHandle;
  Rect selBox;
  bool selBoxIsValid;
  KoPageLayout pLayout;
  bool snapToGrid, snapToHelplines;
  float gridx, gridy;
  std::vector<float> hHelplines, vHelplines;
};

#endif

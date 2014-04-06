/*
 *  Copyright (c) 2014 Shivaraman Aiyer<sra392@gmail.com>
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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef _KIS_PERSPECTIVE_GRIDNG_H_
#define _KIS_PERSPECTIVE_GRIDNG_H_

#include <QString>
#include <QPointF>
#include <QRect>
#include <QFile>
#include <QObject>
#include <QMessageBox>
#include <QFileDialog>

#include <kis_doc2.h>
#include <krita_export.h>
#include <kis_shared.h>
#include <kio/job.h>
#include <kfiledialog.h>
#include <KoStore.h>
#include <kis_canvas2.h>

class QPainter;
class QRect;
class QRectF;
class KisCoordinatesConverter;
class KisDoc2;

#include "kis_abstract_perspective_grid.h"
#include <kis_shared_ptr.h>
#include <KoGenericRegistry.h>

class KisPerspectiveGridNgHandle;
typedef KisSharedPtr<KisPerspectiveGridNgHandle> KisPerspectiveGridNgHandleSP;
class KisPerspectiveGridNg;
class QPainterPath;

/**
  * Represent an handle of the assistant, used to edit the parameters
  * of an assistants. Handles can be shared between assistants.
  */
class KRITAUI_EXPORT KisPerspectiveGridNgHandle: public QPointF, public KisShared
{
    friend class KisPerspectiveGridNg;
public:
    KisPerspectiveGridNgHandle(double x, double y);
    explicit KisPerspectiveGridNgHandle(QPointF p);
    KisPerspectiveGridNgHandle(const KisPerspectiveGridNgHandle&);
    virtual ~KisPerspectiveGridNgHandle();
    void mergeWith(KisPerspectiveGridNgHandleSP);
    QList<KisPerspectiveGridNgHandleSP> split();
    void uncache();
    KisPerspectiveGridNgHandle& operator=(const QPointF&);
    void setType(char type);
    const char handleType() const ;
private:
    void registerAssistant(KisPerspectiveGridNg*);
    void unregisterAssistant(KisPerspectiveGridNg*);
    bool containsAssistant(KisPerspectiveGridNg*);
private:
    struct Private;
    Private* const d;
};

/**
 * A KisPerspectiveGridNg is an object that assist the drawing on the canvas.
 * With this class you can implement virtual equivalent to ruler or compas.
 */
class KRITAUI_EXPORT KisPerspectiveGridNg // : public KisAbstractPerspectiveGrid
{
public:
    KisPerspectiveGridNg();
    KisPerspectiveGridNg(const QString& id, const QString& name);
    ~KisPerspectiveGridNg();
    const QString& id() const;
    const QString& name() const;
    /**
     * Adjust the position given in parameter.
     * @param point the coordinates in point in the document reference
     * @param strokeBegin the coordinates of the beginning of the stroke
     */
    virtual QPointF adjustPosition(const QPointF& point, const QPointF& strokeBegin);
    virtual void endStroke();
    virtual QPointF buttonPosition() const;
    virtual int numHandles() const{ return 4; }
    void replaceHandle(KisPerspectiveGridNgHandleSP _handle, KisPerspectiveGridNgHandleSP _with);
    void addHandle(KisPerspectiveGridNgHandleSP handle);
    void addSideHandle(KisPerspectiveGridNgHandleSP handle);
    virtual void drawAssistant(QPainter& gc, const QRectF& updateRect, const KisCoordinatesConverter *converter, bool cached = true,KisCanvas2 *canvas=0);
    void uncache();
    const QList<KisPerspectiveGridNgHandleSP>& handles() const;
    QList<KisPerspectiveGridNgHandleSP> handles();
    const QList<KisPerspectiveGridNgHandleSP>& sideHandles() const;
    QList<KisPerspectiveGridNgHandleSP> sideHandles();
    QByteArray saveXml( QMap<KisPerspectiveGridNgHandleSP, int> &handleMap);
    void loadXml(KoStore *store, QMap<int, KisPerspectiveGridNgHandleSP> &handleMap, QString path);
    void saveXmlList(QDomDocument& doc, QDomElement& assistantsElement, int count);
    void findHandleLocation();
    KisPerspectiveGridNgHandleSP oppHandleOne();

    /**
      * Get the topLeft, bottomLeft, topRight and BottomRight corners of the assistant
      */
    const KisPerspectiveGridNgHandleSP topLeft() const;
    KisPerspectiveGridNgHandleSP topLeft();
    const KisPerspectiveGridNgHandleSP topRight() const;
    KisPerspectiveGridNgHandleSP topRight();
    const KisPerspectiveGridNgHandleSP bottomLeft() const;
    KisPerspectiveGridNgHandleSP bottomLeft();
    const KisPerspectiveGridNgHandleSP bottomRight() const;
    KisPerspectiveGridNgHandleSP bottomRight();
    const KisPerspectiveGridNgHandleSP topMiddle() const;
    KisPerspectiveGridNgHandleSP topMiddle();
    const KisPerspectiveGridNgHandleSP rightMiddle() const;
    KisPerspectiveGridNgHandleSP rightMiddle();
    const KisPerspectiveGridNgHandleSP leftMiddle() const;
    KisPerspectiveGridNgHandleSP leftMiddle();
    const KisPerspectiveGridNgHandleSP bottomMiddle() const;
    KisPerspectiveGridNgHandleSP bottomMiddle();
    const KisPerspectiveGridNgHandleSP vanishingPointX() const;
    KisPerspectiveGridNgHandleSP vanishingPointX();
    const KisPerspectiveGridNgHandleSP vanishingPointY() const;
    KisPerspectiveGridNgHandleSP vanishingPointY();
    const KisPerspectiveGridNgHandleSP vanishingPointZ() const;
    KisPerspectiveGridNgHandleSP vanishingPointZ();

public:
    /**
     * This will paint a path using a white and black colors.
     */
    static void drawPath(QPainter& painter, const QPainterPath& path);
    static void drawPath(QPainter& painter, const QPainterPath &path, QColor colour);
protected:
    virtual QRect boundingRect() const;
    virtual void drawCache(QPainter& gc, const KisCoordinatesConverter *converter);
    void initHandles(QList<KisPerspectiveGridNgHandleSP> _handles);
    QList<KisPerspectiveGridNgHandleSP> m_handles;
private:
    bool getTransform(QPolygonF& poly, QTransform& transform) const;
    QPointF project(const QPointF& pt, const QPointF& strokeBegin);
    // creates the convex hull, returns false if it's not a quadrilateral
    bool quad(QPolygonF& out) const;
    // finds the transform from perspective coordinates (a unit square) to the document
//    bool getTransform(QPolygonF& polyOut, QTransform& transformOut) const;

    // which direction to snap to (in transformed coordinates)
    QLineF m_snapLine;
    // cached information
    mutable QTransform m_cachedTransform;
    mutable QPolygonF m_cachedPolygon;
    mutable QPointF m_cachedPoints[4];
    mutable bool m_cacheValid;
    struct Private;
    Private* const d;
};

/**
 * Allow to create a painting assistant.
 */
class KRITAUI_EXPORT KisPerspectiveGridNgFactory
{
public:
    KisPerspectiveGridNgFactory();
    virtual ~KisPerspectiveGridNgFactory();
    QString id() const;
    QString name() const;
    virtual KisPerspectiveGridNg* createPerspectiveGridNg() const;

};

class KRITAUI_EXPORT KisPerspectiveGridNgFactoryRegistry : public KoGenericRegistry<KisPerspectiveGridNgFactory*>
{
    KisPerspectiveGridNgFactoryRegistry();
    ~KisPerspectiveGridNgFactoryRegistry();
  public:
    static KisPerspectiveGridNgFactoryRegistry* instance();

};

#endif

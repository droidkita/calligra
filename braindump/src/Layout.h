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

#ifndef _LAYOUT_H_
#define _LAYOUT_H_

#include <QObject>

class QSize;

class KoShape;

class Layout : public QObject {
  Q_OBJECT
  public:
    Layout(const QString& _id);
    virtual ~Layout();
  public:
    const QString& id() const;
  public:
    void addShape(KoShape* _shape);
    void removeShape(KoShape* _shape);
  protected:
    const QList<KoShape*>& shapes() const;
  protected:
    /**
     * This function is called when a shape is added to the layout.
     */
    virtual void shapeAdded(KoShape* _shape) = 0;
    /**
     * This function is called when a shape is removed to the layout.
     */
    virtual void shapeRemoved(KoShape* _shape) = 0;
    /**
     * This function is called when a shape geometry is changed.
     */
    virtual void shapeGeometryChanged(KoShape* _shape) = 0;
  signals:
    void sizedChange( const QSize& _size );
  private:
    struct Private;
    Private* const d;
};

#endif

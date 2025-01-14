/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KSPREAD_RTREE
#define KSPREAD_RTREE

#include <KoRTree.h>

#include "calligra_sheets_limits.h"
#include "Region.h"

// Use dynamic_cast instead of cached root node
// this is much slower but it is here so it is easy to check that still all works.
//#define DYNAMIC_CAST

namespace Calligra
{
namespace Sheets
{

/**
 * \class RTree
 * \brief An R-Tree template
 * \ingroup Storage
 *
 * An R-Tree template extended by special needs of Calligra Sheets:
 * \li adjusts the rectangles on insertion to avoid unwanted overlapping
 * (caused by different intersection/containment behaviour of QRectF and QRect)
 * \li checks for sane rectangle dimensions
 * \li provides insertion and deletion of columns and rows
 *
 * \author Stefan Nikolaus <stefan.nikolaus@kdemail.net>
 */
template<typename T>
class RTree : public KoRTree<T>
{
public:

    /**
     * Constructs an empty R-Tree.
     */
    RTree();

    /**
     * Destroys the whole R-Tree.
     */
    ~RTree() override;

    /**
     * @brief Insert data item into the tree
     *
     * This will insert a data item into the tree. If necessary the tree will
     * adjust itself.
     *
     * \note Reimplemented for Calligra Sheets, because of the QRectF behaviour differs from
     * the one of QRect. Intersection or containment for boundary lines or points is
     * not the same, e.g. QRectF(1, 1, 1, 1) intersects QRectF(2, 1, 1, 1) while for
     * QRect it does not. Therefore, this method subtracts 0.1 from the width and
     * height of \p rect .
     *
     * @param data
     * @param rect
     */
    void insert(const QRectF& rect, const T& data) override;

    void load(const QList<QPair<Region, T> >& data);

    void remove(const QRectF& rect, const T& data, int id = -1);

    /**
     * Finds all data items at the location \p point .
     *
     * \param point where the objects have to be in
     *
     * \return objects at the location
     */
    virtual QList<T> contains(const QPointF& point) const;

    /**
     * Finds all data items that cover \p rect completely.
     *
     * \param rect where the objects have to be in
     *
     * \return objects containing the rect
     */
    virtual QList<T> contains(const QRectF& rect) const;

    /**
     * @brief Find all data items which intersects rect
     *
     * \note Reimplemented for Calligra Sheets, because of the QRectF behaviour differs from
     * the one of QRect. Intersection or containment for boundary lines or points is
     * not the same, e.g. QRectF(1, 1, 1, 1) intersects QRectF(2, 1, 1, 1) while for
     * QRect it does not. Therefore, this method subtracts 0.1 from the width and
     * height of \p rect .
     *
     * @param rect where the objects have to be in
     *
     * @return objects intersecting the rect
     */
    QList<T> intersects(const QRectF& rect) const override;

    virtual QMap<int, QPair<QRectF, T> > intersectingPairs(const QRectF& rect) const;

    /**
     * Inserts \p number rows at the position \p position .
     * It extends or shifts rectangles, respectively.
     * \return the removed rectangle/data pairs
     */
    virtual QVector< QPair<QRectF, T> > insertRows(int position, int number);

    /**
     * Inserts \p number columns at the position \p position .
     * It extends or shifts rectangles, respectively.
     * \return the removed rectangle/data pairs
     */
    virtual QVector< QPair<QRectF, T> > insertColumns(int position, int number);

    /**
     * Deletes \p number rows at the position \p position .
     * It shrinks or shifts rectangles, respectively.
     * \return the removed rectangle/data pairs
     */
    virtual QVector< QPair<QRectF, T> > removeRows(int position, int number);

    /**
     * Deletes \p number columns at the position \p position .
     * It shrinks or shifts rectangles, respectively.
     * \return the removed rectangle/data pairs
     */
    virtual QVector< QPair<QRectF, T> > removeColumns(int position, int number);

    /**
     * Shifts the rows right of \p rect to the right by the width of \p rect .
     * It extends or shifts rectangles, respectively.
     * \return the former rectangle/data pairs
     */
    virtual QVector< QPair<QRectF, T> > insertShiftRight(const QRect& rect);

    /**
     * Shifts the columns at the bottom of \p rect to the bottom by the height of \p rect .
     * It extends or shifts rectangles, respectively.
     * \return the former rectangle/data pairs
     */
    virtual QVector< QPair<QRectF, T> > insertShiftDown(const QRect& rect);

    /**
     * Shifts the rows left of \p rect to the left by the width of \p rect .
     * It shrinks or shifts rectangles, respectively.
     * \return the former rectangle/data pairs
     */
    virtual QVector< QPair<QRectF, T> > removeShiftLeft(const QRect& rect);

    /**
     * Shifts the columns on top of \p rect to the top by the height of \p rect .
     * It shrinks or shifts rectangles, respectively.
     * \return the former rectangle/data pairs
     */
    virtual QVector< QPair<QRectF, T> > removeShiftUp(const QRect& rect);

    /**
     * Assignment.
     */
    void operator=(const RTree& other);

    /**
     * Returns the bounding box for the entire tree.
     */
    QRectF boundingBox() const { return KoRTree<T>::m_root->boundingBox(); }

    void clear() override {
        KoRTree<T>::clear();
        m_castRoot = dynamic_cast<Node*>(this->m_root);
    }

protected:
    class Node;
    class NonLeafNode;
    class LeafNode;

    // factory methods
    LeafNode* createLeafNode(int capacity, int level, typename KoRTree<T>::Node * parent) override {
        return new LeafNode(capacity, level, dynamic_cast<Node*>(parent));
    }
    NonLeafNode* createNonLeafNode(int capacity, int level, typename KoRTree<T>::Node * parent) override {
        return new NonLeafNode(capacity, level, dynamic_cast<Node*>(parent));
    }

    void adjustTree(typename KoRTree<T>::Node *node1, typename KoRTree<T>::Node *node2) override {
        KoRTree<T>::adjustTree(node1, node2);
        m_castRoot = dynamic_cast<Node*>(this->m_root);
    }

    void condenseTree(typename KoRTree<T>::Node * node, QVector<typename KoRTree<T>::Node *> & reinsert) override {
        KoRTree<T>::condenseTree(node, reinsert);
        m_castRoot = dynamic_cast<Node*>(this->m_root);
    }

private:
    // disable copy constructor
    RTree(const RTree& other);

    struct LoadData {
        QRect rect;
        const T* data;
        qreal value;
        LoadData(const QRect& r, const T* d, qreal v)
            : rect(r), data(d), value(v)
        {}
    };
    struct LoadDataIndexCompare {
        const QList<LoadData>& m_data;
        LoadDataIndexCompare(const QList<LoadData>& data) : m_data(data) {}
        bool operator()(int a, int b) {
            return m_data[a].value < m_data[b].value;
        }
    };
    struct NodeLoadDataIndexCompare {
        const QList<QPair<Node*, qreal> >& m_data;
        NodeLoadDataIndexCompare(const QList<QPair<Node*, qreal> >& data) : m_data(data) {}
        bool operator()(int a, int b) {
            return m_data[a].second < m_data[b].second;
        }
    };

    Node* m_castRoot;
};

/**
 * Abstract base class for nodes and leaves.
 */
template<typename T>
class RTree<T>::Node : virtual public KoRTree<T>::Node
{
public:
    Node(int capacity, int level, Node * parent)
            : KoRTree<T>::Node(capacity, level, parent) {}
    ~Node() override {}

    void remove(int index) override {
        KoRTree<T>::Node::remove(index);
    }
    virtual void remove(const QRectF& rect, const T& data, int id = -1) = 0;
    void contains(const QPointF & point, QMap<int, T> & result) const override = 0;
    virtual void contains(const QRectF& rect, QMap<int, T>& result) const = 0;
    virtual void intersectingPairs(const QRectF& rect, QMap<int, QPair<QRectF, T> >& result) const = 0;
    virtual QMap< int, QPair<QRectF, T> > insertRows(int position, int number) = 0;
    virtual QMap< int, QPair<QRectF, T> > insertColumns(int position, int number) = 0;
    virtual QMap< int, QPair<QRectF, T> > removeRows(int position, int number) = 0;
    virtual QMap< int, QPair<QRectF, T> > removeColumns(int position, int number) = 0;
    const QRectF& childBoundingBox(int index) const override {
        return KoRTree<T>::Node::childBoundingBox(index);
    }
    QVector<QRectF> childBoundingBox() const {
        return this->m_childBoundingBox;
    }
private:
    // disable copy constructor
    Node(const Node& other);
};

/**
 * An R-Tree leaf.
 */
template<typename T>
class RTree<T>::LeafNode : public RTree<T>::Node, public KoRTree<T>::LeafNode
{
public:
    LeafNode(int capacity, int level, RTree<T>::Node * parent)
            : KoRTree<T>::Node(capacity, level, parent)
            , RTree<T>::Node(capacity, level, parent)
            , KoRTree<T>::LeafNode(capacity, level, parent) {}
    ~LeafNode() override {}

    void remove(int index) override {
        KoRTree<T>::LeafNode::remove(index);
    }
    void remove(const T& data) override {
        KoRTree<T>::LeafNode::remove(data);
    }
    void remove(const QRectF& rect, const T& data, int id = -1) override;
    void contains(const QPointF & point, QMap<int, T> & result) const override {
        KoRTree<T>::LeafNode::contains(point, result);
    }
    void contains(const QRectF& rect, QMap<int, T>& result) const override;
    void intersectingPairs(const QRectF& rect, QMap<int, QPair<QRectF, T> >& result) const override;
    QMap< int, QPair<QRectF, T> > insertRows(int position, int number) override;
    QMap< int, QPair<QRectF, T> > insertColumns(int position, int number) override;
    QMap< int, QPair<QRectF, T> > removeRows(int position, int number) override;
    QMap< int, QPair<QRectF, T> > removeColumns(int position, int number) override;
    virtual void operator=(const LeafNode& other);
private:
    // disable copy constructor
    LeafNode(const LeafNode& other);
};

/**
 * An R-Tree node.
 */
template<typename T>
class RTree<T>::NonLeafNode : public RTree<T>::Node, public KoRTree<T>::NonLeafNode
{
public:
    NonLeafNode(int capacity, int level, RTree<T>::Node * parent)
            : KoRTree<T>::Node(capacity, level, parent)
            , RTree<T>::Node(capacity, level, parent)
            , KoRTree<T>::NonLeafNode(capacity, level, parent) {}
    ~NonLeafNode() override {}

    void remove(int index) override {
        KoRTree<T>::NonLeafNode::remove(index);
    }
    void remove(const QRectF& rect, const T& data, int id = -1) override;
    void contains(const QPointF & point, QMap<int, T> & result) const override {
        KoRTree<T>::NonLeafNode::contains(point, result);
    }
    void contains(const QRectF& rect, QMap<int, T>& result) const override;
    void intersectingPairs(const QRectF& rect, QMap<int, QPair<QRectF, T> >& result) const override;
    QMap< int, QPair<QRectF, T> > insertRows(int position, int number) override;
    QMap< int, QPair<QRectF, T> > insertColumns(int position, int number) override;
    QMap< int, QPair<QRectF, T> > removeRows(int position, int number) override;
    QMap< int, QPair<QRectF, T> > removeColumns(int position, int number) override;
    virtual void operator=(const NonLeafNode& other);
private:
    // disable copy constructor
    NonLeafNode(const NonLeafNode& other);
};


/////////////////////////////////////////////////////////////////////////////
// RTree definition
//
template<typename T>
RTree<T>::RTree()
        : KoRTree<T>(8, 4)
{
    delete this->m_root;
    this->m_root = new LeafNode(this->m_capacity + 1, 0, 0);
    m_castRoot = dynamic_cast<Node*>(this->m_root);
}

template<typename T>
RTree<T>::~RTree()
{
}

template<typename T>
void RTree<T>::insert(const QRectF& rect, const T& data)
{
    Q_ASSERT(rect.x()      - (int)rect.x()      == 0.0);
    Q_ASSERT(rect.y()      - (int)rect.y()      == 0.0);
    Q_ASSERT(rect.height() - (int)rect.height() == 0.0);
    Q_ASSERT(rect.width()  - (int)rect.width()  == 0.0);
    KoRTree<T>::insert(rect.normalized().adjusted(0, 0, -0.1, -0.1), data);
}

static inline qreal calcLoadingRectValue(const QRectF& r)
{
    QPointF center = r.center();
    // TODO: better value would be hilbert value of center of rect
    return center.x();
}

template<typename T>
void RTree<T>::load(const QList<QPair<Region, T> >& data)
{
    // clear current tree
    clear();

    // make rect->data mapping
    typedef QPair<Region, T> DataRegion;
    
    QList<LoadData> rectData;
    QVector<int> indices;
    for (const DataRegion& dataRegion : data) {
        for (const QRect& rect : dataRegion.first.rects()) {
            qreal h = calcLoadingRectValue(rect);
            rectData.append(LoadData(rect, &dataRegion.second, h));
            indices.append(indices.size());
        }
    }

    std::sort(indices.begin(), indices.end(), LoadDataIndexCompare(rectData));

    QList<QPair<Node*, qreal> > nodes;
    // create LeafNodes
    for (int i = 0; i < indices.size(); i += KoRTree<T>::m_capacity) {
        LeafNode* n = createLeafNode(KoRTree<T>::m_capacity + 1, 0, 0);
        for (int j = 0; j < KoRTree<T>::m_capacity && i+j < indices.size(); j++) {
            const LoadData& d = rectData[indices[i+j]];
            n->insert(QRectF(d.rect).normalized().adjusted(0, 0, -0.1, -0.1), *d.data, LeafNode::dataIdCounter + indices[i+j]);
        }
        n->updateBoundingBox();
        nodes.append(qMakePair<Node*, qreal>(n, calcLoadingRectValue(n->boundingBox())));
    }
    LeafNode::dataIdCounter += indices.size();

    while (nodes.size() > 1) {
        indices.resize(nodes.size());
        for (int i = 0; i < indices.size(); i++) indices[i] = i;

        std::sort(indices.begin(), indices.end(), NodeLoadDataIndexCompare(nodes));

        QList<QPair<Node*, qreal> > newNodes;

        for (int i = 0; i < indices.size(); i += KoRTree<T>::m_capacity) {
            NonLeafNode* n = createNonLeafNode(KoRTree<T>::m_capacity + 1, 0, 0);
            for (int j = 0; j < KoRTree<T>::m_capacity && i+j < indices.size(); j++) {
                Node* oldNode = nodes[indices[i+j]].first;
                n->insert(oldNode->boundingBox(), oldNode);
            }
            n->updateBoundingBox();
            newNodes.append(qMakePair<Node*, qreal>(n, calcLoadingRectValue(n->boundingBox())));
        }
        nodes = newNodes;
    }

    if (!nodes.isEmpty()) {
        // set root node
        delete KoRTree<T>::m_root;
        KoRTree<T>::m_root = nodes.first().first;
        m_castRoot = dynamic_cast<Node*>(this->m_root);
    }
}

template<typename T>
void RTree<T>::remove(const QRectF& rect, const T& data, int id)
{
    Q_ASSERT(rect.x()      - (int)rect.x()      == 0.0);
    Q_ASSERT(rect.y()      - (int)rect.y()      == 0.0);
    Q_ASSERT(rect.height() - (int)rect.height() == 0.0);
    Q_ASSERT(rect.width()  - (int)rect.width()  == 0.0);
#ifdef DYNAMIC_CAST
    dynamic_cast<Node*>(this->m_root)->remove(rect.normalized().adjusted(0, 0, -0.1, -0.1), data, id);
#else
    m_castRoot->remove(rect.normalized().adjusted(0, 0, -0.1, -0.1), data, id);
#endif
}

template<typename T>
QList<T> RTree<T>::contains(const QPointF& point) const
{
    return KoRTree<T>::contains(point);
}

template<typename T>
QList<T> RTree<T>::contains(const QRectF& rect) const
{
    Q_ASSERT(rect.x()      - (int)rect.x()      == 0.0);
    Q_ASSERT(rect.y()      - (int)rect.y()      == 0.0);
    Q_ASSERT(rect.height() - (int)rect.height() == 0.0);
    Q_ASSERT(rect.width()  - (int)rect.width()  == 0.0);
    QMap<int, T> result;
#ifdef DYNAMIC_CAST
    dynamic_cast<Node*>(this->m_root)->contains(rect.normalized().adjusted(0, 0, -0.1, -0.1), result);
#else
    m_castRoot->contains(rect.normalized().adjusted(0, 0, -0.1, -0.1), result);
#endif
    return result.values();
}

template<typename T>
QList<T> RTree<T>::intersects(const QRectF& rect) const
{
    Q_ASSERT(rect.x()      - (int)rect.x()      == 0.0);
    Q_ASSERT(rect.y()      - (int)rect.y()      == 0.0);
    Q_ASSERT(rect.height() - (int)rect.height() == 0.0);
    Q_ASSERT(rect.width()  - (int)rect.width()  == 0.0);
    return KoRTree<T>::intersects(rect.normalized().adjusted(0, 0, -0.1, -0.1));
}

template<typename T>
QMap<int, QPair<QRectF, T> > RTree<T>::intersectingPairs(const QRectF& rect) const
{
    Q_ASSERT(rect.x()      - (int)rect.x()      == 0.0);
    Q_ASSERT(rect.y()      - (int)rect.y()      == 0.0);
    Q_ASSERT(rect.height() - (int)rect.height() == 0.0);
    Q_ASSERT(rect.width()  - (int)rect.width()  == 0.0);
    QMap<int, QPair<QRectF, T> > result;
#ifdef DYNAMIC_CAST
    dynamic_cast<Node*>(this->m_root)->intersectingPairs(rect.normalized().adjusted(0, 0, -0.1, -0.1), result);
#else
    m_castRoot->intersectingPairs(rect.normalized().adjusted(0, 0, -0.1, -0.1), result);
#endif
    return result;
}

template<typename T>
QVector< QPair<QRectF, T> > RTree<T>::insertRows(int position, int number)
{
    Q_ASSERT(position >= 1);
    Q_ASSERT(position <= KS_rowMax);
    if (position < 1 || position > KS_rowMax)
        return QVector< QPair<QRectF, T> >();
#ifdef DYNAMIC_CAST
    return dynamic_cast<Node*>(this->m_root)->insertRows(position, number).values().toVector();
#else
    return m_castRoot->insertRows(position, number).values().toVector();
#endif
}

template<typename T>
QVector< QPair<QRectF, T> > RTree<T>::insertColumns(int position, int number)
{
    Q_ASSERT(position >= 1);
    Q_ASSERT(position <= KS_colMax);
    if (position < 1 || position > KS_colMax)
        return QVector< QPair<QRectF, T> >();
#ifdef DYNAMIC_CAST
    return dynamic_cast<Node*>(this->m_root)->insertColumns(position, number).values().toVector();
#else
    return m_castRoot->insertColumns(position, number).values().toVector();
#endif
}

template<typename T>
QVector< QPair<QRectF, T> > RTree<T>::removeRows(int position, int number)
{
    Q_ASSERT(position >= 1);
    Q_ASSERT(position <= KS_rowMax);
    if (position < 1 || position > KS_rowMax)
        return QVector< QPair<QRectF, T> >();
#ifdef DYNAMIC_CAST
    return dynamic_cast<Node*>(this->m_root)->removeRows(position, number).values().toVector();
#else
    return m_castRoot->removeRows(position, number).values().toVector();
#endif
}

template<typename T>
QVector< QPair<QRectF, T> > RTree<T>::removeColumns(int position, int number)
{
    Q_ASSERT(position >= 1);
    Q_ASSERT(position <= KS_colMax);
    if (position < 1 || position > KS_colMax)
        return QVector< QPair<QRectF, T> >();
#ifdef DYNAMIC_CAST
    return dynamic_cast<Node*>(this->m_root)->removeColumns(position, number).values().toVector();
#else
    return m_castRoot->removeColumns(position, number).values().toVector();
#endif
}

template<typename T>
QVector< QPair<QRectF, T> > RTree<T>::insertShiftRight(const QRect& r)
{
    const QRect rect(r.normalized());
    if (rect.left() < 1 || rect.left() > KS_colMax)
        return QVector< QPair<QRectF, T> >();
    const QRect boundingRect = QRect(rect.topLeft(), QPoint(KS_colMax, rect.bottom()));
    const QVector< QPair<QRectF, T> > oldPairs = intersectingPairs(boundingRect).values().toVector();
    if (oldPairs.isEmpty())
        return QVector< QPair<QRectF, T> >();
    // insert default data at the bounding rectangle
    insert(boundingRect, T());
    // fill the inserted rectangle
    const QRect copyRect = QRect(rect.left(), rect.top(), 1, rect.height());
    const QVector< QPair<QRectF, T> > copyPairs = intersectingPairs(copyRect).values().toVector();
    for (int i = 0; i < copyPairs.count(); ++i) {
        insert((copyPairs[i].first.toRect() & copyRect).adjusted(0, 0, rect.width() - 1, 0), copyPairs[i].second);
    }
    // insert the data at the shifted rectangles
    for (int i = 0; i < oldPairs.count(); ++i) {
        const QRect shiftedRect = oldPairs[i].first.toRect().adjusted(rect.width(), 0, rect.width(), 0);
        insert(shiftedRect & boundingRect, oldPairs[i].second);
    }
    return oldPairs;
}

template<typename T>
QVector< QPair<QRectF, T> > RTree<T>::insertShiftDown(const QRect& r)
{
    const QRect rect(r.normalized());
    if (rect.top() < 1 || rect.top() > KS_rowMax)
        return QVector< QPair<QRectF, T> >();
    const QRect boundingRect = QRect(rect.topLeft(), QPoint(rect.right(), KS_rowMax));
    const QVector< QPair<QRectF, T> > oldPairs = intersectingPairs(boundingRect).values().toVector();
    if (oldPairs.isEmpty())
        return QVector< QPair<QRectF, T> >();
    // insert default data at the bounding rectangle
    insert(boundingRect, T());
    // fill the inserted rectangle
    const QRect copyRect = QRect(rect.left(), rect.top(), rect.width(), 1);
    const QVector< QPair<QRectF, T> > copyPairs = intersectingPairs(copyRect).values().toVector();
    for (int i = 0; i < copyPairs.count(); ++i) {
        insert((copyPairs[i].first.toRect() & copyRect).adjusted(0, 0, 0, rect.height() - 1), copyPairs[i].second);
    }
    // insert the data at the shifted rectangles
    for (int i = 0; i < oldPairs.count(); ++i) {
        const QRect shiftedRect = oldPairs[i].first.toRect().adjusted(0, rect.height(), 0, rect.height());
        insert(shiftedRect & boundingRect, oldPairs[i].second);
    }
    return oldPairs;
}

template<typename T>
QVector< QPair<QRectF, T> > RTree<T>::removeShiftLeft(const QRect& r)
{
    const QRect rect(r.normalized());
    if (rect.left() < 1 || rect.left() > KS_colMax)
        return QVector< QPair<QRectF, T> >();
    const QRect boundingRect = QRect(rect.topLeft(), QPoint(KS_colMax, rect.bottom()));
    const QVector< QPair<QRectF, T> > oldPairs = intersectingPairs(boundingRect).values().toVector();
    if (oldPairs.isEmpty())
        return QVector< QPair<QRectF, T> >();
    // insert default data at the bounding rectangle
    insert(boundingRect, T());
    // insert the data at the shifted rectangles
    for (int i = 0; i < oldPairs.count(); ++i) {
        const QRect shiftedRect = oldPairs[i].first.toRect().adjusted(-rect.width(), 0, -rect.width(), 0);
        insert(shiftedRect & boundingRect, oldPairs[i].second);
    }
    return oldPairs;
}

template<typename T>
QVector< QPair<QRectF, T> > RTree<T>::removeShiftUp(const QRect& r)
{
    const QRect rect(r.normalized());
    if (rect.top() < 1 || rect.top() > KS_rowMax)
        return QVector< QPair<QRectF, T> >();
    const QRect boundingRect = QRect(rect.topLeft(), QPoint(rect.right(), KS_rowMax));
    const QVector< QPair<QRectF, T> > oldPairs = intersectingPairs(boundingRect).values().toVector();
    if (oldPairs.isEmpty())
        return QVector< QPair<QRectF, T> >();
    // insert default data at the bounding rectangle
    insert(boundingRect, T());
    // insert the data at the shifted rectangles
    for (int i = 0; i < oldPairs.count(); ++i) {
        const QRect shiftedRect = oldPairs[i].first.toRect().adjusted(0, -rect.height(), 0, -rect.height());
        insert(shiftedRect & boundingRect, oldPairs[i].second);
    }
    return oldPairs;
}

template<typename T>
void RTree<T>::operator=(const RTree<T>& other)
{
    this->m_capacity = other.m_capacity;
    this->m_minimum = other.m_minimum;
    delete this->m_root;
    if (other.m_root->isLeaf()) {
        this->m_root = new LeafNode(this->m_capacity + 1, 0, 0);
        *dynamic_cast<LeafNode*>(this->m_root) = *dynamic_cast<LeafNode*>(other.m_root);
    } else {
        this->m_root = new NonLeafNode(this->m_capacity + 1, 0, 0);
        *dynamic_cast<NonLeafNode*>(this->m_root) = *dynamic_cast<NonLeafNode*>(other.m_root);
    }
    m_castRoot = dynamic_cast<Node*>(this->m_root);
}

/////////////////////////////////////////////////////////////////////////////
// RTree<T>::LeafNode definition
//
template<typename T>
void RTree<T>::LeafNode::remove(const QRectF& rect, const T& data, int id)
{
    for (int i = 0; i < this->m_counter; ++i) {
        if (this->m_childBoundingBox[i] == rect && this->m_data[i] == data && (id == -1 || this->m_dataIds[i] == id)) {
            //qDebug() << "LeafNode::remove id" << i;
            KoRTree<T>::LeafNode::remove(i);
            break;
        }
    }
}

template<typename T>
void RTree<T>::LeafNode::contains(const QRectF& rect, QMap<int, T>& result) const
{
    for (int i = 0; i < this->m_counter; ++i) {
        if (this->m_childBoundingBox[i].contains(rect)) {
            result.insert(this->m_dataIds[i], this->m_data[i]);
        }
    }
}

template<typename T>
void RTree<T>::LeafNode::intersectingPairs(const QRectF& rect, QMap<int, QPair<QRectF, T> >& result) const
{
    for (int i = 0; i < this->m_counter; ++i) {
        if (this->m_childBoundingBox[i].intersects(rect)) {
            QRectF rect = this->m_childBoundingBox[i].adjusted(0, 0, 0.1, 0.1);
            result.insert(this->m_dataIds[i], qMakePair(rect, this->m_data[i]));
        }
    }
}

template<typename T>
QMap< int, QPair<QRectF, T> > RTree<T>::LeafNode::insertRows(int position, int number)
{
    if (position > this->m_boundingBox.bottom())
        return QMap< int, QPair<QRectF, T> >();

    QMap< int, QPair<QRectF, T> > result;

    int shift = 0, endShift = number;
    // Don't process complete columns.
    if (this->m_boundingBox.top() != 1 || this->m_boundingBox.bottom() != KS_rowMax) {
        if (position < this->m_boundingBox.top())
            shift = number;
        if (position < this->m_boundingBox.toRect().bottom())
            endShift = number;
        else
            endShift = 0;
        this->m_boundingBox.adjust(0, shift, 0, endShift);
    }

    for (int i = 0; i < this->childCount(); ++i) {
        // Don't process complete columns.
        if (this->m_childBoundingBox[i].top() == 1 && this->m_childBoundingBox[i].bottom() == KS_rowMax)
            continue;

        if (position < this->m_childBoundingBox[i].top())
            shift = number;
        else
            shift = 0;

        if (position < this->m_childBoundingBox[i].toRect().bottom())
            endShift = number;
        else
            endShift = 0;
        this->m_childBoundingBox[i].adjust(0, shift, 0, endShift);
    }

    return QMap< int, QPair<QRectF, T> >(); // FIXME
}

template<typename T>
QMap< int, QPair<QRectF, T> > RTree<T>::LeafNode::insertColumns(int position, int number)
{
    if (position > this->m_boundingBox.right())
        return QMap< int, QPair<QRectF, T> >();

    QMap< int, QPair<QRectF, T> > result;

    int shift = 0;
    // Don't process complete rows.
    if (this->m_boundingBox.left() != 1 || this->m_boundingBox.right() != KS_colMax) {
        if (position < this->m_boundingBox.left())
            shift = number;
        this->m_boundingBox.adjust(shift, 0, number, 0);
    }

    for (int i = 0; i < this->childCount(); ++i) {
        // Don't process complete rows.
        if (this->m_childBoundingBox[i].left() == 1 && this->m_childBoundingBox[i].right() == KS_rowMax)
            continue;

        if (position < this->m_childBoundingBox[i].left())
            shift = number;
        else
            shift = 0;
        this->m_childBoundingBox[i].adjust(shift, 0, number, 0);
    }

    return QMap< int, QPair<QRectF, T> >(); // FIXME
}

template<typename T>
QMap< int, QPair<QRectF, T> > RTree<T>::LeafNode::removeRows(int position, int number)
{
    if (position > this->m_boundingBox.bottom())
        return QMap< int, QPair<QRectF, T> >();

    QMap< int, QPair<QRectF, T> > removedPairs;

    QRect rect = this->m_boundingBox.toRect();
    int shift = 0;
    int cut = 0;
    // Don't process complete columns.
    if (this->m_boundingBox.top() != 1 || this->m_boundingBox.bottom() != KS_rowMax) {
        if (position < rect.top()) {
            shift = qMin(rect.top() - position, number);
            cut = qMax(0, position + number - rect.top());
        } else {
            shift = 0;
            cut = qMin(number, rect.bottom() - position + 1);
        }
        this->m_boundingBox.adjust(0, -shift, 0, -shift - cut);
    }

    for (int i = 0; i < this->childCount(); ++i) {
        // Don't process complete columns.
        if (this->m_childBoundingBox[i].top() == 1 && this->m_childBoundingBox[i].bottom() == KS_rowMax)
            continue;

        const QRectF oldRect(this->m_childBoundingBox[ i ]);
        rect = this->m_childBoundingBox[i].toRect();
        if (position < rect.top()) {
            shift = qMin(rect.top() - position, number);
            cut = qMax(0, position + number - rect.top());
        } else {
            shift = 0;
            cut = qMin(number, rect.bottom() - position + 1);
        }
        this->m_childBoundingBox[i].adjust(0, -shift, 0, -shift - cut);

        if (this->m_childBoundingBox[ i ].isEmpty()) {
            removedPairs.insert(this->m_dataIds[i], qMakePair(oldRect, this->m_data[i]));
            KoRTree<T>::LeafNode::remove(i--);
        }
    }
    return removedPairs;
}

template<typename T>
QMap< int, QPair<QRectF, T> > RTree<T>::LeafNode::removeColumns(int position, int number)
{
    if (position > this->m_boundingBox.right())
        return QMap< int, QPair<QRectF, T> >();

    QMap< int, QPair<QRectF, T> > removedPairs;

    QRect rect = this->m_boundingBox.toRect();
    int shift = 0;
    int cut = 0;
    // Don't process complete rows.
    if (this->m_boundingBox.left() != 1 || this->m_boundingBox.right() != KS_colMax) {
        if (position < rect.left()) {
            shift = qMin(rect.left() - position, number);
            cut = qMax(0, position + number - rect.left());
        } else {
            shift = 0;
            cut = qMin(number, rect.right() - position + 1);
        }
        this->m_boundingBox.adjust(-shift, 0, -shift - cut, 0);
    }

    for (int i = 0; i < this->childCount(); ++i) {
        // Don't process complete rows.
        if (this->m_childBoundingBox[i].left() == 1 && this->m_childBoundingBox[i].right() == KS_rowMax)
            continue;

        const QRectF oldRect(this->m_childBoundingBox[ i ]);
        rect = this->m_childBoundingBox[i].toRect();
        if (position < rect.left()) {
            shift = qMin(rect.left() - position, number);
            cut = qMax(0, position + number - rect.left());
        } else {
            shift = 0;
            cut = qMin(number, rect.right() - position + 1);
        }
        this->m_childBoundingBox[i].adjust(-shift, 0, -shift - cut, 0);

        if (this->m_childBoundingBox[ i ].isEmpty()) {
            removedPairs.insert(this->m_dataIds[i], qMakePair(oldRect, this->m_data[i]));
            KoRTree<T>::LeafNode::remove(i--);
        }
    }
    return removedPairs;
}

template<typename T>
void RTree<T>::LeafNode::operator=(const LeafNode& other)
{
    // leave alone the m_parent
    this->m_boundingBox = other.m_boundingBox;
    this->m_childBoundingBox = other.m_childBoundingBox;
    this->m_counter = other.m_counter;
    this->m_place = other.m_place;
#ifdef CALLIGRA_RTREE_DEBUG
    this->m_nodeId = other.m_nodeId;
#endif
    this->m_level = other.m_level;
    this->m_data = other.m_data;
    this->m_dataIds = other.m_dataIds;
}

/////////////////////////////////////////////////////////////////////////////
// RTree<T>::NonLeafNode definition
//
template<typename T>
void RTree<T>::NonLeafNode::remove(const QRectF& rect, const T& data, int id)
{
    for (int i = 0; i < this->m_counter; ++i) {
        if (this->m_childBoundingBox[i].contains(rect)) {
            dynamic_cast<Node*>(this->m_childs[i])->remove(rect, data, id);
        }
    }
}

template<typename T>
void RTree<T>::NonLeafNode::contains(const QRectF& rect, QMap<int, T>& result) const
{
    for (int i = 0; i < this->m_counter; ++i) {
        if (this->m_childBoundingBox[i].intersects(rect)) {
            this->m_childs[i]->intersects(rect, result);
        }
    }
}

template<typename T>
void RTree<T>::NonLeafNode::intersectingPairs(const QRectF& rect, QMap<int, QPair<QRectF, T> >& result) const
{
    for (int i = 0; i < this->m_counter; ++i) {
        if (this->m_childBoundingBox[i].intersects(rect)) {
            dynamic_cast<Node*>(this->m_childs[i])->intersectingPairs(rect, result);
        }
    }
}

template<typename T>
QMap< int, QPair<QRectF, T> > RTree<T>::NonLeafNode::insertRows(int position, int number)
{
    if (position > this->m_boundingBox.bottom())
        return QMap< int, QPair<QRectF, T> >();

    QMap< int, QPair<QRectF, T> > result;

    for (int i = 0; i < this->childCount(); ++i) {
        this->m_childBoundingBox[i].adjust(0, (position < this->m_childBoundingBox[i].top()) ? number : 0, 0, number);
        result.unite(dynamic_cast<Node*>(this->m_childs[i])->insertRows(position, number));
    }

    // position < m_rect.top() ? shift : extend
    this->m_boundingBox.adjust(0, (position < this->m_boundingBox.top()) ? number : 0, 0, number);
    return QMap< int, QPair<QRectF, T> >(); // FIXME
}

template<typename T>
QMap< int, QPair<QRectF, T> > RTree<T>::NonLeafNode::insertColumns(int position, int number)
{
    if (position > this->m_boundingBox.right())
        return QMap< int, QPair<QRectF, T> >();

    QMap< int, QPair<QRectF, T> > result;

    for (int i = 0; i < this->childCount(); ++i) {
        this->m_childBoundingBox[i].adjust((position < this->m_childBoundingBox[i].left()) ? number : 0, 0, number, 0);
        result.unite(dynamic_cast<Node*>(this->m_childs[i])->insertColumns(position, number));
    }

    // position < m_rect.left() ? shift : extend
    this->m_boundingBox.adjust((position < this->m_boundingBox.left()) ? number : 0, 0, number, 0);
    return QMap< int, QPair<QRectF, T> >(); // FIXME
}

template<typename T>
QMap< int, QPair<QRectF, T> > RTree<T>::NonLeafNode::removeRows(int position, int number)
{
    if (position > this->m_boundingBox.bottom())
        return QMap< int, QPair<QRectF, T> >();

    QMap< int, QPair<QRectF, T> > removedPairs;

    QRect rect = this->m_boundingBox.toRect();
    int shift = 0;
    int cut = 0;
    if (position < rect.top()) {
        shift = qMin(rect.top() - position, number);
        cut = qMax(0, position + number - rect.top());
    } else {
        shift = 0;
        cut = qMin(number, rect.bottom() - position + 1);
    }
    this->m_boundingBox.adjust(0, -shift, 0, -shift - cut);

    for (int i = 0; i < this->childCount(); ++i) {
        rect = this->m_childBoundingBox[i].toRect();
        if (position < rect.top()) {
            shift = qMin(rect.top() - position, number);
            cut = qMax(0, position + number - rect.top());
        } else {
            shift = 0;
            cut = qMin(number, rect.bottom() - position + 1);
        }
        this->m_childBoundingBox[i].adjust(0, -shift, 0, -shift - cut);

        removedPairs.unite(dynamic_cast<Node*>(this->m_childs[i])->removeRows(position, number));
        if (this->m_childBoundingBox[ i ].isEmpty()) {
            delete this->m_childs[i];
            KoRTree<T>::NonLeafNode::remove(i--);
        }
    }
    return removedPairs;
}

template<typename T>
QMap< int, QPair<QRectF, T> > RTree<T>::NonLeafNode::removeColumns(int position, int number)
{
    if (position > this->m_boundingBox.right())
        return QMap< int, QPair<QRectF, T> >();

    QMap< int, QPair<QRectF, T> > removedPairs;

    QRect rect = this->m_boundingBox.toRect();
    int shift = 0;
    int cut = 0;
    if (position < rect.left()) {
        shift = qMin(rect.left() - position, number);
        cut = qMax(0, position + number - rect.left());
    } else {
        shift = 0;
        cut = qMin(number, rect.right() - position + 1);
    }
    this->m_boundingBox.adjust(-shift, 0, -shift - cut, 0);

    for (int i = 0; i < this->childCount(); ++i) {
        rect = this->m_childBoundingBox[i].toRect();
        if (position < rect.left()) {
            shift = qMin(rect.left() - position, number);
            cut = qMax(0, position + number - rect.left());
        } else {
            shift = 0;
            cut = qMin(number, rect.right() - position + 1);
        }
        this->m_childBoundingBox[i].adjust(-shift, 0, -shift - cut, 0);

        removedPairs.unite(dynamic_cast<Node*>(this->m_childs[i])->removeColumns(position, number));
        if (this->m_childBoundingBox[ i ].isEmpty()) {
            delete this->m_childs[i];
            KoRTree<T>::NonLeafNode::remove(i--);
        }
    }
    return removedPairs;
}

template<typename T>
void RTree<T>::NonLeafNode::operator=(const NonLeafNode& other)
{
    // leave alone the m_parent
    this->m_boundingBox = other.m_boundingBox;
    this->m_childBoundingBox = other.childBoundingBox();
    this->m_counter = other.m_counter;
    this->m_place = other.m_place;
#ifdef CALLIGRA_RTREE_DEBUG
    this->m_nodeId = other.m_nodeId;
#endif
    this->m_level = other.m_level;
    for (int i = 0; i < other.m_counter; ++i) {
        if (other.m_childs[i]->isLeaf()) {
            LeafNode* child = dynamic_cast<LeafNode*>(other.m_childs[i]);
            this->m_childs[i] = new LeafNode(child->childBoundingBox().size(), child->level(), this);
            *dynamic_cast<LeafNode*>(this->m_childs[i]) = *child;
        } else {
            NonLeafNode* child = dynamic_cast<NonLeafNode*>(other.m_childs[i]);
            this->m_childs[i] = new NonLeafNode(child->childBoundingBox().size(), child->level(), this);
            *dynamic_cast<NonLeafNode*>(this->m_childs[i]) = *child;
        }
    }
}

} // namespace Sheets
} // namespace Calligra

#endif // KSPREAD_RTREE

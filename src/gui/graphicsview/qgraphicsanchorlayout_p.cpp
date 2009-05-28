/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QWidget>
#include <QLinkedList>

#include "qgraphicsanchorlayout_p.h"

QSimplexConstraint *GraphPath::constraint(const GraphPath &path) const
{
    // Calculate
    QSet<AnchorData *> cPositives;
    QSet<AnchorData *> cNegatives;
    QSet<AnchorData *> intersection;

    cPositives = positives + path.negatives;
    cNegatives = negatives + path.positives;

    intersection = cPositives & cNegatives;

    cPositives -= intersection;
    cNegatives -= intersection;

    // Fill
    QSimplexConstraint *c = new QSimplexConstraint;
    QSet<AnchorData *>::iterator i;
    for (i = cPositives.begin(); i != cPositives.end(); ++i)
        c->variables.insert(*i, 1.0);

    for (i = cNegatives.begin(); i != cNegatives.end(); ++i)
        c->variables.insert(*i, -1.0);

    return c;
}

QString GraphPath::toString() const
{
    QString string(QLatin1String("Path: "));
    foreach(AnchorData *edge, positives)
        string += QString::fromAscii(" (+++) %1").arg(edge->toString());

    foreach(AnchorData *edge, negatives)
        string += QString::fromAscii(" (---) %1").arg(edge->toString());

    return string;
}


QGraphicsAnchorLayoutPrivate::QGraphicsAnchorLayoutPrivate()
    : calculateGraphCacheDirty(1)
{
}

QGraphicsAnchorLayout::Edge QGraphicsAnchorLayoutPrivate::oppositeEdge(
    QGraphicsAnchorLayout::Edge edge)
{
    switch (edge) {
    case QGraphicsAnchorLayout::Left:
        edge = QGraphicsAnchorLayout::Right;
        break;
    case QGraphicsAnchorLayout::Right:
        edge = QGraphicsAnchorLayout::Left;
        break;
    case QGraphicsAnchorLayout::Top:
        edge = QGraphicsAnchorLayout::Bottom;
        break;
    case QGraphicsAnchorLayout::Bottom:
        edge = QGraphicsAnchorLayout::Top;
        break;
    default:
        break;
    }
    return edge;
}

QGraphicsAnchorLayoutPrivate::Orientation
QGraphicsAnchorLayoutPrivate::edgeOrientation(QGraphicsAnchorLayout::Edge edge)
{
    return edge > QGraphicsAnchorLayout::Right ? Vertical : Horizontal;
}

/*!
  \internal

  Create two internal anchors to connect the layout edges and its
  central anchorage point.
  These anchors doesn't have size restrictions other than the fact they
  should always have the same size, which is something we enforce later,
  when creating restrictions for the Simplex solver.
*/
void QGraphicsAnchorLayoutPrivate::createLayoutEdges()
{
    Q_Q(QGraphicsAnchorLayout);
    QGraphicsLayoutItem *layout = q;

    // Horizontal
    QSimplexConstraint *c = new QSimplexConstraint;
    AnchorData *data = new AnchorData(0, 0, QWIDGETSIZE_MAX);
    addAnchor(layout, QGraphicsAnchorLayout::Left, layout,
              QGraphicsAnchorLayout::HCenter, data);
    data->skipInPreferred = 1;
    c->variables.insert(data, 1.0);

    data = new AnchorData(0, 0, QWIDGETSIZE_MAX);
    addAnchor(layout, QGraphicsAnchorLayout::HCenter,
              layout, QGraphicsAnchorLayout::Right, data);
    data->skipInPreferred = 1;
    c->variables.insert(data, -1.0);

    itemCenterConstraints[Horizontal].append(c);

    // Set the Layout Left edge as the root of the horizontal graph.
    AnchorVertex *v = internalVertex(layout, QGraphicsAnchorLayout::Left);
    graph[Horizontal].setRootVertex(v);

    // Vertical
    c = new QSimplexConstraint;
    data = new AnchorData(0, 0, QWIDGETSIZE_MAX);
    addAnchor(layout, QGraphicsAnchorLayout::Top, layout,
              QGraphicsAnchorLayout::VCenter, data);
    data->skipInPreferred = 1;
    c->variables.insert(data, 1.0);

    data = new AnchorData(0, 0, QWIDGETSIZE_MAX);
    addAnchor(layout, QGraphicsAnchorLayout::VCenter,
              layout, QGraphicsAnchorLayout::Bottom, data);
    data->skipInPreferred = 1;
    c->variables.insert(data, -1.0);

    itemCenterConstraints[Vertical].append(c);

    // Set the Layout Top edge as the root of the vertical graph.
    v = internalVertex(layout, QGraphicsAnchorLayout::Top);
    graph[Vertical].setRootVertex(v);
}

void QGraphicsAnchorLayoutPrivate::deleteLayoutEdges()
{
    Q_Q(QGraphicsAnchorLayout);

    removeAnchor(q, QGraphicsAnchorLayout::Left, q, QGraphicsAnchorLayout::HCenter);
    removeAnchor(q, QGraphicsAnchorLayout::HCenter, q, QGraphicsAnchorLayout::Right);
    removeAnchor(q, QGraphicsAnchorLayout::Top, q, QGraphicsAnchorLayout::VCenter);
    removeAnchor(q, QGraphicsAnchorLayout::VCenter, q, QGraphicsAnchorLayout::Bottom);
}

void QGraphicsAnchorLayoutPrivate::createItemEdges(QGraphicsLayoutItem *item)
{
    items.append(item);

    // Horizontal
    int minimumSize = item->minimumWidth() / 2;
    int preferredSize = item->preferredWidth() / 2;
    int maximumSize = item->maximumWidth() / 2;

    QSimplexConstraint *c = new QSimplexConstraint;

    AnchorData *data = new AnchorData(minimumSize, preferredSize, maximumSize);
    addAnchor(item, QGraphicsAnchorLayout::Left, item,
              QGraphicsAnchorLayout::HCenter, data);
    c->variables.insert(data, 1.0);

    data = new AnchorData(minimumSize, preferredSize, maximumSize);
    addAnchor(item, QGraphicsAnchorLayout::HCenter,
              item, QGraphicsAnchorLayout::Right, data);
    c->variables.insert(data, -1.0);

    itemCenterConstraints[Horizontal].append(c);

    // Vertical
    minimumSize = item->minimumHeight() / 2;
    preferredSize = item->preferredHeight() / 2;
    maximumSize = item->maximumHeight() / 2;

    c = new QSimplexConstraint;

    data = new AnchorData(minimumSize, preferredSize, maximumSize);
    addAnchor(item, QGraphicsAnchorLayout::Top, item,
              QGraphicsAnchorLayout::VCenter, data);
    c->variables.insert(data, 1.0);

    data = new AnchorData(minimumSize, preferredSize, maximumSize);
    addAnchor(item, QGraphicsAnchorLayout::VCenter,
              item, QGraphicsAnchorLayout::Bottom, data);
    c->variables.insert(data, -1.0);

    itemCenterConstraints[Vertical].append(c);
}

void QGraphicsAnchorLayoutPrivate::addAnchor(QGraphicsLayoutItem *firstItem,
                                             QGraphicsAnchorLayout::Edge firstEdge,
                                             QGraphicsLayoutItem *secondItem,
                                             QGraphicsAnchorLayout::Edge secondEdge,
                                             AnchorData *data)
{
    // Is the Vertex (firstItem, firstEdge) already represented in our
    // internal structure?
    AnchorVertex *v1 = addInternalVertex(firstItem, firstEdge);
    AnchorVertex *v2 = addInternalVertex(secondItem, secondEdge);

    // Create a bi-directional edge in the sense it can be transversed both
    // from v1 or v2. "data" however is shared between the two references
    // so we still know that the anchor direction is from 1 to 2.
    data->origin = v1;
    data->name = QString::fromAscii("%1 --to--> %2").arg(v1->toString()).arg(v2->toString());

    graph[edgeOrientation(firstEdge)].createEdge(v1, v2, data);
}

AnchorVertex *QGraphicsAnchorLayoutPrivate::addInternalVertex(QGraphicsLayoutItem *item,
                                                              QGraphicsAnchorLayout::Edge edge)
{
    QPair<QGraphicsLayoutItem *, QGraphicsAnchorLayout::Edge> pair(item, edge);
    QPair<AnchorVertex *, int> v = m_vertexList.value(pair);

    if (!v.first) {
        Q_ASSERT(v.second == 0);
        v.first = new AnchorVertex(item, edge);
    }
    v.second++;
    m_vertexList.insert(pair, v);
    return v.first;
}

/**
 * \internal
 *
 * returns the AnchorVertex that was dereferenced, also when it was removed.
 * returns 0 if it did not exist.
 */
void QGraphicsAnchorLayoutPrivate::removeInternalVertex(QGraphicsLayoutItem *item,
                                                        QGraphicsAnchorLayout::Edge edge)
{
    QPair<QGraphicsLayoutItem *, QGraphicsAnchorLayout::Edge> pair(item, edge);
    QPair<AnchorVertex *, int> v = m_vertexList.value(pair);

    if (!v.first) {
        qWarning("This item with this edge is not in the graph");
        return;
    }

    v.second--;
    if (v.second == 0) {
        // Remove reference and delete vertex
        m_vertexList.remove(pair);
        delete v.first;
    } else {
        // Update reference count
        m_vertexList.insert(pair, v);
    }
}

void QGraphicsAnchorLayoutPrivate::removeAnchor(QGraphicsLayoutItem *firstItem,
                                                QGraphicsAnchorLayout::Edge firstEdge,
                                                QGraphicsLayoutItem *secondItem,
                                                QGraphicsAnchorLayout::Edge secondEdge)
{
    // Look for both vertices
    AnchorVertex *v1 = internalVertex(firstItem, firstEdge);
    AnchorVertex *v2 = internalVertex(secondItem, secondEdge);

    // Remove edge from graph
    if (v1 && v2) {
        graph[edgeOrientation(firstEdge)].removeEdge(v1, v2);
    }

    // Decrease vertices reference count (may trigger a deletion)
    removeInternalVertex(firstItem, firstEdge);
    removeInternalVertex(secondItem, secondEdge);
}

void QGraphicsAnchorLayoutPrivate::removeAnchors(QGraphicsLayoutItem *item)
{
    AnchorVertex *v1 = 0;
    AnchorVertex *v2 = 0;
    QList<AnchorVertex *> allVertex;
    int edge;

    for (edge = QGraphicsAnchorLayout::Left; edge <= QGraphicsAnchorLayout::Bottom; ++edge) {
        // Remove all vertex for all edges
        QGraphicsAnchorLayout::Edge e = static_cast<QGraphicsAnchorLayout::Edge>(edge);

        if ((v1 = internalVertex(item, e))) {
            // Remove all edges
            allVertex = graph[edgeOrientation(e)].adjacentVertices(v1);

            foreach (v2, allVertex) {
                graph[edgeOrientation(e)].removeEdge(v1, v2);
                removeInternalVertex(item, e);
                removeInternalVertex(v2->m_item, v2->m_edge);
            }
        }
    }
}

/*!
  \internal

  Use heuristics to determine the correct orientation of a given anchor.

  After API discussions, we decided we would like expressions like
  anchor(A, Left, B, Right) to mean the same as anchor(B, Right, A, Left).
  The problem with this is that anchors could become ambiguous, for
  instance, what does the anchor A, B of size X mean?

     "pos(B) = pos(A) + X"  or  "pos(A) = pos(B) + X" ?

  To keep the API user friendly and at the same time, keep our algorithm
  deterministic, we use an heuristic to determine a direction for each
  added anchor and then keep it. The heuristic is based on the fact
  that people usually avoid overlapping items, therefore:

     "A, RIGHT to B, LEFT" means that B is to the LEFT of A.
     "B, LEFT to A, RIGHT" is corrected to the above anchor.

  Special correction is also applied when one of the items is the
  layout. We handle Layout Left as if it was another items's Right
  and Layout Right as another item's Left.
*/
void QGraphicsAnchorLayoutPrivate::correctEdgeDirection(QGraphicsLayoutItem *&firstItem,
                                                        QGraphicsAnchorLayout::Edge &firstEdge,
                                                        QGraphicsLayoutItem *&secondItem,
                                                        QGraphicsAnchorLayout::Edge &secondEdge)
{
    Q_Q(QGraphicsAnchorLayout);

    QGraphicsAnchorLayout::Edge effectiveFirst = firstEdge;
    QGraphicsAnchorLayout::Edge effectiveSecond = secondEdge;

    if (firstItem == q)
        effectiveFirst = QGraphicsAnchorLayoutPrivate::oppositeEdge(firstEdge);
    if (secondItem == q)
        effectiveSecond = QGraphicsAnchorLayoutPrivate::oppositeEdge(secondEdge);

    if (effectiveFirst < effectiveSecond) {

        // ### DEBUG
        /*        printf("Swapping Anchor from %s %d --to--> %s %d\n",
               firstItem->isLayout() ? "<layout>" :
               qPrintable(static_cast<QGraphicsWidget *>(firstItem)->data(0).toString()),
               firstEdge,
               secondItem->isLayout() ? "<layout>" :
               qPrintable(static_cast<QGraphicsWidget *>(secondItem)->data(0).toString()),
               secondEdge);
        */
        qSwap(firstItem, secondItem);
        qSwap(firstEdge, secondEdge);
    }
}

/*!
  \internal

  XXX: REMOVE THIS ONCE WE INHERIT SOMEONE ELSE!
*/
QGraphicsItem *QGraphicsAnchorLayoutPrivate::parentItem() const
{
    Q_Q(const QGraphicsLayoutItem);

    const QGraphicsLayoutItem *parent = q;
    while (parent && parent->isLayout()) {
        parent = parent->parentLayoutItem();
    }
    return parent ? parent->graphicsItem() : 0;
}

/*!
  \internal
*/
void QGraphicsAnchorLayoutPrivate::addChildItem(QGraphicsLayoutItem *child)
{
    // XXX: Re-implement this!!
    if (child) {
        Q_Q(QGraphicsAnchorLayout);
        child->setParentLayoutItem(q);

        child->graphicsItem()->setParentItem(parentItem());
    }
}

/*!
  \internal

  Called on activation. Uses Linear Programming to define minimum, preferred
  and maximum sizes for the layout. Also calculates the sizes that each item
  should assume when the layout is in one of such situations.
*/
void QGraphicsAnchorLayoutPrivate::calculateGraphs()
{
    if (!calculateGraphCacheDirty)
        return;

    calculateGraphs(Horizontal);
    calculateGraphs(Vertical);

    calculateGraphCacheDirty = 0;
}

// ### remove me:
QList<AnchorData *> getVariables(QList<QSimplexConstraint *> constraints)
{
    QSet<AnchorData *> variableSet;
    for (int i = 0; i < constraints.count(); ++i) {
        const QSimplexConstraint *c = constraints[i];
        foreach (QSimplexVariable *var, c->variables.keys()) {
            variableSet += static_cast<AnchorData *>(var);
        }
    }
    return variableSet.toList();
}

void QGraphicsAnchorLayoutPrivate::calculateGraphs(
    QGraphicsAnchorLayoutPrivate::Orientation orientation)
{
    Q_Q(QGraphicsAnchorLayout);

    // Reset the nominal sizes of each anchor based on the current item sizes
    setAnchorSizeHintsFromItems(orientation);

    // Traverse all graph edges and store the possible paths to each vertex
    findPaths(orientation);

    // From the paths calculated above, extract the constraints that the current
    // anchor setup impose, to our Linear Programming problem.
    constraintsFromPaths(orientation);

    // Split the constraints and anchors into groups that should be fed to the
    // simplex solver independently. Currently we find two groups:
    //
    //  1) The "trunk", that is, the set of anchors (items) that are connected
    //     to the two opposite sides of our layout, and thus need to stretch in
    //     order to fit in the current layout size.
    //
    //  2) The floating or semi-floating anchors (items) that are those which
    //     are connected to only one (or none) of the layout sides, thus are not
    //     influenced by the layout size.
    QList<QList<QSimplexConstraint *> > parts;
    parts = getGraphParts(orientation);

    // Now run the simplex solver to calculate Minimum, Preferred and Maximum sizes
    // of the "trunk" set of constraints and variables.
    // ### does trunk always exist? empty = trunk is the layout left->center->right
    QList<QSimplexConstraint *> trunkConstraints = parts[0];

    // For minimum and maximum, use the path between the two layout sides as the
    // objective function.

    // Retrieve that path
    QGraphicsAnchorLayout::Edge end;
    if (orientation == Horizontal) {
        end = QGraphicsAnchorLayout::Right;
    } else {
        end = QGraphicsAnchorLayout::Bottom;
    }
    AnchorVertex *v = internalVertex(q, end);
    GraphPath trunkPath = graphPaths[orientation].value(v);

    // Solve min and max size hints for trunk
    QPair<qreal, qreal> minMax = solveMinMax(trunkConstraints, trunkPath);
    sizeHints[orientation][Qt::MinimumSize] = minMax.first;
    sizeHints[orientation][Qt::MaximumSize] = minMax.second;

    // Solve for preferred. The objective function is calculated from the constraints
    // and variables internally.
    solvePreferred(trunkConstraints);

    // Calculate and set the preferred size for the layout from the edge sizes that
    // were calculated above.
    qreal pref(0.0);
    foreach (const AnchorData *ad, trunkPath.positives) {
        pref += ad->sizeAtPreferred;
    }
    foreach (const AnchorData *ad, trunkPath.negatives) {
        pref -= ad->sizeAtPreferred;
    }
    sizeHints[orientation][Qt::PreferredSize] = pref;

    // For the other parts that not the trunk, solve only for the preferred size
    // that is the size they will remain at, since they are not stretched by the
    // layout.

    // Solve the other only for preferred, skip trunk
    for (int i = 1; i < parts.count(); ++i) {
        QList<QSimplexConstraint *> partConstraints = parts[i];
        QList<AnchorData *> partVariables = getVariables(partConstraints);

        // ###
        if (partVariables.isEmpty())
            continue;

        solvePreferred(partConstraints);

        // Propagate size at preferred to other sizes. Semi-floats
        // always will be in their sizeAtPreferred.
        for (int j = 0; j < partVariables.count(); ++j) {
            AnchorData *ad = partVariables[j];
            Q_ASSERT(ad);
            ad->sizeAtMinimum = ad->sizeAtPreferred;
            ad->sizeAtMaximum = ad->sizeAtPreferred;
        }
    }

    // Clean up our data structures. They are not needed anymore since
    // distribution uses just interpolation.
    qDeleteAll(constraints[orientation]);
    constraints[orientation].clear();
    graphPaths[orientation].clear(); // ###
}

void QGraphicsAnchorLayoutPrivate::setAnchorSizeHintsFromItems(Orientation orientation)
{
    QPair<QGraphicsLayoutItem *, QGraphicsAnchorLayout::Edge> beginningKey;
    QPair<QGraphicsLayoutItem *, QGraphicsAnchorLayout::Edge> centerKey;
    QPair<QGraphicsLayoutItem *, QGraphicsAnchorLayout::Edge> endKey;

    if (orientation == Horizontal) {
        beginningKey.second = QGraphicsAnchorLayout::Left;
        centerKey.second = QGraphicsAnchorLayout::HCenter;
        endKey.second = QGraphicsAnchorLayout::Right;
    } else {
        beginningKey.second = QGraphicsAnchorLayout::Top;
        centerKey.second = QGraphicsAnchorLayout::VCenter;
        endKey.second = QGraphicsAnchorLayout::Bottom;
    }

    foreach (QGraphicsLayoutItem *item, items) {
        AnchorVertex *beginning, *center, *end;
        qreal min, pref, max;

        beginningKey.first = item;
        centerKey.first = item;
        endKey.first = item;

        beginning = internalVertex(beginningKey);
        center = internalVertex(centerKey);
        end = internalVertex(endKey);

        if (orientation == Horizontal) {
            min = item->minimumWidth();
            pref = item->preferredWidth();
            max = item->maximumWidth();
        } else {
            min = item->minimumHeight();
            pref = item->preferredHeight();
            max = item->maximumHeight();
        }

        // To support items that are represented by a single anchor as well as
        // those that have been divided into two halfs, we must do this check.
        AnchorData *data;
        if (center == 0) {
            data = graph[orientation].edgeData(beginning, end);
            // Set the anchor nominal sizes to those of the corresponding item
            data->minSize = min;
            data->prefSize = pref;
            data->maxSize = max;

            // Set the anchor effective sizes to preferred.
            // Note: The idea here is that all items should remain at
            // their preferred size unless where that's impossible.
            // In cases where the item is subject to restrictions
            // (anchored to the layout edges, for instance), the
            // simplex solver will be run to recalculate and override
            // the values we set here.
            data->sizeAtMinimum = pref;
            data->sizeAtPreferred = pref;
            data->sizeAtMaximum = pref;
        } else {
            min = min / 2;
            pref = pref / 2;
            max = max / 2;

            // Same as above, for each half
            data = graph[orientation].edgeData(beginning, center);
            data->minSize = min;
            data->prefSize = pref;
            data->maxSize = max;
            data->sizeAtMinimum = pref;
            data->sizeAtPreferred = pref;
            data->sizeAtMaximum = pref;

            data = graph[orientation].edgeData(center, end);
            data->minSize = min;
            data->prefSize = pref;
            data->maxSize = max;
            data->sizeAtMinimum = pref;
            data->sizeAtPreferred = pref;
            data->sizeAtMaximum = pref;
        }
    }
}

/*!
  \internal

  This method walks the graph using a breadth-first search to find paths
  between the root vertex and each vertex on the graph. The edges
  directions in each path are considered and they are stored as a
  positive edge (left-to-right) or negative edge (right-to-left).

  The list of paths is used later to generate a list of constraints.
 */
void QGraphicsAnchorLayoutPrivate::findPaths(Orientation orientation)
{
    QQueue<QPair<AnchorVertex *, AnchorVertex *> > queue;

    QSet<AnchorData *> visited;

    bool ok;
    AnchorVertex *root = graph[orientation].firstVertex(&ok);

    graphPaths[orientation].insert(root, GraphPath());

    foreach (AnchorVertex *v, graph[orientation].adjacentVertices(root)) {
        queue.enqueue(qMakePair(root, v));
    }

    while(!queue.isEmpty()) {
        QPair<AnchorVertex *, AnchorVertex *>  pair = queue.dequeue();
        AnchorData *edge = graph[orientation].edgeData(pair.first, pair.second);

        if (visited.contains(edge))
            continue;

        visited.insert(edge);
        GraphPath current = graphPaths[orientation].value(pair.first);

        if (edge->origin == pair.first)
            current.positives.insert(edge);
        else
            current.negatives.insert(edge);

        graphPaths[orientation].insert(pair.second, current);

        foreach (AnchorVertex *v,
                graph[orientation].adjacentVertices(pair.second)) {
            queue.enqueue(qMakePair(pair.second, v));
        }
    }
}

/*!
  \internal

  Each vertex on the graph that has more than one path to it
  represents a contra int to the sizes of the items in these paths.

  This method walks the list of paths to each vertex, generate
  the constraints and store them in a list so they can be used later
  by the Simplex solver.
*/
void QGraphicsAnchorLayoutPrivate::constraintsFromPaths(Orientation orientation)
{
    foreach (AnchorVertex *vertex, graphPaths[orientation].uniqueKeys())
    {
        int valueCount = graphPaths[orientation].count(vertex);
        if (valueCount == 1)
            continue;

        QList<GraphPath> pathsToVertex = graphPaths[orientation].values(vertex);
        for (int i = 1; i < valueCount; ++i) {
            constraints[orientation] += \
                pathsToVertex[0].constraint(pathsToVertex[i]);
        }
    }
}

/*!
  \Internal
*/
QList< QList<QSimplexConstraint *> >
QGraphicsAnchorLayoutPrivate::getGraphParts(Orientation orientation)
{
    Q_Q(QGraphicsAnchorLayout);

    // Find layout vertices and edges for the current orientation.
    AnchorVertex *layoutFirstVertex =
        internalVertex(q, orientation == Horizontal ?
                       QGraphicsAnchorLayout::Left : QGraphicsAnchorLayout::Top);

    AnchorVertex *layoutCentralVertex =
        internalVertex(q, orientation == Horizontal ?
                       QGraphicsAnchorLayout::HCenter : QGraphicsAnchorLayout::VCenter);

    AnchorVertex *layoutLastVertex =
        internalVertex(q, orientation == Horizontal ?
                       QGraphicsAnchorLayout::Right : QGraphicsAnchorLayout::Bottom);

    AnchorData *edgeL1 = graph[orientation].edgeData(layoutFirstVertex, layoutCentralVertex);
    AnchorData *edgeL2 = graph[orientation].edgeData(layoutCentralVertex, layoutLastVertex);

    QLinkedList<QSimplexConstraint *> remainingConstraints;
    for (int i = 0; i < constraints[orientation].count(); ++i) {
        remainingConstraints += constraints[orientation][i];
    }
    for (int i = 0; i < itemCenterConstraints[orientation].count(); ++i) {
        remainingConstraints += itemCenterConstraints[orientation][i];
    }

    QList<QSimplexConstraint *> trunkConstraints;
    QSet<QSimplexVariable *> trunkVariables;

    trunkVariables += edgeL1;
    trunkVariables += edgeL2;

    bool dirty;
    do {
        dirty = false;

        QLinkedList<QSimplexConstraint *>::iterator it = remainingConstraints.begin();
        while (it != remainingConstraints.end()) {
            QSimplexConstraint *c = *it;
            bool match = false;

            // Check if this constraint have some overlap with current
            // trunk variables...
            foreach (QSimplexVariable *ad, trunkVariables) {
                if (c->variables.contains(ad)) {
                    match = true;
                    break;
                }
            }

            // If so, we add it to trunk, and erase it from the
            // remaining constraints.
            if (match) {
                trunkConstraints += c;
                trunkVariables += QSet<QSimplexVariable *>::fromList(c->variables.keys());
                it = remainingConstraints.erase(it);
                dirty = true;
            } else {
                ++it;
            }
        }
    } while (dirty);

    QList< QList<QSimplexConstraint *> > result;
    result += trunkConstraints;

    if (!remainingConstraints.isEmpty()) {
        QList<QSimplexConstraint *> nonTrunkConstraints;
        QLinkedList<QSimplexConstraint *>::iterator it = remainingConstraints.begin();
        while (it != remainingConstraints.end()) {
            nonTrunkConstraints += *it;
            ++it;
        }
        result += nonTrunkConstraints;
    }

    return result;
}

/*!
  \internal

  Use the current vertices distance to calculate and set the geometry of
  each item.
*/
void QGraphicsAnchorLayoutPrivate::setItemsGeometries()
{
    AnchorVertex *firstH, *secondH, *firstV, *secondV;

    foreach (QGraphicsLayoutItem *item, items) {
        firstH = internalVertex(item, QGraphicsAnchorLayout::Left);
        secondH = internalVertex(item, QGraphicsAnchorLayout::Right);
        firstV = internalVertex(item, QGraphicsAnchorLayout::Top);
        secondV = internalVertex(item, QGraphicsAnchorLayout::Bottom);

        QPointF topLeft(firstH->distance, firstV->distance);
        QPointF bottomRight(secondH->distance, secondV->distance);

        item->setGeometry(QRectF(topLeft, bottomRight));
    }
}

/*!
  \internal

  Calculate the position of each vertex based on the paths to each of
  them as well as the current edges sizes.
*/
void QGraphicsAnchorLayoutPrivate::calculateVertexPositions(
    QGraphicsAnchorLayoutPrivate::Orientation orientation)
{
    Q_Q(QGraphicsAnchorLayout);
    QQueue<QPair<AnchorVertex *, AnchorVertex *> > queue;
    QSet<AnchorVertex *> visited;

    // Get root vertex
    bool ok;
    AnchorVertex *root = graph[orientation].firstVertex(&ok);

    qreal widgetMargin;
    qreal layoutMargin;

    // Initialize the first vertex
    if (orientation == Horizontal) {
        widgetMargin = q->geometry().x();
        q->getContentsMargins(&layoutMargin, 0, 0, 0);
    } else {
        // Root position is equal to the top margin
        widgetMargin = q->geometry().y();
        q->getContentsMargins(0, &layoutMargin, 0, 0);
    }
    root->distance = widgetMargin + layoutMargin;
    visited.insert(root);

    //  Add initial edges to the queue
    foreach (AnchorVertex *v, graph[orientation].adjacentVertices(root)) {
        queue.enqueue(qMakePair(root, v));
    }

    // Do initial calculation required by "interpolateEdge()"
    setupEdgesInterpolation(orientation);

    // Traverse the graph and calculate vertex positions.
    while (!queue.isEmpty()) {
        QPair<AnchorVertex *, AnchorVertex *> pair = queue.dequeue();

        if (visited.contains(pair.second))
            continue;
        visited.insert(pair.second);

        // The distance to the next vertex is equal the distance to the
        // previous one plus (or less) the size of the edge between them.
        qreal distance;
        AnchorData *edge = graph[orientation].edgeData(pair.first, pair.second);

        if (edge->origin == pair.first) {
            distance = pair.first->distance + interpolateEdge(edge);
        } else {
            distance = pair.first->distance - interpolateEdge(edge);
        }
        pair.second->distance = distance;

        foreach (AnchorVertex *v,
                graph[orientation].adjacentVertices(pair.second)) {
            queue.enqueue(qMakePair(pair.second, v));
        }
    }
}

/*!
  \internal

  Calculate interpolation parameters based on current Layout Size.
  Must once before calling "interpolateEdgeSize()" for each edge.
*/
void QGraphicsAnchorLayoutPrivate::setupEdgesInterpolation(
    Orientation orientation)
{
    Q_Q(QGraphicsAnchorLayout);
    qreal lower, upper, current;

    if (orientation == Horizontal) {
        current = q->contentsRect().width();
    } else {
        current = q->contentsRect().height();
    }

    if (current < sizeHints[orientation][Qt::PreferredSize]) {
        interpolationInterval[orientation] = MinToPreferred;
        lower = sizeHints[orientation][Qt::MinimumSize];
        upper = sizeHints[orientation][Qt::PreferredSize];
    } else {
        interpolationInterval[orientation] = PreferredToMax;
        lower = sizeHints[orientation][Qt::PreferredSize];
        upper = sizeHints[orientation][Qt::MaximumSize];
    }

    if (upper == lower) {
        interpolationProgress[orientation] = 0;
    } else {
        interpolationProgress[orientation] = (current - lower) / (upper - lower);
    }
}

/*!
  \internal

  Calculate the current Edge size based on the current Layout size and the
  size the edge is supposed to have when:

   - the layout is at its minimum size.
   - the layout is at its preferred size.
   - the layout is at its maximum size.

   These three key values are calculated in advance using linear programming
   (more expensive), then subsequential resizes of the parent layout require
   a simple interpolation.
*/
qreal QGraphicsAnchorLayoutPrivate::interpolateEdge(AnchorData *edge)
{
    qreal lower, upper;

    Orientation orientation = edgeOrientation(edge->origin->m_edge);

    if (interpolationInterval[orientation] == MinToPreferred) {
        lower = edge->sizeAtMinimum;
        upper = edge->sizeAtPreferred;
    } else {
        lower = edge->sizeAtPreferred;
        upper = edge->sizeAtMaximum;
    }

    return (interpolationProgress[orientation] * (upper - lower)) + lower;
}


QPair<qreal, qreal>
QGraphicsAnchorLayoutPrivate::solveMinMax(QList<QSimplexConstraint *> constraints,
                                          GraphPath path)
{
    QList<AnchorData *> variables = getVariables(constraints);
    QList<QSimplexConstraint *> itemConstraints;

    for (int i = 0; i < variables.size(); ++i) {
        QSimplexConstraint *c = new QSimplexConstraint;
        c->variables.insert(variables[i], 1.0);
        c->constant = variables[i]->minSize;
        c->ratio = QSimplexConstraint::MoreOrEqual;
        itemConstraints += c;

        c = new QSimplexConstraint;
        c->variables.insert(variables[i], 1.0);
        c->constant = variables[i]->maxSize;
        c->ratio = QSimplexConstraint::LessOrEqual;
        itemConstraints += c;
    }

    QSimplex simplex;
    simplex.setConstraints(constraints + itemConstraints);

    // Obtain the objective constraint
    QSimplexConstraint objective;
    QSet<AnchorData *>::const_iterator iter;
    for (iter = path.positives.constBegin(); iter != path.positives.constEnd(); ++iter)
        objective.variables.insert(*iter, 1.0);

    for (iter = path.negatives.constBegin(); iter != path.negatives.constEnd(); ++iter)
        objective.variables.insert(*iter, -1.0);

    simplex.setObjective(&objective);

    // Calculate minimum values
    qreal min = simplex.solveMin();

    // Save sizeAtMinimum results
    for (int i = 0; i < variables.size(); ++i) {
        AnchorData *ad = static_cast<AnchorData *>(variables[i]);
        ad->sizeAtMinimum = ad->result;
    }

    // Calculate maximum values
    qreal max = simplex.solveMax();

    // Save sizeAtMaximum results
    for (int i = 0; i < variables.size(); ++i) {
        AnchorData *ad = static_cast<AnchorData *>(variables[i]);
        ad->sizeAtMaximum = ad->result;
    }

    qDeleteAll(itemConstraints);

    return qMakePair<qreal, qreal>(min, max);
}

void QGraphicsAnchorLayoutPrivate::solvePreferred(QList<QSimplexConstraint *> constraints)
{
    QList<AnchorData *> variables = getVariables(constraints);

    // ###
    QList<QSimplexConstraint *> itemConstraints;

    for (int i = 0; i < variables.size(); ++i) {
        QSimplexConstraint *c = new QSimplexConstraint;
        c->variables.insert(variables[i], 1.0);
        c->constant = variables[i]->minSize;
        c->ratio = QSimplexConstraint::MoreOrEqual;
        itemConstraints += c;

        c = new QSimplexConstraint;
        c->variables.insert(variables[i], 1.0);
        c->constant = variables[i]->maxSize;
        c->ratio = QSimplexConstraint::LessOrEqual;
        itemConstraints += c;
    }

    QList<QSimplexConstraint *> preferredConstraints;
    QList<QSimplexVariable *> preferredVariables;
    QSimplexConstraint objective;

    // Fill the objective coefficients for this variable. In the
    // end the objective function will be
    //
    //     z = n * (A_shrink + B_shrink + ...) + (A_grower + B_grower + ...)
    //
    // where n is the number of variables that have
    // slacks. Note that here we use the number of variables
    // as coefficient, this is to mark the "shrinker slack
    // variable" less likely to get value than the "grower
    // slack variable".

    // This will fill the values for the structural constraints
    // and we now fill the values for the slack constraints (one per variable),
    // which have this form (the constant A_pref was set when creating the slacks):
    //
    //      A + A_shrinker - A_grower = A_pref
    //
    for (int i = 0; i < variables.size(); ++i) {
        AnchorData *ad = static_cast<AnchorData *>(variables[i]);
        if (ad->skipInPreferred)
            continue;

        QSimplexVariable *grower = new QSimplexVariable;
        QSimplexVariable *shrinker = new QSimplexVariable;
        QSimplexConstraint *c = new QSimplexConstraint;
        c->variables.insert(ad, 1.0);
        c->variables.insert(shrinker, 1.0);
        c->variables.insert(grower, -1.0);
        c->constant = ad->prefSize;

        preferredConstraints += c;
        preferredVariables += grower;
        preferredVariables += shrinker;

        objective.variables.insert(grower, 1.0);
        objective.variables.insert(shrinker, variables.size());
    }


    QSimplex simplex;
    simplex.setConstraints(constraints + itemConstraints + preferredConstraints);

    simplex.setObjective(&objective);

    // Calculate minimum values
    simplex.solveMin();

    // Save sizeAtPreferred results
    for (int i = 0; i < variables.size(); ++i) {
        AnchorData *ad = static_cast<AnchorData *>(variables[i]);
        ad->sizeAtPreferred = ad->result;
    }

    qDeleteAll(itemConstraints);
    qDeleteAll(preferredConstraints);
    qDeleteAll(preferredVariables);

}

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
#include <QtCore/qstack.h>

#include "qgraphicsanchorlayout_p.h"

void ParallelAnchorData::updateChildrenSizes()
{
    firstEdge->sizeAtMinimum = secondEdge->sizeAtMinimum = sizeAtMinimum;
    firstEdge->sizeAtPreferred = secondEdge->sizeAtPreferred = sizeAtPreferred;
    firstEdge->sizeAtMaximum = secondEdge->sizeAtMaximum = sizeAtMaximum;

    firstEdge->updateChildrenSizes();
    secondEdge->updateChildrenSizes();
}

void ParallelAnchorData::refreshSizeHints()
{
    // ### should we warn if the parallel connection is invalid?
    // e.g. 1-2-3 with 10-20-30, the minimum of the latter is
    // bigger than the maximum of the former.

    firstEdge->refreshSizeHints();
    secondEdge->refreshSizeHints();

    minSize = qMax(firstEdge->minSize, secondEdge->minSize);
    maxSize = qMin(firstEdge->maxSize, secondEdge->maxSize);

    prefSize = qMax(firstEdge->prefSize, secondEdge->prefSize);
    prefSize = qMin(prefSize, maxSize);

    sizeAtMinimum = prefSize;
    sizeAtPreferred = prefSize;
    sizeAtMaximum = prefSize;
}

void SequentialAnchorData::updateChildrenSizes()
{
    qreal minFactor = sizeAtMinimum / minSize;
    qreal prefFactor = sizeAtPreferred / prefSize;
    qreal maxFactor = sizeAtMaximum / maxSize;

    for (int i = 0; i < m_edges.count(); ++i) {
        m_edges[i]->sizeAtMinimum = m_edges[i]->minSize * minFactor;
        m_edges[i]->sizeAtPreferred = m_edges[i]->prefSize * prefFactor;
        m_edges[i]->sizeAtMaximum = m_edges[i]->maxSize * maxFactor;
        m_edges[i]->updateChildrenSizes();
    }
}

void SequentialAnchorData::refreshSizeHints()
{
    minSize = 0;
    prefSize = 0;
    maxSize = 0;
    for (int i = 0; i < m_edges.count(); ++i) {
        AnchorData *edge = m_edges.at(i);
        edge->refreshSizeHints();
        minSize += edge->minSize;
        prefSize += edge->prefSize;
        maxSize += edge->maxSize;
    }

    sizeAtMinimum = prefSize;
    sizeAtPreferred = prefSize;
    sizeAtMaximum = prefSize;
}

void AnchorData::dump(int indent) {
    if (type == Parallel) {
        qDebug("%*s type: parallel:", indent, "");
        ParallelAnchorData *p = static_cast<ParallelAnchorData *>(this);
        p->firstEdge->dump(indent+2);
        p->secondEdge->dump(indent+2);
    } else if (type == Sequential) {
        SequentialAnchorData *s = static_cast<SequentialAnchorData *>(this);
        int kids = s->m_edges.count();
        qDebug("%*s type: sequential(%d):", indent, "", kids);
        for (int i = 0; i < kids; ++i) {
            s->m_edges.at(i)->dump(indent+2);
        }
    } else {
        qDebug("%*s type: Normal:", indent, "");
    }
}

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
    for (int i = 0; i < NOrientations; ++i)
        spacing[i] = -1;
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


/*!
 * \internal
 *
 * helper function in order to avoid overflowing anchor sizes
 * the returned size will never be larger than FLT_MAX
 *
 */
inline static qreal checkAdd(qreal a, qreal b)
{
    if (FLT_MAX - b  < a)
        return FLT_MAX;
    return a + b;
}

/*!
 * \internal
 *
 * Takes the sequence of vertices described by (\a before, \a vertices, \a after) and replaces
 * all anchors connected to the vertices in \a vertices with one simplified anchor between
 * \a before and \a after. The simplified anchor will be a placeholder for all the previous
 * anchors between \a before and \a after, and can be restored back to the anchors it is a
 * placeholder for.
 */
static void simplifySequentialChunk(Graph<AnchorVertex, AnchorData> *graph,
                                    AnchorVertex *before,
                                    const QVector<AnchorVertex*> &vertices,
                                    AnchorVertex *after)
{
    int i;
#if 0
    QString strVertices;
    for (i = 0; i < vertices.count(); ++i)
        strVertices += QString::fromAscii("%1 - ").arg(vertices.at(i)->toString());
    QString strPath = QString::fromAscii("%1 - %2%3").arg(before->toString(), strVertices, after->toString());
    qDebug("simplifying [%s] to [%s - %s]", qPrintable(strPath), qPrintable(before->toString()), qPrintable(after->toString()));
#endif

    qreal min = 0;
    qreal pref = 0;
    qreal max = 0;

    SequentialAnchorData *sequence = new SequentialAnchorData;
    AnchorVertex *prev = before;
    AnchorData *data;
    for (i = 0; i <= vertices.count(); ++i) {
        AnchorVertex *next = (i < vertices.count()) ? vertices.at(i) : after;
        data = graph->takeEdge(prev, next);
        min += data->minSize;
        pref += data->prefSize;
        max = checkAdd(max, data->maxSize);
        sequence->m_edges.append(data);
        prev = next;
    }

    // insert new
    sequence->minSize = min;
    sequence->prefSize = pref;
    sequence->maxSize = max;

    // Unless these values are overhidden by the simplex solver later-on,
    // anchors will keep their own preferred size.
    sequence->sizeAtMinimum = pref;
    sequence->sizeAtPreferred = pref;
    sequence->sizeAtMaximum = pref;

    sequence->setVertices(vertices);
    sequence->origin = data->origin == vertices.last() ? before : after;
    AnchorData *newAnchor = sequence;
    if (AnchorData *oldAnchor = graph->takeEdge(before, after)) {
        newAnchor = new ParallelAnchorData(oldAnchor, sequence);
        min = qMax(oldAnchor->minSize, sequence->minSize);
        max = qMin(oldAnchor->maxSize, sequence->maxSize);

        pref = qMax(oldAnchor->prefSize, sequence->prefSize);
        pref = qMin(pref, max);

        newAnchor->minSize = min;
        newAnchor->prefSize = pref;
        newAnchor->maxSize = max;

        // Same as above, by default, keep preferred size.
        newAnchor->sizeAtMinimum = pref;
        newAnchor->sizeAtPreferred = pref;
        newAnchor->sizeAtMaximum = pref;
    }
    graph->createEdge(before, after, newAnchor);
}

/*!
 * \internal
 *
 * The purpose of this function is to simplify the graph. The process of simplification can be
 * described as:
 *
 * 1. Simplify all sequences of anchors into one anchor.
 *    If not first iteration and no further simplification was done, go to (3)
 * 2. Simplify two parallel anchors into one anchor.
 *    If any simplification was done, go to (1)
 * 3. Done
 *
 *
 * The algorithm walks the graph in depth-first order, and only collects vertices that has two
 * edges connected to it. If the vertex does not have two edges or if it is a layout edge,
 * it will take all the previously collected vertices and try to create a simplified sequential
 * anchor representing all the previously collected vertices.
 * Once the simplified anchor is inserted, the collected list is cleared in order to find the next
 * sequence to simplify.
 * Note that there are some catches to this that are not covered by the above explanation.
 *
 *
 * Notes:
 *  * The algorithm should not make a sequence of the layout edge anchors.
 *      => Make sure those edges are not traversed
 *  * A generic algorithm will make a sequential simplification node of a Left-HCenter-Right
 *    sequence. This is ok, but that sequence should not be affected by stretch factors.
 *
 */
void QGraphicsAnchorLayoutPrivate::simplifyGraph(QGraphicsAnchorLayoutPrivate::Orientation orientation)
{
    Q_Q(QGraphicsAnchorLayout);
    Graph<AnchorVertex, AnchorData> &g = graph[orientation];
    AnchorVertex *v = g.rootVertex();

    if (!v)
        return;
    QSet<AnchorVertex*> visited;
    QStack<AnchorVertex *> stack;
    stack.push(v);
    QVector<AnchorVertex*> candidates;

    const QGraphicsAnchorLayout::Edge centerEdge = pickEdge(QGraphicsAnchorLayout::HCenter, orientation);
    const QGraphicsAnchorLayout::Edge layoutEdge = oppositeEdge(v->m_edge);

    // walk depth-first.
    while (!stack.isEmpty()) {
        v = stack.pop();
        QList<AnchorVertex *> vertices = g.adjacentVertices(v);
        const int count = vertices.count();
        bool endOfSequence = (v->m_item == q && v->m_edge == layoutEdge) || count != 2;
        if (count == 2 && v->m_item != q) {
            candidates.append(v);
            if (visited.contains(vertices.first()) && visited.contains(vertices.last())) {
                // in case of a cycle
                endOfSequence = true;
            }
        }
        if (endOfSequence && candidates.count() >= 2) {
            int i;
            AnchorVertex *afterSequence= 0;
            QList<AnchorVertex *> adjacentOfSecondLastVertex = g.adjacentVertices(candidates.last());
            Q_ASSERT(adjacentOfSecondLastVertex.count() == 2);
            if (adjacentOfSecondLastVertex.first() == candidates.at(candidates.count() - 2))
                afterSequence = adjacentOfSecondLastVertex.last();
            else
                afterSequence = adjacentOfSecondLastVertex.first();

            AnchorVertex *beforeSequence = 0;
            QList<AnchorVertex *> adjacentOfSecondVertex = g.adjacentVertices(candidates.first());
            Q_ASSERT(adjacentOfSecondVertex.count() == 2);
            if (adjacentOfSecondVertex.first() == candidates.at(1))
                beforeSequence = adjacentOfSecondVertex.last();
            else
                beforeSequence = adjacentOfSecondVertex.first();
            // The complete path of the sequence to simplify is: beforeSequence, <candidates>, afterSequence
            // where beforeSequence and afterSequence are the endpoints where the anchor is inserted
            // between.
#if 0
            // ### DEBUG
            QString strCandidates;
            for (i = 0; i < candidates.count(); ++i)
                strCandidates += QString::fromAscii("%1 - ").arg(candidates.at(i)->toString());
            QString strPath = QString::fromAscii("%1 - %2%3").arg(beforeSequence->toString(), strCandidates, afterSequence->toString());
            qDebug("candidate list for sequential simplification:\n[%s]", qPrintable(strPath));
#endif

            bool forward;
            AnchorVertex *prev = beforeSequence;
            int intervalFrom = 0;

            // Check for directionality (origin). We don't want to destroy that information,
            // thus we only combine anchors with the same direction.

            // "i" is the index *including* the beforeSequence and afterSequence vertices.
            for (i = 1; i <= candidates.count() + 1; ++i) {
                bool atVertexAfter = i > candidates.count();
                AnchorVertex *v1 = atVertexAfter ? afterSequence : candidates.at(i - 1);
                AnchorData *data = g.edgeData(prev, v1);
                Q_ASSERT(data);
                if (i == 1) {
                    forward = (prev == data->origin ? true : false);
                } else if (forward != (prev == data->origin) || atVertexAfter) {
                    int intervalTo = i;
                    if (forward != (prev == data->origin))
                        --intervalTo;

                    // intervalFrom and intervalTo should now be indices to the vertex before and
                    // after the sequential anchor.
                    if (intervalTo - intervalFrom >= 2) {
                        // simplify in the range [intervalFrom, intervalTo]

                        // Trim off internal center anchors (Left-Center/Center-Right) from the
                        // start and the end of the sequence. We never want to simplify internal
                        // center anchors where there is an external anchor connected to the center.
                        AnchorVertex *intervalVertexFrom = intervalFrom == 0 ? beforeSequence : candidates.at(intervalFrom - 1);
                        if (intervalVertexFrom->m_edge == centerEdge
                            && intervalVertexFrom->m_item == candidates.at(intervalFrom)->m_item) {
                            ++intervalFrom;
                            intervalVertexFrom = candidates.at(intervalFrom - 1);
                        }
                        AnchorVertex *intervalVertexTo = intervalTo <= candidates.count() ? candidates.at(intervalTo - 1) : afterSequence;
                        if (intervalVertexTo->m_edge == centerEdge
                            && intervalVertexTo->m_item == candidates.at(intervalTo - 2)->m_item) {
                            --intervalTo;
                            intervalVertexTo = candidates.at(intervalTo - 1);
                        }

                        QVector<AnchorVertex*> subCandidates;
                        if (forward) {
                           subCandidates = candidates.mid(intervalFrom, intervalTo - intervalFrom - 1);
                        } else {
                            // reverse the order of the candidates.
                            qSwap(intervalVertexFrom, intervalVertexTo);
                            do {
                                ++intervalFrom;
                                subCandidates.prepend(candidates.at(intervalFrom - 1));
                            } while (intervalFrom < intervalTo - 1);
                        }
                        simplifySequentialChunk(&g, intervalVertexFrom, subCandidates, intervalVertexTo);
                        // finished simplification of chunk with same direction
                    }
                    if (forward == (prev == data->origin))
                        --intervalTo;
                    intervalFrom = intervalTo;

                    forward = !forward;
                }
                prev = v1;
            }
        }
        if (endOfSequence)
            candidates.clear();

        for (int i = 0; i < count; ++i) {
            AnchorVertex *next = vertices.at(i);
            if (next->m_item == q && next->m_edge == centerEdge)
                continue;
            if (visited.contains(next))
                continue;
            stack.push(next);
        }
        visited.insert(v);
    }
}

static void restoreSimplifiedAnchor(Graph<AnchorVertex, AnchorData> &g,
                                    AnchorData *edge,
                                    AnchorVertex *before,
                                    AnchorVertex *after)
{
    Q_ASSERT(edge->type != AnchorData::Normal);
#if 0
    static const char *anchortypes[] = {"Normal",
                                        "Sequential",
                                        "Parallel"};
    qDebug("Restoring %s edge.", anchortypes[int(edge->type)]);
#endif
    if (edge->type == AnchorData::Sequential) {
        SequentialAnchorData* seqEdge = static_cast<SequentialAnchorData*>(edge);
        // restore the sequential anchor
        AnchorVertex *prev = before;
        AnchorVertex *last = after;
        if (edge->origin != prev)
            qSwap(last, prev);

        for (int i = 0; i < seqEdge->m_edges.count(); ++i) {
            AnchorVertex *v1 = (i < seqEdge->m_children.count()) ? seqEdge->m_children.at(i) : last;
            AnchorData *data = seqEdge->m_edges.at(i);
            if (data->type != AnchorData::Normal) {
                restoreSimplifiedAnchor(g, data, prev, v1);
            } else {
                g.createEdge(prev, v1, data);
            }
            prev = v1;
        }
    } else if (edge->type == AnchorData::Parallel) {
        ParallelAnchorData* parallelEdge = static_cast<ParallelAnchorData*>(edge);
        AnchorData *parallelEdges[2] = {parallelEdge->firstEdge,
                                        parallelEdge->secondEdge};
        for (int i = 0; i < 2; ++i) {
            AnchorData *data = parallelEdges[i];
            if (data->type == AnchorData::Normal) {
                g.createEdge(before, after, data);
            } else {
                restoreSimplifiedAnchor(g, data, before, after);
            }
        }
    }
}

void QGraphicsAnchorLayoutPrivate::restoreSimplifiedGraph(Orientation orientation)
{
    Q_Q(QGraphicsAnchorLayout);
    Graph<AnchorVertex, AnchorData> &g = graph[orientation];

    QList<QPair<AnchorVertex*, AnchorVertex*> > connections = g.connections();
    for (int i = 0; i < connections.count(); ++i) {
        AnchorVertex *v1 = connections.at(i).first;
        AnchorVertex *v2 = connections.at(i).second;
        AnchorData *edge = g.edgeData(v1, v2);
        if (edge->type != AnchorData::Normal) {
            AnchorData *oldEdge = g.takeEdge(v1, v2);
            restoreSimplifiedAnchor(g, edge, v1, v2);
            delete oldEdge;
        }
    }
}

QGraphicsAnchorLayoutPrivate::Orientation
QGraphicsAnchorLayoutPrivate::edgeOrientation(QGraphicsAnchorLayout::Edge edge)
{
    return edge > QGraphicsAnchorLayout::Right ? Vertical : Horizontal;
}

/*!
  \internal

  Create internal anchors to connect the layout edges (Left to Right and
  Top to Bottom).

  These anchors doesn't have size restrictions, that will be enforced by
  other anchors and items in the layout.
*/
void QGraphicsAnchorLayoutPrivate::createLayoutEdges()
{
    Q_Q(QGraphicsAnchorLayout);
    QGraphicsLayoutItem *layout = q;

    // Horizontal
    AnchorData *data = new AnchorData(0, 0, QWIDGETSIZE_MAX);
    addAnchor(layout, QGraphicsAnchorLayout::Left, layout,
              QGraphicsAnchorLayout::Right, data);
    data->skipInPreferred = 1;

    // Set the Layout Left edge as the root of the horizontal graph.
    AnchorVertex *v = internalVertex(layout, QGraphicsAnchorLayout::Left);
    graph[Horizontal].setRootVertex(v);

    // Vertical
    data = new AnchorData(0, 0, QWIDGETSIZE_MAX);
    addAnchor(layout, QGraphicsAnchorLayout::Top, layout,
              QGraphicsAnchorLayout::Bottom, data);
    data->skipInPreferred = 1;

    // Set the Layout Top edge as the root of the vertical graph.
    v = internalVertex(layout, QGraphicsAnchorLayout::Top);
    graph[Vertical].setRootVertex(v);
}

void QGraphicsAnchorLayoutPrivate::deleteLayoutEdges()
{
    Q_Q(QGraphicsAnchorLayout);

    Q_ASSERT(internalVertex(q, QGraphicsAnchorLayout::HCenter) == NULL);
    Q_ASSERT(internalVertex(q, QGraphicsAnchorLayout::VCenter) == NULL);

    removeAnchor(q, QGraphicsAnchorLayout::Left, q, QGraphicsAnchorLayout::Right);
    removeAnchor(q, QGraphicsAnchorLayout::Top, q, QGraphicsAnchorLayout::Bottom);
}

void QGraphicsAnchorLayoutPrivate::createItemEdges(QGraphicsLayoutItem *item)
{
    items.append(item);

    // Horizontal
    int minimumSize = item->minimumWidth();
    int preferredSize = item->preferredWidth();
    int maximumSize = item->maximumWidth();

    AnchorData *data = new AnchorData(minimumSize, preferredSize, maximumSize);
    addAnchor(item, QGraphicsAnchorLayout::Left, item,
              QGraphicsAnchorLayout::Right, data);

    // Vertical
    minimumSize = item->minimumHeight();
    preferredSize = item->preferredHeight();
    maximumSize = item->maximumHeight();

    data = new AnchorData(minimumSize, preferredSize, maximumSize);
    addAnchor(item, QGraphicsAnchorLayout::Top, item,
              QGraphicsAnchorLayout::Bottom, data);
}

/*!
  \internal

  By default, each item in the layout is represented internally as
  a single anchor in each direction. For instance, from Left to Right.

  However, to support anchorage of items to the center of items, we
  must split this internal anchor into two half-anchors. From Left
  to Center and then from Center to Right, with the restriction that
  these anchors must have the same time at all times.
*/
void QGraphicsAnchorLayoutPrivate::createCenterAnchors(
    QGraphicsLayoutItem *item, QGraphicsAnchorLayout::Edge centerEdge)
{
    Orientation orientation;
    switch (centerEdge) {
    case QGraphicsAnchorLayout::HCenter:
        orientation = Horizontal;
        break;
    case QGraphicsAnchorLayout::VCenter:
        orientation = Vertical;
        break;
    default:
        // Don't create center edges unless needed
        return;
    }

    // Check if vertex already exists
    if (internalVertex(item, centerEdge))
        return;

    // Orientation code
    QGraphicsAnchorLayout::Edge firstEdge;
    QGraphicsAnchorLayout::Edge lastEdge;

    if (orientation == Horizontal) {
        firstEdge = QGraphicsAnchorLayout::Left;
        lastEdge = QGraphicsAnchorLayout::Right;
    } else {
        firstEdge = QGraphicsAnchorLayout::Top;
        lastEdge = QGraphicsAnchorLayout::Bottom;
    }

    AnchorVertex *first = internalVertex(item, firstEdge);
    AnchorVertex *last = internalVertex(item, lastEdge);
    Q_ASSERT(first && last);

    // Create new anchors
    AnchorData *oldData = graph[orientation].edgeData(first, last);

    int minimumSize = oldData->minSize / 2;
    int preferredSize = oldData->prefSize / 2;
    int maximumSize = oldData->maxSize / 2;

    QSimplexConstraint *c = new QSimplexConstraint;
    AnchorData *data = new AnchorData(minimumSize, preferredSize, maximumSize);
    c->variables.insert(data, 1.0);
    addAnchor(item, firstEdge, item, centerEdge, data);

    data = new AnchorData(minimumSize, preferredSize, maximumSize);
    c->variables.insert(data, -1.0);
    addAnchor(item, centerEdge, item, lastEdge, data);

    itemCenterConstraints[orientation].append(c);

    // Remove old one
    removeAnchor(item, firstEdge, item, lastEdge);
}

void QGraphicsAnchorLayoutPrivate::removeCenterAnchors(
    QGraphicsLayoutItem *item, QGraphicsAnchorLayout::Edge centerEdge,
    bool substitute)
{
    Orientation orientation;
    switch (centerEdge) {
    case QGraphicsAnchorLayout::HCenter:
        orientation = Horizontal;
        break;
    case QGraphicsAnchorLayout::VCenter:
        orientation = Vertical;
        break;
    default:
        // Don't remove edges that not the center ones
        return;
    }

    // Orientation code
    QGraphicsAnchorLayout::Edge firstEdge;
    QGraphicsAnchorLayout::Edge lastEdge;

    if (orientation == Horizontal) {
        firstEdge = QGraphicsAnchorLayout::Left;
        lastEdge = QGraphicsAnchorLayout::Right;
    } else {
        firstEdge = QGraphicsAnchorLayout::Top;
        lastEdge = QGraphicsAnchorLayout::Bottom;
    }

    AnchorVertex *center = internalVertex(item, centerEdge);
    if (!center)
        return;
    AnchorVertex *first = internalVertex(item, firstEdge);
    AnchorVertex *last = internalVertex(item, lastEdge);

    Q_ASSERT(first);
    Q_ASSERT(center);
    Q_ASSERT(last);

    Graph<AnchorVertex, AnchorData> &g = graph[orientation];


    AnchorData *oldData = g.edgeData(first, center);
    // Remove center constraint
    for (int i = itemCenterConstraints[orientation].count() - 1; i >= 0; --i) {
        if (itemCenterConstraints[orientation][i]->variables.contains(oldData)) {
            delete itemCenterConstraints[orientation].takeAt(i);
            break;
        }
    }

    if (substitute) {
        // Create the new anchor that should substitute the left-center-right anchors.
        AnchorData *oldData = g.edgeData(first, center);

        int minimumSize = oldData->minSize * 2;
        int preferredSize = oldData->prefSize * 2;
        int maximumSize = oldData->maxSize * 2;

        AnchorData *data = new AnchorData(minimumSize, preferredSize, maximumSize);
        addAnchor(item, firstEdge, item, lastEdge, data);

        // Remove old anchors
        removeAnchor(item, firstEdge, item, centerEdge);
        removeAnchor(item, centerEdge, item, lastEdge);

    } else {
        // this is only called from removeAnchors()
        // first, remove all non-internal anchors
        QList<AnchorVertex*> adjacents = g.adjacentVertices(center);
        for (int i = 0; i < adjacents.count(); ++i) {
            AnchorVertex *v = adjacents.at(i);
            if (v->m_item != item) {
                removeAnchor(item, centerEdge, v->m_item, v->m_edge);
            }
        }
        // when all non-internal anchors is removed it will automatically merge the
        // center anchor into a left-right (or top-bottom) anchor. We must also delete that.
        // by this time, the center vertex is deleted and merged into a non-centered internal anchor
        removeAnchor(item, firstEdge, item, lastEdge);
    }
}


void QGraphicsAnchorLayoutPrivate::removeCenterConstraints(QGraphicsLayoutItem *item,
                                                           Orientation orientation)
{
    // Remove the item center constraints associated to this item
    // ### This is a temporary solution. We should probably use a better
    // data structure to hold items and/or their associated constraints
    // so that we can remove those easily

    AnchorVertex *first = internalVertex(item, orientation == Horizontal ?
                                       QGraphicsAnchorLayout::Left :
                                       QGraphicsAnchorLayout::Top);
    AnchorVertex *center = internalVertex(item, orientation == Horizontal ?
                                        QGraphicsAnchorLayout::HCenter :
                                        QGraphicsAnchorLayout::VCenter);

    // Skip if no center constraints exist
    if (!center)
        return;

    Q_ASSERT(first);
    AnchorData *internalAnchor = graph[orientation].edgeData(first, center);

    // Look for our anchor in all item center constraints, then remove it
    for (int i = 0; i < itemCenterConstraints[orientation].size(); ++i) {
        if (itemCenterConstraints[orientation][i]->variables.contains(internalAnchor)) {
            delete itemCenterConstraints[orientation].takeAt(i);
            break;
        }
    }
}

/*!
 * \internal
 *
 * Helper function that is called from the anchor functions in the public API.
 * If \a spacing is 0, it will pick up the spacing defined by the style.
 */
void QGraphicsAnchorLayoutPrivate::anchor(QGraphicsLayoutItem *firstItem,
                                          QGraphicsAnchorLayout::Edge firstEdge,
                                          QGraphicsLayoutItem *secondItem,
                                          QGraphicsAnchorLayout::Edge secondEdge,
                                          qreal *spacing)
{
    Q_Q(QGraphicsAnchorLayout);
    if ((firstItem == 0) || (secondItem == 0)) {
        qWarning("QGraphicsAnchorLayout::anchor(): "
                 "Cannot anchor NULL items");
        return;
    }

    if (firstItem == secondItem) {
        qWarning("QGraphicsAnchorLayout::anchor(): "
                 "Cannot anchor the item to itself");
        return;
    }

    if (edgeOrientation(secondEdge) != edgeOrientation(firstEdge)) {
        qWarning("QGraphicsAnchorLayout::anchor(): "
                 "Cannot anchor edges of different orientations");
        return;
    }

    // In QGraphicsAnchorLayout, items are represented in its internal
    // graph as four anchors that connect:
    //  - Left -> HCenter
    //  - HCenter-> Right
    //  - Top -> VCenter
    //  - VCenter -> Bottom

    // Ensure that the internal anchors have been created for both items.
    if (firstItem != q && !items.contains(firstItem)) {
        createItemEdges(firstItem);
        addChildLayoutItem(firstItem);
    }
    if (secondItem != q && !items.contains(secondItem)) {
        createItemEdges(secondItem);
        addChildLayoutItem(secondItem);
    }

    // Create center edges if needed
    createCenterAnchors(firstItem, firstEdge);
    createCenterAnchors(secondItem, secondEdge);

    // Use heuristics to find out what the user meant with this anchor.
    correctEdgeDirection(firstItem, firstEdge, secondItem, secondEdge);

    AnchorData *data;
    if (!spacing) {
        // If firstItem or secondItem is the layout itself, the spacing will default to 0.
        // Otherwise, the following matrix is used (questionmark means that the spacing
        // is queried from the style):
        //                from
        //  to      Left    HCenter Right
        //  Left    0       0       ?
        //  HCenter 0       0       0
        //  Right   ?       0       0
        if (firstItem != q
            && secondItem != q
            && pickEdge(firstEdge, Horizontal) != QGraphicsAnchorLayout::HCenter
            && oppositeEdge(firstEdge) == secondEdge) {
            data = new AnchorData;      // ask the style later
        } else {
            data = new AnchorData(0);   // spacing should be 0
        }
        addAnchor(firstItem, firstEdge, secondItem, secondEdge, data);
    } else if (*spacing >= 0) {
        data = new AnchorData(*spacing);
        addAnchor(firstItem, firstEdge, secondItem, secondEdge, data);
    } else {
        data = new AnchorData(-*spacing);
        addAnchor(secondItem, secondEdge, firstItem, firstEdge, data);
    }
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

    // Remove previous anchor
    // ### Could we update the existing edgeData rather than creating a new one?
    if (graph[edgeOrientation(firstEdge)].edgeData(v1, v2))
        removeAnchor(firstItem, firstEdge, secondItem, secondEdge);

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

        if ((v.second == 2) &&
            ((edge == QGraphicsAnchorLayout::HCenter) ||
             (edge == QGraphicsAnchorLayout::VCenter))) {
            removeCenterAnchors(item, edge, true);
        }
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

    Q_ASSERT(v1 && v2);

    // Remove edge from graph
    graph[edgeOrientation(firstEdge)].removeEdge(v1, v2);

    // Decrease vertices reference count (may trigger a deletion)
    removeInternalVertex(firstItem, firstEdge);
    removeInternalVertex(secondItem, secondEdge);
}

void QGraphicsAnchorLayoutPrivate::removeVertex(QGraphicsLayoutItem *item, QGraphicsAnchorLayout::Edge edge)
{
    if (AnchorVertex *v = internalVertex(item, edge)) {
        Graph<AnchorVertex, AnchorData> &g = graph[edgeOrientation(edge)];
        const QList<AnchorVertex *> allVertices = graph[edgeOrientation(edge)].adjacentVertices(v);
        AnchorVertex *v2;
        foreach (v2, allVertices) {
            g.removeEdge(v, v2);
            removeInternalVertex(item, edge);
            removeInternalVertex(v2->m_item, v2->m_edge);
        }
    }
}

void QGraphicsAnchorLayoutPrivate::removeAnchors(QGraphicsLayoutItem *item)
{
    // remove the center anchor first!!
    removeCenterAnchors(item, QGraphicsAnchorLayout::HCenter, false);
    removeVertex(item, QGraphicsAnchorLayout::Left);
    removeVertex(item, QGraphicsAnchorLayout::Right);

    removeCenterAnchors(item, QGraphicsAnchorLayout::VCenter, false);
    removeVertex(item, QGraphicsAnchorLayout::Top);
    removeVertex(item, QGraphicsAnchorLayout::Bottom);

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

qreal QGraphicsAnchorLayoutPrivate::effectiveSpacing(Orientation orientation) const
{
    Q_Q(const QGraphicsAnchorLayout);
    qreal s = spacing[orientation];
    if (s < 0) {
        QGraphicsLayoutItem *parent = q->parentLayoutItem();
        while (parent && parent->isLayout()) {
            parent = parent->parentLayoutItem();
        }
        if (parent) {
            QGraphicsItem *parentItem = parent->graphicsItem();
            if (parentItem && parentItem->isWidget()) {
                QGraphicsWidget *w = static_cast<QGraphicsWidget*>(parentItem);
                s = w->style()->pixelMetric(orientation == Horizontal
                                            ? QStyle::PM_LayoutHorizontalSpacing
                                            : QStyle::PM_LayoutVerticalSpacing);
            }
        }
    }
    return s;
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

    //simplifyGraph(Horizontal);
    //simplifyGraph(Vertical);
    //Q_Q(QGraphicsAnchorLayout);
    //q->dumpGraph();
    //restoreSimplifiedGraph(Horizontal);    // should not be here, but currently crashes if not
    //restoreSimplifiedGraph(Vertical);    // should not be here, but currently crashes if not


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

/*!
  \internal

  Calculate graphs is the method that puts together all the helper routines
  so that the AnchorLayout can calculate the sizes of each item.

  In a nutshell it should do:

  1) Update anchor nominal sizes, that is, the size that each anchor would
     have if no other restrictions applied. This is done by quering the
     layout style and the sizeHints of the items belonging to the layout.

  2) Simplify the graph by grouping together parallel and sequential anchors
     into "group anchors". These have equivalent minimum, preferred and maximum
     sizeHints as the anchors they replace.

  3) Check if we got to a trivial case. In some cases, the whole graph can be
     simplified into a single anchor. If so, use this information. If not,
     then call the Simplex solver to calculate the anchors sizes.

  4) Once the root anchors had its sizes calculated, propagate that to the
     anchors they represent.
*/
void QGraphicsAnchorLayoutPrivate::calculateGraphs(
    QGraphicsAnchorLayoutPrivate::Orientation orientation)
{
    Q_Q(QGraphicsAnchorLayout);

    // Reset the nominal sizes of each anchor based on the current item sizes
    setAnchorSizeHintsFromItems(orientation);

    // Simplify the graph
    // ### Ideally we would like to do that if, and only if, anchors had
    //     been added or removed since the last time this method was called.
    //     However, as the two setAnchorSizeHints methods above are not
    //     ready to be run on top of a simplified graph, we must simplify
    //     and restore it every time we get here.
    //simplifyGraph(orientation);

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
    QList<QSimplexConstraint *> sizeHintConstraints;
    sizeHintConstraints = constraintsFromSizeHints(getVariables(trunkConstraints));
    trunkConstraints += sizeHintConstraints;

    // For minimum and maximum, use the path between the two layout sides as the
    // objective function.

    // Retrieve that path
    AnchorVertex *v = internalVertex(q, pickEdge(QGraphicsAnchorLayout::Right, orientation));
    GraphPath trunkPath = graphPaths[orientation].value(v);

    if (!trunkConstraints.isEmpty()) {
        qDebug("Simplex used for trunk of %s",
               orientation == Horizontal ? "Horizontal" : "Vertical");

        // Solve min and max size hints for trunk
        QPair<qreal, qreal> minMax = solveMinMax(trunkConstraints, trunkPath);
        sizeHints[orientation][Qt::MinimumSize] = minMax.first;
        sizeHints[orientation][Qt::MaximumSize] = minMax.second;

        // Solve for preferred. The objective function is calculated from the constraints
        // and variables internally.
        solvePreferred(trunkConstraints);

        // Propagate the new sizes down the simplified graph, ie. tell the
        // group anchors to set their children anchors sizes.

        // ### we calculated variables already a few times, can't we reuse that?
        QList<AnchorData *> trunkVariables = getVariables(trunkConstraints);

        for (int i = 0; i < trunkVariables.count(); ++i)
            trunkVariables.at(i)->updateChildrenSizes();

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
    } else {
        qDebug("Simplex NOT used for trunk of %s",
               orientation == Horizontal ? "Horizontal" : "Vertical");

        // No Simplex is necessary because the path was simplified all the way to a single
        // anchor.
        Q_ASSERT(trunkPath.positives.count() == 1);
        Q_ASSERT(trunkPath.negatives.count() == 0);

        AnchorData *ad = trunkPath.positives.toList()[0];
        ad->sizeAtMinimum = ad->minSize;
        ad->sizeAtPreferred = ad->prefSize;
        ad->sizeAtMaximum = ad->maxSize;

        // Propagate
        ad->updateChildrenSizes();

        sizeHints[orientation][Qt::MinimumSize] = ad->sizeAtMinimum;
        sizeHints[orientation][Qt::PreferredSize] = ad->sizeAtPreferred;
        sizeHints[orientation][Qt::MaximumSize] = ad->sizeAtMaximum;
    }

    // Delete the constraints, we won't use them anymore.
    qDeleteAll(sizeHintConstraints);
    sizeHintConstraints.clear();

    // For the other parts that not the trunk, solve only for the preferred size
    // that is the size they will remain at, since they are not stretched by the
    // layout.

    // Solve the other only for preferred, skip trunk
    for (int i = 1; i < parts.count(); ++i) {
        QList<QSimplexConstraint *> partConstraints = parts[i];
        QList<AnchorData *> partVariables = getVariables(partConstraints);
        Q_ASSERT(!partVariables.isEmpty());

        sizeHintConstraints = constraintsFromSizeHints(partVariables);
        partConstraints += sizeHintConstraints;
        solvePreferred(partConstraints);

        // Propagate size at preferred to other sizes. Semi-floats
        // always will be in their sizeAtPreferred.
        for (int j = 0; j < partVariables.count(); ++j) {
            AnchorData *ad = partVariables[j];
            Q_ASSERT(ad);
            ad->sizeAtMinimum = ad->sizeAtPreferred;
            ad->sizeAtMaximum = ad->sizeAtPreferred;
            ad->updateChildrenSizes();
        }

        // Delete the constraints, we won't use them anymore.
        qDeleteAll(sizeHintConstraints);
        sizeHintConstraints.clear();
    }

    // Clean up our data structures. They are not needed anymore since
    // distribution uses just interpolation.
    qDeleteAll(constraints[orientation]);
    constraints[orientation].clear();
    graphPaths[orientation].clear(); // ###
}

/*!
  \internal

  For graph edges ("anchors") that represent items, this method updates their
  intrinsic size restrictions, based on the item size hints.

  ################################################################################
  ### TODO: This method is not simplification ready. The fact that the graph may
            have been simplified means that not all anchors exist in the graph.
            This causes the Q_ASSERT(data) calls below to fail.

            A possible approach is to use a recursive method that goes through
            all edges in the graph updating their sizes based on their kind, i.e.:
             - Anchors -> update their size based on the style defaults
             - ItemAnchors -> update their size based on the item sizeHints
             - SimplificationAnchors -> call "update()" on its children and then
               calculate its own sizes
  ################################################################################
*/
void QGraphicsAnchorLayoutPrivate::setAnchorSizeHintsFromItems(Orientation orientation)
{
    Q_Q(QGraphicsAnchorLayout);
    Graph<AnchorVertex, AnchorData> &g = graph[orientation];
    QList<QPair<AnchorVertex*, AnchorVertex*> > conns = g.connections();
    QGraphicsAnchorLayout::Edge centerEdge = pickEdge(QGraphicsAnchorLayout::HCenter, orientation);

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

    for (int i = 0; i < conns.count(); ++i) {
        AnchorVertex *from = conns.at(i).first;
        AnchorVertex *to = conns.at(i).second;

        QGraphicsLayoutItem *item = from->m_item;
        qreal min, pref, max;

        AnchorData *data = g.edgeData(from, to);
        Q_ASSERT(data);
        // Internal item anchor
        if (item != q && item == to->m_item) {
            // internal item anchor: get size from sizeHint
            if (orientation == Horizontal) {
                min = item->minimumWidth();
                pref = item->preferredWidth();
                max = item->maximumWidth();
            } else {
                min = item->minimumHeight();
                pref = item->preferredHeight();
                max = item->maximumHeight();
            }

            if (from->m_edge == centerEdge || to->m_edge == centerEdge) {
                min /= 2;
                pref /= 2;
                max /= 2;
            }
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
        } else if (data->type != AnchorData::Normal) {
            data->refreshSizeHints();
        } else {
            // anchors between items, their sizes may depend on the style.
            if (!data->hasSize) {
                qreal s = effectiveSpacing(orientation);
                data->minSize = s;
                data->prefSize = s;
                data->maxSize = s;
                data->sizeAtMinimum = s;
                data->sizeAtPreferred = s;
                data->sizeAtMaximum = s;
            }
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

    AnchorVertex *root = graph[orientation].rootVertex();

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
  \internal

  Create LP constraints for each anchor based on its minimum and maximum
  sizes, as specified in its size hints
*/
QList<QSimplexConstraint *> QGraphicsAnchorLayoutPrivate::constraintsFromSizeHints(
    const QList<AnchorData *> &anchors)
{
    QList<QSimplexConstraint *> anchorConstraints;
    for (int i = 0; i < anchors.size(); ++i) {
        QSimplexConstraint *c = new QSimplexConstraint;
        c->variables.insert(anchors[i], 1.0);
        c->constant = anchors[i]->minSize;
        c->ratio = QSimplexConstraint::MoreOrEqual;
        anchorConstraints += c;

        c = new QSimplexConstraint;
        c->variables.insert(anchors[i], 1.0);
        c->constant = anchors[i]->maxSize;
        c->ratio = QSimplexConstraint::LessOrEqual;
        anchorConstraints += c;
    }

    return anchorConstraints;
}

/*!
  \Internal
*/
QList< QList<QSimplexConstraint *> >
QGraphicsAnchorLayoutPrivate::getGraphParts(Orientation orientation)
{
    Q_Q(QGraphicsAnchorLayout);

    // Find layout vertices and edges for the current orientation.
    AnchorVertex *layoutFirstVertex = \
        internalVertex(q, pickEdge(QGraphicsAnchorLayout::Left, orientation));

    AnchorVertex *layoutCentralVertex = \
        internalVertex(q, pickEdge(QGraphicsAnchorLayout::HCenter, orientation));

    AnchorVertex *layoutLastVertex = \
        internalVertex(q, pickEdge(QGraphicsAnchorLayout::Right, orientation));

    Q_ASSERT(layoutFirstVertex && layoutLastVertex);

    AnchorData *edgeL1 = NULL;
    AnchorData *edgeL2 = NULL;

    // The layout may have a single anchor between Left and Right or two half anchors
    // passing through the center
    if (layoutCentralVertex) {
        edgeL1 = graph[orientation].edgeData(layoutFirstVertex, layoutCentralVertex);
        edgeL2 = graph[orientation].edgeData(layoutCentralVertex, layoutLastVertex);
    } else {
        edgeL1 = graph[orientation].edgeData(layoutFirstVertex, layoutLastVertex);
    }

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
    if (edgeL2)
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
                // Note that we don't erase the constraint if it's not
                // a match, since in a next iteration of a do-while we
                // can pass on it again and it will be a match.
                //
                // For example: if trunk share a variable with
                // remainingConstraints[1] and it shares with
                // remainingConstraints[0], we need a second iteration
                // of the do-while loop to match both.
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
    AnchorVertex *root = graph[orientation].rootVertex();

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
    QSimplex simplex;
    simplex.setConstraints(constraints);

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
    QList<QSimplexVariable *> variables = simplex.constraintsVariables();
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

    return qMakePair<qreal, qreal>(min, max);
}

void QGraphicsAnchorLayoutPrivate::solvePreferred(QList<QSimplexConstraint *> constraints)
{
    QList<AnchorData *> variables = getVariables(constraints);
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


    QSimplex *simplex = new QSimplex;
    simplex->setConstraints(constraints + preferredConstraints);
    simplex->setObjective(&objective);

    // Calculate minimum values
    simplex->solveMin();

    // Save sizeAtPreferred results
    for (int i = 0; i < variables.size(); ++i) {
        AnchorData *ad = static_cast<AnchorData *>(variables[i]);
        ad->sizeAtPreferred = ad->result;
    }

    // Make sure we delete the simplex solver -before- we delete the
    // constraints used by it.
    delete simplex;

    // Delete constraints and variables we created.
    qDeleteAll(preferredConstraints);
    qDeleteAll(preferredVariables);
}

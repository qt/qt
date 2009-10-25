/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui/qwidget.h>
#include <QtCore/qlinkedlist.h>
#include <QtCore/qstack.h>

#ifdef QT_DEBUG
#include <QtCore/qfile.h>
#endif

#include "qgraphicsanchorlayout_p.h"

QT_BEGIN_NAMESPACE


QGraphicsAnchorPrivate::QGraphicsAnchorPrivate(int version)
    : QObjectPrivate(version), layoutPrivate(0), data(0),
      sizePolicy(QSizePolicy::Fixed)
{
}

QGraphicsAnchorPrivate::~QGraphicsAnchorPrivate()
{
    layoutPrivate->removeAnchor(data->from, data->to);
}

void QGraphicsAnchorPrivate::setSizePolicy(QSizePolicy::Policy policy)
{
    if (sizePolicy != policy) {
        sizePolicy = policy;
        layoutPrivate->q_func()->invalidate();
    }
}

void QGraphicsAnchorPrivate::setSpacing(qreal value)
{
    if (data) {
        layoutPrivate->setAnchorSize(data, &value);
    } else {
        qWarning("QGraphicsAnchor::setSpacing: The anchor does not exist.");
    }
}

void QGraphicsAnchorPrivate::unsetSpacing()
{
    if (data) {
        layoutPrivate->setAnchorSize(data, 0);
    } else {
        qWarning("QGraphicsAnchor::setSpacing: The anchor does not exist.");
    }
}

qreal QGraphicsAnchorPrivate::spacing() const
{
    qreal size = 0;
    if (data) {
        layoutPrivate->anchorSize(data, 0, &size, 0);
    } else {
        qWarning("QGraphicsAnchor::setSpacing: The anchor does not exist.");
    }
    return size;
}


static void internalSizeHints(QSizePolicy::Policy policy,
                              qreal minSizeHint, qreal prefSizeHint, qreal maxSizeHint,
                              qreal *minSize, qreal *prefSize,
                              qreal *expSize, qreal *maxSize)
{
    // minSize, prefSize and maxSize are initialized
    // with item's preferred Size: this is QSizePolicy::Fixed.
    //
    // Then we check each flag to find the resultant QSizePolicy,
    // according to the following table:
    //
    //      constant               value
    // QSizePolicy::Fixed            0
    // QSizePolicy::Minimum       GrowFlag
    // QSizePolicy::Maximum       ShrinkFlag
    // QSizePolicy::Preferred     GrowFlag | ShrinkFlag
    // QSizePolicy::Ignored       GrowFlag | ShrinkFlag | IgnoreFlag

    if (policy & QSizePolicy::ShrinkFlag)
        *minSize = minSizeHint;
    else
        *minSize = prefSizeHint;

    if (policy & QSizePolicy::GrowFlag)
        *maxSize = maxSizeHint;
    else
        *maxSize = prefSizeHint;

    // Note that these two initializations are affected by the previous flags
    if (policy & QSizePolicy::IgnoreFlag)
        *prefSize = *minSize;
    else
        *prefSize = prefSizeHint;

    if (policy & QSizePolicy::ExpandFlag)
        *expSize = *maxSize;
    else
        *expSize = *prefSize;
}

void AnchorData::refreshSizeHints(qreal effectiveSpacing)
{
    const bool isInternalAnchor = from->m_item == to->m_item;

    QSizePolicy::Policy policy;
    qreal minSizeHint;
    qreal prefSizeHint;
    qreal maxSizeHint;

    if (isInternalAnchor) {
        const QGraphicsAnchorLayoutPrivate::Orientation orient =
            QGraphicsAnchorLayoutPrivate::edgeOrientation(from->m_edge);
        const Qt::AnchorPoint centerEdge =
            QGraphicsAnchorLayoutPrivate::pickEdge(Qt::AnchorHorizontalCenter, orient);
        bool hasCenter = (from->m_edge == centerEdge || to->m_edge == centerEdge);

        if (isLayoutAnchor) {
            minSize = 0;
            prefSize = 0;
            expSize = 0;
            maxSize = QWIDGETSIZE_MAX;
            if (hasCenter)
                maxSize /= 2;
            return;
        } else {

            QGraphicsLayoutItem *item = from->m_item;
            if (orient == QGraphicsAnchorLayoutPrivate::Horizontal) {
                policy = item->sizePolicy().horizontalPolicy();
                minSizeHint = item->effectiveSizeHint(Qt::MinimumSize).width();
                prefSizeHint = item->effectiveSizeHint(Qt::PreferredSize).width();
                maxSizeHint = item->effectiveSizeHint(Qt::MaximumSize).width();
            } else {
                policy = item->sizePolicy().verticalPolicy();
                minSizeHint = item->effectiveSizeHint(Qt::MinimumSize).height();
                prefSizeHint = item->effectiveSizeHint(Qt::PreferredSize).height();
                maxSizeHint = item->effectiveSizeHint(Qt::MaximumSize).height();
            }

            if (hasCenter) {
                minSizeHint /= 2;
                prefSizeHint /= 2;
                maxSizeHint /= 2;
            }
        }
    } else {
        Q_ASSERT(graphicsAnchor);
        policy = graphicsAnchor->sizePolicy();
        minSizeHint = 0;
        if (hasSize) {
            // One can only configure the preferred size of a normal anchor. Their minimum and
            // maximum "size hints" are always 0 and QWIDGETSIZE_MAX, correspondingly. However,
            // their effective size hints might be narrowed down due to their size policies.
            prefSizeHint = prefSize;
        } else {
            prefSizeHint = effectiveSpacing;
        }
        maxSizeHint = QWIDGETSIZE_MAX;
    }
    internalSizeHints(policy, minSizeHint, prefSizeHint, maxSizeHint, 
                      &minSize, &prefSize, &expSize, &maxSize);

    // Set the anchor effective sizes to preferred.
    //
    // Note: The idea here is that all items should remain at their
    // preferred size unless where that's impossible.  In cases where
    // the item is subject to restrictions (anchored to the layout
    // edges, for instance), the simplex solver will be run to
    // recalculate and override the values we set here.
    sizeAtMinimum = prefSize;
    sizeAtPreferred = prefSize;
    sizeAtExpanding = prefSize;
    sizeAtMaximum = prefSize;
}

void ParallelAnchorData::updateChildrenSizes()
{
    firstEdge->sizeAtMinimum = secondEdge->sizeAtMinimum = sizeAtMinimum;
    firstEdge->sizeAtPreferred = secondEdge->sizeAtPreferred = sizeAtPreferred;
    firstEdge->sizeAtExpanding = secondEdge->sizeAtExpanding = sizeAtExpanding;
    firstEdge->sizeAtMaximum = secondEdge->sizeAtMaximum = sizeAtMaximum;

    firstEdge->updateChildrenSizes();
    secondEdge->updateChildrenSizes();
}

void ParallelAnchorData::refreshSizeHints(qreal effectiveSpacing)
{
    refreshSizeHints_helper(effectiveSpacing);
}

void ParallelAnchorData::refreshSizeHints_helper(qreal effectiveSpacing,
                                                 bool refreshChildren)
{
    if (refreshChildren) {
        firstEdge->refreshSizeHints(effectiveSpacing);
        secondEdge->refreshSizeHints(effectiveSpacing);
    }

    // ### should we warn if the parallel connection is invalid?
    // e.g. 1-2-3 with 10-20-30, the minimum of the latter is
    // bigger than the maximum of the former.

    minSize = qMax(firstEdge->minSize, secondEdge->minSize);
    maxSize = qMin(firstEdge->maxSize, secondEdge->maxSize);

    expSize = qMax(firstEdge->expSize, secondEdge->expSize);
    expSize = qMin(expSize, maxSize);

    prefSize = qMax(firstEdge->prefSize, secondEdge->prefSize);
    prefSize = qMin(prefSize, expSize);

    // See comment in AnchorData::refreshSizeHints() about sizeAt* values
    sizeAtMinimum = prefSize;
    sizeAtPreferred = prefSize;
    sizeAtExpanding = prefSize;
    sizeAtMaximum = prefSize;
}

/*!
    \internal
    returns the factor in the interval [-1, 1].
    -1 is at Minimum
     0 is at Preferred
     1 is at Maximum
*/
static QPair<QGraphicsAnchorLayoutPrivate::Interval, qreal> getFactor(qreal value, qreal min,
                                                                      qreal pref, qreal exp,
                                                                      qreal max)
{
    QGraphicsAnchorLayoutPrivate::Interval interval;
    qreal lower;
    qreal upper;

    if (value < pref) {
        interval = QGraphicsAnchorLayoutPrivate::MinToPreferred;
        lower = min;
        upper = pref;
    } else if (value < exp) {
        interval = QGraphicsAnchorLayoutPrivate::PreferredToExpanding;
        lower = pref;
        upper = exp;
    } else {
        interval = QGraphicsAnchorLayoutPrivate::ExpandingToMax;
        lower = exp;
        upper = max;
    }

    qreal progress;
    if (upper == lower) {
        progress = 0;
    } else {
        progress = (value - lower) / (upper - lower);
    }

    return qMakePair(interval, progress);
}

static qreal interpolate(const QPair<QGraphicsAnchorLayoutPrivate::Interval, qreal> &factor,
                         qreal min, qreal pref,
                         qreal exp, qreal max)
{
    qreal lower;
    qreal upper;

    switch (factor.first) {
    case QGraphicsAnchorLayoutPrivate::MinToPreferred:
        lower = min;
        upper = pref;
        break;
    case QGraphicsAnchorLayoutPrivate::PreferredToExpanding:
        lower = pref;
        upper = exp;
        break;
    case QGraphicsAnchorLayoutPrivate::ExpandingToMax:
        lower = exp;
        upper = max;
        break;
    }

    return lower + factor.second * (upper - lower);
}

void SequentialAnchorData::updateChildrenSizes()
{
    // ### REMOVE ME
    // ### check whether we are guarantee to get those or we need to warn stuff at this
    // point.
    Q_ASSERT(sizeAtMinimum > minSize || qFuzzyCompare(sizeAtMinimum, minSize));
    Q_ASSERT(sizeAtMinimum < maxSize || qFuzzyCompare(sizeAtMinimum, maxSize));
    Q_ASSERT(sizeAtPreferred > minSize || qFuzzyCompare(sizeAtPreferred, minSize));
    Q_ASSERT(sizeAtPreferred < maxSize || qFuzzyCompare(sizeAtPreferred, maxSize));
    Q_ASSERT(sizeAtExpanding > minSize || qFuzzyCompare(sizeAtExpanding, minSize));
    Q_ASSERT(sizeAtExpanding < maxSize || qFuzzyCompare(sizeAtExpanding, maxSize));
    Q_ASSERT(sizeAtMaximum > minSize || qFuzzyCompare(sizeAtMaximum, minSize));
    Q_ASSERT(sizeAtMaximum < maxSize || qFuzzyCompare(sizeAtMaximum, maxSize));

    // Band here refers if the value is in the Minimum To Preferred
    // band (the lower band) or the Preferred To Maximum (the upper band).

    const QPair<QGraphicsAnchorLayoutPrivate::Interval, qreal> minFactor =
        getFactor(sizeAtMinimum, minSize, prefSize, expSize, maxSize);
    const QPair<QGraphicsAnchorLayoutPrivate::Interval, qreal> prefFactor =
        getFactor(sizeAtPreferred, minSize, prefSize, expSize, maxSize);
    const QPair<QGraphicsAnchorLayoutPrivate::Interval, qreal> expFactor =
        getFactor(sizeAtExpanding, minSize, prefSize, expSize, maxSize);
    const QPair<QGraphicsAnchorLayoutPrivate::Interval, qreal> maxFactor =
        getFactor(sizeAtMaximum, minSize, prefSize, expSize, maxSize);

    for (int i = 0; i < m_edges.count(); ++i) {
        AnchorData *e = m_edges.at(i);

        e->sizeAtMinimum = interpolate(minFactor, e->minSize, e->prefSize, e->expSize, e->maxSize);
        e->sizeAtPreferred = interpolate(prefFactor, e->minSize, e->prefSize, e->expSize, e->maxSize);
        e->sizeAtExpanding = interpolate(expFactor, e->minSize, e->prefSize, e->expSize, e->maxSize);
        e->sizeAtMaximum = interpolate(maxFactor, e->minSize, e->prefSize, e->expSize, e->maxSize);

        e->updateChildrenSizes();
    }
}

void SequentialAnchorData::refreshSizeHints(qreal effectiveSpacing)
{
    refreshSizeHints_helper(effectiveSpacing);
}

void SequentialAnchorData::refreshSizeHints_helper(qreal effectiveSpacing,
                                                   bool refreshChildren)
{
    minSize = 0;
    prefSize = 0;
    expSize = 0;
    maxSize = 0;

    for (int i = 0; i < m_edges.count(); ++i) {
        AnchorData *edge = m_edges.at(i);

        // If it's the case refresh children information first
        if (refreshChildren)
            edge->refreshSizeHints(effectiveSpacing);

        minSize += edge->minSize;
        prefSize += edge->prefSize;
        expSize += edge->expSize;
        maxSize += edge->maxSize;
    }

    // See comment in AnchorData::refreshSizeHints() about sizeAt* values
    sizeAtMinimum = prefSize;
    sizeAtPreferred = prefSize;
    sizeAtExpanding = prefSize;
    sizeAtMaximum = prefSize;
}

#ifdef QT_DEBUG
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

#endif

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

#ifdef QT_DEBUG
QString GraphPath::toString() const
{
    QString string(QLatin1String("Path: "));
    foreach(AnchorData *edge, positives)
        string += QString::fromAscii(" (+++) %1").arg(edge->toString());

    foreach(AnchorData *edge, negatives)
        string += QString::fromAscii(" (---) %1").arg(edge->toString());

    return string;
}
#endif

QGraphicsAnchorLayoutPrivate::QGraphicsAnchorLayoutPrivate()
    : calculateGraphCacheDirty(1)
{
    for (int i = 0; i < NOrientations; ++i) {
        for (int j = 0; j < 3; ++j) {
            sizeHints[i][j] = -1;
        }
        sizeAtExpanding[i] = -1;
        interpolationProgress[i] = -1;

        spacings[i] = -1;
        graphSimplified[i] = false;
        graphHasConflicts[i] = false;
    }
}

Qt::AnchorPoint QGraphicsAnchorLayoutPrivate::oppositeEdge(Qt::AnchorPoint edge)
{
    switch (edge) {
    case Qt::AnchorLeft:
        edge = Qt::AnchorRight;
        break;
    case Qt::AnchorRight:
        edge = Qt::AnchorLeft;
        break;
    case Qt::AnchorTop:
        edge = Qt::AnchorBottom;
        break;
    case Qt::AnchorBottom:
        edge = Qt::AnchorTop;
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
static bool simplifySequentialChunk(Graph<AnchorVertex, AnchorData> *graph,
                                    AnchorVertex *before,
                                    const QVector<AnchorVertex*> &vertices,
                                    AnchorVertex *after)
{
    AnchorData *data = graph->edgeData(before, vertices.first());
    Q_ASSERT(data);

    const bool forward = (before == data->from);
    QVector<AnchorVertex *> orderedVertices;

    if (forward) {
        orderedVertices = vertices;
    } else {
        qSwap(before, after);
        for (int i = vertices.count() - 1; i >= 0; --i)
            orderedVertices.append(vertices.at(i));
    }

#if defined(QT_DEBUG) && 0
    QString strVertices;
    for (int i = 0; i < orderedVertices.count(); ++i) {
        strVertices += QString::fromAscii("%1 - ").arg(orderedVertices.at(i)->toString());
    }
    QString strPath = QString::fromAscii("%1 - %2%3").arg(before->toString(), strVertices, after->toString());
    qDebug("simplifying [%s] to [%s - %s]", qPrintable(strPath), qPrintable(before->toString()), qPrintable(after->toString()));
#endif

    SequentialAnchorData *sequence = new SequentialAnchorData;
    AnchorVertex *prev = before;

    for (int i = 0; i <= orderedVertices.count(); ++i) {
        AnchorVertex *next = (i < orderedVertices.count()) ? orderedVertices.at(i) : after;
        AnchorData *ad = graph->takeEdge(prev, next);
        Q_ASSERT(ad);
        sequence->m_edges.append(ad);
        prev = next;
    }

    sequence->setVertices(orderedVertices);
    sequence->from = before;
    sequence->to = after;

    sequence->refreshSizeHints_helper(0, false);

    // Note that since layout 'edges' can't be simplified away from
    // the graph, it's safe to assume that if there's a layout
    // 'edge', it'll be in the boundaries of the sequence.
    sequence->isLayoutAnchor = (sequence->m_edges.first()->isLayoutAnchor
                                || sequence->m_edges.last()->isLayoutAnchor);

    AnchorData *newAnchor = sequence;
    if (AnchorData *oldAnchor = graph->takeEdge(before, after)) {
        ParallelAnchorData *parallel = new ParallelAnchorData(oldAnchor, sequence);
        parallel->isLayoutAnchor = (oldAnchor->isLayoutAnchor
                                     || sequence->isLayoutAnchor);
        parallel->refreshSizeHints_helper(0, false);
        newAnchor = parallel;
    }
    graph->createEdge(before, after, newAnchor);

    // True if we created a parallel anchor
    return newAnchor != sequence;
}

/*!
   \internal

   The purpose of this function is to simplify the graph.
   Simplification serves two purposes:
   1. Reduce the number of edges in the graph, (thus the number of variables to the equation
      solver is reduced, and the solver performs better).
   2. Be able to do distribution of sequences of edges more intelligently (esp. with sequential
      anchors)

   It is essential that it must be possible to restore simplified anchors back to their "original"
   form. This is done by restoreSimplifiedAnchor().

   There are two types of simplification that can be done:
   1. Sequential simplification
      Sequential simplification means that all sequences of anchors will be merged into one single
      anchor. Only anhcors that points in the same direction will be merged.
   2. Parallel simplification
      If a simplified sequential anchor is about to be inserted between two vertices in the graph
      and there already exist an anchor between those two vertices, a parallel anchor will be
      created that serves as a placeholder for the sequential anchor and the anchor that was
      already between the two vertices.

   The process of simplification can be described as:

   1. Simplify all sequences of anchors into one anchor.
      If no further simplification was done, go to (3)
      - If there already exist an anchor where the sequential anchor is supposed to be inserted,
        take that anchor out of the graph
      - Then create a parallel anchor that holds the sequential anchor and the anchor just taken
        out of the graph.
   2. Go to (1)
   3. Done

*/
void QGraphicsAnchorLayoutPrivate::simplifyGraph(Orientation orientation)
{
    static bool noSimplification = !qgetenv("QT_ANCHORLAYOUT_NO_SIMPLIFICATION").isEmpty();
    if (noSimplification || items.isEmpty())
        return;

    if (graphSimplified[orientation])
        return;
    graphSimplified[orientation] = true;

#if 0
    qDebug("Simplifying Graph for %s",
           orientation == Horizontal ? "Horizontal" : "Vertical");
#endif

    if (!graph[orientation].rootVertex())
        return;

    bool dirty;
    do {
        dirty = simplifyGraphIteration(orientation);
    } while (dirty);
}

/*!
    \internal

    One iteration of the simplification algorithm. Returns true if another iteration is needed.

    The algorithm walks the graph in depth-first order, and only collects vertices that has two
    edges connected to it.  If the vertex does not have two edges or if it is a layout edge, it
    will take all the previously collected vertices and try to create a simplified sequential
    anchor representing all the previously collected vertices.  Once the simplified anchor is
    inserted, the collected list is cleared in order to find the next sequence to simplify.

    Note that there are some catches to this that are not covered by the above explanation, see
    the function comments for more details.
*/
bool QGraphicsAnchorLayoutPrivate::simplifyGraphIteration(QGraphicsAnchorLayoutPrivate::Orientation orientation)
{
    Q_Q(QGraphicsAnchorLayout);
    Graph<AnchorVertex, AnchorData> &g = graph[orientation];

    QSet<AnchorVertex *> visited;
    QStack<QPair<AnchorVertex *, AnchorVertex *> > stack;
    stack.push(qMakePair(static_cast<AnchorVertex *>(0), g.rootVertex()));
    QVector<AnchorVertex*> candidates;
    bool candidatesForward;

    const Qt::AnchorPoint centerEdge = pickEdge(Qt::AnchorHorizontalCenter, orientation);

    // Walk depth-first, in the stack we store start of the candidate sequence (beforeSequence)
    // and the vertex to be visited.
    while (!stack.isEmpty()) {
        QPair<AnchorVertex *, AnchorVertex *> pair = stack.pop();
        AnchorVertex *beforeSequence = pair.first;
        AnchorVertex *v = pair.second;

        // The basic idea is to determine whether we found an end of sequence,
        // if that's the case, we stop adding vertices to the candidate list
        // and do a simplification step.
        //
        // A vertex can trigger an end of sequence if
        // (a) it is a layout vertex, we don't simplify away the layout vertices;
        // (b) it does not have exactly 2 adjacents;
        // (c) it will change the direction of the sequence;
        // (d) its next adjacent is already visited (a cycle in the graph).

        const QList<AnchorVertex *> &adjacents = g.adjacentVertices(v);
        const bool isLayoutVertex = v->m_item == q;
        AnchorVertex *afterSequence = v;
        bool endOfSequence = false;

        //
        // Identify the end cases.
        //

        // Identifies cases (a) and (b)
        endOfSequence = isLayoutVertex || adjacents.count() != 2;

        if (!endOfSequence) {
            // If this is the first vertice, determine what is the direction to use for this
            // sequence.
            if (candidates.isEmpty()) {
                const AnchorData *data = g.edgeData(beforeSequence, v);
                Q_ASSERT(data);
                candidatesForward = (beforeSequence == data->from);
            }

            // This is a tricky part. We peek at the next vertex to find out
            //
            // - whether the edge from this vertex to the next vertex has the same direction;
            // - whether we already visited the next vertex.
            //
            // Those are needed to identify (c) and (d). Note that unlike (a) and (b), we preempt
            // the end of sequence by looking into the next vertex.

            // Peek at the next vertex
            AnchorVertex *after;
            if (candidates.isEmpty())
                after = (beforeSequence == adjacents.last() ? adjacents.first() : adjacents.last());
            else
                after = (candidates.last() == adjacents.last() ? adjacents.first() : adjacents.last());

            // ### At this point we assumed that candidates will not contain 'after', this may not hold
            // when simplifying FLOATing anchors.
            Q_ASSERT(!candidates.contains(after));

            const AnchorData *data = g.edgeData(v, after);
            Q_ASSERT(data);
            const bool willChangeDirection = (candidatesForward != (v == data->from));
            const bool cycleFound = visited.contains(after);

            // Now cases (c) and (d)...
            endOfSequence = willChangeDirection || cycleFound;

            if (endOfSequence) {
                if (!willChangeDirection) {
                    // If the direction will not change, we can add the current vertex to the
                    // candidates list and we know that 'after' can be used as afterSequence.
                    candidates.append(v);
                    afterSequence = after;
                }
            } else {
                // If it's not an end of sequence, then the vertex didn't trigger neither of the
                // previously four cases, so it can be added to the candidates list.
                candidates.append(v);
            }
        }

        //
        // Add next non-visited vertices to the stack.
        //
        for (int i = 0; i < adjacents.count(); ++i) {
            AnchorVertex *next = adjacents.at(i);
            if (visited.contains(next))
                continue;

            // If current vertex is an end of sequence, and it'll reset the candidates list. So
            // the next vertices will build candidates lists with the current vertex as 'before'
            // vertex. If it's not an end of sequence, we keep the original 'before' vertex,
            // since we are keeping the candidates list.
            if (endOfSequence)
                stack.push(qMakePair(v, next));
            else
                stack.push(qMakePair(beforeSequence, next));
        }

        visited.insert(v);

        if (!endOfSequence || candidates.isEmpty())
            continue;

        //
        // Create a sequence for (beforeSequence, candidates, afterSequence).
        //

        // One restriction we have is to not simplify half of an anchor and let the other half
        // unsimplified. So we remove center edges before and after the sequence.
        if (beforeSequence->m_edge == centerEdge && beforeSequence->m_item == candidates.first()->m_item) {
            beforeSequence = candidates.first();
            candidates.remove(0);

            // If there's not candidates to be simplified, leave.
            if (candidates.isEmpty())
                continue;
        }

        if (afterSequence->m_edge == centerEdge && afterSequence->m_item == candidates.last()->m_item) {
            afterSequence = candidates.last();
            candidates.remove(candidates.count() - 1);

            if (candidates.isEmpty())
                continue;
        }

        // This function will remove the candidates from the graph and create one edge between
        // beforeSequence and afterSequence. This function returns true if the sequential
        // simplification also caused a parallel simplification to be created. In this case we end
        // the iteration and start again (since all the visited state we have may be outdated).
        if (simplifySequentialChunk(&g, beforeSequence, candidates, afterSequence))
            return true;

        // If there was no parallel simplification, we'll keep walking the graph. So we clear the
        // candidates list to start again.
        candidates.clear();
    }

    return false;
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
        if (edge->from != prev)
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
    if (!graphSimplified[orientation])
        return;
    graphSimplified[orientation] = false;

#if 0
    qDebug("Restoring Simplified Graph for %s",
           orientation == Horizontal ? "Horizontal" : "Vertical");
#endif

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
QGraphicsAnchorLayoutPrivate::edgeOrientation(Qt::AnchorPoint edge)
{
    return edge > Qt::AnchorRight ? Vertical : Horizontal;
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
    AnchorData *data = new AnchorData;
    addAnchor_helper(layout, Qt::AnchorLeft, layout,
                     Qt::AnchorRight, data);
    data->maxSize = QWIDGETSIZE_MAX;
    data->skipInPreferred = 1;

    // Set the Layout Left edge as the root of the horizontal graph.
    AnchorVertex *v = internalVertex(layout, Qt::AnchorLeft);
    graph[Horizontal].setRootVertex(v);

    // Vertical
    data = new AnchorData;
    addAnchor_helper(layout, Qt::AnchorTop, layout,
                     Qt::AnchorBottom, data);
    data->maxSize = QWIDGETSIZE_MAX;
    data->skipInPreferred = 1;

    // Set the Layout Top edge as the root of the vertical graph.
    v = internalVertex(layout, Qt::AnchorTop);
    graph[Vertical].setRootVertex(v);
}

void QGraphicsAnchorLayoutPrivate::deleteLayoutEdges()
{
    Q_Q(QGraphicsAnchorLayout);

    Q_ASSERT(internalVertex(q, Qt::AnchorHorizontalCenter) == NULL);
    Q_ASSERT(internalVertex(q, Qt::AnchorVerticalCenter) == NULL);

    removeAnchor_helper(internalVertex(q, Qt::AnchorLeft),
                        internalVertex(q, Qt::AnchorRight));
    removeAnchor_helper(internalVertex(q, Qt::AnchorTop),
                        internalVertex(q, Qt::AnchorBottom));
}

void QGraphicsAnchorLayoutPrivate::createItemEdges(QGraphicsLayoutItem *item)
{
    Q_ASSERT(!graphSimplified[Horizontal] && !graphSimplified[Vertical]);

    items.append(item);

    // Create horizontal and vertical internal anchors for the item and
    // refresh its size hint / policy values.
    AnchorData *data = new AnchorData;
    addAnchor_helper(item, Qt::AnchorLeft, item, Qt::AnchorRight, data);
    data->refreshSizeHints(0); // 0 = effectiveSpacing, will not be used

    data = new AnchorData;
    addAnchor_helper(item, Qt::AnchorTop, item, Qt::AnchorBottom, data);
    data->refreshSizeHints(0); // 0 = effectiveSpacing, will not be used
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
    QGraphicsLayoutItem *item, Qt::AnchorPoint centerEdge)
{
    Orientation orientation;
    switch (centerEdge) {
    case Qt::AnchorHorizontalCenter:
        orientation = Horizontal;
        break;
    case Qt::AnchorVerticalCenter:
        orientation = Vertical;
        break;
    default:
        // Don't create center edges unless needed
        return;
    }

    Q_ASSERT(!graphSimplified[orientation]);

    // Check if vertex already exists
    if (internalVertex(item, centerEdge))
        return;

    // Orientation code
    Qt::AnchorPoint firstEdge;
    Qt::AnchorPoint lastEdge;

    if (orientation == Horizontal) {
        firstEdge = Qt::AnchorLeft;
        lastEdge = Qt::AnchorRight;
    } else {
        firstEdge = Qt::AnchorTop;
        lastEdge = Qt::AnchorBottom;
    }

    AnchorVertex *first = internalVertex(item, firstEdge);
    AnchorVertex *last = internalVertex(item, lastEdge);
    Q_ASSERT(first && last);

    // Create new anchors
    QSimplexConstraint *c = new QSimplexConstraint;

    AnchorData *data = new AnchorData;
    c->variables.insert(data, 1.0);
    addAnchor_helper(item, firstEdge, item, centerEdge, data);
    data->refreshSizeHints(0);

    data = new AnchorData;
    c->variables.insert(data, -1.0);
    addAnchor_helper(item, centerEdge, item, lastEdge, data);
    data->refreshSizeHints(0);

    itemCenterConstraints[orientation].append(c);

    // Remove old one
    removeAnchor_helper(first, last);
}

void QGraphicsAnchorLayoutPrivate::removeCenterAnchors(
    QGraphicsLayoutItem *item, Qt::AnchorPoint centerEdge,
    bool substitute)
{
    Orientation orientation;
    switch (centerEdge) {
    case Qt::AnchorHorizontalCenter:
        orientation = Horizontal;
        break;
    case Qt::AnchorVerticalCenter:
        orientation = Vertical;
        break;
    default:
        // Don't remove edges that not the center ones
        return;
    }

    Q_ASSERT(!graphSimplified[orientation]);

    // Orientation code
    Qt::AnchorPoint firstEdge;
    Qt::AnchorPoint lastEdge;

    if (orientation == Horizontal) {
        firstEdge = Qt::AnchorLeft;
        lastEdge = Qt::AnchorRight;
    } else {
        firstEdge = Qt::AnchorTop;
        lastEdge = Qt::AnchorBottom;
    }

    AnchorVertex *center = internalVertex(item, centerEdge);
    if (!center)
        return;
    AnchorVertex *first = internalVertex(item, firstEdge);

    Q_ASSERT(first);
    Q_ASSERT(center);

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
        AnchorData *data = new AnchorData;
        addAnchor_helper(item, firstEdge, item, lastEdge, data);
        data->refreshSizeHints(0);

        // Remove old anchors
        removeAnchor_helper(first, center);
        removeAnchor_helper(center, internalVertex(item, lastEdge));

    } else {
        // this is only called from removeAnchors()
        // first, remove all non-internal anchors
        QList<AnchorVertex*> adjacents = g.adjacentVertices(center);
        for (int i = 0; i < adjacents.count(); ++i) {
            AnchorVertex *v = adjacents.at(i);
            if (v->m_item != item) {
                removeAnchor_helper(center, internalVertex(v->m_item, v->m_edge));
            }
        }
        // when all non-internal anchors is removed it will automatically merge the
        // center anchor into a left-right (or top-bottom) anchor. We must also delete that.
        // by this time, the center vertex is deleted and merged into a non-centered internal anchor
        removeAnchor_helper(first, internalVertex(item, lastEdge));
    }
}


void QGraphicsAnchorLayoutPrivate::removeCenterConstraints(QGraphicsLayoutItem *item,
                                                           Orientation orientation)
{
    Q_ASSERT(!graphSimplified[orientation]);

    // Remove the item center constraints associated to this item
    // ### This is a temporary solution. We should probably use a better
    // data structure to hold items and/or their associated constraints
    // so that we can remove those easily

    AnchorVertex *first = internalVertex(item, orientation == Horizontal ?
                                       Qt::AnchorLeft :
                                       Qt::AnchorTop);
    AnchorVertex *center = internalVertex(item, orientation == Horizontal ?
                                        Qt::AnchorHorizontalCenter :
                                        Qt::AnchorVerticalCenter);

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
QGraphicsAnchor *QGraphicsAnchorLayoutPrivate::addAnchor(QGraphicsLayoutItem *firstItem,
                                                      Qt::AnchorPoint firstEdge,
                                                      QGraphicsLayoutItem *secondItem,
                                                      Qt::AnchorPoint secondEdge,
                                                      qreal *spacing)
{
    Q_Q(QGraphicsAnchorLayout);
    if ((firstItem == 0) || (secondItem == 0)) {
        qWarning("QGraphicsAnchorLayout::addAnchor(): "
                 "Cannot anchor NULL items");
        return 0;
    }

    if (firstItem == secondItem) {
        qWarning("QGraphicsAnchorLayout::addAnchor(): "
                 "Cannot anchor the item to itself");
        return 0;
    }

    if (edgeOrientation(secondEdge) != edgeOrientation(firstEdge)) {
        qWarning("QGraphicsAnchorLayout::addAnchor(): "
                 "Cannot anchor edges of different orientations");
        return 0;
    }

    // Guarantee that the graph is no simplified when adding this anchor,
    // anchor manipulation always happen in the full graph
    restoreSimplifiedGraph(edgeOrientation(firstEdge));

    // In QGraphicsAnchorLayout, items are represented in its internal
    // graph as four anchors that connect:
    //  - Left -> HCenter
    //  - HCenter-> Right
    //  - Top -> VCenter
    //  - VCenter -> Bottom

    // Ensure that the internal anchors have been created for both items.
    if (firstItem != q && !items.contains(firstItem)) {
        restoreSimplifiedGraph(edgeOrientation(firstEdge) == Horizontal ? Vertical : Horizontal);
        createItemEdges(firstItem);
        addChildLayoutItem(firstItem);
    }
    if (secondItem != q && !items.contains(secondItem)) {
        restoreSimplifiedGraph(edgeOrientation(firstEdge) == Horizontal ? Vertical : Horizontal);
        createItemEdges(secondItem);
        addChildLayoutItem(secondItem);
    }

    // Create center edges if needed
    createCenterAnchors(firstItem, firstEdge);
    createCenterAnchors(secondItem, secondEdge);

    // Use heuristics to find out what the user meant with this anchor.
    correctEdgeDirection(firstItem, firstEdge, secondItem, secondEdge);

    AnchorData *data = new AnchorData;
    if (!spacing) {
        // If firstItem or secondItem is the layout itself, the spacing will default to 0.
        // Otherwise, the following matrix is used (questionmark means that the spacing
        // is queried from the style):
        //                from
        //  to      Left    HCenter Right
        //  Left    0       0       ?
        //  HCenter 0       0       0
        //  Right   ?       0       0
        if (firstItem == q
            || secondItem == q
            || pickEdge(firstEdge, Horizontal) == Qt::AnchorHorizontalCenter
            || oppositeEdge(firstEdge) != secondEdge) {
            data->setPreferredSize(0);
        } else {
            data->unsetSize();
        }
        addAnchor_helper(firstItem, firstEdge, secondItem, secondEdge, data);

    } else if (*spacing >= 0) {
        data->setPreferredSize(*spacing);
        addAnchor_helper(firstItem, firstEdge, secondItem, secondEdge, data);

    } else {
        data->setPreferredSize(-*spacing);
        addAnchor_helper(secondItem, secondEdge, firstItem, firstEdge, data);
    }

    return acquireGraphicsAnchor(data);
}

void QGraphicsAnchorLayoutPrivate::addAnchor_helper(QGraphicsLayoutItem *firstItem,
                                             Qt::AnchorPoint firstEdge,
                                             QGraphicsLayoutItem *secondItem,
                                             Qt::AnchorPoint secondEdge,
                                             AnchorData *data)
{
    Q_Q(QGraphicsAnchorLayout);

    // Guarantee that the graph is no simplified when adding this anchor,
    // anchor manipulation always happen in the full graph
    restoreSimplifiedGraph(edgeOrientation(firstEdge));

    // Is the Vertex (firstItem, firstEdge) already represented in our
    // internal structure?
    AnchorVertex *v1 = addInternalVertex(firstItem, firstEdge);
    AnchorVertex *v2 = addInternalVertex(secondItem, secondEdge);

    // Remove previous anchor
    // ### Could we update the existing edgeData rather than creating a new one?
    if (graph[edgeOrientation(firstEdge)].edgeData(v1, v2)) {
        removeAnchor_helper(v1, v2);
    }

    // Create a bi-directional edge in the sense it can be transversed both
    // from v1 or v2. "data" however is shared between the two references
    // so we still know that the anchor direction is from 1 to 2.
    data->from = v1;
    data->to = v2;
#ifdef QT_DEBUG
    data->name = QString::fromAscii("%1 --to--> %2").arg(v1->toString()).arg(v2->toString());
#endif
    // Keep track of anchors that are connected to the layout 'edges'
    data->isLayoutAnchor = (v1->m_item == q || v2->m_item == q);

    graph[edgeOrientation(firstEdge)].createEdge(v1, v2, data);
}

QGraphicsAnchor *QGraphicsAnchorLayoutPrivate::getAnchor(QGraphicsLayoutItem *firstItem,
                                                         Qt::AnchorPoint firstEdge,
                                                         QGraphicsLayoutItem *secondItem,
                                                         Qt::AnchorPoint secondEdge)
{
    Orientation orient = edgeOrientation(firstEdge);
    restoreSimplifiedGraph(orient);

    AnchorVertex *v1 = internalVertex(firstItem, firstEdge);
    AnchorVertex *v2 = internalVertex(secondItem, secondEdge);

    QGraphicsAnchor *graphicsAnchor = 0;

    AnchorData *data = graph[orient].edgeData(v1, v2);
    if (data)
        graphicsAnchor = acquireGraphicsAnchor(data);
    return graphicsAnchor;
}

/*!
 * \internal
 *
 * Implements the high level "removeAnchor" feature. Called by
 * the QAnchorData destructor.
 */
void QGraphicsAnchorLayoutPrivate::removeAnchor(AnchorVertex *firstVertex,
                                                AnchorVertex *secondVertex)
{
    Q_Q(QGraphicsAnchorLayout);

    // Actually delete the anchor
    removeAnchor_helper(firstVertex, secondVertex);

    QGraphicsLayoutItem *firstItem = firstVertex->m_item;
    QGraphicsLayoutItem *secondItem = secondVertex->m_item;

    // Checking if the item stays in the layout or not
    bool keepFirstItem = false;
    bool keepSecondItem = false;

    QPair<AnchorVertex *, int> v;
    int refcount = -1;

    if (firstItem != q) {
        for (int i = Qt::AnchorLeft; i <= Qt::AnchorBottom; ++i) {
            v = m_vertexList.value(qMakePair(firstItem, static_cast<Qt::AnchorPoint>(i)));
            if (v.first) {
                if (i == Qt::AnchorHorizontalCenter || i == Qt::AnchorVerticalCenter)
                    refcount = 2;
                else
                    refcount = 1;

                if (v.second > refcount) {
                    keepFirstItem = true;
                    break;
                }
            }
        }
    } else
        keepFirstItem = true;

    if (secondItem != q) {
        for (int i = Qt::AnchorLeft; i <= Qt::AnchorBottom; ++i) {
            v = m_vertexList.value(qMakePair(secondItem, static_cast<Qt::AnchorPoint>(i)));
            if (v.first) {
                if (i == Qt::AnchorHorizontalCenter || i == Qt::AnchorVerticalCenter)
                    refcount = 2;
                else
                    refcount = 1;

                if (v.second > refcount) {
                    keepSecondItem = true;
                    break;
                }
            }
        }
    } else
        keepSecondItem = true;

    if (!keepFirstItem)
        q->removeAt(items.indexOf(firstItem));

    if (!keepSecondItem)
        q->removeAt(items.indexOf(secondItem));

    // Removing anchors invalidates the layout
    q->invalidate();
}

/*
  \internal

  Implements the low level "removeAnchor" feature. Called by
  private methods.
*/
void QGraphicsAnchorLayoutPrivate::removeAnchor_helper(AnchorVertex *v1, AnchorVertex *v2)
{
    Q_ASSERT(v1 && v2);
    // Guarantee that the graph is no simplified when removing this anchor,
    // anchor manipulation always happen in the full graph
    Orientation o = edgeOrientation(v1->m_edge);
    restoreSimplifiedGraph(o);

    // Remove edge from graph
    graph[o].removeEdge(v1, v2);

    // Decrease vertices reference count (may trigger a deletion)
    removeInternalVertex(v1->m_item, v1->m_edge);
    removeInternalVertex(v2->m_item, v2->m_edge);
}

/*!
    \internal
    Only called from outside. (calls invalidate())
*/
void QGraphicsAnchorLayoutPrivate::setAnchorSize(AnchorData *data, const qreal *anchorSize)
{
    Q_Q(QGraphicsAnchorLayout);
    // ### we can avoid restoration if we really want to, but we would have to
    // search recursively through all composite anchors
    Q_ASSERT(data);
    restoreSimplifiedGraph(edgeOrientation(data->from->m_edge));

    QGraphicsLayoutItem *firstItem = data->from->m_item;
    QGraphicsLayoutItem *secondItem = data->to->m_item;
    Qt::AnchorPoint firstEdge = data->from->m_edge;
    Qt::AnchorPoint secondEdge = data->to->m_edge;

    // Use heuristics to find out what the user meant with this anchor.
    correctEdgeDirection(firstItem, firstEdge, secondItem, secondEdge);
    if (data->from->m_item != firstItem)
        qSwap(data->from, data->to);

    if (anchorSize) {
        // ### The current implementation makes "setAnchorSize" behavior
        //     dependent on the argument order for cases where we have
        //     no heuristic. Ie. two widgets, same anchor point.

        // We cannot have negative sizes inside the graph. This would cause
        // the simplex solver to fail because all simplex variables are
        // positive by definition.
        // "negative spacing" is handled by inverting the standard item order.
        if (*anchorSize >= 0) {
            data->setPreferredSize(*anchorSize);
        } else {
            data->setPreferredSize(-*anchorSize);
            qSwap(data->from, data->to);
        }
    } else {
        data->unsetSize();
    }
    q->invalidate();
}

void QGraphicsAnchorLayoutPrivate::anchorSize(const AnchorData *data,
                                              qreal *minSize,
                                              qreal *prefSize,
                                              qreal *maxSize) const
{
    Q_ASSERT(minSize || prefSize || maxSize);
    Q_ASSERT(data);
    QGraphicsAnchorLayoutPrivate *that = const_cast<QGraphicsAnchorLayoutPrivate *>(this);
    that->restoreSimplifiedGraph(edgeOrientation(data->from->m_edge));

    if (minSize)
        *minSize = data->minSize;
    if (prefSize)
        *prefSize = data->prefSize;
    if (maxSize)
        *maxSize = data->maxSize;
}

AnchorVertex *QGraphicsAnchorLayoutPrivate::addInternalVertex(QGraphicsLayoutItem *item,
                                                              Qt::AnchorPoint edge)
{
    QPair<QGraphicsLayoutItem *, Qt::AnchorPoint> pair(item, edge);
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
                                                        Qt::AnchorPoint edge)
{
    QPair<QGraphicsLayoutItem *, Qt::AnchorPoint> pair(item, edge);
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
            ((edge == Qt::AnchorHorizontalCenter) ||
             (edge == Qt::AnchorVerticalCenter))) {
            removeCenterAnchors(item, edge, true);
        }
    }
}

void QGraphicsAnchorLayoutPrivate::removeVertex(QGraphicsLayoutItem *item, Qt::AnchorPoint edge)
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
    Q_ASSERT(!graphSimplified[Horizontal] && !graphSimplified[Vertical]);

    // remove the center anchor first!!
    removeCenterAnchors(item, Qt::AnchorHorizontalCenter, false);
    removeVertex(item, Qt::AnchorLeft);
    removeVertex(item, Qt::AnchorRight);

    removeCenterAnchors(item, Qt::AnchorVerticalCenter, false);
    removeVertex(item, Qt::AnchorTop);
    removeVertex(item, Qt::AnchorBottom);
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
                                                        Qt::AnchorPoint &firstEdge,
                                                        QGraphicsLayoutItem *&secondItem,
                                                        Qt::AnchorPoint &secondEdge)
{
    Q_Q(QGraphicsAnchorLayout);

    if ((firstItem != q) && (secondItem != q)) {
        // If connection is between widgets (not the layout itself)
        // Ensure that "right-edges" sit to the left of "left-edges".
        if (firstEdge < secondEdge) {
            qSwap(firstItem, secondItem);
            qSwap(firstEdge, secondEdge);
        }
    } else if (firstItem == q) {
        // If connection involves the right or bottom of a layout, ensure
        // the layout is the second item.
        if ((firstEdge == Qt::AnchorRight) || (firstEdge == Qt::AnchorBottom)) {
            qSwap(firstItem, secondItem);
            qSwap(firstEdge, secondEdge);
        }
    } else if ((secondEdge != Qt::AnchorRight) && (secondEdge != Qt::AnchorBottom)) {
        // If connection involves the left, center or top of layout, ensure
        // the layout is the first item.
        qSwap(firstItem, secondItem);
        qSwap(firstEdge, secondEdge);
    }
}

qreal QGraphicsAnchorLayoutPrivate::effectiveSpacing(Orientation orientation) const
{
    Q_Q(const QGraphicsAnchorLayout);
    qreal s = spacings[orientation];
    if (s < 0) {
        // ### make sure behaviour is the same as in QGraphicsGridLayout
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

    // ### Currently we do not support negative anchors inside the graph.
    // To avoid those being created by a negative style spacing, we must
    // make this test.
    if (s < 0)
        s = 0;

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

#if defined(QT_DEBUG) && 0
    static int count = 0;
    count++;
    dumpGraph(QString::fromAscii("%1-before").arg(count));
#endif

    calculateGraphs(Horizontal);
    calculateGraphs(Vertical);

#if defined(QT_DEBUG) && 0
    dumpGraph(QString::fromAscii("%1-after").arg(count));
#endif

    calculateGraphCacheDirty = 0;
}

// ### Maybe getGraphParts could return the variables when traversing, at least
// for trunk...
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

    // Simplify the graph
    simplifyGraph(orientation);

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
    QList<QList<QSimplexConstraint *> > parts = getGraphParts(orientation);

    // Now run the simplex solver to calculate Minimum, Preferred and Maximum sizes
    // of the "trunk" set of constraints and variables.
    // ### does trunk always exist? empty = trunk is the layout left->center->right
    QList<QSimplexConstraint *> trunkConstraints = parts[0];
    QList<AnchorData *> trunkVariables = getVariables(trunkConstraints);

    // For minimum and maximum, use the path between the two layout sides as the
    // objective function.
    AnchorVertex *v = internalVertex(q, pickEdge(Qt::AnchorRight, orientation));
    GraphPath trunkPath = graphPaths[orientation].value(v);

    bool feasible = calculateTrunk(orientation, trunkPath, trunkConstraints, trunkVariables);

    // For the other parts that not the trunk, solve only for the preferred size
    // that is the size they will remain at, since they are not stretched by the
    // layout.

    // Skipping the first (trunk)
    for (int i = 1; i < parts.count(); ++i) {
        if (!feasible)
            break;

        QList<QSimplexConstraint *> partConstraints = parts[i];
        QList<AnchorData *> partVariables = getVariables(partConstraints);
        Q_ASSERT(!partVariables.isEmpty());
        feasible &= calculateNonTrunk(partConstraints, partVariables);
    }

    // Propagate the new sizes down the simplified graph, ie. tell the
    // group anchors to set their children anchors sizes.
    updateAnchorSizes(orientation);

    graphHasConflicts[orientation] = !feasible;

    // Clean up our data structures. They are not needed anymore since
    // distribution uses just interpolation.
    qDeleteAll(constraints[orientation]);
    constraints[orientation].clear();
    graphPaths[orientation].clear(); // ###
}

/*!
    \internal

    Calculate the sizes for all anchors which are part of the trunk. This works
    on top of a (possibly) simplified graph.
*/
bool QGraphicsAnchorLayoutPrivate::calculateTrunk(Orientation orientation, const GraphPath &path,
                                                  const QList<QSimplexConstraint *> &constraints,
                                                  const QList<AnchorData *> &variables)
{
    bool feasible = true;
    bool needsSimplex = !constraints.isEmpty();

#if 0
    qDebug("Simplex %s for trunk of %s", needsSimplex ? "used" : "NOT used",
           orientation == Horizontal ? "Horizontal" : "Vertical");
#endif

    if (needsSimplex) {

        QList<QSimplexConstraint *> sizeHintConstraints = constraintsFromSizeHints(variables);
        QList<QSimplexConstraint *> allConstraints = constraints + sizeHintConstraints;

        // Solve min and max size hints
        qreal min, max;
        feasible = solveMinMax(allConstraints, path, &min, &max);

        if (feasible) {
            solvePreferred(allConstraints, variables);

            // Note that we don't include the sizeHintConstraints, since they
            // have a different logic for solveExpanding().
            solveExpanding(constraints, variables);

            // Calculate and set the preferred and expanding sizes for the layout,
            // from the edge sizes that were calculated above.
            qreal pref(0.0);
            qreal expanding(0.0);
            foreach (const AnchorData *ad, path.positives) {
                pref += ad->sizeAtPreferred;
                expanding += ad->sizeAtExpanding;
            }
            foreach (const AnchorData *ad, path.negatives) {
                pref -= ad->sizeAtPreferred;
                expanding -= ad->sizeAtExpanding;
            }

            sizeHints[orientation][Qt::MinimumSize] = min;
            sizeHints[orientation][Qt::PreferredSize] = pref;
            sizeHints[orientation][Qt::MaximumSize] = max;
            sizeAtExpanding[orientation] = expanding;
        }

        qDeleteAll(sizeHintConstraints);

    } else {
        // No Simplex is necessary because the path was simplified all the way to a single
        // anchor.
        Q_ASSERT(path.positives.count() == 1);
        Q_ASSERT(path.negatives.count() == 0);

        AnchorData *ad = path.positives.toList()[0];
        ad->sizeAtMinimum = ad->minSize;
        ad->sizeAtPreferred = ad->prefSize;
        ad->sizeAtExpanding = ad->expSize;
        ad->sizeAtMaximum = ad->maxSize;

        sizeHints[orientation][Qt::MinimumSize] = ad->sizeAtMinimum;
        sizeHints[orientation][Qt::PreferredSize] = ad->sizeAtPreferred;
        sizeHints[orientation][Qt::MaximumSize] = ad->sizeAtMaximum;
        sizeAtExpanding[orientation] = ad->sizeAtExpanding;
    }

#if defined(QT_DEBUG) || defined(Q_AUTOTEST_EXPORT)
    lastCalculationUsedSimplex[orientation] = needsSimplex;
#endif

    return feasible;
}

/*!
    \internal
*/
bool QGraphicsAnchorLayoutPrivate::calculateNonTrunk(const QList<QSimplexConstraint *> &constraints,
                                                     const QList<AnchorData *> &variables)
{
    QList<QSimplexConstraint *> sizeHintConstraints = constraintsFromSizeHints(variables);
    bool feasible = solvePreferred(constraints + sizeHintConstraints, variables);

    if (feasible) {
        // Propagate size at preferred to other sizes. Semi-floats always will be
        // in their sizeAtPreferred.
        for (int j = 0; j < variables.count(); ++j) {
            AnchorData *ad = variables[j];
            Q_ASSERT(ad);
            ad->sizeAtMinimum = ad->sizeAtPreferred;
            ad->sizeAtExpanding = ad->sizeAtPreferred;
            ad->sizeAtMaximum = ad->sizeAtPreferred;
        }
    }

    qDeleteAll(sizeHintConstraints);
    return feasible;
}

/*!
  \internal

  For graph edges ("anchors") that represent items, this method updates their
  intrinsic size restrictions, based on the item size hints.
*/
void QGraphicsAnchorLayoutPrivate::setAnchorSizeHintsFromItems(Orientation orientation)
{
    Graph<AnchorVertex, AnchorData> &g = graph[orientation];
    QList<QPair<AnchorVertex *, AnchorVertex *> > vertices = g.connections();

    qreal spacing = effectiveSpacing(orientation);

    for (int i = 0; i < vertices.count(); ++i) {
        AnchorData *data = g.edgeData(vertices.at(i).first, vertices.at(i).second);;
        Q_ASSERT(data->from && data->to);
        data->refreshSizeHints(spacing);
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

        if (edge->from == pair.first)
            current.positives.insert(edge);
        else
            current.negatives.insert(edge);

        graphPaths[orientation].insert(pair.second, current);

        foreach (AnchorVertex *v,
                graph[orientation].adjacentVertices(pair.second)) {
            queue.enqueue(qMakePair(pair.second, v));
        }
    }

    // We will walk through every reachable items (non-float) store them in a temporary set.
    // We them create a set of all items and subtract the non-floating items from the set in
    // order to get the floating items. The floating items is then stored in m_floatItems
    identifyFloatItems(visited, orientation);
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
*/
void QGraphicsAnchorLayoutPrivate::updateAnchorSizes(Orientation orientation)
{
    Graph<AnchorVertex, AnchorData> &g = graph[orientation];
    const QList<QPair<AnchorVertex *, AnchorVertex *> > &vertices = g.connections();

    for (int i = 0; i < vertices.count(); ++i) {
        AnchorData *ad = g.edgeData(vertices.at(i).first, vertices.at(i).second);
        ad->updateChildrenSizes();
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
  \internal
*/
QList< QList<QSimplexConstraint *> >
QGraphicsAnchorLayoutPrivate::getGraphParts(Orientation orientation)
{
    Q_Q(QGraphicsAnchorLayout);

    // Find layout vertices and edges for the current orientation.
    AnchorVertex *layoutFirstVertex = \
        internalVertex(q, pickEdge(Qt::AnchorLeft, orientation));

    AnchorVertex *layoutCentralVertex = \
        internalVertex(q, pickEdge(Qt::AnchorHorizontalCenter, orientation));

    AnchorVertex *layoutLastVertex = \
        internalVertex(q, pickEdge(Qt::AnchorRight, orientation));

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

  Use all visited Anchors on findPaths() so we can identify non-float Items.
*/
void QGraphicsAnchorLayoutPrivate::identifyFloatItems(const QSet<AnchorData *> &visited, Orientation orientation)
{
    QSet<QGraphicsLayoutItem *> nonFloating;

    foreach (const AnchorData *ad, visited)
        identifyNonFloatItems_helper(ad, &nonFloating);

    QSet<QGraphicsLayoutItem *> allItems;
    foreach (QGraphicsLayoutItem *item, items)
        allItems.insert(item);
    m_floatItems[orientation] = allItems - nonFloating;
}


/*!
 \internal

  Given an anchor, if it is an internal anchor and Normal we must mark it's item as non-float.
  If the anchor is Sequential or Parallel, we must iterate on its children recursively until we reach
  internal anchors (items).
*/
void QGraphicsAnchorLayoutPrivate::identifyNonFloatItems_helper(const AnchorData *ad, QSet<QGraphicsLayoutItem *> *nonFloatingItemsIdentifiedSoFar)
{
    Q_Q(QGraphicsAnchorLayout);

    switch(ad->type) {
    case AnchorData::Normal:
        if (ad->from->m_item == ad->to->m_item && ad->to->m_item != q)
            nonFloatingItemsIdentifiedSoFar->insert(ad->to->m_item);
        break;
    case AnchorData::Sequential:
        foreach (const AnchorData *d, static_cast<const SequentialAnchorData *>(ad)->m_edges)
            identifyNonFloatItems_helper(d, nonFloatingItemsIdentifiedSoFar);
        break;
    case AnchorData::Parallel:
        identifyNonFloatItems_helper(static_cast<const ParallelAnchorData *>(ad)->firstEdge, nonFloatingItemsIdentifiedSoFar);
        identifyNonFloatItems_helper(static_cast<const ParallelAnchorData *>(ad)->secondEdge, nonFloatingItemsIdentifiedSoFar);
        break;
    }
}

/*!
  \internal

  Use the current vertices distance to calculate and set the geometry of
  each item.
*/
void QGraphicsAnchorLayoutPrivate::setItemsGeometries(const QRectF &geom)
{
    Q_Q(QGraphicsAnchorLayout);
    AnchorVertex *firstH, *secondH, *firstV, *secondV;

    qreal top;
    qreal left;
    qreal right;

    q->getContentsMargins(&left, &top, &right, 0);
    const Qt::LayoutDirection visualDir = visualDirection();
    if (visualDir == Qt::RightToLeft)
        qSwap(left, right);

    left += geom.left();
    top += geom.top();
    right = geom.right() - right;

    foreach (QGraphicsLayoutItem *item, items) {
        QRectF newGeom;
        QSizeF itemPreferredSize = item->effectiveSizeHint(Qt::PreferredSize);
        if (m_floatItems[Horizontal].contains(item)) {
            newGeom.setLeft(0);
            newGeom.setRight(itemPreferredSize.width());
        } else {
            firstH = internalVertex(item, Qt::AnchorLeft);
            secondH = internalVertex(item, Qt::AnchorRight);

            if (visualDir == Qt::LeftToRight) {
                newGeom.setLeft(left + firstH->distance);
                newGeom.setRight(left + secondH->distance);
            } else {
                newGeom.setLeft(right - secondH->distance);
                newGeom.setRight(right - firstH->distance);
            }
        }

        if (m_floatItems[Vertical].contains(item)) {
            newGeom.setTop(0);
            newGeom.setBottom(itemPreferredSize.height());
        } else {
            firstV = internalVertex(item, Qt::AnchorTop);
            secondV = internalVertex(item, Qt::AnchorBottom);

            newGeom.setTop(top + firstV->distance);
            newGeom.setBottom(top + secondV->distance);
        }

        item->setGeometry(newGeom);
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
    QQueue<QPair<AnchorVertex *, AnchorVertex *> > queue;
    QSet<AnchorVertex *> visited;

    // Get root vertex
    AnchorVertex *root = graph[orientation].rootVertex();

    root->distance = 0;
    visited.insert(root);

    // Add initial edges to the queue
    foreach (AnchorVertex *v, graph[orientation].adjacentVertices(root)) {
        queue.enqueue(qMakePair(root, v));
    }

    // Do initial calculation required by "interpolateEdge()"
    setupEdgesInterpolation(orientation);

    // Traverse the graph and calculate vertex positions, we need to
    // visit all pairs since each of them could have a sequential
    // anchor inside, which hides more vertices.
    while (!queue.isEmpty()) {
        QPair<AnchorVertex *, AnchorVertex *> pair = queue.dequeue();
        AnchorData *edge = graph[orientation].edgeData(pair.first, pair.second);

        // Both vertices were interpolated, and the anchor itself can't have other
        // anchors inside (it's not a complex anchor).
        if (edge->type == AnchorData::Normal && visited.contains(pair.second))
            continue;

        visited.insert(pair.second);
        interpolateEdge(pair.first, edge, orientation);

        QList<AnchorVertex *> adjacents = graph[orientation].adjacentVertices(pair.second);
        for (int i = 0; i < adjacents.count(); ++i) {
            if (!visited.contains(adjacents.at(i)))
                queue.enqueue(qMakePair(pair.second, adjacents.at(i)));
        }
    }
}

/*!
  \internal

  Calculate interpolation parameters based on current Layout Size.
  Must be called once before calling "interpolateEdgeSize()" for
  the edges.
*/
void QGraphicsAnchorLayoutPrivate::setupEdgesInterpolation(
    Orientation orientation)
{
    Q_Q(QGraphicsAnchorLayout);

    qreal current;
    current = (orientation == Horizontal) ? q->contentsRect().width() : q->contentsRect().height();

    QPair<Interval, qreal> result;
    result = getFactor(current,
                       sizeHints[orientation][Qt::MinimumSize],
                       sizeHints[orientation][Qt::PreferredSize],
                       sizeAtExpanding[orientation],
                       sizeHints[orientation][Qt::MaximumSize]);

    interpolationInterval[orientation] = result.first;
    interpolationProgress[orientation] = result.second;
}

/*!
  \internal

  Calculate the current Edge size based on the current Layout size and the
  size the edge is supposed to have when the layout is at its:

   - minimum size,
   - preferred size,
   - size when all expanding anchors are expanded,
   - maximum size.

   These three key values are calculated in advance using linear
   programming (more expensive) or the simplification algorithm, then
   subsequential resizes of the parent layout require a simple
   interpolation.

   If the edge is sequential or parallel, it's possible to have more
   vertices to be initalized, so it calls specialized functions that
   will recurse back to interpolateEdge().
 */
void QGraphicsAnchorLayoutPrivate::interpolateEdge(AnchorVertex *base,
                                                   AnchorData *edge,
                                                   Orientation orientation)
{
    const QPair<Interval, qreal> factor(interpolationInterval[orientation],
                                        interpolationProgress[orientation]);

    qreal edgeDistance = interpolate(factor, edge->sizeAtMinimum, edge->sizeAtPreferred,
                                     edge->sizeAtExpanding, edge->sizeAtMaximum);

    Q_ASSERT(edge->from == base || edge->to == base);

    if (edge->from == base)
        edge->to->distance = base->distance + edgeDistance;
    else
        edge->from->distance = base->distance - edgeDistance;

    // Process child anchors
    if (edge->type == AnchorData::Sequential)
        interpolateSequentialEdges(edge->from,
                                   static_cast<SequentialAnchorData *>(edge),
                                   orientation);
    else if (edge->type == AnchorData::Parallel)
        interpolateParallelEdges(edge->from,
                                 static_cast<ParallelAnchorData *>(edge),
                                 orientation);
}

void QGraphicsAnchorLayoutPrivate::interpolateParallelEdges(
    AnchorVertex *base, ParallelAnchorData *data, Orientation orientation)
{
    // In parallels the boundary vertices are already calculate, we
    // just need to look for sequential groups inside, because only
    // them may have new vertices associated.

    // First edge
    if (data->firstEdge->type == AnchorData::Sequential)
        interpolateSequentialEdges(base,
                                   static_cast<SequentialAnchorData *>(data->firstEdge),
                                   orientation);
    else if (data->firstEdge->type == AnchorData::Parallel)
        interpolateParallelEdges(base,
                                 static_cast<ParallelAnchorData *>(data->firstEdge),
                                 orientation);

    // Second edge
    if (data->secondEdge->type == AnchorData::Sequential)
        interpolateSequentialEdges(base,
                                   static_cast<SequentialAnchorData *>(data->secondEdge),
                                   orientation);
    else if (data->secondEdge->type == AnchorData::Parallel)
        interpolateParallelEdges(base,
                                 static_cast<ParallelAnchorData *>(data->secondEdge),
                                 orientation);
}

void QGraphicsAnchorLayoutPrivate::interpolateSequentialEdges(
    AnchorVertex *base, SequentialAnchorData *data, Orientation orientation)
{
    AnchorVertex *prev = base;

    // ### I'm not sure whether this assumption is safe. If not,
    // consider that m_edges.last() could be used instead (so
    // at(0) would be the one to be treated specially).
    Q_ASSERT(base == data->m_edges.at(0)->to || base == data->m_edges.at(0)->from);

    // Skip the last
    for (int i = 0; i < data->m_edges.count() - 1; ++i) {
        AnchorData *child = data->m_edges.at(i);
        interpolateEdge(prev, child, orientation);
        prev = child->to;
    }

    // Treat the last specially, since we already calculated it's end
    // vertex, so it's only interesting if it's a complex one
    if (data->m_edges.last()->type != AnchorData::Normal)
        interpolateEdge(prev, data->m_edges.last(), orientation);
}

bool QGraphicsAnchorLayoutPrivate::solveMinMax(const QList<QSimplexConstraint *> &constraints,
                                               GraphPath path, qreal *min, qreal *max)
{
    QSimplex simplex;
    bool feasible = simplex.setConstraints(constraints);
    if (feasible) {
        // Obtain the objective constraint
        QSimplexConstraint objective;
        QSet<AnchorData *>::const_iterator iter;
        for (iter = path.positives.constBegin(); iter != path.positives.constEnd(); ++iter)
            objective.variables.insert(*iter, 1.0);

        for (iter = path.negatives.constBegin(); iter != path.negatives.constEnd(); ++iter)
            objective.variables.insert(*iter, -1.0);

        simplex.setObjective(&objective);

        // Calculate minimum values
        *min = simplex.solveMin();

        // Save sizeAtMinimum results
        QList<QSimplexVariable *> variables = simplex.constraintsVariables();
        for (int i = 0; i < variables.size(); ++i) {
            AnchorData *ad = static_cast<AnchorData *>(variables[i]);
            Q_ASSERT(ad->result >= ad->minSize || qFuzzyCompare(ad->result, ad->minSize));
            ad->sizeAtMinimum = ad->result;
        }

        // Calculate maximum values
        *max = simplex.solveMax();

        // Save sizeAtMaximum results
        for (int i = 0; i < variables.size(); ++i) {
            AnchorData *ad = static_cast<AnchorData *>(variables[i]);
            Q_ASSERT(ad->result <= ad->maxSize || qFuzzyCompare(ad->result, ad->maxSize));
            ad->sizeAtMaximum = ad->result;
        }
    }
    return feasible;
}

bool QGraphicsAnchorLayoutPrivate::solvePreferred(const QList<QSimplexConstraint *> &constraints,
                                                  const QList<AnchorData *> &variables)
{
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
        AnchorData *ad = variables[i];
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
    bool feasible = simplex->setConstraints(constraints + preferredConstraints);
    if (feasible) {
        simplex->setObjective(&objective);

        // Calculate minimum values
        simplex->solveMin();

        // Save sizeAtPreferred results
        for (int i = 0; i < variables.size(); ++i) {
            AnchorData *ad = variables[i];
            ad->sizeAtPreferred = ad->result;
        }

        // Make sure we delete the simplex solver -before- we delete the
        // constraints used by it.
        delete simplex;
    }
    // Delete constraints and variables we created.
    qDeleteAll(preferredConstraints);
    qDeleteAll(preferredVariables);

    return feasible;
}

/*!
    \internal
    Calculate the "expanding" keyframe

    This new keyframe sits between the already existing sizeAtPreferred and
    sizeAtMaximum keyframes. Its goal is to modify the interpolation between
    the latter as to respect the "expanding" size policy of some anchors.

    Previously all items would be subject to a linear interpolation between
    sizeAtPreferred and sizeAtMaximum values. This will change now, the
    expanding anchors will change their size before the others. To calculate
    this keyframe we use the following logic:

    1) Ask each anchor for their desired expanding size (ad->expSize), this
    value depends on the anchor expanding property in the following way:

    - Expanding normal anchors want to grow towards their maximum size
    - Non-expanding normal anchors want to remain at their preferred size.
    - Sequential anchors wants to grow towards a size that is calculated by:
        summarizing it's child anchors, where it will use preferred size for non-expanding anchors
        and maximum size for expanding anchors.
    - Parallel anchors want to grow towards the smallest maximum size of all the expanding anchors.

    2) Clamp their desired values to the value they assume in the neighbour
    keyframes (sizeAtPreferred and sizeAtExpanding)

    3) Run simplex with a setup that ensures the following:

      a. Anchors will change their value from their sizeAtPreferred towards
         their sizeAtMaximum as much as required to ensure that ALL anchors
         reach their respective "desired" expanding sizes.

      b. No anchors will change their value beyond what is NEEDED to satisfy
         the requirement above.

    The final result is that, at the "expanding" keyframe expanding anchors
    will grow and take with them all anchors that are parallel to them.
    However, non-expanding anchors will remain at their preferred size unless
    they are forced to grow by a parallel expanding anchor.

    Note: For anchors where the sizeAtPreferred is bigger than sizeAtMaximum,
          the visual effect when the layout grows from its preferred size is
          the following: Expanding anchors will keep their size while non
          expanding ones will shrink. Only after non-expanding anchors have
          shrinked all the way, the expanding anchors will start to shrink too.
*/
void QGraphicsAnchorLayoutPrivate::solveExpanding(const QList<QSimplexConstraint *> &constraints,
                                                  const QList<AnchorData *> &variables)
{
    QList<QSimplexConstraint *> itemConstraints;
    QSimplexConstraint *objective = new QSimplexConstraint;
    bool hasExpanding = false;

    // Construct the simplex constraints and objective
    for (int i = 0; i < variables.size(); ++i) {
        // For each anchor
        AnchorData *ad = variables[i];

        // Clamp the desired expanding size
        qreal upperBoundary = qMax(ad->sizeAtPreferred, ad->sizeAtMaximum);
        qreal lowerBoundary = qMin(ad->sizeAtPreferred, ad->sizeAtMaximum);
        qreal boundedExpSize = qBound(lowerBoundary, ad->expSize, upperBoundary);

        // Expanding anchors are those that want to move from their preferred size
        if (boundedExpSize != ad->sizeAtPreferred)
            hasExpanding = true;

        // Lock anchor between boundedExpSize and sizeAtMaximum (ensure 3.a)
        if (boundedExpSize == ad->sizeAtMaximum) {
            // The interval has only one possible value, we can use an "Equal"
            // constraint and don't need to add this variable to the objective.
            QSimplexConstraint *itemC = new QSimplexConstraint;
            itemC->ratio = QSimplexConstraint::Equal;
            itemC->variables.insert(ad, 1.0);
            itemC->constant = boundedExpSize;
            itemConstraints << itemC;
        } else {
            // Add MoreOrEqual and LessOrEqual constraints.
            QSimplexConstraint *itemC = new QSimplexConstraint;
            itemC->ratio = QSimplexConstraint::MoreOrEqual;
            itemC->variables.insert(ad, 1.0);
            itemC->constant = qMin(boundedExpSize, ad->sizeAtMaximum);
            itemConstraints << itemC;

            itemC = new QSimplexConstraint;
            itemC->ratio = QSimplexConstraint::LessOrEqual;
            itemC->variables.insert(ad, 1.0);
            itemC->constant = qMax(boundedExpSize, ad->sizeAtMaximum);
            itemConstraints << itemC;

            // Create objective to avoid the anchors from moving away from
            // the preferred size more than the needed amount. (ensure 3.b)
            // The objective function is the distance between sizeAtPreferred
            // and sizeAtExpanding, it will be minimized.
            if (ad->sizeAtExpanding < ad->sizeAtMaximum) {
                // Try to shrink this variable towards its sizeAtPreferred value
                objective->variables.insert(ad, 1.0);
            } else {
                // Try to grow this variable towards its sizeAtPreferred value
                objective->variables.insert(ad, -1.0);
            }
        }
    }

    // Solve
    if (hasExpanding == false) {
        // If no anchors are expanding, we don't need to run the simplex
        // Set all variables to their preferred size
        for (int i = 0; i < variables.size(); ++i) {
            variables[i]->sizeAtExpanding = variables[i]->sizeAtPreferred;
        }
    } else {
        // Run simplex
        QSimplex simplex;

        // Satisfy expanding (3.a)
        bool feasible = simplex.setConstraints(constraints + itemConstraints);
        Q_ASSERT(feasible);

        // Reduce damage (3.b)
        simplex.setObjective(objective);
        simplex.solveMin();

        // Collect results
        for (int i = 0; i < variables.size(); ++i) {
            variables[i]->sizeAtExpanding = variables[i]->result;
        }
    }

    delete objective;
    qDeleteAll(itemConstraints);
}

/*!
    \internal
    Returns true if there are no arrangement that satisfies all constraints.
    Otherwise returns false.

    \sa addAnchor()
*/
bool QGraphicsAnchorLayoutPrivate::hasConflicts() const
{
    QGraphicsAnchorLayoutPrivate *that = const_cast<QGraphicsAnchorLayoutPrivate*>(this);
    that->calculateGraphs();

    bool floatConflict = !m_floatItems[0].isEmpty() || !m_floatItems[1].isEmpty();

    return graphHasConflicts[0] || graphHasConflicts[1] || floatConflict;
}

#ifdef QT_DEBUG
void QGraphicsAnchorLayoutPrivate::dumpGraph(const QString &name)
{
    QFile file(QString::fromAscii("anchorlayout.%1.dot").arg(name));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
        qWarning("Could not write to %s", file.fileName().toLocal8Bit().constData());

    QString str = QString::fromAscii("digraph anchorlayout {\nnode [shape=\"rect\"]\n%1}");
    QString dotContents = graph[0].serializeToDot();
    dotContents += graph[1].serializeToDot();
    file.write(str.arg(dotContents).toLocal8Bit());

    file.close();
}
#endif

QT_END_NAMESPACE

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

#include <QGraphicsWidget>

#include "qgraphicslayout_p.h"
#include "qgraphicsanchorlayout.h"
#include "qgraph_p.h"
#include "qsimplex_p.h"

/*
  The public QGraphicsAnchorLayout interface represents an anchorage point
  as a pair of a <QGraphicsLayoutItem *> and a <QGraphicsAnchorLayout::Edge>.

  Internally though, it has a graph of anchorage points (vertices) and
  anchors (edges), represented by the AnchorVertex and AnchorData structs
  respectively.
*/

/*!
  \internal

  Represents a vertex (anchorage point) in the internal graph
*/
struct AnchorVertex {
    AnchorVertex(QGraphicsLayoutItem *item, QGraphicsAnchorLayout::Edge edge)
        : m_item(item), m_edge(edge) {}

    AnchorVertex()
        : m_item(0), m_edge(QGraphicsAnchorLayout::Edge(0)) {}

    inline QString toString() const;

    QGraphicsLayoutItem *m_item;
    QGraphicsAnchorLayout::Edge m_edge;

    // Current distance from this vertex to the layout edge (Left or Top)
    // Value is calculated from the current anchors sizes.
    qreal distance;
};

inline QString AnchorVertex::toString() const
{
    if (!this || !m_item) {
        return QLatin1String("NULL");
    }
    QString edge;
    switch (m_edge) {
    case QGraphicsAnchorLayout::Left:
        edge = QLatin1String("Left");
        break;
    case QGraphicsAnchorLayout::HCenter:
        edge = QLatin1String("HorizontalCenter");
        break;
    case QGraphicsAnchorLayout::Right:
        edge = QLatin1String("Right");
        break;
    case QGraphicsAnchorLayout::Top:
        edge = QLatin1String("Top");
        break;
    case QGraphicsAnchorLayout::VCenter:
        edge = QLatin1String("VerticalCenter");
        break;
    case QGraphicsAnchorLayout::Bottom:
        edge = QLatin1String("Bottom");
        break;
    default:
        edge = QLatin1String("None");
        break;
    }
    QString item;
    if (m_item->isLayout()) {
        item = QLatin1String("layout");
    } else {
        QGraphicsWidget *w = static_cast<QGraphicsWidget *>(m_item);
        item = w->data(0).toString();
    }
    edge.insert(0, QLatin1String("%1_"));
    return edge.arg(item);
}


/*!
  \internal

  Represents an edge (anchor) in the internal graph.
*/
struct AnchorData : public QSimplexVariable {
    enum Type {
        Normal = 0,
        Sequential,
        Parallel
    };
    AnchorData(qreal minimumSize, qreal preferredSize, qreal maximumSize)
        : QSimplexVariable(), minSize(minimumSize), prefSize(preferredSize),
          maxSize(maximumSize), sizeAtMinimum(preferredSize),
          sizeAtPreferred(preferredSize), sizeAtMaximum(preferredSize),
          skipInPreferred(0), type(Normal) {}

    AnchorData(qreal size = 0)
        : QSimplexVariable(), minSize(size), prefSize(size), maxSize(size),
          sizeAtMinimum(size), sizeAtPreferred(size), sizeAtMaximum(size),
          skipInPreferred(0), type(Normal) {}

    inline QString toString() const;
    QString name;

    // Anchor is semantically directed
    AnchorVertex *origin;

    // Size restrictions of this edge. For anchors internal to items, these
    // values are derived from the respective item size hints. For anchors
    // that were added by users, these values are equal to the specified anchor
    // size.
    qreal minSize;
    qreal prefSize;
    qreal maxSize;

    // These attributes define which sizes should that anchor be in when the
    // layout is at its minimum, preferred or maximum sizes. Values are
    // calculated by the Simplex solver based on the current layout setup.
    qreal sizeAtMinimum;
    qreal sizeAtPreferred;
    qreal sizeAtMaximum;

    uint skipInPreferred : 1;
    uint type : 2;          // either Normal, Sequential or Parallel
protected:
    AnchorData(Type type, qreal size = 0)
        : QSimplexVariable(), minSize(size), prefSize(size),
          maxSize(size), sizeAtMinimum(size),
          sizeAtPreferred(size), sizeAtMaximum(size),
          skipInPreferred(0), type(type) {}
};

inline QString AnchorData::toString() const
{
    return QString::fromAscii("Anchor(%1)").arg(name);
    //return QString().sprintf("Anchor %%1 <Min %.1f  Pref %.1f  Max %.1f>",
    //                         minSize, prefSize, maxSize).arg(name);
}


struct SequentialAnchorData : public AnchorData
{
    SequentialAnchorData() : AnchorData(AnchorData::Sequential) {}
    QVector<AnchorVertex*> m_children;          // list of vertices in the sequence
    QVector<AnchorData*> m_edges;               // keep the list of edges too.
};

struct ParallelAnchorData : public AnchorData
{
    ParallelAnchorData() : AnchorData(AnchorData::Parallel) {}
    QVector<AnchorData*> children;      // list of parallel edges
};

/*!
  \internal

  Representation of a valid path for a given vertex in the graph.
  In this struct, "positives" is the set of anchors that have been
  traversed in the forward direction, while "negatives" is the set
  with the ones walked backwards.

  This paths are compared against each other to produce LP Constraints,
  the exact order in which the anchors were traversed is not relevant.
*/
class GraphPath
{
public:
    GraphPath() {};

    QSimplexConstraint *constraint(const GraphPath &path) const;

    QString toString() const;

    QSet<AnchorData *> positives;
    QSet<AnchorData *> negatives;
};


/*!
  \internal

  QGraphicsAnchorLayout private methods and attributes.
*/
class QGraphicsAnchorLayoutPrivate : public QGraphicsLayoutPrivate
{
    Q_DECLARE_PUBLIC(QGraphicsAnchorLayout)

public:
    // When the layout geometry is different from its Minimum, Preferred
    // or Maximum values, interpolation is used to calculate the geometries
    // of the items.
    //
    // Interval represents which interpolation interval are we operating in.
    enum Interval {
        MinToPreferred = 0,
        PreferredToMax
    };

    // Several structures internal to the layout are duplicated to handle
    // both Horizontal and Vertical restrictions.
    //
    // Orientation is used to reference the right structure in each context
    enum Orientation {
        Horizontal = 0,
        Vertical,
        NOrientations
    };

    QGraphicsAnchorLayoutPrivate();

    static QGraphicsAnchorLayout::Edge oppositeEdge(
        QGraphicsAnchorLayout::Edge edge);

    static Orientation edgeOrientation(QGraphicsAnchorLayout::Edge edge);

    static QGraphicsAnchorLayout::Edge pickEdge(QGraphicsAnchorLayout::Edge edge, Orientation orientation)
    {
        if (orientation == Vertical && int(edge) <= 2)
            return (QGraphicsAnchorLayout::Edge)(edge + 3);
        else if (orientation == Horizontal && int(edge) >= 3) {
            return (QGraphicsAnchorLayout::Edge)(edge - 3);            
        }
        return edge;
    }

    // Init methods
    void createLayoutEdges();
    void deleteLayoutEdges();
    void createItemEdges(QGraphicsLayoutItem *item);
    void removeCenterConstraints(QGraphicsLayoutItem *item, Orientation orientation);

    // Anchor Manipulation methods
    void addAnchor(QGraphicsLayoutItem *firstItem,
                   QGraphicsAnchorLayout::Edge firstEdge,
                   QGraphicsLayoutItem *secondItem,
                   QGraphicsAnchorLayout::Edge secondEdge,
                   AnchorData *data);

    void removeAnchor(QGraphicsLayoutItem *firstItem,
                      QGraphicsAnchorLayout::Edge firstEdge,
                      QGraphicsLayoutItem *secondItem,
                      QGraphicsAnchorLayout::Edge secondEdge);

    void removeAnchors(QGraphicsLayoutItem *item);

    void correctEdgeDirection(QGraphicsLayoutItem *&firstItem,
                              QGraphicsAnchorLayout::Edge &firstEdge,
                              QGraphicsLayoutItem *&secondItem,
                              QGraphicsAnchorLayout::Edge &secondEdge);

    // Child manipulation methods
    QGraphicsItem *parentItem() const;
    void addChildItem(QGraphicsLayoutItem *child);

    // Activation methods
    void simplifyGraph(Orientation orientation);
    void restoreSimplifiedGraph(Orientation orientation);
    void calculateGraphs();
    void calculateGraphs(Orientation orientation);
    void setAnchorSizeHintsFromItems(Orientation orientation);
    void findPaths(Orientation orientation);
    void constraintsFromPaths(Orientation orientation);
    QList<QSimplexConstraint *> constraintsFromSizeHints(const QList<AnchorData *> &anchors);
    QList<QList<QSimplexConstraint *> > getGraphParts(Orientation orientation);

    inline AnchorVertex *internalVertex(const QPair<QGraphicsLayoutItem*, QGraphicsAnchorLayout::Edge> &itemEdge)
    {
        return m_vertexList.value(itemEdge).first;
    }

    inline AnchorVertex *internalVertex(QGraphicsLayoutItem *item, QGraphicsAnchorLayout::Edge edge)
    {
        return internalVertex(qMakePair(item, edge));
    }

    AnchorVertex *addInternalVertex(QGraphicsLayoutItem *item, QGraphicsAnchorLayout::Edge edge);
    void removeInternalVertex(QGraphicsLayoutItem *item, QGraphicsAnchorLayout::Edge edge);

    // Geometry interpolation methods
    void setItemsGeometries();
    void calculateVertexPositions(Orientation orientation);
    void setupEdgesInterpolation(Orientation orientation);
    qreal interpolateEdge(AnchorData *edge);

    // Linear Programming solver methods
    QPair<qreal, qreal> solveMinMax(QList<QSimplexConstraint *> constraints,
                                    GraphPath path);
    void solvePreferred(QList<QSimplexConstraint *> constraints);

    qreal spacing[NOrientations];
    // Size hints from simplex engine
    qreal sizeHints[2][3];

    // Items
    QVector<QGraphicsLayoutItem *> items;

    // Mapping between high level anchorage points (Item, Edge) to low level
    // ones (Graph Vertices)

    QHash<QPair<QGraphicsLayoutItem*, QGraphicsAnchorLayout::Edge>, QPair<AnchorVertex *, int> > m_vertexList;

    // Internal graph of anchorage points and anchors, for both orientations
    Graph<AnchorVertex, AnchorData> graph[2];

    // Graph paths and constraints, for both orientations
    QMultiHash<AnchorVertex *, GraphPath> graphPaths[2];
    QList<QSimplexConstraint *> constraints[2];
    QList<QSimplexConstraint *> itemCenterConstraints[2];

    // The interpolation interval and progress based on the current size
    // as well as the key values (minimum, preferred and maximum)
    Interval interpolationInterval[2];
    qreal interpolationProgress[2];

    uint calculateGraphCacheDirty : 1;
};

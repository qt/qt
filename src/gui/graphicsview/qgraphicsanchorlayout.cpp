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

/*!
    \class QGraphicsAnchorLayout
    \brief The QGraphicsAnchorLayout class provides a layout where one can anchor widgets
    together in Graphics View.
    \since 4.6
    \ingroup appearance
    \ingroup geomanagement
    \ingroup graphicsview-api

    The anchor layout is a layout where one can specify how widgets should be placed relative to
    each other. The specification is called an anchor, and it is set up by calling anchor().
    Anchors are always set up between edges of an item, where the "center" is also considered to
    be an edge. Considering this example:
    \code
        QGraphicsAnchorLayout *l = new QGraphicsAnchorLayout;
        QGraphicsWidget *a = new QGraphicsWidget;
        QGraphicsWidget *b = new QGraphicsWidget;
        l->anchor(a, Qt::AnchorRight, b, Qt::AnchorLeft);
    \endcode

    Here is the right edge of item A anchored to the left edge of item B, with the result that
    item B will be placed to the right of item A, with a spacing between A and B. If the
    spacing is negative, the items will overlap to some extent. Items that are anchored are
    automatically added to the layout, and if items are removed, all their anchors will be
    automatically removed

    \section1 Size Hints and Size Policies in QGraphicsAnchorLayout

    QGraphicsAnchorLayout respects each item's size hints and size policies. However it does
    not respect stretch factors currently. This might change in the future, so please refrain
    from using stretch factors in anchor layout to avoid any future regressions.

    \section1 Spacing within QGraphicsAnchorLayout

    Between the items, the layout can distribute some space. If the spacing has not been
    explicitly specified, the actual amount of space will usually be 0, but if the first edge
    is the "opposite" of the second edge (i.e. Right is anchored to Left or vice-versa), the
    size of the anchor will be queried from the style through the pixelMetric
    PM_LayoutHorizontalSpacing (or PM_LayoutVerticalSpacing for vertical anchors).
*/

/*!
    \class QGraphicsAnchor
    \brief The QGraphicsAnchor class represents an anchor between two items in a
           QGraphicsAnchorLayout.
    \since 4.6
    \ingroup appearance
    \ingroup geomanagement
    \ingroup graphicsview-api

    The graphics anchor provides an API that enables you to query and manipulate the
    properties an anchor has. When an anchor is added to the layout with
    QGraphicsAnchorLayout::addAnchor(), a QGraphicsAnchor instance is returned where the properties
    are initialized to their default values. The properties can then be further changed, and they
    will be picked up the next time the layout is activated.

    \sa QGraphicsAnchorLayout::anchor()

*/
#include "qgraphicsanchorlayout_p.h"

QT_BEGIN_NAMESPACE

QGraphicsAnchor::QGraphicsAnchor(QGraphicsAnchorLayout *parentLayout)
    : QObject(*(new QGraphicsAnchorPrivate))
{
    Q_D(QGraphicsAnchor);
    Q_ASSERT(parentLayout);
    d->layoutPrivate = parentLayout->d_func();
}

/*!
    Removes the QGraphicsAnchor object from the layout and destroys it.
*/
QGraphicsAnchor::~QGraphicsAnchor()
{
}

/*!
    \property QGraphicsAnchor::sizePolicy
    \brief the size policy for the QGraphicsAnchor.

    By setting the size policy on an anchor you can configure how the item can resize itself
    from its preferred spacing. For instance, if the anchor has the size policy
    QSizePolicy::Minimum, the spacing is the minimum size of the anchor. However, its size
    can grow up to the anchors maximum size. If the default size policy is QSizePolicy::Fixed,
    the anchor can neither grow or shrink, which means that the only size the anchor can have
    is the spacing. QSizePolicy::Fixed is the default size policy.
    QGraphicsAnchor always has a minimum spacing of 0 and a very large maximum spacing.

    \sa QGraphicsAnchor::spacing
*/

void QGraphicsAnchor::setSizePolicy(QSizePolicy::Policy policy)
{
    Q_D(QGraphicsAnchor);
    d->setSizePolicy(policy);
}

QSizePolicy::Policy QGraphicsAnchor::sizePolicy() const
{
    Q_D(const QGraphicsAnchor);
    return d->sizePolicy;
}

/*!
    \property QGraphicsAnchor::spacing
    \brief the preferred space between items in the QGraphicsAnchorLayout.

    Depending on the anchor type, the default spacing is either
    0 or a value returned from the style.

    \sa QGraphicsAnchorLayout::addAnchor()
*/
void QGraphicsAnchor::setSpacing(qreal spacing)
{
    Q_D(QGraphicsAnchor);
    d->setSpacing(spacing);
}

qreal QGraphicsAnchor::spacing() const
{
    Q_D(const QGraphicsAnchor);
    return d->spacing();
}

void QGraphicsAnchor::unsetSpacing()
{
    Q_D(QGraphicsAnchor);
    d->unsetSpacing();
}

/*!
    Constructs a QGraphicsAnchorLayout instance.  \a parent is passed to
    QGraphicsLayout's constructor.
  */
QGraphicsAnchorLayout::QGraphicsAnchorLayout(QGraphicsLayoutItem *parent)
    : QGraphicsLayout(*new QGraphicsAnchorLayoutPrivate(), parent)
{
    Q_D(QGraphicsAnchorLayout);
    d->createLayoutEdges();
}

/*!
    Destroys the QGraphicsAnchorLayout object.
*/
QGraphicsAnchorLayout::~QGraphicsAnchorLayout()
{
    Q_D(QGraphicsAnchorLayout);

    for (int i = count() - 1; i >= 0; --i) {
        QGraphicsLayoutItem *item = d->items.at(i);
        removeAt(i);
        if (item) {
            if (item->ownedByLayout())
                delete item;
        }
    }

    d->removeCenterConstraints(this, QGraphicsAnchorLayoutPrivate::Horizontal);
    d->removeCenterConstraints(this, QGraphicsAnchorLayoutPrivate::Vertical);
    d->deleteLayoutEdges();

    Q_ASSERT(d->itemCenterConstraints[0].isEmpty());
    Q_ASSERT(d->itemCenterConstraints[1].isEmpty());
    Q_ASSERT(d->items.isEmpty());
    Q_ASSERT(d->m_vertexList.isEmpty());
}

/*!
    Creates an anchor between the edge \a firstEdge of item \a firstItem and the edge \a secondEdge
    of item \a secondItem. The magnitude of the anchor is picked up from the style. Anchors
    between a layout edge and an item edge will have a size of 0.
    If there is already an anchor between the edges, the the new anchor will replace the old one.

    \a firstItem and \a secondItem are automatically added to the layout if they are not part
    of the layout. This means that count() can increase with up to 2.

    The spacing an anchor will get depends on the type of anchor. For instance, anchors from the
    Right edge of one item to the Left edge of another (or vice versa) will use the default
    horizontal spacing. The same behaviour applies to Bottom to Top anchors, (but they will use
    the default vertical spacing). For all other anchor combinations, the spacing will be 0.
    All anchoring functions will follow this rule.

    The spacing can also be set manually by using QGraphicsAnchor::setSpacing() method.

    Calling this function where \a firstItem or \a secondItem are ancestors of the layout have
    undefined behaviour.

    \sa addCornerAnchors(), addAnchors()
 */
QGraphicsAnchor *
QGraphicsAnchorLayout::addAnchor(QGraphicsLayoutItem *firstItem, Qt::AnchorPoint firstEdge,
                                 QGraphicsLayoutItem *secondItem, Qt::AnchorPoint secondEdge)
{
    Q_D(QGraphicsAnchorLayout);
    QGraphicsAnchor *a = d->addAnchor(firstItem, firstEdge, secondItem, secondEdge);
    invalidate();
    return a;
}

/*!
    Returns the anchor between the anchor points defined by \a firstItem and \a firstEdge and
    \a secondItem and \a secondEdge. If there is no such anchor, the function will return 0.
*/
QGraphicsAnchor *
QGraphicsAnchorLayout::anchor(QGraphicsLayoutItem *firstItem, Qt::AnchorPoint firstEdge,
                              QGraphicsLayoutItem *secondItem, Qt::AnchorPoint secondEdge)
{
    Q_D(QGraphicsAnchorLayout);
    return d->getAnchor(firstItem, firstEdge, secondItem, secondEdge);
}

/*!
    Creates two anchors between \a firstItem and \a secondItem, where one is for the horizontal
    edge and another one for the vertical edge that the corners \a firstCorner and \a
    secondCorner specifies.
    The magnitude of the anchors is picked up from the style.

    This is a convenience function, since anchoring corners can be expressed as anchoring two edges.
    For instance,
    \code
      layout->addAnchor(layout, Qt::AnchorTop, b, Qt::AnchorTop);
      layout->addAnchor(layout, Qt::AnchorLeft, b, Qt::AnchorLeft);
    \endcode

    has the same effect as

    \code
      layout->addCornerAnchors(layout, Qt::TopLeft, b, Qt::TopLeft);
    \endcode

    If there is already an anchor between the edge pairs, it will be replaced by the anchors that
    this function specifies.

    \a firstItem and \a secondItem are automatically added to the layout if they are not part of the
    layout. This means that count() can increase with up to 2.
*/
void QGraphicsAnchorLayout::addCornerAnchors(QGraphicsLayoutItem *firstItem,
                                             Qt::Corner firstCorner,
                                             QGraphicsLayoutItem *secondItem,
                                             Qt::Corner secondCorner)
{
    Q_D(QGraphicsAnchorLayout);

    // Horizontal anchor
    Qt::AnchorPoint firstEdge = (firstCorner & 1 ? Qt::AnchorRight: Qt::AnchorLeft);
    Qt::AnchorPoint secondEdge = (secondCorner & 1 ? Qt::AnchorRight: Qt::AnchorLeft);
    d->addAnchor(firstItem, firstEdge, secondItem, secondEdge);

    // Vertical anchor
    firstEdge = (firstCorner & 2 ? Qt::AnchorBottom: Qt::AnchorTop);
    secondEdge = (secondCorner & 2 ? Qt::AnchorBottom: Qt::AnchorTop);
    d->addAnchor(firstItem, firstEdge, secondItem, secondEdge);

    invalidate();
}

/*!
    Anchors two or four edges of \a firstItem with the corresponding
    edges of \a secondItem, so that \a firstItem has the same size as
    \a secondItem in the dimensions specified by \a orientations.

    Calling this convenience function with the following arguments
    \code
    l->addAnchors(firstItem, secondItem, Qt::Horizontal)
    \endcode

    is the same as

    \code
    l->addAnchor(firstItem, Qt::AnchorLeft, secondItem, Qt::AnchorLeft);
    l->addAnchor(firstItem, Qt::AnchorRight, secondItem, Qt::AnchorRight);
    \endcode
*/
void QGraphicsAnchorLayout::addAnchors(QGraphicsLayoutItem *firstItem,
                                       QGraphicsLayoutItem *secondItem,
                                       Qt::Orientations orientations)
{
    if (orientations & Qt::Horizontal) {
        addAnchor(secondItem, Qt::AnchorLeft, firstItem, Qt::AnchorLeft);
        addAnchor(firstItem, Qt::AnchorRight, secondItem, Qt::AnchorRight);
    }
    if (orientations & Qt::Vertical) {
        addAnchor(secondItem, Qt::AnchorTop, firstItem, Qt::AnchorTop);
        addAnchor(firstItem, Qt::AnchorBottom, secondItem, Qt::AnchorBottom);
    }
}

/*!
    Sets the default horizontal spacing for the anchor layout to \a spacing.

    \sa horizontalSpacing(), setVerticalSpacing(), setSpacing()
*/
void QGraphicsAnchorLayout::setHorizontalSpacing(qreal spacing)
{
    Q_D(QGraphicsAnchorLayout);

    // ### We don't support negative spacing yet
    if (spacing < 0) {
        spacing = 0;
        qWarning() << "QGraphicsAnchorLayout does not support negative spacing.";
    }

    d->spacings[0] = spacing;
    invalidate();
}

/*!
    Sets the default vertical spacing for the anchor layout to \a spacing.

    \sa verticalSpacing(), setHorizontalSpacing(), setSpacing()
*/
void QGraphicsAnchorLayout::setVerticalSpacing(qreal spacing)
{
    Q_D(QGraphicsAnchorLayout);

    // ### We don't support negative spacing yet
    if (spacing < 0) {
        spacing = 0;
        qWarning() << "QGraphicsAnchorLayout does not support negative spacing.";
    }

    d->spacings[1] = spacing;
    invalidate();
}

/*!
    Sets the default horizontal and the default vertical spacing for the anchor layout to \a spacing.

    If an item is anchored with no spacing associated with the anchor, it will use the default
    spacing.

    Currently QGraphicsAnchorLayout does not support negative default spacings.

    \sa setHorizontalSpacing(), setVerticalSpacing()
*/
void QGraphicsAnchorLayout::setSpacing(qreal spacing)
{
    Q_D(QGraphicsAnchorLayout);

    // ### Currently we do not support negative anchors inside the graph.
    // To avoid those being created by a negative spacing, we must
    // make this test.
    if (spacing < 0) {
        spacing = 0;
        qWarning() << "QGraphicsAnchorLayout does not support negative spacing.";
    }

    d->spacings[0] = d->spacings[1] = spacing;
    invalidate();
}

/*!
    Returns the default horizontal spacing for the anchor layout.

    \sa verticalSpacing(), setHorizontalSpacing()
*/
qreal QGraphicsAnchorLayout::horizontalSpacing() const
{
    Q_D(const QGraphicsAnchorLayout);
    return d->effectiveSpacing(QGraphicsAnchorLayoutPrivate::Horizontal);
}

/*!
    Returns the default vertical spacing for the anchor layout.

    \sa horizontalSpacing(), setVerticalSpacing()
*/
qreal QGraphicsAnchorLayout::verticalSpacing() const
{
    Q_D(const QGraphicsAnchorLayout);
    return d->effectiveSpacing(QGraphicsAnchorLayoutPrivate::Vertical);
}

/*!
    \reimp
*/
void QGraphicsAnchorLayout::setGeometry(const QRectF &geom)
{
    Q_D(QGraphicsAnchorLayout);

    QGraphicsLayout::setGeometry(geom);
    d->calculateVertexPositions(QGraphicsAnchorLayoutPrivate::Horizontal);
    d->calculateVertexPositions(QGraphicsAnchorLayoutPrivate::Vertical);
    d->setItemsGeometries(geom);
}

/*!
    Removes the layout item at \a index without destroying it. Ownership of
    the item is transferred to the caller.

    Removing an item will also remove any of the anchors associated with it.

    \sa itemAt(), count()
*/
void QGraphicsAnchorLayout::removeAt(int index)
{
    Q_D(QGraphicsAnchorLayout);
    QGraphicsLayoutItem *item = d->items.value(index);

    if (!item)
        return;

    // Removing an item affects both horizontal and vertical graphs
    d->restoreSimplifiedGraph(QGraphicsAnchorLayoutPrivate::Horizontal);
    d->restoreSimplifiedGraph(QGraphicsAnchorLayoutPrivate::Vertical);

    d->removeCenterConstraints(item, QGraphicsAnchorLayoutPrivate::Horizontal);
    d->removeCenterConstraints(item, QGraphicsAnchorLayoutPrivate::Vertical);
    d->removeAnchors(item);
    d->items.remove(index);

    item->setParentLayoutItem(0);
    invalidate();
}

/*!
    \reimp
*/
int QGraphicsAnchorLayout::count() const
{
    Q_D(const QGraphicsAnchorLayout);
    return d->items.size();
}

/*!
    \reimp
*/
QGraphicsLayoutItem *QGraphicsAnchorLayout::itemAt(int index) const
{
    Q_D(const QGraphicsAnchorLayout);
    return d->items.value(index);
}

/*!
    \reimp
*/
void QGraphicsAnchorLayout::invalidate()
{
    Q_D(QGraphicsAnchorLayout);
    QGraphicsLayout::invalidate();
    d->calculateGraphCacheDirty = 1;
}

/*!
    \reimp
*/
QSizeF QGraphicsAnchorLayout::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
    Q_UNUSED(constraint);
    Q_D(const QGraphicsAnchorLayout);

    // Some setup calculations are delayed until the information is
    // actually needed, avoiding unnecessary recalculations when
    // adding multiple anchors.

    // sizeHint() / effectiveSizeHint() already have a cache
    // mechanism, using invalidate() to force recalculation. However
    // sizeHint() is called three times after invalidation (for max,
    // min and pref), but we just need do our setup once.

    const_cast<QGraphicsAnchorLayoutPrivate *>(d)->calculateGraphs();

    // ### apply constraint!
    QSizeF engineSizeHint(
        d->sizeHints[QGraphicsAnchorLayoutPrivate::Horizontal][which],
        d->sizeHints[QGraphicsAnchorLayoutPrivate::Vertical][which]);

    qreal left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);

    return engineSizeHint + QSizeF(left + right, top + bottom);
}

QT_END_NAMESPACE

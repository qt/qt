/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#include "private/qdeclarativeflickable_p.h"
#include "private/qdeclarativeflickable_p_p.h"
#include <qdeclarativeinfo.h>
#include <QGraphicsSceneMouseEvent>
#include <QPointer>

#include <QScroller>
#include <QScrollerProperties>


QT_BEGIN_NAMESPACE

QDeclarativeFlickableVisibleArea::QDeclarativeFlickableVisibleArea(QDeclarativeFlickable *parent)
    : QObject(parent), flickable(parent), m_xPosition(0.), m_widthRatio(0.)
    , m_yPosition(0.), m_heightRatio(0.)
{
}

qreal QDeclarativeFlickableVisibleArea::widthRatio() const
{
    return m_widthRatio;
}

qreal QDeclarativeFlickableVisibleArea::xPosition() const
{
    return m_xPosition;
}

qreal QDeclarativeFlickableVisibleArea::heightRatio() const
{
    return m_heightRatio;
}

qreal QDeclarativeFlickableVisibleArea::yPosition() const
{
    return m_yPosition;
}

/*! \internal
    Updates the page position and ratio values and sends out the appropriate position
    and ratio changed signals.
    Will be called by updateBeginningEnd.
*/
void QDeclarativeFlickableVisibleArea::updateVisible()
{
    QDeclarativeFlickablePrivate *p = static_cast<QDeclarativeFlickablePrivate *>(QGraphicsItemPrivate::get(flickable));

    bool changeX = false;
    bool changeY = false;
    bool changeWidth = false;
    bool changeHeight = false;

    // Vertical
    const qreal viewheight = flickable->height();
    const qreal maxyextent = flickable->maxYExtent();
    qreal pagePos = (p->contentItem->pos().y() + flickable->minYExtent()) / maxyextent;
    qreal pageSize = viewheight / maxyextent;

    // qDebug() << "updateVisible" << viewheight << "ext:" << flickable->minYExtent() <<"-"<<flickable->maxYExtent() << "pagesize:"<<pageSize;

    if (pageSize != m_heightRatio) {
        m_heightRatio = pageSize;
        changeHeight = true;
    }
    if (pagePos != m_yPosition) {
        m_yPosition = pagePos;
        changeY = true;
    }

    // Horizontal
    const qreal viewwidth = flickable->width();
    const qreal maxxextent = flickable->maxXExtent();
    pagePos = (p->contentItem->pos().x() + flickable->minXExtent()) / maxxextent;
    pageSize = viewwidth / maxxextent;

    if (pageSize != m_widthRatio) {
        m_widthRatio = pageSize;
        changeWidth = true;
    }
    if (pagePos != m_xPosition) {
        m_xPosition = pagePos;
        changeX = true;
    }

    if (changeX)
        emit xPositionChanged(m_xPosition);
    if (changeY)
        emit yPositionChanged(m_yPosition);
    if (changeWidth)
        emit widthRatioChanged(m_widthRatio);
    if (changeHeight)
        emit heightRatioChanged(m_heightRatio);
}


QDeclarativeFlickablePrivate::QDeclarativeFlickablePrivate()
  : contentItem(new QDeclarativeItem)
    , updateScrollerValuesRequested(false)
    , duringScrollEvent(false)
    , isUserGenerated(false)
    , isFlicking(false)
    , isMoving(false)
    , movingHorizontally(false)
    , movingVertically(false)
    , atBeginningX(true)
    , atBeginningY(true)
    , atEndX(false)
    , atEndY(false)
    , interactive(true)
    , visibleArea(0)
    , flickableDirection(QDeclarativeFlickable::AutoFlickDirection)
    , boundsBehavior(QDeclarativeFlickable::DragAndOvershootBounds)
{
}

void QDeclarativeFlickablePrivate::init()
{
    Q_Q(QDeclarativeFlickable);
    QDeclarative_setParent_noEvent(contentItem, q);
    contentItem->setParentItem(q);

    q->setAcceptedMouseButtons(Qt::LeftButton);
    q->setAcceptTouchEvents(true);

    QScroller::grabGesture(q, QScroller::LeftMouseButtonGesture);
    QScroller *scroller = QScroller::scroller(q);

    QScrollerProperties sp = scroller->scrollerProperties();
    sp.setScrollMetric(QScrollerProperties::MousePressEventDelay, 0);
    sp.setScrollMetric(QScrollerProperties::OvershootDragDistanceFactor, 1.0);
    sp.setScrollMetric(QScrollerProperties::OvershootScrollDistanceFactor, 1.0);
    scroller->setScrollerProperties(sp);

    QObject::connect(scroller, SIGNAL(stateChanged(QScroller::State)), q, SLOT(scrollerStateChanged(QScroller::State)));
}

/*! /internal
    Requests a prepare event to be send to the scroller informing it about changed
    positions or scroll area bounds.
*/
void QDeclarativeFlickablePrivate::updateScrollerValues()
{
    Q_Q(QDeclarativeFlickable);
    //qDebug() << "DeclarativeFlickablePrivate::updateScrollerValues" << duringScrollEvent;
    if (!duringScrollEvent)
        QScroller::scroller(q)->resendPrepareEvent();
    else
        updateScrollerValuesRequested = true;
}

void QDeclarativeFlickablePrivate::itemGeometryChanged(QDeclarativeItem *item, const QRectF &newGeom, const QRectF &oldGeom)
{
    // TODO
    Q_Q(QDeclarativeFlickable);
    if (item == contentItem) {
        if (newGeom.x() != oldGeom.x())
            emit q->contentXChanged();
        if (newGeom.y() != oldGeom.y())
            emit q->contentYChanged();
    }
}

/*! \internal
    Updates the atBeginning and atEnd flags and sends out the changed signals
*/
void QDeclarativeFlickablePrivate::updateBeginningEnd()
{
    Q_Q(QDeclarativeFlickable);

    bool boundaryChanged = false;

    bool newAtBeginningX = -contentItem->pos().x() <= q->minXExtent();
    bool newAtEndX = -contentItem->pos().x() >= (q->maxXExtent() - width());
    bool newAtBeginningY = -contentItem->pos().y() <= q->minYExtent();
    bool newAtEndY = -contentItem->pos().y() >= (q->maxYExtent() - height());

    // Horizontal
    if (newAtBeginningX != atBeginningX) {
        atBeginningX = newAtBeginningX;
        boundaryChanged = true;
    }
    if (newAtEndX != atEndX) {
        atEndX = newAtEndX;
        boundaryChanged = true;
    }

    // Vertical
    if (newAtBeginningY != atBeginningY) {
        atBeginningY = newAtBeginningY;
        boundaryChanged = true;
    }
    if (newAtEndY != atEndY) {
        atEndY = newAtEndY;
        boundaryChanged = true;
    }

    if (boundaryChanged)
        emit q->isAtBoundaryChanged();

    if (visibleArea)
        visibleArea->updateVisible();
}

/*!
    \qmlclass Flickable QDeclarativeFlickable
    \since 4.7
    \ingroup qml-basic-interaction-elements

    \brief The Flickable item provides a surface that can be "flicked".
    \inherits Item

    The Flickable item places its children on a surface that can be dragged
    and flicked, causing the view onto the child items to scroll. This
    behavior forms the basis of Items that are designed to show large numbers
    of child items, such as \l ListView and \l GridView.

    In traditional user interfaces, views can be scrolled using standard
    controls, such as scroll bars and arrow buttons. In some situations, it
    is also possible to drag the view directly by pressing and holding a
    mouse button while moving the cursor. In touch-based user interfaces,
    this dragging action is often complemented with a flicking action, where
    scrolling continues after the user has stopped touching the view.

    Flickable does not automatically clip its contents. If it is not used as
    a full-screen item, you should consider setting the \l{Item::}{clip} property
    to true.

    \section1 Example Usage

    \beginfloatright
    \inlineimage flickable.gif
    \endfloat

    The following example shows a small view onto a large image in which the
    user can drag or flick the image in order to view different parts of it.

    \snippet doc/src/snippets/declarative/flickable.qml document

    \clearfloat

    Items declared as children of a Flickable are automatically parented to the
    Flickable's \l contentItem.  This should be taken into account when
    operating on the children of the Flickable; it is usually the children of
    \c contentItem that are relevant.  For example, the bound of Items added
    to the Flickable will be available by \c contentItem.childrenRect

    \section1 Limitations

    \note Due to an implementation detail, items placed inside a Flickable cannot anchor to it by
    \c id. Use \c parent instead.
*/

/*!
    \qmlsignal Flickable::onMovementStarted()

    This handler is called when the view begins moving due to user
    interaction.
*/

/*!
    \qmlsignal Flickable::onMovementEnded()

    This handler is called when the view stops moving due to user
    interaction.  If a flick was generated, this handler will
    be triggered once the flick stops.  If a flick was not
    generated, the handler will be triggered when the
    user stops dragging - i.e. a mouse or touch release.
*/

/*!
    \qmlsignal Flickable::onFlickStarted()

    This handler is called when the view is flicked.  A flick
    starts from the point that the mouse or touch is released,
    while still in motion.
*/

/*!
    \qmlsignal Flickable::onFlickEnded()

    This handler is called when the view stops moving due to a flick.
*/

/*!
    \qmlproperty real Flickable::visibleArea.xPosition
    \qmlproperty real Flickable::visibleArea.widthRatio
    \qmlproperty real Flickable::visibleArea.yPosition
    \qmlproperty real Flickable::visibleArea.heightRatio

    These properties describe the position and size of the currently viewed area.
    The size is defined as the percentage of the full view currently visible,
    scaled to 0.0 - 1.0.  The page position is usually in the range 0.0 (beginning) to
    1.0 minus size ratio (end), i.e. \c yPosition is in the range 0.0 to 1.0-\c heightRatio.
    However, it is possible for the contents to be dragged outside of the normal
    range, resulting in the page positions also being outside the normal range.

    These properties are typically used to draw a scrollbar. For example:

    \snippet doc/src/snippets/declarative/flickableScrollbar.qml 0
    \dots 8
    \snippet doc/src/snippets/declarative/flickableScrollbar.qml 1

    \sa {declarative/ui-components/scrollbar}{scrollbar example}
*/

QDeclarativeFlickable::QDeclarativeFlickable(QDeclarativeItem *parent)
  : QDeclarativeItem(*(new QDeclarativeFlickablePrivate), parent)
{
    Q_D(QDeclarativeFlickable);
    d->init();
}

QDeclarativeFlickable::QDeclarativeFlickable(QDeclarativeFlickablePrivate &dd, QDeclarativeItem *parent)
  : QDeclarativeItem(dd, parent)
{
    Q_D(QDeclarativeFlickable);
    d->init();
}

QDeclarativeFlickable::~QDeclarativeFlickable()
{
}

/*!
    \qmlproperty real Flickable::contentX
    \qmlproperty real Flickable::contentY

    These properties hold the surface coordinate currently at the top-left
    corner of the Flickable. For example, if you flick an image up 100 pixels,
    \c contentY will be 100.
*/
qreal QDeclarativeFlickable::contentX() const
{
    Q_D(const QDeclarativeFlickable);
    return -d->contentItem->x();
}

void QDeclarativeFlickable::setContentX(qreal pos)
{
    Q_D(QDeclarativeFlickable);
    QScroller::scroller(this)->stop();

    if (pos == -d->contentItem->x())
        return;
    viewportAboutToMove(QPointF(pos, contentY()));
    d->contentItem->setX(-pos);
    viewportMoved();
    emit contentXChanged();
}

qreal QDeclarativeFlickable::contentY() const
{
    Q_D(const QDeclarativeFlickable);
    return -d->contentItem->y();
}

void QDeclarativeFlickable::setContentY(qreal pos)
{
    Q_D(QDeclarativeFlickable);
    QScroller::scroller(this)->stop();

    if (pos == -d->contentItem->y())
        return;
    viewportAboutToMove(QPointF(contentX(), pos));
    d->contentItem->setY(-pos);
    viewportMoved();
    emit contentYChanged();
}

/*!
    \qmlproperty bool Flickable::interactive

    This property describes whether the user can interact with the Flickable.
    A user cannot drag or flick a Flickable that is not interactive.

    By default, this property is true.

    This property is useful for temporarily disabling flicking. This allows
    special interaction with Flickable's children; for example, you might want
    to freeze a flickable map while scrolling through a pop-up dialog that
    is a child of the Flickable.
*/
bool QDeclarativeFlickable::isInteractive() const
{
    Q_D(const QDeclarativeFlickable);
    return d->interactive;
}

void QDeclarativeFlickable::setInteractive(bool interactive)
{
    Q_D(QDeclarativeFlickable);
    if (interactive != d->interactive) {
        d->interactive = interactive;
        QScroller::scroller(this)->stop();
        emit interactiveChanged();
    }
}

/*!
    \qmlproperty real Flickable::horizontalVelocity
    \qmlproperty real Flickable::verticalVelocity

    The instantaneous velocity of movement along the x and y axes, in pixels/sec.
*/
qreal QDeclarativeFlickable::horizontalVelocity() const
{
    return QScroller::scroller(this)->velocity().x();
}

qreal QDeclarativeFlickable::verticalVelocity() const
{
    return QScroller::scroller(this)->velocity().y();
}

/*!
    \qmlproperty bool Flickable::atXBeginning
    \qmlproperty bool Flickable::atXEnd
    \qmlproperty bool Flickable::atYBeginning
    \qmlproperty bool Flickable::atYEnd

    These properties are true if the flickable view is positioned at the beginning,
    or end respecively.
*/
bool QDeclarativeFlickable::isAtXEnd() const
{
    Q_D(const QDeclarativeFlickable);
    return d->atEndX;
}

bool QDeclarativeFlickable::isAtXBeginning() const
{
    Q_D(const QDeclarativeFlickable);
    return d->atBeginningX;
}

bool QDeclarativeFlickable::isAtYEnd() const
{
    Q_D(const QDeclarativeFlickable);
    return d->atEndY;
}

bool QDeclarativeFlickable::isAtYBeginning() const
{
    Q_D(const QDeclarativeFlickable);
    return d->atBeginningY;
}

/*!
    \qmlproperty Item Flickable::contentItem

    The internal item that contains the Items to be moved in the Flickable.

    Items declared as children of a Flickable are automatically parented to the Flickable's contentItem.

    Items created dynamically need to be explicitly parented to the \e contentItem:
    \code
    Flickable {
        id: myFlickable
        function addItem(file) {
            var component = Qt.createComponent(file)
            component.createObject(myFlickable.contentItem);
        }
    }
    \endcode
*/
QDeclarativeItem *QDeclarativeFlickable::contentItem()
{
    Q_D(QDeclarativeFlickable);
    return d->contentItem;
}

QDeclarativeFlickableVisibleArea *QDeclarativeFlickable::visibleArea()
{
    Q_D(QDeclarativeFlickable);
    if (!d->visibleArea)
        d->visibleArea = new QDeclarativeFlickableVisibleArea(this);
    return d->visibleArea;
}

void QDeclarativeFlickable::scrollerStateChanged(QScroller::State state)
{
    Q_D(QDeclarativeFlickable);

    //qDebug() << "scrollerStateChanged" << int(state);

    switch (state) {
    case QScroller::Inactive:
        d->isUserGenerated = false;
        // fall through
    case QScroller::Pressed:
        if (d->isMoving) {
            d->isMoving = false;
            emit movingChanged();
            emit movementEnded();
        }
        if (d->isFlicking) {
            d->isFlicking = false;
            emit flickingChanged();
            emit flickEnded();
        }
        break;

    case QScroller::Dragging:
        d->isUserGenerated = true;
        if (!d->isMoving) {
            d->isMoving = true;
            emit movingChanged();
            emit movementStarted();
        }
        break;

    case QScroller::Scrolling:
        QScroller::scroller(this)->resendPrepareEvent(); // sometimes snappoints depend on the scrolling direction.
        if (d->isUserGenerated) {
            if (!d->isFlicking) {
                emit flickStarted();
                emit flickingChanged();
            }
            d->isFlicking = true;
        }
        break;
    }
}


/*!
    \qmlproperty enumeration Flickable::flickableDirection

    This property determines which directions the view can be flicked.

    \list
    \o Flickable.AutoFlickDirection (default) - allows flicking vertically if the
    \e contentHeight is not equal to the \e height of the Flickable.
    Allows flicking horizontally if the \e contentWidth is not equal
    to the \e width of the Flickable.
    \o Flickable.HorizontalFlick - allows flicking horizontally.
    \o Flickable.VerticalFlick - allows flicking vertically.
    \o Flickable.HorizontalAndVerticalFlick - allows flicking in both directions.
    \endlist
*/
QDeclarativeFlickable::FlickableDirection QDeclarativeFlickable::flickableDirection() const
{
    Q_D(const QDeclarativeFlickable);
    return d->flickableDirection;
}

void QDeclarativeFlickable::setFlickableDirection(FlickableDirection direction)
{
    Q_D(QDeclarativeFlickable);
    if (direction != d->flickableDirection) {
        d->flickableDirection = direction;
        emit flickableDirectionChanged();
    }
}

/*! \internal
    Returns the minimum horizontal content position.
    Note: this function will be overloaded.
 */
qreal QDeclarativeFlickable::minXExtent() const
{
    return 0;
}

/*! \internal
    Returns the minimum vertical content position.
    Note: this function will be overloaded.
 */
qreal QDeclarativeFlickable::minYExtent() const
{
    return 0;
}
/*! \internal
    Returns the maximum horizontal content position.
 */
qreal QDeclarativeFlickable::maxXExtent() const
{
    Q_D(const QDeclarativeFlickable);
    return qMax(qreal(0), d->contentItem->width());
}

/*! \internal
    Returns the maximum vertical content position.
 */
qreal QDeclarativeFlickable::maxYExtent() const
{
    Q_D(const QDeclarativeFlickable);
    return qMax(qreal(0), d->contentItem->height());
}

/*! \internal
    This function is called before the viewport starts to move.
    This is a good chance to fill up some buffers.
 */
void QDeclarativeFlickable::viewportAboutToMove(QPointF newPos)
{
    Q_UNUSED(newPos);
}

/*! \internal
    This function is called after the viewport was moved (and the
    move finished)
 */
void QDeclarativeFlickable::viewportMoved()
{
    Q_D(QDeclarativeFlickable);
    d->updateBeginningEnd();
}

void QDeclarativeFlickable::geometryChanged(const QRectF &newGeometry,
                                            const QRectF &oldGeometry)
{
    Q_D(QDeclarativeFlickable);
    QDeclarativeItem::geometryChanged(newGeometry, oldGeometry);

    bool changed = false;
    if (newGeometry.width() != oldGeometry.width()) {
        changed = true;
        // strangely this causes problems
        // d->contentItem->setWidth(newGeometry.width());
        emit contentWidthChanged();
    }
    if (newGeometry.height() != oldGeometry.height()) {
        changed = true;
        // strangely this causes problems
        // d->contentItem->setHeight(newGeometry.height());
        emit contentHeightChanged();
    }

    if (changed) {
        QScroller *scroller = QScroller::scroller(this);

        //qDebug() << "xxx geometryChanged: "<<this<<"new:" << newGeometry;

        if (scroller->state() == QScroller::Inactive) {
            scroller->ensureVisible(QRectF(minXExtent(), minYExtent(),
                                           d->contentItem->width(), d->contentItem->height()), 0, 0, 0);
        } else
            d->updateScrollerValues();
        d->updateBeginningEnd();
    }
}

void QDeclarativeFlickablePrivate::data_append(QDeclarativeListProperty<QObject> *prop, QObject *o)
{
    QGraphicsObject *i = qobject_cast<QGraphicsObject *>(o);
    if (i) {
        QGraphicsItemPrivate *d = QGraphicsItemPrivate::get(i);
        if (static_cast<QDeclarativeItemPrivate*>(d)->componentComplete) {
            i->setParentItem(static_cast<QDeclarativeFlickablePrivate*>(prop->data)->contentItem);
        } else {
            d->setParentItemHelper(static_cast<QDeclarativeFlickablePrivate*>(prop->data)->contentItem, 0, 0);
        }
    } else {
        o->setParent(prop->object);
    }
}

int QDeclarativeFlickablePrivate::data_count(QDeclarativeListProperty<QObject> *property)
{
    QDeclarativeItem *contentItem= static_cast<QDeclarativeFlickablePrivate*>(property->data)->contentItem;
    return contentItem->childItems().count() + contentItem->children().count();
}

QObject *QDeclarativeFlickablePrivate::data_at(QDeclarativeListProperty<QObject> *property, int index)
{
    QDeclarativeItem *contentItem = static_cast<QDeclarativeFlickablePrivate*>(property->data)->contentItem;

    int childItemCount = contentItem->childItems().count();

    if (index < 0)
        return 0;

    if (index < childItemCount) {
        return contentItem->childItems().at(index)->toGraphicsObject();
    } else {
        return contentItem->children().at(index - childItemCount);
    }

    return 0;
}

void QDeclarativeFlickablePrivate::data_clear(QDeclarativeListProperty<QObject> *property)
{
    QDeclarativeItem *contentItem = static_cast<QDeclarativeFlickablePrivate*>(property->data)->contentItem;

    const QList<QGraphicsItem*> graphicsItems = contentItem->childItems();
    for (int i = 0; i < graphicsItems.count(); i++)
        contentItem->scene()->removeItem(graphicsItems[i]);

    const QList<QObject*> objects = contentItem->children();
    for (int i = 0; i < objects.count(); i++)
        objects[i]->setParent(0);
}

QDeclarativeListProperty<QObject> QDeclarativeFlickable::flickableData()
{
    Q_D(QDeclarativeFlickable);
    return QDeclarativeListProperty<QObject>(this, (void *)d, QDeclarativeFlickablePrivate::data_append,
                                                              QDeclarativeFlickablePrivate::data_count,
                                                              QDeclarativeFlickablePrivate::data_at,
                                                              QDeclarativeFlickablePrivate::data_clear);
}

QDeclarativeListProperty<QGraphicsObject> QDeclarativeFlickable::flickableChildren()
{
    Q_D(QDeclarativeFlickable);
    return QGraphicsItemPrivate::get(d->contentItem)->childrenList();
}

/*!
    \qmlproperty enumeration Flickable::boundsBehavior
    This property holds whether the surface may be dragged
    beyond the Fickable's boundaries, or overshoot the
    Flickable's boundaries when flicked.

    This enables the feeling that the edges of the view are soft,
    rather than a hard physical boundary.

    The \c boundsBehavior can be one of:

    \list
    \o Flickable.StopAtBounds - the contents can not be dragged beyond the boundary
    of the flickable, and flicks will not overshoot.
    \o Flickable.DragOverBounds - the contents can be dragged beyond the boundary
    of the Flickable, but flicks will not overshoot.
    \o Flickable.DragAndOvershootBounds - the contents can be dragged
    beyond the boundary of the Flickable, and can overshoot the
    boundary when flicked.
    \endlist

    The default is platform dependant.
*/
QDeclarativeFlickable::BoundsBehavior QDeclarativeFlickable::boundsBehavior() const
{
    Q_D(const QDeclarativeFlickable);
    return d->boundsBehavior;
}

void QDeclarativeFlickable::setBoundsBehavior(BoundsBehavior b)
{
    Q_D(QDeclarativeFlickable);
    if (b == d->boundsBehavior)
        return;
    d->boundsBehavior = b;

    QScroller *scroller = QScroller::scroller(this);
    QScrollerProperties props = scroller->scrollerProperties();
    if (b == StopAtBounds) {
        props.setScrollMetric(QScrollerProperties::OvershootDragDistanceFactor, 0.0);
        props.setScrollMetric(QScrollerProperties::OvershootScrollDistanceFactor, 0.0);

    } else if (b == DragOverBounds) {
        props.setScrollMetric(QScrollerProperties::OvershootDragDistanceFactor, 1.0);
        props.setScrollMetric(QScrollerProperties::OvershootScrollDistanceFactor, 0.0);

    } else {
        props.setScrollMetric(QScrollerProperties::OvershootDragDistanceFactor, 1.0);
        props.setScrollMetric(QScrollerProperties::OvershootScrollDistanceFactor, 1.0);
    }
    scroller->setScrollerProperties(props);

    emit boundsBehaviorChanged();
}

/*!
    \qmlproperty real Flickable::contentWidth
    \qmlproperty real Flickable::contentHeight

    The dimensions of the content (the surface controlled by Flickable).
    This should typically be set to the combined size of the items placed in the
    Flickable.

    The following snippet shows how these properties are used to display
    an image that is larger than the Flickable item itself:

    \snippet doc/src/snippets/declarative/flickable.qml document

    In some cases, the the content dimensions can be automatically set
    using the \l {Item::childrenRect.width}{childrenRect.width}
    and \l {Item::childrenRect.height}{childrenRect.height} properties.
*/
qreal QDeclarativeFlickable::contentWidth() const
{
    Q_D(const QDeclarativeFlickable);
    return d->contentItem->width();
}

void QDeclarativeFlickable::setContentWidth(qreal w)
{
    Q_D(QDeclarativeFlickable);
    if (d->contentItem->width() == w)
        return;

    if (w < 0)
        d->contentItem->setWidth(width());
    else
        d->contentItem->setWidth(w);

    // Make sure that we're entirely in view.
    QScroller *scroller = QScroller::scroller(this);
    if (scroller->state() == QScroller::Inactive) {
        scroller->ensureVisible(QRectF(minXExtent(), minYExtent(),
                                       d->contentItem->width(), d->contentItem->height()), 0, 0, 0);
    } else
        d->updateScrollerValues();

    emit contentWidthChanged();
    d->updateBeginningEnd();
}

qreal QDeclarativeFlickable::contentHeight() const
{
    Q_D(const QDeclarativeFlickable);
    return d->contentItem->height();
}

void QDeclarativeFlickable::setContentHeight(qreal h)
{
    Q_D(QDeclarativeFlickable);
    if (d->contentItem->height() == h)
        return;

    if (h < 0)
        d->contentItem->setHeight(height());
    else
        d->contentItem->setHeight(h);

    // Make sure that we're entirely in view.
    QScroller *scroller = QScroller::scroller(this);
    if (scroller->state() == QScroller::Inactive) {
        scroller->ensureVisible(QRectF(minXExtent(), minYExtent(),
                                       d->contentItem->width(), d->contentItem->height()), 0, 0, 0);
    } else
        d->updateScrollerValues();

    emit contentHeightChanged();
    d->updateBeginningEnd();
}

bool QDeclarativeFlickable::event(QEvent *event)
{
    Q_D(QDeclarativeFlickable);

    switch (event->type()) {
    case QEvent::ScrollPrepare: {
        if (!d->interactive)
            return false;

        QScrollPrepareEvent *se = static_cast<QScrollPrepareEvent *>(event);
        se->setViewportSize(QSizeF(d->width(), d->height()));

        QRectF contentPosRange(QPointF(minXExtent(), minYExtent()),
                               QPointF(maxXExtent() - d->width(), maxYExtent() - d->height()));
        if (d->flickableDirection == HorizontalFlick)
            contentPosRange.setHeight(0);
        if (d->flickableDirection == VerticalFlick)
            contentPosRange.setWidth(0);
        se->setContentPosRange(contentPosRange);
        se->setContentPos(-d->contentItem->pos());

        //qDebug() << "QDeclarativeFlickable::event: ScPrEv " << this
        //         << " pos:" << se->contentPos()
        //         << " range:" << se->contentPosRange();

        se->accept();
        d->updateScrollerValuesRequested = false;
        return true;
    }
    case QEvent::Scroll: {
        d->duringScrollEvent = true;

        QScrollEvent *se = static_cast<QScrollEvent *>(event);

        /*
        qDebug() << "QDeclarativeFlickable::event: Scroll "
                 << " scrollerPos:" << se->contentPos()
                 << " pos:" << -d->contentItem->pos()
                 << " overshoot:" << se->overshootDistance()
                 << " newPos: " << (se->contentPos() + se->overshootDistance());
                 */

        QPointF oldPos = -d->contentItem->pos();
        QPointF newPos = se->contentPos() + se->overshootDistance();
        QPointF delta = newPos - oldPos;

        viewportAboutToMove(newPos);
        d->contentItem->setPos(-newPos);
        viewportMoved();
        if (delta.x())
            emit contentXChanged();
        if (delta.y())
            emit contentYChanged();

        d->duringScrollEvent = false;
        if (d->updateScrollerValuesRequested)
            QScroller::scroller(this)->resendPrepareEvent();

         bool newMovingHorizontally = delta.x() != 0;
         bool newMovingVertically   = delta.y() != 0;
         if (se->scrollState() == QScrollEvent::ScrollFinished) {
             newMovingHorizontally = false;
             newMovingVertically = false;
         }

         if (newMovingHorizontally != d->movingHorizontally) {
             d->movingHorizontally = newMovingHorizontally;
             emit movingHorizontallyChanged();
         }
         if (newMovingVertically != d->movingVertically) {
             d->movingVertically = newMovingVertically;
             emit movingVerticallyChanged();
         }

         if (newMovingHorizontally)
             emit horizontalVelocityChanged();
         if (newMovingVertically)
             emit verticalVelocityChanged();

         se->accept();
         return true;
    }
    default:
        break;
    }
    return QDeclarativeItem::event(event);
}

/*!
    \qmlproperty real Flickable::maximumFlickVelocity
    This property holds the maximum velocity that the user can flick the view in pixel per second

    The default is platform dependant.
*/
qreal QDeclarativeFlickable::maximumFlickVelocity() const
{
    // 4000 ~ the conversion factor from [m/s] to [pix/s] on a 100dpi screen
    return 4000 * QScroller::scroller(const_cast<QDeclarativeFlickable*>(this))->scrollerProperties().scrollMetric(QScrollerProperties::MaximumVelocity).toDouble();
}

void QDeclarativeFlickable::setMaximumFlickVelocity(qreal v)
{
    // 4000 ~ the conversion factor from [m/s] to [pix/s] on a 100dpi screen
    v /= qreal(4000);
    QScrollerProperties props = QScroller::scroller(this)->scrollerProperties();
    qreal oldV = props.scrollMetric(QScrollerProperties::MaximumVelocity).toReal();
    if (v == oldV)
        return;
    props.setScrollMetric(QScrollerProperties::MaximumVelocity, v);
    QScroller::scroller(this)->setScrollerProperties(props);
    emit maximumFlickVelocityChanged();
}

/*!
    \qmlproperty real Flickable::flickDeceleration
    This property holds the rate at which a flick will decelerate.

    The default is platform dependant.
*/
qreal QDeclarativeFlickable::flickDeceleration() const
{
    // 4000 ~ the conversion factor from [m/s] to [pix/s] on a 100dpi screen
    return qreal(4000) * QScroller::scroller(const_cast<QDeclarativeFlickable*>(this))->scrollerProperties().scrollMetric(QScrollerProperties::DecelerationFactor).toDouble();
}

void QDeclarativeFlickable::setFlickDeceleration(qreal deceleration)
{
    // 4000 ~ the conversion factor from [m/s] to [pix/s] on a 100dpi screen
    deceleration /= qreal(4000);
    QScrollerProperties props = QScroller::scroller(this)->scrollerProperties();
    qreal oldDeceleration = props.scrollMetric(QScrollerProperties::DecelerationFactor).toReal();
    if (deceleration == oldDeceleration)
        return;
    props.setScrollMetric(QScrollerProperties::DecelerationFactor, deceleration);
    QScroller::scroller(this)->setScrollerProperties(props);
    emit flickDecelerationChanged();
}

bool QDeclarativeFlickable::isFlicking() const
{
    Q_D(const QDeclarativeFlickable);
    return d->isFlicking;
}

/*!
    \qmlproperty bool Flickable::flicking
    \qmlproperty bool Flickable::flickingHorizontally
    \qmlproperty bool Flickable::flickingVertically

    These properties describe whether the view is currently moving horizontally,
    vertically or in either direction, due to the user flicking the view.
*/
bool QDeclarativeFlickable::isFlickingHorizontally() const
{
    Q_D(const QDeclarativeFlickable);
    return d->movingHorizontally && d->isFlicking;
}

bool QDeclarativeFlickable::isFlickingVertically() const
{
    Q_D(const QDeclarativeFlickable);
    return d->movingVertically && d->isFlicking;
}

/*!
    \qmlproperty int Flickable::pressDelay

    This property holds the time to delay (ms) delivering a press to
    children of the Flickable.  This can be useful where reacting
    to a press before a flicking action has undesirable effects.

    If the flickable is dragged/flicked before the delay times out
    the press event will not be delivered.  If the button is released
    within the timeout, both the press and release will be delivered.
*/
int QDeclarativeFlickable::pressDelay() const
{
    return int(qreal(1000) * QScroller::scroller(const_cast<QDeclarativeFlickable*>(this))->scrollerProperties().scrollMetric(QScrollerProperties::MousePressEventDelay).toReal());
}

void QDeclarativeFlickable::setPressDelay(int delay)
{
    qreal pressDelay = qreal(delay) / qreal(1000); // [ms] -> [s]
    QScrollerProperties props = QScroller::scroller(this)->scrollerProperties();
    qreal oldPressDelay = props.scrollMetric(QScrollerProperties::MousePressEventDelay).toReal();
    if (pressDelay == oldPressDelay)
        return;
    props.setScrollMetric(QScrollerProperties::MousePressEventDelay, pressDelay);
    QScroller::scroller(this)->setScrollerProperties(props);
    emit pressDelayChanged();
}


bool QDeclarativeFlickable::isMoving() const
{
    Q_D(const QDeclarativeFlickable);
    return d->isMoving;
}

/*!
    \qmlproperty bool Flickable::moving
    \qmlproperty bool Flickable::movingHorizontally
    \qmlproperty bool Flickable::movingVertically

    These properties describe whether the view is currently moving horizontally,
    vertically or in either direction, due to the user either dragging or
    flicking the view.
*/
bool QDeclarativeFlickable::isMovingHorizontally() const
{
    Q_D(const QDeclarativeFlickable);
    return d->movingHorizontally;
}

bool QDeclarativeFlickable::isMovingVertically() const
{
    Q_D(const QDeclarativeFlickable);
    return d->movingVertically;
}

QT_END_NAMESPACE

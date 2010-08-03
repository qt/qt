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
#include <QTimer>

QT_BEGIN_NAMESPACE


// FlickThreshold determines how far the "mouse" must have moved
// before we perform a flick.
static const int FlickThreshold = 20;

// Really slow flicks can be annoying.
static const int minimumFlickVelocity = 200;

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

void QDeclarativeFlickableVisibleArea::updateVisible()
{
    QDeclarativeFlickablePrivate *p = static_cast<QDeclarativeFlickablePrivate *>(QGraphicsItemPrivate::get(flickable));
    bool pageChange = false;

    // Vertical
    const qreal viewheight = flickable->height();
    const qreal maxyextent = -flickable->maxYExtent() + flickable->minYExtent();
    qreal pagePos = (-p->vData.move.value() + flickable->minYExtent()) / (maxyextent + viewheight);
    qreal pageSize = viewheight / (maxyextent + viewheight);

    if (pageSize != m_heightRatio) {
        m_heightRatio = pageSize;
        pageChange = true;
    }
    if (pagePos != m_yPosition) {
        m_yPosition = pagePos;
        pageChange = true;
    }

    // Horizontal
    const qreal viewwidth = flickable->width();
    const qreal maxxextent = -flickable->maxXExtent() + flickable->minXExtent();
    pagePos = (-p->hData.move.value() + flickable->minXExtent()) / (maxxextent + viewwidth);
    pageSize = viewwidth / (maxxextent + viewwidth);

    if (pageSize != m_widthRatio) {
        m_widthRatio = pageSize;
        pageChange = true;
    }
    if (pagePos != m_xPosition) {
        m_xPosition = pagePos;
        pageChange = true;
    }
    if (pageChange)
        emit pageChanged();
}


QDeclarativeFlickablePrivate::QDeclarativeFlickablePrivate()
  : contentItem(new QDeclarativeItem)
    , hData(this, &QDeclarativeFlickablePrivate::setRoundedViewportX)
    , vData(this, &QDeclarativeFlickablePrivate::setRoundedViewportY)
    , flickingHorizontally(false), flickingVertically(false)
    , hMoved(false), vMoved(false)
    , movingHorizontally(false), movingVertically(false)
    , stealMouse(false), pressed(false)
    , interactive(true), deceleration(500), maxVelocity(2000), reportedVelocitySmoothing(100)
    , delayedPressEvent(0), delayedPressTarget(0), pressDelay(0), fixupDuration(600)
    , vTime(0), visibleArea(0)
    , flickableDirection(QDeclarativeFlickable::AutoFlickDirection)
    , boundsBehavior(QDeclarativeFlickable::DragAndOvershootBounds)
{
}

void QDeclarativeFlickablePrivate::init()
{
    Q_Q(QDeclarativeFlickable);
    QDeclarative_setParent_noEvent(contentItem, q);
    contentItem->setParentItem(q);
    static int timelineUpdatedIdx = -1;
    static int timelineCompletedIdx = -1;
    static int flickableTickedIdx = -1;
    static int flickableMovementEndingIdx = -1;
    if (timelineUpdatedIdx == -1) {
        timelineUpdatedIdx = QDeclarativeTimeLine::staticMetaObject.indexOfSignal("updated()");
        timelineCompletedIdx = QDeclarativeTimeLine::staticMetaObject.indexOfSignal("completed()");
        flickableTickedIdx = QDeclarativeFlickable::staticMetaObject.indexOfSlot("ticked()");
        flickableMovementEndingIdx = QDeclarativeFlickable::staticMetaObject.indexOfSlot("movementEnding()");
    }
    QMetaObject::connect(&timeline, timelineUpdatedIdx,
                         q, flickableTickedIdx, Qt::DirectConnection);
    QMetaObject::connect(&timeline, timelineCompletedIdx,
                         q, flickableMovementEndingIdx, Qt::DirectConnection);
    q->setAcceptedMouseButtons(Qt::LeftButton);
    q->setFiltersChildEvents(true);
    QDeclarativeItemPrivate *viewportPrivate = static_cast<QDeclarativeItemPrivate*>(QGraphicsItemPrivate::get(contentItem));
    viewportPrivate->addItemChangeListener(this, QDeclarativeItemPrivate::Geometry);
    lastPosTime.invalidate();
}

/*
    Returns the amount to overshoot by given a velocity.
    Will be roughly in range 0 - size/4
*/
qreal QDeclarativeFlickablePrivate::overShootDistance(qreal velocity, qreal size)
{
    if (maxVelocity <= 0)
        return 0.0;

    velocity = qAbs(velocity);
    if (velocity > maxVelocity)
        velocity = maxVelocity;
    qreal dist = size / 4 * velocity / maxVelocity;
    return dist;
}

void QDeclarativeFlickablePrivate::itemGeometryChanged(QDeclarativeItem *item, const QRectF &newGeom, const QRectF &oldGeom)
{
    Q_Q(QDeclarativeFlickable);
    if (item == contentItem) {
        if (newGeom.x() != oldGeom.x())
            emit q->contentXChanged();
        if (newGeom.y() != oldGeom.y())
            emit q->contentYChanged();
    }
}

void QDeclarativeFlickablePrivate::flickX(qreal velocity)
{
    Q_Q(QDeclarativeFlickable);
    flick(hData, q->minXExtent(), q->maxXExtent(), q->width(), fixupX_callback, velocity);
}

void QDeclarativeFlickablePrivate::flickY(qreal velocity)
{
    Q_Q(QDeclarativeFlickable);
    flick(vData, q->minYExtent(), q->maxYExtent(), q->height(), fixupY_callback, velocity);
}

void QDeclarativeFlickablePrivate::flick(AxisData &data, qreal minExtent, qreal maxExtent, qreal vSize,
                                         QDeclarativeTimeLineCallback::Callback fixupCallback, qreal velocity)
{
    Q_Q(QDeclarativeFlickable);
    qreal maxDistance = -1;
    bool overShoot = boundsBehavior == QDeclarativeFlickable::DragAndOvershootBounds;
    // -ve velocity means list is moving up
    if (velocity > 0) {
        if (data.move.value() < minExtent)
            maxDistance = qAbs(minExtent - data.move.value() + (overShoot?overShootDistance(velocity,vSize):0));
        data.flickTarget = minExtent;
    } else {
        if (data.move.value() > maxExtent)
            maxDistance = qAbs(maxExtent - data.move.value()) + (overShoot?overShootDistance(velocity,vSize):0);
        data.flickTarget = maxExtent;
    }
    if (maxDistance > 0) {
        qreal v = velocity;
        if (maxVelocity != -1 && maxVelocity < qAbs(v)) {
            if (v < 0)
                v = -maxVelocity;
            else
                v = maxVelocity;
        }
        timeline.reset(data.move);
        timeline.accel(data.move, v, deceleration, maxDistance);
        timeline.callback(QDeclarativeTimeLineCallback(&data.move, fixupCallback, this));
        if (!flickingHorizontally && q->xflick()) {
            flickingHorizontally = true;
            emit q->flickingChanged();
            emit q->flickingHorizontallyChanged();
            if (!flickingVertically)
                emit q->flickStarted();
        }
        if (!flickingVertically && q->yflick()) {
            flickingVertically = true;
            emit q->flickingChanged();
            emit q->flickingVerticallyChanged();
            if (!flickingHorizontally)
                emit q->flickStarted();
        }
    } else {
        timeline.reset(data.move);
        fixup(data, minExtent, maxExtent);
    }
}

void QDeclarativeFlickablePrivate::fixupY_callback(void *data)
{
    ((QDeclarativeFlickablePrivate *)data)->fixupY();
}

void QDeclarativeFlickablePrivate::fixupX_callback(void *data)
{
    ((QDeclarativeFlickablePrivate *)data)->fixupX();
}

void QDeclarativeFlickablePrivate::fixupX()
{
    Q_Q(QDeclarativeFlickable);
    fixup(hData, q->minXExtent(), q->maxXExtent());
}

void QDeclarativeFlickablePrivate::fixupY()
{
    Q_Q(QDeclarativeFlickable);
    fixup(vData, q->minYExtent(), q->maxYExtent());
}

void QDeclarativeFlickablePrivate::fixup(AxisData &data, qreal minExtent, qreal maxExtent)
{
    Q_Q(QDeclarativeFlickable);
    if (data.move.value() > minExtent || maxExtent > minExtent) {
        timeline.reset(data.move);
        if (data.move.value() != minExtent) {
            if (fixupDuration) {
                qreal dist = minExtent - data.move;
                timeline.move(data.move, minExtent - dist/2, QEasingCurve(QEasingCurve::InQuad), fixupDuration/4);
                timeline.move(data.move, minExtent, QEasingCurve(QEasingCurve::OutExpo), 3*fixupDuration/4);
            } else {
                data.move.setValue(minExtent);
                q->viewportMoved();
            }
        }
    } else if (data.move.value() < maxExtent) {
        timeline.reset(data.move);
        if (fixupDuration) {
            qreal dist = maxExtent - data.move;
            timeline.move(data.move, maxExtent - dist/2, QEasingCurve(QEasingCurve::InQuad), fixupDuration/4);
            timeline.move(data.move, maxExtent, QEasingCurve(QEasingCurve::OutExpo), 3*fixupDuration/4);
        } else {
            data.move.setValue(maxExtent);
            q->viewportMoved();
        }
    }
    vTime = timeline.time();
}

void QDeclarativeFlickablePrivate::updateBeginningEnd()
{
    Q_Q(QDeclarativeFlickable);
    bool atBoundaryChange = false;

    // Vertical
    const int maxyextent = int(-q->maxYExtent());
    const qreal ypos = -vData.move.value();
    bool atBeginning = (ypos <= -q->minYExtent());
    bool atEnd = (maxyextent <= ypos);

    if (atBeginning != vData.atBeginning) {
        vData.atBeginning = atBeginning;
        atBoundaryChange = true;
    }
    if (atEnd != vData.atEnd) {
        vData.atEnd = atEnd;
        atBoundaryChange = true;
    }

    // Horizontal
    const int maxxextent = int(-q->maxXExtent());
    const qreal xpos = -hData.move.value();
    atBeginning = (xpos <= -q->minXExtent());
    atEnd = (maxxextent <= xpos);

    if (atBeginning != hData.atBeginning) {
        hData.atBeginning = atBeginning;
        atBoundaryChange = true;
    }
    if (atEnd != hData.atEnd) {
        hData.atEnd = atEnd;
        atBoundaryChange = true;
    }

    if (atBoundaryChange)
        emit q->isAtBoundaryChanged();

    if (visibleArea)
        visibleArea->updateVisible();
}

/*!
    \qmlclass Flickable QDeclarativeFlickable
    \since 4.7
    \brief The Flickable item provides a surface that can be "flicked".
    \inherits Item

    Flickable places its children on a surface that can be dragged and flicked.

    \code
    import Qt 4.7

    Flickable {
        width: 200; height: 200
        contentWidth: image.width; contentHeight: image.height

        Image { id: image; source: "bigImage.png" }
    }
    \endcode

    \image flickable.gif

    Flickable does not automatically clip its contents. If
    it is not full-screen it is likely that \l {Item::clip}{clip} should be set
    to \c true.

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
    \qmlproperty int Flickable::contentX
    \qmlproperty int Flickable::contentY

    These properties hold the surface coordinate currently at the top-left
    corner of the Flickable. For example, if you flick an image up 100 pixels,
    \c contentY will be 100.
*/
qreal QDeclarativeFlickable::contentX() const
{
    Q_D(const QDeclarativeFlickable);
    return -d->hData.move.value();
}

void QDeclarativeFlickable::setContentX(qreal pos)
{
    Q_D(QDeclarativeFlickable);
    pos = qRound(pos);
    d->timeline.reset(d->hData.move);
    d->vTime = d->timeline.time();
    if (-pos != d->hData.move.value()) {
        d->hData.move.setValue(-pos);
        viewportMoved();
    }
}

qreal QDeclarativeFlickable::contentY() const
{
    Q_D(const QDeclarativeFlickable);
    return -d->vData.move.value();
}

void QDeclarativeFlickable::setContentY(qreal pos)
{
    Q_D(QDeclarativeFlickable);
    pos = qRound(pos);
    d->timeline.reset(d->vData.move);
    d->vTime = d->timeline.time();
    if (-pos != d->vData.move.value()) {
        d->vData.move.setValue(-pos);
        viewportMoved();
    }
}

/*!
    \qmlproperty bool Flickable::interactive

    This property holds whether the user can interact with the Flickable. A user
    cannot drag or flick a Flickable that is not interactive.

    This property is useful for temporarily disabling flicking. This allows
    special interaction with Flickable's children: for example, you might want to
    freeze a flickable map while scrolling through a pop-up dialog that is a child of the Flickable.
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
        if (!interactive && (d->flickingHorizontally || d->flickingVertically)) {
            d->timeline.clear();
            d->vTime = d->timeline.time();
            d->flickingHorizontally = false;
            d->flickingVertically = false;
            emit flickingChanged();
            emit flickingHorizontallyChanged();
            emit flickingVerticallyChanged();
            emit flickEnded();
        }
        emit interactiveChanged();
    }
}

/*!
    \qmlproperty real Flickable::horizontalVelocity
    \qmlproperty real Flickable::verticalVelocity

    The instantaneous velocity of movement along the x and y axes, in pixels/sec.

    The reported velocity is smoothed to avoid erratic output.
*/
qreal QDeclarativeFlickable::horizontalVelocity() const
{
    Q_D(const QDeclarativeFlickable);
    return d->hData.smoothVelocity.value();
}

qreal QDeclarativeFlickable::verticalVelocity() const
{
    Q_D(const QDeclarativeFlickable);
    return d->vData.smoothVelocity.value();
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
    return d->hData.atEnd;
}

bool QDeclarativeFlickable::isAtXBeginning() const
{
    Q_D(const QDeclarativeFlickable);
    return d->hData.atBeginning;
}

bool QDeclarativeFlickable::isAtYEnd() const
{
    Q_D(const QDeclarativeFlickable);
    return d->vData.atEnd;
}

bool QDeclarativeFlickable::isAtYBeginning() const
{
    Q_D(const QDeclarativeFlickable);
    return d->vData.atBeginning;
}

void QDeclarativeFlickable::ticked()
{
    viewportMoved();
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

void QDeclarativeFlickablePrivate::handleMousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (interactive && timeline.isActive() && (qAbs(hData.velocity) > 10 || qAbs(vData.velocity) > 10))
        stealMouse = true; // If we've been flicked then steal the click.
    else
        stealMouse = false;
    pressed = true;
    timeline.clear();
    hData.velocity = 0;
    vData.velocity = 0;
    lastPos = QPoint();
    QDeclarativeItemPrivate::start(lastPosTime);
    pressPos = event->pos();
    hData.pressPos = hData.move.value();
    vData.pressPos = vData.move.value();
    flickingHorizontally = false;
    flickingVertically = false;
    QDeclarativeItemPrivate::start(pressTime);
    QDeclarativeItemPrivate::start(velocityTime);
}

void QDeclarativeFlickablePrivate::handleMouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    Q_Q(QDeclarativeFlickable);
    if (!interactive || !lastPosTime.isValid())
        return;
    bool rejectY = false;
    bool rejectX = false;

    if (q->yflick()) {
        int dy = int(event->pos().y() - pressPos.y());
        if (qAbs(dy) > QApplication::startDragDistance() || QDeclarativeItemPrivate::elapsed(pressTime) > 200) {
            qreal newY = dy + vData.pressPos;
            const qreal minY = q->minYExtent();
            const qreal maxY = q->maxYExtent();
            if (newY > minY)
                newY = minY + (newY - minY) / 2;
            if (newY < maxY && maxY - minY <= 0)
                newY = maxY + (newY - maxY) / 2;
            if (boundsBehavior == QDeclarativeFlickable::StopAtBounds && (newY > minY || newY < maxY)) {
                rejectY = true;
                if (newY < maxY) {
                    newY = maxY;
                    rejectY = false;
                }
                if (newY > minY) {
                    newY = minY;
                    rejectY = false;
                }
            }
            if (!rejectY && stealMouse) {
                vData.move.setValue(qRound(newY));
                vMoved = true;
            }
            if (qAbs(dy) > QApplication::startDragDistance())
                stealMouse = true;
        }
    }

    if (q->xflick()) {
        int dx = int(event->pos().x() - pressPos.x());
        if (qAbs(dx) > QApplication::startDragDistance() || QDeclarativeItemPrivate::elapsed(pressTime) > 200) {
            qreal newX = dx + hData.pressPos;
            const qreal minX = q->minXExtent();
            const qreal maxX = q->maxXExtent();
            if (newX > minX)
                newX = minX + (newX - minX) / 2;
            if (newX < maxX && maxX - minX <= 0)
                newX = maxX + (newX - maxX) / 2;
            if (boundsBehavior == QDeclarativeFlickable::StopAtBounds && (newX > minX || newX < maxX)) {
                rejectX = true;
                if (newX < maxX) {
                    newX = maxX;
                    rejectX = false;
                }
                if (newX > minX) {
                    newX = minX;
                    rejectX = false;
                }
            }
            if (!rejectX && stealMouse) {
                hData.move.setValue(qRound(newX));
                hMoved = true;
            }

            if (qAbs(dx) > QApplication::startDragDistance())
                stealMouse = true;
        }
    }

    if (!lastPos.isNull()) {
        qreal elapsed = qreal(QDeclarativeItemPrivate::restart(lastPosTime)) / 1000.;
        if (elapsed <= 0)
            elapsed = 1;
        if (q->yflick()) {
            qreal diff = event->pos().y() - lastPos.y();
            // average to reduce the effect of spurious moves
            vData.velocity += diff / elapsed;
            vData.velocity /= 2;
        }

        if (q->xflick()) {
            qreal diff = event->pos().x() - lastPos.x();
            // average to reduce the effect of spurious moves
            hData.velocity += diff / elapsed;
            hData.velocity /= 2;
        }
    }

    if (rejectY) vData.velocity = 0;
    if (rejectX) hData.velocity = 0;

    if (hMoved || vMoved) {
        q->movementStarting();
        q->viewportMoved();
    }

    lastPos = event->pos();
}

void QDeclarativeFlickablePrivate::handleMouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_Q(QDeclarativeFlickable);
    stealMouse = false;
    q->setKeepMouseGrab(false);
    pressed = false;
    if (!lastPosTime.isValid())
        return;

    if (QDeclarativeItemPrivate::elapsed(lastPosTime) > 100) {
        // if we drag then pause before release we should not cause a flick.
        hData.velocity = 0.0;
        vData.velocity = 0.0;
    }

    vTime = timeline.time();
    if (qAbs(vData.velocity) > 10 && qAbs(event->pos().y() - pressPos.y()) > FlickThreshold) {
        qreal velocity = vData.velocity;
        if (qAbs(velocity) < minimumFlickVelocity) // Minimum velocity to avoid annoyingly slow flicks.
            velocity = velocity < 0 ? -minimumFlickVelocity : minimumFlickVelocity;
        flickY(velocity);
    } else {
        fixupY();
    }

    if (qAbs(hData.velocity) > 10 && qAbs(event->pos().x() - pressPos.x()) > FlickThreshold) {
        qreal velocity = hData.velocity;
        if (qAbs(velocity) < minimumFlickVelocity) // Minimum velocity to avoid annoyingly slow flicks.
            velocity = velocity < 0 ? -minimumFlickVelocity : minimumFlickVelocity;
        flickX(velocity);
    } else {
        fixupX();
    }

    lastPosTime.invalidate();

    if (!timeline.isActive())
        q->movementEnding();
}

void QDeclarativeFlickable::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QDeclarativeFlickable);
    if (d->interactive) {
        d->handleMousePressEvent(event);
        event->accept();
    } else {
        QDeclarativeItem::mousePressEvent(event);
    }
}

void QDeclarativeFlickable::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QDeclarativeFlickable);
    if (d->interactive) {
        d->handleMouseMoveEvent(event);
        if (d->stealMouse)
            setKeepMouseGrab(true);
        event->accept();
    } else {
        QDeclarativeItem::mouseMoveEvent(event);
    }
}

void QDeclarativeFlickable::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QDeclarativeFlickable);
    if (d->interactive) {
        d->clearDelayedPress();
        d->handleMouseReleaseEvent(event);
        event->accept();
        ungrabMouse();
    } else {
        QDeclarativeItem::mouseReleaseEvent(event);
    }
}

void QDeclarativeFlickable::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    Q_D(QDeclarativeFlickable);
    if (!d->interactive) {
        QDeclarativeItem::wheelEvent(event);
    } else if (yflick()) {
        if (event->delta() > 0)
            d->vData.velocity = qMax(event->delta() - d->vData.smoothVelocity.value(), qreal(250.0));
        else
            d->vData.velocity = qMin(event->delta() - d->vData.smoothVelocity.value(), qreal(-250.0));
        d->flickingVertically = false;
        d->flickY(d->vData.velocity);
        if (d->flickingVertically) {
            d->vMoved = true;
            movementStarting();
        }
        event->accept();
    } else if (xflick()) {
        if (event->delta() > 0)
            d->hData.velocity = qMax(event->delta() - d->hData.smoothVelocity.value(), qreal(250.0));
        else
            d->hData.velocity = qMin(event->delta() - d->hData.smoothVelocity.value(), qreal(-250.0));
        d->flickingHorizontally = false;
        d->flickX(d->hData.velocity);
        if (d->flickingHorizontally) {
            d->hMoved = true;
            movementStarting();
        }
        event->accept();
    } else {
        QDeclarativeItem::wheelEvent(event);
    }
}

void QDeclarativeFlickablePrivate::captureDelayedPress(QGraphicsSceneMouseEvent *event)
{
    Q_Q(QDeclarativeFlickable);
    if (!q->scene() || pressDelay <= 0)
        return;
    delayedPressTarget = q->scene()->mouseGrabberItem();
    delayedPressEvent = new QGraphicsSceneMouseEvent(event->type());
    delayedPressEvent->setAccepted(false);
    for (int i = 0x1; i <= 0x10; i <<= 1) {
        if (event->buttons() & i) {
            Qt::MouseButton button = Qt::MouseButton(i);
            delayedPressEvent->setButtonDownPos(button, event->buttonDownPos(button));
            delayedPressEvent->setButtonDownScenePos(button, event->buttonDownScenePos(button));
            delayedPressEvent->setButtonDownScreenPos(button, event->buttonDownScreenPos(button));
        }
    }
    delayedPressEvent->setButtons(event->buttons());
    delayedPressEvent->setButton(event->button());
    delayedPressEvent->setPos(event->pos());
    delayedPressEvent->setScenePos(event->scenePos());
    delayedPressEvent->setScreenPos(event->screenPos());
    delayedPressEvent->setLastPos(event->lastPos());
    delayedPressEvent->setLastScenePos(event->lastScenePos());
    delayedPressEvent->setLastScreenPos(event->lastScreenPos());
    delayedPressEvent->setModifiers(event->modifiers());
    delayedPressTimer.start(pressDelay, q);
}

void QDeclarativeFlickablePrivate::clearDelayedPress()
{
    if (delayedPressEvent) {
        delayedPressTimer.stop();
        delete delayedPressEvent;
        delayedPressEvent = 0;
    }
}

void QDeclarativeFlickablePrivate::setRoundedViewportX(qreal x)
{
    contentItem->setX(qRound(x));
}

void QDeclarativeFlickablePrivate::setRoundedViewportY(qreal y)
{
    contentItem->setY(qRound(y));
}

void QDeclarativeFlickable::timerEvent(QTimerEvent *event)
{
    Q_D(QDeclarativeFlickable);
    if (event->timerId() == d->delayedPressTimer.timerId()) {
        d->delayedPressTimer.stop();
        if (d->delayedPressEvent) {
            QDeclarativeItem *grabber = scene() ? qobject_cast<QDeclarativeItem*>(scene()->mouseGrabberItem()) : 0;
            if (!grabber || grabber != this) {
                // We replay the mouse press but the grabber we had might not be interessted by the event (e.g. overlay)
                // so we reset the grabber
                if (scene()->mouseGrabberItem() == d->delayedPressTarget)
                    d->delayedPressTarget->ungrabMouse();
                //Use the event handler that will take care of finding the proper item to propagate the event
                QApplication::sendEvent(scene(), d->delayedPressEvent);
            }
            delete d->delayedPressEvent;
            d->delayedPressEvent = 0;
        }
    }
}

qreal QDeclarativeFlickable::minYExtent() const
{
    return 0.0;
}

qreal QDeclarativeFlickable::minXExtent() const
{
    return 0.0;
}

/* returns -ve */
qreal QDeclarativeFlickable::maxXExtent() const
{
    return width() - vWidth();
}
/* returns -ve */
qreal QDeclarativeFlickable::maxYExtent() const
{
    return height() - vHeight();
}

void QDeclarativeFlickable::viewportMoved()
{
    Q_D(QDeclarativeFlickable);

    qreal prevY = d->lastFlickablePosition.x();
    qreal prevX = d->lastFlickablePosition.y();
    d->velocityTimeline.clear();
    if (d->pressed) {
        int elapsed = QDeclarativeItemPrivate::restart(d->velocityTime);
        if (elapsed > 0) {
            qreal horizontalVelocity = (prevX - d->hData.move.value()) * 1000 / elapsed;
            qreal verticalVelocity = (prevY - d->vData.move.value()) * 1000 / elapsed;
            d->velocityTimeline.move(d->hData.smoothVelocity, horizontalVelocity, d->reportedVelocitySmoothing);
            d->velocityTimeline.move(d->hData.smoothVelocity, 0, d->reportedVelocitySmoothing);
            d->velocityTimeline.move(d->vData.smoothVelocity, verticalVelocity, d->reportedVelocitySmoothing);
            d->velocityTimeline.move(d->vData.smoothVelocity, 0, d->reportedVelocitySmoothing);
        }
    } else {
        if (d->timeline.time() > d->vTime) {
            qreal horizontalVelocity = (prevX - d->hData.move.value()) * 1000 / (d->timeline.time() - d->vTime);
            qreal verticalVelocity = (prevY - d->vData.move.value()) * 1000 / (d->timeline.time() - d->vTime);
            d->hData.smoothVelocity.setValue(horizontalVelocity);
            d->vData.smoothVelocity.setValue(verticalVelocity);
        }
    }

    d->lastFlickablePosition = QPointF(d->vData.move.value(), d->hData.move.value());

    d->vTime = d->timeline.time();
    d->updateBeginningEnd();
}

void QDeclarativeFlickable::geometryChanged(const QRectF &newGeometry,
                             const QRectF &oldGeometry)
{
    Q_D(QDeclarativeFlickable);
    QDeclarativeItem::geometryChanged(newGeometry, oldGeometry);

    bool changed = false;
    if (newGeometry.width() != oldGeometry.width()) {
        if (d->hData.viewSize < 0) {
            d->contentItem->setWidth(width());
            emit contentWidthChanged();
        }
    }
    if (newGeometry.height() != oldGeometry.height()) {
        if (d->vData.viewSize < 0) {
            d->contentItem->setHeight(height());
            emit contentHeightChanged();
        }
    }

    if (changed)
        d->updateBeginningEnd();
}

void QDeclarativeFlickable::cancelFlick()
{
    Q_D(QDeclarativeFlickable);
    d->timeline.reset(d->hData.move);
    d->timeline.reset(d->vData.move);
    movementEnding();
}

void QDeclarativeFlickablePrivate::data_append(QDeclarativeListProperty<QObject> *prop, QObject *o)
{
    QDeclarativeItem *i = qobject_cast<QDeclarativeItem *>(o);
    if (i)
        i->setParentItem(static_cast<QDeclarativeFlickablePrivate*>(prop->data)->contentItem);
    else
        o->setParent(prop->object);
}

QDeclarativeListProperty<QObject> QDeclarativeFlickable::flickableData()
{
    Q_D(QDeclarativeFlickable);
    return QDeclarativeListProperty<QObject>(this, (void *)d, QDeclarativeFlickablePrivate::data_append);
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
    \o Flickable.DragAndOvershootBounds (default) - the contents can be dragged
    beyond the boundary of the Flickable, and can overshoot the
    boundary when flicked.
    \endlist
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
    emit boundsBehaviorChanged();
}

/*!
    \qmlproperty int Flickable::contentWidth
    \qmlproperty int Flickable::contentHeight

    The dimensions of the content (the surface controlled by Flickable). Typically this
    should be set to the combined size of the items placed in the Flickable. Note this
    can be set automatically using \l {Item::childrenRect.width}{childrenRect.width}
    and \l {Item::childrenRect.height}{childrenRect.height}. For example:

    \code
    Flickable {
        width: 320; height: 480
        contentWidth: childrenRect.width; contentHeight: childrenRect.height

        Image { id: image; source: "bigImage.png" }
    }
    \endcode
*/
qreal QDeclarativeFlickable::contentWidth() const
{
    Q_D(const QDeclarativeFlickable);
    return d->hData.viewSize;
}

void QDeclarativeFlickable::setContentWidth(qreal w)
{
    Q_D(QDeclarativeFlickable);
    if (d->hData.viewSize == w)
        return;
    d->hData.viewSize = w;
    if (w < 0)
        d->contentItem->setWidth(width());
    else
        d->contentItem->setWidth(w);
    // Make sure that we're entirely in view.
    if (!d->pressed && !d->movingHorizontally && !d->movingVertically) {
        int oldDuration = d->fixupDuration;
        d->fixupDuration = 0;
        d->fixupX();
        d->fixupDuration = oldDuration;
    }
    emit contentWidthChanged();
    d->updateBeginningEnd();
}

qreal QDeclarativeFlickable::contentHeight() const
{
    Q_D(const QDeclarativeFlickable);
    return d->vData.viewSize;
}

void QDeclarativeFlickable::setContentHeight(qreal h)
{
    Q_D(QDeclarativeFlickable);
    if (d->vData.viewSize == h)
        return;
    d->vData.viewSize = h;
    if (h < 0)
        d->contentItem->setHeight(height());
    else
        d->contentItem->setHeight(h);
    // Make sure that we're entirely in view.
    if (!d->pressed && !d->movingHorizontally && !d->movingVertically) {
        int oldDuration = d->fixupDuration;
        d->fixupDuration = 0;
        d->fixupY();
        d->fixupDuration = oldDuration;
    }
    emit contentHeightChanged();
    d->updateBeginningEnd();
}

qreal QDeclarativeFlickable::vWidth() const
{
    Q_D(const QDeclarativeFlickable);
    if (d->hData.viewSize < 0)
        return width();
    else
        return d->hData.viewSize;
}

qreal QDeclarativeFlickable::vHeight() const
{
    Q_D(const QDeclarativeFlickable);
    if (d->vData.viewSize < 0)
        return height();
    else
        return d->vData.viewSize;
}

bool QDeclarativeFlickable::xflick() const
{
    Q_D(const QDeclarativeFlickable);
    if (d->flickableDirection == QDeclarativeFlickable::AutoFlickDirection)
        return vWidth() != width();
    return d->flickableDirection & QDeclarativeFlickable::HorizontalFlick;
}

bool QDeclarativeFlickable::yflick() const
{
    Q_D(const QDeclarativeFlickable);
    if (d->flickableDirection == QDeclarativeFlickable::AutoFlickDirection)
        return vHeight() !=  height();
    return d->flickableDirection & QDeclarativeFlickable::VerticalFlick;
}

bool QDeclarativeFlickable::sendMouseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QDeclarativeFlickable);
    QGraphicsSceneMouseEvent mouseEvent(event->type());
    QRectF myRect = mapToScene(QRectF(0, 0, width(), height())).boundingRect();

    QGraphicsScene *s = scene();
    QDeclarativeItem *grabber = s ? qobject_cast<QDeclarativeItem*>(s->mouseGrabberItem()) : 0;
    bool stealThisEvent = d->stealMouse;
    if ((stealThisEvent || myRect.contains(event->scenePos().toPoint())) && (!grabber || !grabber->keepMouseGrab())) {
        mouseEvent.setAccepted(false);
        for (int i = 0x1; i <= 0x10; i <<= 1) {
            if (event->buttons() & i) {
                Qt::MouseButton button = Qt::MouseButton(i);
                mouseEvent.setButtonDownPos(button, mapFromScene(event->buttonDownPos(button)));
            }
        }
        mouseEvent.setScenePos(event->scenePos());
        mouseEvent.setLastScenePos(event->lastScenePos());
        mouseEvent.setPos(mapFromScene(event->scenePos()));
        mouseEvent.setLastPos(mapFromScene(event->lastScenePos()));

        switch(mouseEvent.type()) {
        case QEvent::GraphicsSceneMouseMove:
            d->handleMouseMoveEvent(&mouseEvent);
            break;
        case QEvent::GraphicsSceneMousePress:
            if (d->delayedPressEvent)
                return false;

            d->handleMousePressEvent(&mouseEvent);
            d->captureDelayedPress(event);
            stealThisEvent = d->stealMouse;   // Update stealThisEvent in case changed by function call above
            break;
        case QEvent::GraphicsSceneMouseRelease:
            if (d->delayedPressEvent) {
                // We replay the mouse press but the grabber we had might not be interessted by the event (e.g. overlay)
                // so we reset the grabber
                if (s->mouseGrabberItem() == d->delayedPressTarget)
                    d->delayedPressTarget->ungrabMouse();
                //Use the event handler that will take care of finding the proper item to propagate the event
                QApplication::sendEvent(scene(), d->delayedPressEvent);
                d->clearDelayedPress();
                // We send the release
                scene()->sendEvent(s->mouseGrabberItem(), event);
                // And the event has been consumed
                return true;
            }
            d->handleMouseReleaseEvent(&mouseEvent);
            break;
        default:
            break;
        }
        grabber = qobject_cast<QDeclarativeItem*>(s->mouseGrabberItem());
        if (grabber && stealThisEvent && !grabber->keepMouseGrab() && grabber != this) {
            d->clearDelayedPress();
            grabMouse();
        }

        return stealThisEvent || d->delayedPressEvent;
    } else if (d->lastPosTime.isValid()) {
        d->lastPosTime.invalidate();
    }
    if (mouseEvent.type() == QEvent::GraphicsSceneMouseRelease) {
        d->clearDelayedPress();
        d->stealMouse = false;
    }
    return false;
}

bool QDeclarativeFlickable::sceneEventFilter(QGraphicsItem *i, QEvent *e)
{
    Q_D(QDeclarativeFlickable);
    if (!isVisible() || !d->interactive)
        return QDeclarativeItem::sceneEventFilter(i, e);
    switch (e->type()) {
    case QEvent::GraphicsSceneMousePress:
    case QEvent::GraphicsSceneMouseMove:
    case QEvent::GraphicsSceneMouseRelease:
        return sendMouseEvent(static_cast<QGraphicsSceneMouseEvent *>(e));
    default:
        break;
    }

    return QDeclarativeItem::sceneEventFilter(i, e);
}

/*!
    \qmlproperty real Flickable::maximumFlickVelocity
    This property holds the maximum velocity that the user can flick the view in pixels/second.

    The default is 2000 pixels/s
*/
qreal QDeclarativeFlickable::maximumFlickVelocity() const
{
    Q_D(const QDeclarativeFlickable);
    return d->maxVelocity;
}

void QDeclarativeFlickable::setMaximumFlickVelocity(qreal v)
{
    Q_D(QDeclarativeFlickable);
    if (v == d->maxVelocity)
        return;
    d->maxVelocity = v;
    emit maximumFlickVelocityChanged();
}

/*!
    \qmlproperty real Flickable::flickDeceleration
    This property holds the rate at which a flick will decelerate.

    The default is 500.
*/
qreal QDeclarativeFlickable::flickDeceleration() const
{
    Q_D(const QDeclarativeFlickable);
    return d->deceleration;
}

void QDeclarativeFlickable::setFlickDeceleration(qreal deceleration)
{
    Q_D(QDeclarativeFlickable);
    if (deceleration == d->deceleration)
        return;
    d->deceleration = deceleration;
    emit flickDecelerationChanged();
}

bool QDeclarativeFlickable::isFlicking() const
{
    Q_D(const QDeclarativeFlickable);
    return d->flickingHorizontally ||  d->flickingVertically;
}

/*!
    \qmlproperty bool Flickable::flicking
    \qmlproperty bool Flickable::flickingHorizontally
    \qmlproperty bool Flickable::flickingVertically

    These properties hold whether the view is currently moving horizontally
    or vertically due to the user flicking the view.
*/
bool QDeclarativeFlickable::isFlickingHorizontally() const
{
    Q_D(const QDeclarativeFlickable);
    return d->flickingHorizontally;
}

bool QDeclarativeFlickable::isFlickingVertically() const
{
    Q_D(const QDeclarativeFlickable);
    return d->flickingVertically;
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
    Q_D(const QDeclarativeFlickable);
    return d->pressDelay;
}

void QDeclarativeFlickable::setPressDelay(int delay)
{
    Q_D(QDeclarativeFlickable);
    if (d->pressDelay == delay)
        return;
    d->pressDelay = delay;
    emit pressDelayChanged();
}


bool QDeclarativeFlickable::isMoving() const
{
    Q_D(const QDeclarativeFlickable);
    return d->movingHorizontally || d->movingVertically;
}

/*!
    \qmlproperty bool Flickable::moving
    \qmlproperty bool Flickable::movingHorizontally
    \qmlproperty bool Flickable::movingVertically

    These properties hold whether the view is currently moving horizontally
    or vertically due to the user either dragging or flicking the view.
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

void QDeclarativeFlickable::movementStarting()
{
    Q_D(QDeclarativeFlickable);
    if (d->hMoved && !d->movingHorizontally) {
        d->movingHorizontally = true;
        emit movingChanged();
        emit movingHorizontallyChanged();
        if (!d->movingVertically)
            emit movementStarted();
    }
    else if (d->vMoved && !d->movingVertically) {
        d->movingVertically = true;
        emit movingChanged();
        emit movingVerticallyChanged();
        if (!d->movingHorizontally)
            emit movementStarted();
    }
}

void QDeclarativeFlickable::movementEnding()
{
    Q_D(QDeclarativeFlickable);
    if (d->flickingHorizontally) {
        d->flickingHorizontally = false;
        emit flickingChanged();
        emit flickingHorizontallyChanged();
        if (!d->flickingVertically)
           emit flickEnded();
    }
    if (d->flickingVertically) {
        d->flickingVertically = false;
        emit flickingChanged();
        emit flickingVerticallyChanged();
        if (!d->flickingHorizontally)
           emit flickEnded();
    }
    if (!d->pressed && !d->stealMouse) {
        if (d->movingHorizontally) {
            d->movingHorizontally = false;
            d->hMoved = false;
            emit movingChanged();
            emit movingHorizontallyChanged();
            if (!d->movingVertically)
                emit movementEnded();
        }
        if (d->movingVertically) {
            d->movingVertically = false;
            d->vMoved = false;
            emit movingChanged();
            emit movingVerticallyChanged();
            if (!d->movingHorizontally)
                emit movementEnded();
        }
    }
    d->hData.smoothVelocity.setValue(0);
    d->vData.smoothVelocity.setValue(0);
}

void QDeclarativeFlickablePrivate::updateVelocity()
{
    Q_Q(QDeclarativeFlickable);
    emit q->horizontalVelocityChanged();
    emit q->verticalVelocityChanged();
}

QT_END_NAMESPACE

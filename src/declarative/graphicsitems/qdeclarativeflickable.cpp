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

#include "qdeclarativeflickable_p.h"
#include "qdeclarativeflickable_p_p.h"

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
    qreal pagePos = (-p->_moveY.value() + flickable->minYExtent()) / (maxyextent + viewheight);
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
    pagePos = (-p->_moveX.value() + flickable->minXExtent()) / (maxxextent + viewwidth);
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
  : viewport(new QDeclarativeItem)
    , _moveX(this, &QDeclarativeFlickablePrivate::setRoundedViewportX)
    , _moveY(this, &QDeclarativeFlickablePrivate::setRoundedViewportY)
    , vWidth(-1), vHeight(-1), overShoot(true), flicked(false), moving(false), stealMouse(false)
    , pressed(false), atXEnd(false), atXBeginning(true), atYEnd(false), atYBeginning(true)
    , interactive(true), deceleration(500), maxVelocity(2000), reportedVelocitySmoothing(100)
    , delayedPressEvent(0), delayedPressTarget(0), pressDelay(0), fixupDuration(200)
    , horizontalVelocity(this), verticalVelocity(this), vTime(0), visibleArea(0)
    , flickDirection(QDeclarativeFlickable::AutoFlickDirection)
{
}

void QDeclarativeFlickablePrivate::init()
{
    Q_Q(QDeclarativeFlickable);
    viewport->setParent(q);
    QObject::connect(&timeline, SIGNAL(updated()), q, SLOT(ticked()));
    QObject::connect(&timeline, SIGNAL(completed()), q, SLOT(movementEnding()));
    q->setAcceptedMouseButtons(Qt::LeftButton);
    q->setFiltersChildEvents(true);
    QObject::connect(viewport, SIGNAL(xChanged()), q, SIGNAL(contentXChanged()));
    QObject::connect(viewport, SIGNAL(yChanged()), q, SIGNAL(contentYChanged()));
    QObject::connect(q, SIGNAL(heightChanged()), q, SLOT(heightChange()));
    QObject::connect(q, SIGNAL(widthChanged()), q, SLOT(widthChange()));
}

void QDeclarativeFlickablePrivate::flickX(qreal velocity)
{
    Q_Q(QDeclarativeFlickable);
    qreal maxDistance = -1;
    // -ve velocity means list is moving up
    if (velocity > 0) {
        const qreal minX = q->minXExtent();
        if (_moveX.value() < minX)
            maxDistance = qAbs(minX -_moveX.value() + (overShoot?30:0));
        flickTargetX = minX;
    } else {
        const qreal maxX = q->maxXExtent();
        if (_moveX.value() > maxX)
            maxDistance = qAbs(maxX - _moveX.value()) + (overShoot?30:0);
        flickTargetX = maxX;
    }
    if (maxDistance > 0) {
        qreal v = velocity;
        if (maxVelocity != -1 && maxVelocity < qAbs(v)) {
            if (v < 0)
                v = -maxVelocity;
            else
                v = maxVelocity;
        }
        timeline.reset(_moveX);
        timeline.accel(_moveX, v, deceleration, maxDistance);
        timeline.callback(QDeclarativeTimeLineCallback(&_moveX, fixupX_callback, this));
        if (!flicked) {
            flicked = true;
            emit q->flickingChanged();
            emit q->flickStarted();
        }
    } else {
        timeline.reset(_moveX);
        fixupX();
    }
}

void QDeclarativeFlickablePrivate::flickY(qreal velocity)
{
    Q_Q(QDeclarativeFlickable);
    qreal maxDistance = -1;
    // -ve velocity means list is moving up
    if (velocity > 0) {
        const qreal minY = q->minYExtent();
        if (_moveY.value() < minY)
            maxDistance = qAbs(minY -_moveY.value() + (overShoot?30:0));
        flickTargetY = minY;
    } else {
        const qreal maxY = q->maxYExtent();
        if (_moveY.value() > maxY)
            maxDistance = qAbs(maxY - _moveY.value()) + (overShoot?30:0);
        flickTargetY = maxY;
    }
    if (maxDistance > 0) {
        qreal v = velocity;
        if (maxVelocity != -1 && maxVelocity < qAbs(v)) {
            if (v < 0)
                v = -maxVelocity;
            else
                v = maxVelocity;
        }
        timeline.reset(_moveY);
        timeline.accel(_moveY, v, deceleration, maxDistance);
        timeline.callback(QDeclarativeTimeLineCallback(&_moveY, fixupY_callback, this));
        if (!flicked) {
            flicked = true;
            emit q->flickingChanged();
            emit q->flickStarted();
        }
    } else {
        timeline.reset(_moveY);
        fixupY();
    }
}

void QDeclarativeFlickablePrivate::fixupX()
{
    Q_Q(QDeclarativeFlickable);
    if (!q->xflick() || _moveX.timeLine())
        return;

    if (_moveX.value() > q->minXExtent() || (q->maxXExtent() > q->minXExtent())) {
        timeline.reset(_moveX);
        if (_moveX.value() != q->minXExtent()) {
            if (fixupDuration)
                timeline.move(_moveX, q->minXExtent(), QEasingCurve(QEasingCurve::InOutQuad), fixupDuration);
            else
                _moveY.setValue(q->minYExtent());
        }
        //emit flickingChanged();
    } else if (_moveX.value() < q->maxXExtent()) {
        timeline.reset(_moveX);
        if (fixupDuration)
            timeline.move(_moveX,  q->maxXExtent(), QEasingCurve(QEasingCurve::InOutQuad), fixupDuration);
        else
            _moveY.setValue(q->maxYExtent());
        //emit flickingChanged();
    } else {
        flicked = false;
    }

    vTime = timeline.time();
}

void QDeclarativeFlickablePrivate::fixupY_callback(void *data)
{
    ((QDeclarativeFlickablePrivate *)data)->fixupY();
}

void QDeclarativeFlickablePrivate::fixupX_callback(void *data)
{
    ((QDeclarativeFlickablePrivate *)data)->fixupX();
}

void QDeclarativeFlickablePrivate::fixupY()
{
    Q_Q(QDeclarativeFlickable);
    if (!q->yflick() || _moveY.timeLine())
        return;

    if (_moveY.value() > q->minYExtent() || (q->maxYExtent() > q->minYExtent())) {
        timeline.reset(_moveY);
        if (_moveY.value() != q->minYExtent()) {
            if (fixupDuration)
                timeline.move(_moveY, q->minYExtent(), QEasingCurve(QEasingCurve::InOutQuad), fixupDuration);
            else
                _moveY.setValue(q->minYExtent());
        }
        //emit flickingChanged();
    } else if (_moveY.value() < q->maxYExtent()) {
        timeline.reset(_moveY);
        if (fixupDuration)
            timeline.move(_moveY,  q->maxYExtent(), QEasingCurve(QEasingCurve::InOutQuad), fixupDuration);
        else
            _moveY.setValue(q->maxYExtent());
        //emit flickingChanged();
    } else {
        flicked = false;
    }

    vTime = timeline.time();
}

void QDeclarativeFlickablePrivate::updateBeginningEnd()
{
    Q_Q(QDeclarativeFlickable);
    bool atBoundaryChange = false;

    // Vertical
    const int maxyextent = int(-q->maxYExtent());
    const qreal ypos = -_moveY.value();
    bool atBeginning = (ypos <= -q->minYExtent());
    bool atEnd = (maxyextent <= ypos);

    if (atBeginning != atYBeginning) {
        atYBeginning = atBeginning;
        atBoundaryChange = true;
    }
    if (atEnd != atYEnd) {
        atYEnd = atEnd;
        atBoundaryChange = true;
    }

    // Horizontal
    const int maxxextent = int(-q->maxXExtent());
    const qreal xpos = -_moveX.value();
    atBeginning = (xpos <= -q->minXExtent());
    atEnd = (maxxextent <= xpos);

    if (atBeginning != atXBeginning) {
        atXBeginning = atBeginning;
        atBoundaryChange = true;
    }
    if (atEnd != atXEnd) {
        atXEnd = atEnd;
        atBoundaryChange = true;
    }

    if (atBoundaryChange)
        emit q->isAtBoundaryChanged();

    if (visibleArea)
        visibleArea->updateVisible();
}

/*!
    \qmlclass Flickable QDeclarativeFlickable
    \brief The Flickable item provides a surface that can be "flicked".
    \inherits Item

    Flickable places its children on a surface that can be dragged and flicked.

    \code
    Flickable {
        width: 200; height: 200; contentWidth: image.width; contentHeight: image.height
        Image { id: image; source: "bigimage.png" }
    }
    \endcode

    \image flickable.gif

    \note Flickable does not automatically clip its contents. If
    it is not full-screen it is likely that \c clip should be set
    to true.

    \note Due to an implementation detail items placed inside a flickable cannot anchor to it by
    id, use 'parent' instead.
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
    scaled to 0.0 - 1.0.  The page position is in the range 0.0 (beginning) to
    size ratio (end), i.e. yPosition is in the range 0.0 - heightRatio.

    These properties are typically used to draw a scrollbar, for example:
    \code
    Rectangle {
        opacity: 0.5; anchors.right: MyListView.right-2; width: 6
        y: MyListView.visibleArea.yPosition * MyListView.height
        height: MyListView.visibleArea.heightRatio * MyListView.height
    }
    \endcode
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
    return -d->_moveX.value();
}

void QDeclarativeFlickable::setContentX(qreal pos)
{
    Q_D(QDeclarativeFlickable);
    pos = qRound(pos);
    d->timeline.reset(d->_moveX);
    d->vTime = d->timeline.time();
    if (-pos != d->_moveX.value()) {
        d->_moveX.setValue(-pos);
        viewportMoved();
    }
}

qreal QDeclarativeFlickable::contentY() const
{
    Q_D(const QDeclarativeFlickable);
    return -d->_moveY.value();
}

void QDeclarativeFlickable::setContentY(qreal pos)
{
    Q_D(QDeclarativeFlickable);
    pos = qRound(pos);
    d->timeline.reset(d->_moveY);
    d->vTime = d->timeline.time();
    if (-pos != d->_moveY.value()) {
        d->_moveY.setValue(-pos);
        viewportMoved();
    }
}

/*!
    \qmlproperty bool Flickable::interactive

    A user cannot drag or flick a Flickable that is not interactive.

    This property is useful for temporarily disabling flicking. This allows
    special interaction with Flickable's children: for example, you might want to
    freeze a flickable map while viewing detailed information on a location popup that is a child of the Flickable.
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
        if (!interactive && d->flicked) {
            d->timeline.clear();
            d->vTime = d->timeline.time();
            d->flicked = false;
            emit flickingChanged();
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
    return d->horizontalVelocity.value();
}

qreal QDeclarativeFlickable::verticalVelocity() const
{
    Q_D(const QDeclarativeFlickable);
    return d->verticalVelocity.value();
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
    return d->atXEnd;
}

bool QDeclarativeFlickable::isAtXBeginning() const
{
    Q_D(const QDeclarativeFlickable);
    return d->atXBeginning;
}

bool QDeclarativeFlickable::isAtYEnd() const
{
    Q_D(const QDeclarativeFlickable);
    return d->atYEnd;
}

bool QDeclarativeFlickable::isAtYBeginning() const
{
    Q_D(const QDeclarativeFlickable);
    return d->atYBeginning;
}

void QDeclarativeFlickable::ticked()
{
    viewportMoved();
}

QDeclarativeItem *QDeclarativeFlickable::viewport()
{
    Q_D(QDeclarativeFlickable);
    return d->viewport;
}

QDeclarativeFlickableVisibleArea *QDeclarativeFlickable::visibleArea()
{
    Q_D(QDeclarativeFlickable);
    if (!d->visibleArea)
        d->visibleArea = new QDeclarativeFlickableVisibleArea(this);
    return d->visibleArea;
}

/*!
    \qmlproperty enumeration Flickable::flickDirection

    This property determines which directions the view can be flicked.

    \list
    \o AutoFlickDirection (default) - allows flicking vertically if the
    \e contentHeight is not equal to the \e height of the Flickable.
    Allows flicking horizontally if the \e contentWidth is not equal
    to the \e width of the Flickable.
    \o HorizontalFlick - allows flicking horizontally.
    \o VerticalFlick - allows flicking vertically.
    \o HorizontalAndVerticalFlick - allows flicking in both directions.
    \endlist
*/
QDeclarativeFlickable::FlickDirection QDeclarativeFlickable::flickDirection() const
{
    Q_D(const QDeclarativeFlickable);
    return d->flickDirection;
}

void QDeclarativeFlickable::setFlickDirection(FlickDirection direction)
{
    Q_D(QDeclarativeFlickable);
    if (direction != d->flickDirection) {
        d->flickDirection = direction;
        emit flickDirectionChanged();
    }
}

void QDeclarativeFlickablePrivate::handleMousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (interactive && timeline.isActive() && (qAbs(velocityX) > 10 || qAbs(velocityY) > 10))
        stealMouse = true; // If we've been flicked then steal the click.
    else
        stealMouse = false;
    pressed = true;
    timeline.clear();
    velocityX = 0;
    velocityY = 0;
    lastPos = QPoint();
    QDeclarativeItemPrivate::start(lastPosTime);
    pressPos = event->pos();
    pressX = _moveX.value();
    pressY = _moveY.value();
    flicked = false;
    QDeclarativeItemPrivate::start(pressTime);
    QDeclarativeItemPrivate::start(velocityTime);
}

void QDeclarativeFlickablePrivate::handleMouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    Q_Q(QDeclarativeFlickable);
    if (!interactive || lastPosTime.isNull())
        return;
    bool rejectY = false;
    bool rejectX = false;
    bool moved = false;

    if (q->yflick()) {
        int dy = int(event->pos().y() - pressPos.y());
        if (qAbs(dy) > QApplication::startDragDistance() || QDeclarativeItemPrivate::elapsed(pressTime) > 200) {
            qreal newY = dy + pressY;
            const qreal minY = q->minYExtent();
            const qreal maxY = q->maxYExtent();
            if (newY > minY)
                newY = minY + (newY - minY) / 2;
            if (newY < maxY && maxY - minY <= 0)
                newY = maxY + (newY - maxY) / 2;
            if (!q->overShoot() && (newY > minY || newY < maxY)) {
                if (newY > minY)
                    newY = minY;
                else if (newY < maxY)
                    newY = maxY;
                else
                    rejectY = true;
            }
            if (!rejectY) {
                _moveY.setValue(newY);
                moved = true;
            }
            if (qAbs(dy) > QApplication::startDragDistance())
                stealMouse = true;
        }
    }

    if (q->xflick()) {
        int dx = int(event->pos().x() - pressPos.x());
        if (qAbs(dx) > QApplication::startDragDistance() || QDeclarativeItemPrivate::elapsed(pressTime) > 200) {
            qreal newX = dx + pressX;
            const qreal minX = q->minXExtent();
            const qreal maxX = q->maxXExtent();
            if (newX > minX)
                newX = minX + (newX - minX) / 2;
            if (newX < maxX && maxX - minX <= 0)
                newX = maxX + (newX - maxX) / 2;
            if (!q->overShoot() && (newX > minX || newX < maxX)) {
                if (newX > minX)
                    newX = minX;
                else if (newX < maxX)
                    newX = maxX;
                else
                    rejectX = true;
            }
            if (!rejectX) {
                _moveX.setValue(newX);
                moved = true;
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
            velocityY += diff / elapsed;
            velocityY /= 2;
        }

        if (q->xflick()) {
            qreal diff = event->pos().x() - lastPos.x();
            // average to reduce the effect of spurious moves
            velocityX += diff / elapsed;
            velocityX /= 2;
        }
    }

    if (rejectY) velocityY = 0;
    if (rejectX) velocityX = 0;

    if (moved) {
        q->movementStarting();
        q->viewportMoved();
    }

    lastPos = event->pos();
}

void QDeclarativeFlickablePrivate::handleMouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_Q(QDeclarativeFlickable);
    pressed = false;
    if (lastPosTime.isNull())
        return;

    if (QDeclarativeItemPrivate::elapsed(lastPosTime) > 100) {
        // if we drag then pause before release we should not cause a flick.
        velocityX = 0.0;
        velocityY = 0.0;
    }

    vTime = timeline.time();
    if (qAbs(velocityY) > 10 && qAbs(event->pos().y() - pressPos.y()) > FlickThreshold) {
        qreal velocity = velocityY;
        if (qAbs(velocity) < minimumFlickVelocity) // Minimum velocity to avoid annoyingly slow flicks.
            velocity = velocity < 0 ? -minimumFlickVelocity : minimumFlickVelocity;
        flickY(velocity);
    } else {
        fixupY();
    }

    if (qAbs(velocityX) > 10 && qAbs(event->pos().x() - pressPos.x()) > FlickThreshold) {
        qreal velocity = velocityX;
        if (qAbs(velocity) < minimumFlickVelocity) // Minimum velocity to avoid annoyingly slow flicks.
            velocity = velocity < 0 ? -minimumFlickVelocity : minimumFlickVelocity;
        flickX(velocity);
    } else {
        fixupX();
    }

    stealMouse = false;
    lastPosTime = QTime();

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
            d->velocityY = qMax(event->delta() - d->verticalVelocity.value(), qreal(250.0));
        else
            d->velocityY = qMin(event->delta() - d->verticalVelocity.value(), qreal(-250.0));
        d->flicked = false;
        d->flickY(d->velocityY);
        event->accept();
    } else if (xflick()) {
        if (event->delta() > 0)
            d->velocityX = qMax(event->delta() - d->horizontalVelocity.value(), qreal(250.0));
        else
            d->velocityX = qMin(event->delta() - d->horizontalVelocity.value(), qreal(-250.0));
        d->flicked = false;
        d->flickX(d->velocityX);
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
    viewport->setX(qRound(x));
}

void QDeclarativeFlickablePrivate::setRoundedViewportY(qreal y)
{
    viewport->setY(qRound(y));
}

void QDeclarativeFlickable::timerEvent(QTimerEvent *event)
{
    Q_D(QDeclarativeFlickable);
    if (event->timerId() == d->delayedPressTimer.timerId()) {
        d->delayedPressTimer.stop();
        if (d->delayedPressEvent) {
            QDeclarativeItem *grabber = scene() ? qobject_cast<QDeclarativeItem*>(scene()->mouseGrabberItem()) : 0;
            if (!grabber || grabber != this)
                scene()->sendEvent(d->delayedPressTarget, d->delayedPressEvent);
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

    int elapsed = QDeclarativeItemPrivate::restart(d->velocityTime);
    if (!elapsed)
        return;

    qreal prevY = d->lastFlickablePosition.x();
    qreal prevX = d->lastFlickablePosition.y();
    d->velocityTimeline.clear();
    if (d->pressed) {
        qreal horizontalVelocity = (prevX - d->_moveX.value()) * 1000 / elapsed;
        qreal verticalVelocity = (prevY - d->_moveY.value()) * 1000 / elapsed;
        d->velocityTimeline.move(d->horizontalVelocity, horizontalVelocity, d->reportedVelocitySmoothing);
        d->velocityTimeline.move(d->horizontalVelocity, 0, d->reportedVelocitySmoothing);
        d->velocityTimeline.move(d->verticalVelocity, verticalVelocity, d->reportedVelocitySmoothing);
        d->velocityTimeline.move(d->verticalVelocity, 0, d->reportedVelocitySmoothing);
    } else {
        if (d->timeline.time() > d->vTime) {
            qreal horizontalVelocity = (prevX - d->_moveX.value()) * 1000 / (d->timeline.time() - d->vTime);
            qreal verticalVelocity = (prevY - d->_moveY.value()) * 1000 / (d->timeline.time() - d->vTime);
            d->horizontalVelocity.setValue(horizontalVelocity);
            d->verticalVelocity.setValue(verticalVelocity);
        }
    }

    d->lastFlickablePosition = QPointF(d->_moveY.value(), d->_moveX.value());

    d->vTime = d->timeline.time();
    d->updateBeginningEnd();
}

void QDeclarativeFlickable::cancelFlick()
{
    Q_D(QDeclarativeFlickable);
    d->timeline.reset(d->_moveX);
    d->timeline.reset(d->_moveY);
    movementEnding();
}

void QDeclarativeFlickablePrivate::data_append(QDeclarativeListProperty<QObject> *prop, QObject *o)
{
    QDeclarativeItem *i = qobject_cast<QDeclarativeItem *>(o);
    if (i)
        i->setParentItem(static_cast<QDeclarativeFlickablePrivate*>(prop->data)->viewport);
    else
        o->setParent(prop->object);
}

QDeclarativeListProperty<QObject> QDeclarativeFlickable::flickableData()
{
    Q_D(QDeclarativeFlickable);
    return QDeclarativeListProperty<QObject>(this, (void *)d, QDeclarativeFlickablePrivate::data_append);
}

QDeclarativeListProperty<QDeclarativeItem> QDeclarativeFlickable::flickableChildren()
{
    Q_D(QDeclarativeFlickable);
    return d->viewport->fxChildren();
}

/*!
    \qmlproperty bool Flickable::overShoot
    This property holds whether the surface may overshoot the
    Flickable's boundaries when flicked.

    If overShoot is true the contents can be flicked beyond the boundary
    of the Flickable before being moved back to the boundary.  This provides
    the feeling that the edges of the view are soft, rather than a hard
    physical boundary.
*/
bool QDeclarativeFlickable::overShoot() const
{
    Q_D(const QDeclarativeFlickable);
    return d->overShoot;
}

void QDeclarativeFlickable::setOverShoot(bool o)
{
    Q_D(QDeclarativeFlickable);
    if (d->overShoot == o)
        return;
    d->overShoot = o;
    emit overShootChanged();
}

/*!
    \qmlproperty int Flickable::contentWidth
    \qmlproperty int Flickable::contentHeight

    The dimensions of the content (the surface controlled by Flickable). Typically this
    should be set to the combined size of the items placed in the Flickable.

    \code
    Flickable {
        width: 320; height: 480; contentWidth: image.width; contentHeight: image.height
        Image { id: image; source: "bigimage.png" }
    }
    \endcode
*/
qreal QDeclarativeFlickable::contentWidth() const
{
    Q_D(const QDeclarativeFlickable);
    return d->vWidth;
}

void QDeclarativeFlickable::setContentWidth(qreal w)
{
    Q_D(QDeclarativeFlickable);
    if (d->vWidth == w)
        return;
    d->vWidth = w;
    if (w < 0)
        d->viewport->setWidth(width());
    else
        d->viewport->setWidth(w);
    // Make sure that we're entirely in view.
    if (!d->pressed)
        d->fixupX();
    emit contentWidthChanged();
    d->updateBeginningEnd();
}

void QDeclarativeFlickable::widthChange()
{
    Q_D(QDeclarativeFlickable);
    if (d->vWidth < 0) {
        d->viewport->setWidth(width());
        emit contentWidthChanged();
    }
    d->updateBeginningEnd();
}

void QDeclarativeFlickable::heightChange()
{
    Q_D(QDeclarativeFlickable);
    if (d->vHeight < 0) {
        d->viewport->setHeight(height());
        emit contentHeightChanged();
    }
    d->updateBeginningEnd();
}

qreal QDeclarativeFlickable::contentHeight() const
{
    Q_D(const QDeclarativeFlickable);
    return d->vHeight;
}

void QDeclarativeFlickable::setContentHeight(qreal h)
{
    Q_D(QDeclarativeFlickable);
    if (d->vHeight == h)
        return;
    d->vHeight = h;
    if (h < 0)
        d->viewport->setHeight(height());
    else
        d->viewport->setHeight(h);
    // Make sure that we're entirely in view.
    if (!d->pressed)
        d->fixupY();
    emit contentHeightChanged();
    d->updateBeginningEnd();
}

qreal QDeclarativeFlickable::vWidth() const
{
    Q_D(const QDeclarativeFlickable);
    if (d->vWidth < 0)
        return width();
    else
        return d->vWidth;
}

qreal QDeclarativeFlickable::vHeight() const
{
    Q_D(const QDeclarativeFlickable);
    if (d->vHeight < 0)
        return height();
    else
        return d->vHeight;
}

bool QDeclarativeFlickable::xflick() const
{
    Q_D(const QDeclarativeFlickable);
    if (d->flickDirection == QDeclarativeFlickable::AutoFlickDirection)
        return vWidth() != width();
    return d->flickDirection & QDeclarativeFlickable::HorizontalFlick;
}

bool QDeclarativeFlickable::yflick() const
{
    Q_D(const QDeclarativeFlickable);
    if (d->flickDirection == QDeclarativeFlickable::AutoFlickDirection)
        return vHeight() !=  height();
    return d->flickDirection & QDeclarativeFlickable::VerticalFlick;
}

bool QDeclarativeFlickable::sendMouseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QDeclarativeFlickable);
    QGraphicsSceneMouseEvent mouseEvent(event->type());
    QRectF myRect = mapToScene(QRectF(0, 0, width(), height())).boundingRect();

    QGraphicsScene *s = scene();
    QDeclarativeItem *grabber = s ? qobject_cast<QDeclarativeItem*>(s->mouseGrabberItem()) : 0;
    if ((d->stealMouse || myRect.contains(event->scenePos().toPoint())) && (!grabber || !grabber->keepMouseGrab())) {
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
            break;
        case QEvent::GraphicsSceneMouseRelease:
            if (d->delayedPressEvent) {
                scene()->sendEvent(d->delayedPressTarget, d->delayedPressEvent);
                d->clearDelayedPress();
            }
            d->handleMouseReleaseEvent(&mouseEvent);
            break;
        default:
            break;
        }
        grabber = qobject_cast<QDeclarativeItem*>(s->mouseGrabberItem());
        if (grabber && d->stealMouse && !grabber->keepMouseGrab() && grabber != this) {
            d->clearDelayedPress();
            grabMouse();
        }

        return d->stealMouse || d->delayedPressEvent;
    } else if (!d->lastPosTime.isNull()) {
        d->lastPosTime = QTime();
    }
    if (mouseEvent.type() == QEvent::GraphicsSceneMouseRelease)
        d->clearDelayedPress();
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

/*!
    \qmlproperty bool Flickable::flicking

    This property holds whether the view is currently moving due to
    the user flicking the view.
*/
bool QDeclarativeFlickable::isFlicking() const
{
    Q_D(const QDeclarativeFlickable);
    return d->flicked;
}

/*!
    \qmlproperty int Flickable::pressDelay

    This property holds the time to delay (ms) delivering a press to
    children of the Flickable.  This can be useful where reacting
    to a press before a flicking action has undesireable effects.

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

/*!
    \qmlproperty bool Flickable::moving

    This property holds whether the view is currently moving due to
    the user either dragging or flicking the view.
*/
bool QDeclarativeFlickable::isMoving() const
{
    Q_D(const QDeclarativeFlickable);
    return d->moving;
}

void QDeclarativeFlickable::movementStarting()
{
    Q_D(QDeclarativeFlickable);
    if (!d->moving) {
        d->moving = true;
        emit movingChanged();
        emit movementStarted();
    }
}

void QDeclarativeFlickable::movementEnding()
{
    Q_D(QDeclarativeFlickable);
    if (d->moving) {
        d->moving = false;
        emit movingChanged();
        emit movementEnded();
    }
    if (d->flicked) {
        d->flicked = false;
        emit flickingChanged();
        emit flickEnded();
    }
    d->horizontalVelocity.setValue(0);
    d->verticalVelocity.setValue(0);
}

void QDeclarativeFlickablePrivate::updateVelocity()
{
    Q_Q(QDeclarativeFlickable);
    emit q->horizontalVelocityChanged();
    emit q->verticalVelocityChanged();
}

QT_END_NAMESPACE

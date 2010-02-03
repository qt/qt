/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qmlgraphicsflickable_p.h"
#include "qmlgraphicsflickable_p_p.h"

#include <QGraphicsSceneMouseEvent>
#include <QPointer>
#include <QTimer>

QT_BEGIN_NAMESPACE


// FlickThreshold determines how far the "mouse" must have moved
// before we perform a flick.
static const int FlickThreshold = 20;

// Really slow flicks can be annoying.
static const int minimumFlickVelocity = 200;

class QmlGraphicsFlickableVisibleArea : public QObject
{
    Q_OBJECT

    Q_PROPERTY(qreal xPosition READ xPosition NOTIFY pageChanged)
    Q_PROPERTY(qreal yPosition READ yPosition NOTIFY pageChanged)
    Q_PROPERTY(qreal widthRatio READ widthRatio NOTIFY pageChanged)
    Q_PROPERTY(qreal heightRatio READ heightRatio NOTIFY pageChanged)

public:
    QmlGraphicsFlickableVisibleArea(QmlGraphicsFlickable *parent=0);

    qreal xPosition() const;
    qreal widthRatio() const;
    qreal yPosition() const;
    qreal heightRatio() const;

    void updateVisible();

signals:
    void pageChanged();

private:
    QmlGraphicsFlickable *flickable;
    qreal m_xPosition;
    qreal m_widthRatio;
    qreal m_yPosition;
    qreal m_heightRatio;
};

QmlGraphicsFlickableVisibleArea::QmlGraphicsFlickableVisibleArea(QmlGraphicsFlickable *parent)
    : QObject(parent), flickable(parent), m_xPosition(0.), m_widthRatio(0.)
    , m_yPosition(0.), m_heightRatio(0.)
{
}

qreal QmlGraphicsFlickableVisibleArea::widthRatio() const
{
    return m_widthRatio;
}

qreal QmlGraphicsFlickableVisibleArea::xPosition() const
{
    return m_xPosition;
}

qreal QmlGraphicsFlickableVisibleArea::heightRatio() const
{
    return m_heightRatio;
}

qreal QmlGraphicsFlickableVisibleArea::yPosition() const
{
    return m_yPosition;
}

void QmlGraphicsFlickableVisibleArea::updateVisible()
{
    QmlGraphicsFlickablePrivate *p = static_cast<QmlGraphicsFlickablePrivate *>(QGraphicsItemPrivate::get(flickable));
    bool pageChange = false;

    // Vertical
    const qreal viewheight = flickable->height();
    const qreal maxyextent = -flickable->maxYExtent();
    qreal pagePos = -p->_moveY.value() / (maxyextent + viewheight);
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
    const qreal maxxextent = -flickable->maxXExtent();
    pagePos = -p->_moveX.value() / (maxxextent + viewwidth);
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


QmlGraphicsFlickablePrivate::QmlGraphicsFlickablePrivate()
  : viewport(new QmlGraphicsItem)
    , _moveX(this, &QmlGraphicsFlickablePrivate::setRoundedViewportX)
    , _moveY(this, &QmlGraphicsFlickablePrivate::setRoundedViewportY)
    , vWidth(-1), vHeight(-1), overShoot(true), flicked(false), moving(false), stealMouse(false)
    , pressed(false), atXEnd(false), atXBeginning(true), atYEnd(false), atYBeginning(true)
    , interactive(true), deceleration(500), maxVelocity(2000), reportedVelocitySmoothing(100)
    , delayedPressEvent(0), delayedPressTarget(0), pressDelay(0), fixupDuration(200)
    , horizontalVelocity(this), verticalVelocity(this), vTime(0), visibleArea(0)
    , flickDirection(QmlGraphicsFlickable::AutoFlickDirection)
{
    fixupXEvent = QmlTimeLineEvent::timeLineEvent<QmlGraphicsFlickablePrivate, &QmlGraphicsFlickablePrivate::fixupX>(&_moveX, this);
    fixupYEvent = QmlTimeLineEvent::timeLineEvent<QmlGraphicsFlickablePrivate, &QmlGraphicsFlickablePrivate::fixupY>(&_moveY, this);
}

void QmlGraphicsFlickablePrivate::init()
{
    Q_Q(QmlGraphicsFlickable);
    viewport->setParent(q);
    QObject::connect(&timeline, SIGNAL(updated()), q, SLOT(ticked()));
    QObject::connect(&timeline, SIGNAL(completed()), q, SLOT(movementEnding()));
    q->setAcceptedMouseButtons(Qt::LeftButton);
    q->setFiltersChildEvents(true);
    QObject::connect(viewport, SIGNAL(xChanged()), q, SIGNAL(positionXChanged()));
    QObject::connect(viewport, SIGNAL(yChanged()), q, SIGNAL(positionYChanged()));
    QObject::connect(q, SIGNAL(heightChanged()), q, SLOT(heightChange()));
    QObject::connect(q, SIGNAL(widthChanged()), q, SLOT(widthChange()));
}

void QmlGraphicsFlickablePrivate::flickX(qreal velocity)
{
    Q_Q(QmlGraphicsFlickable);
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
        timeline.execute(fixupXEvent);
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

void QmlGraphicsFlickablePrivate::flickY(qreal velocity)
{
    Q_Q(QmlGraphicsFlickable);
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
        timeline.execute(fixupYEvent);
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

void QmlGraphicsFlickablePrivate::fixupX()
{
    Q_Q(QmlGraphicsFlickable);
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

void QmlGraphicsFlickablePrivate::fixupY()
{
    Q_Q(QmlGraphicsFlickable);
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

void QmlGraphicsFlickablePrivate::updateBeginningEnd()
{
    Q_Q(QmlGraphicsFlickable);
    bool atBoundaryChange = false;

    // Vertical
    const int maxyextent = int(-q->maxYExtent());
    const qreal ypos = -_moveY.value();
    bool atBeginning = (ypos <= 0.0);
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
    atBeginning = (xpos <= 0.0);
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

QML_DEFINE_TYPE(Qt,4,6,Flickable,QmlGraphicsFlickable)

/*!
    \qmlclass Flickable QmlGraphicsFlickable
    \brief The Flickable item provides a surface that can be "flicked".
    \inherits Item

    Flickable places its children on a surface that can be dragged and flicked.

    \code
    Flickable {
        width: 200; height: 200; viewportWidth: image.width; viewportHeight: image.height
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

QmlGraphicsFlickable::QmlGraphicsFlickable(QmlGraphicsItem *parent)
  : QmlGraphicsItem(*(new QmlGraphicsFlickablePrivate), parent)
{
    Q_D(QmlGraphicsFlickable);
    d->init();
}

QmlGraphicsFlickable::QmlGraphicsFlickable(QmlGraphicsFlickablePrivate &dd, QmlGraphicsItem *parent)
  : QmlGraphicsItem(dd, parent)
{
    Q_D(QmlGraphicsFlickable);
    d->init();
}

QmlGraphicsFlickable::~QmlGraphicsFlickable()
{
}

/*!
    \qmlproperty int Flickable::viewportX
    \qmlproperty int Flickable::viewportY

    These properties hold the surface coordinate currently at the top-left
    corner of the Flickable. For example, if you flick an image up 100 pixels,
    \c yPosition will be 100.
*/
qreal QmlGraphicsFlickable::viewportX() const
{
    Q_D(const QmlGraphicsFlickable);
    return -d->_moveX.value();
}

void QmlGraphicsFlickable::setViewportX(qreal pos)
{
    Q_D(QmlGraphicsFlickable);
    pos = qRound(pos);
    d->timeline.reset(d->_moveX);
    d->vTime = d->timeline.time();
    if (-pos != d->_moveX.value()) {
        d->_moveX.setValue(-pos);
        viewportMoved();
    }
}

qreal QmlGraphicsFlickable::viewportY() const
{
    Q_D(const QmlGraphicsFlickable);
    return -d->_moveY.value();
}

void QmlGraphicsFlickable::setViewportY(qreal pos)
{
    Q_D(QmlGraphicsFlickable);
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
bool QmlGraphicsFlickable::isInteractive() const
{
    Q_D(const QmlGraphicsFlickable);
    return d->interactive;
}

void QmlGraphicsFlickable::setInteractive(bool interactive)
{
    Q_D(QmlGraphicsFlickable);
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
    \qmlproperty real Flickable::reportedVelocitySmoothing

    The instantaneous velocity of movement along the x and y axes, in pixels/sec.

    The reported velocity is smoothed to avoid erratic output.
    reportedVelocitySmoothing determines how much smoothing is applied.
*/
qreal QmlGraphicsFlickable::horizontalVelocity() const
{
    Q_D(const QmlGraphicsFlickable);
    return d->horizontalVelocity.value();
}

qreal QmlGraphicsFlickable::verticalVelocity() const
{
    Q_D(const QmlGraphicsFlickable);
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
bool QmlGraphicsFlickable::isAtXEnd() const
{
    Q_D(const QmlGraphicsFlickable);
    return d->atXEnd;
}

bool QmlGraphicsFlickable::isAtXBeginning() const
{
    Q_D(const QmlGraphicsFlickable);
    return d->atXBeginning;
}

bool QmlGraphicsFlickable::isAtYEnd() const
{
    Q_D(const QmlGraphicsFlickable);
    return d->atYEnd;
}

bool QmlGraphicsFlickable::isAtYBeginning() const
{
    Q_D(const QmlGraphicsFlickable);
    return d->atYBeginning;
}

void QmlGraphicsFlickable::ticked()
{
    viewportMoved();
}

QmlGraphicsItem *QmlGraphicsFlickable::viewport()
{
    Q_D(QmlGraphicsFlickable);
    return d->viewport;
}

QmlGraphicsFlickableVisibleArea *QmlGraphicsFlickable::visibleArea()
{
    Q_D(QmlGraphicsFlickable);
    if (!d->visibleArea)
        d->visibleArea = new QmlGraphicsFlickableVisibleArea(this);
    return d->visibleArea;
}

/*!
    \qmlproperty enumeration Flickable::flickDirection

    This property determines which directions the view can be flicked.

    \list
    \o AutoFlickDirection (default) - allows flicking vertically if the
    \e viewportHeight is not equal to the \e height of the Flickable.
    Allows flicking horizontally if the \e viewportWidth is not equal
    to the \e width of the Flickable.
    \o HorizontalFlick - allows flicking horizontally.
    \o VerticalFlick - allows flicking vertically.
    \o HorizontalAndVerticalFlick - allows flicking in both directions.
    \endlist
*/
QmlGraphicsFlickable::FlickDirection QmlGraphicsFlickable::flickDirection() const
{
    Q_D(const QmlGraphicsFlickable);
    return d->flickDirection;
}

void QmlGraphicsFlickable::setFlickDirection(FlickDirection direction)
{
    Q_D(QmlGraphicsFlickable);
    if (direction != d->flickDirection) {
        d->flickDirection = direction;
        emit flickDirectionChanged();
    }
}

void QmlGraphicsFlickablePrivate::handleMousePressEvent(QGraphicsSceneMouseEvent *event)
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
    QmlGraphicsItemPrivate::start(lastPosTime);
    pressPos = event->pos();
    pressX = _moveX.value();
    pressY = _moveY.value();
    flicked = false;
    QmlGraphicsItemPrivate::start(pressTime);
    QmlGraphicsItemPrivate::start(velocityTime);
}

void QmlGraphicsFlickablePrivate::handleMouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    Q_Q(QmlGraphicsFlickable);
    if (!interactive || lastPosTime.isNull())
        return;
    bool rejectY = false;
    bool rejectX = false;
    bool moved = false;

    if (q->yflick()) {
        int dy = int(event->pos().y() - pressPos.y());
        if (qAbs(dy) > QApplication::startDragDistance() || QmlGraphicsItemPrivate::elapsed(pressTime) > 200) {
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
        if (qAbs(dx) > QApplication::startDragDistance() || QmlGraphicsItemPrivate::elapsed(pressTime) > 200) {
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
        qreal elapsed = qreal(QmlGraphicsItemPrivate::restart(lastPosTime)) / 1000.;
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

void QmlGraphicsFlickablePrivate::handleMouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_Q(QmlGraphicsFlickable);
    pressed = false;
    if (lastPosTime.isNull())
        return;

    if (QmlGraphicsItemPrivate::elapsed(lastPosTime) > 100) {
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

void QmlGraphicsFlickable::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QmlGraphicsFlickable);
    if (d->interactive) {
        d->handleMousePressEvent(event);
        event->accept();
    } else {
        QmlGraphicsItem::mousePressEvent(event);
    }
}

void QmlGraphicsFlickable::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QmlGraphicsFlickable);
    if (d->interactive) {
        d->handleMouseMoveEvent(event);
        event->accept();
    } else {
        QmlGraphicsItem::mouseMoveEvent(event);
    }
}

void QmlGraphicsFlickable::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QmlGraphicsFlickable);
    if (d->interactive) {
        d->clearDelayedPress();
        d->handleMouseReleaseEvent(event);
        event->accept();
        ungrabMouse();
    } else {
        QmlGraphicsItem::mouseReleaseEvent(event);
    }
}

void QmlGraphicsFlickable::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    Q_D(QmlGraphicsFlickable);
    if (!d->interactive) {
        QmlGraphicsItem::wheelEvent(event);
    } else if (yflick()) {
        if (event->delta() > 0)
            d->velocityY = qMax(event->delta() - d->verticalVelocity.value(), 250.0);
        else
            d->velocityY = qMin(event->delta() - d->verticalVelocity.value(), -250.0);
        d->flicked = false;
        d->flickY(d->velocityY);
        event->accept();
    } else if (xflick()) {
        if (event->delta() > 0)
            d->velocityX = qMax(event->delta() - d->horizontalVelocity.value(), 250.0);
        else
            d->velocityX = qMin(event->delta() - d->horizontalVelocity.value(), -250.0);
        d->flicked = false;
        d->flickX(d->velocityX);
        event->accept();
    } else {
        QmlGraphicsItem::wheelEvent(event);
    }
}

void QmlGraphicsFlickablePrivate::captureDelayedPress(QGraphicsSceneMouseEvent *event)
{
    Q_Q(QmlGraphicsFlickable);
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

void QmlGraphicsFlickablePrivate::clearDelayedPress()
{
    if (delayedPressEvent) {
        delayedPressTimer.stop();
        delete delayedPressEvent;
        delayedPressEvent = 0;
    }
}

void QmlGraphicsFlickablePrivate::setRoundedViewportX(qreal x)
{
    viewport->setX(qRound(x));
}

void QmlGraphicsFlickablePrivate::setRoundedViewportY(qreal y)
{
    viewport->setY(qRound(y));
}

void QmlGraphicsFlickable::timerEvent(QTimerEvent *event)
{
    Q_D(QmlGraphicsFlickable);
    if (event->timerId() == d->delayedPressTimer.timerId()) {
        d->delayedPressTimer.stop();
        if (d->delayedPressEvent) {
            QmlGraphicsItem *grabber = scene() ? qobject_cast<QmlGraphicsItem*>(scene()->mouseGrabberItem()) : 0;
            if (!grabber || grabber != this)
                scene()->sendEvent(d->delayedPressTarget, d->delayedPressEvent);
            delete d->delayedPressEvent;
            d->delayedPressEvent = 0;
        }
    }
}

qreal QmlGraphicsFlickable::minYExtent() const
{
    return 0.0;
}

qreal QmlGraphicsFlickable::minXExtent() const
{
    return 0.0;
}

/* returns -ve */
qreal QmlGraphicsFlickable::maxXExtent() const
{
    return width() - vWidth();
}
/* returns -ve */
qreal QmlGraphicsFlickable::maxYExtent() const
{
    return height() - vHeight();
}

void QmlGraphicsFlickable::viewportMoved()
{
    Q_D(QmlGraphicsFlickable);

    int elapsed = QmlGraphicsItemPrivate::restart(d->velocityTime);
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

void QmlGraphicsFlickable::cancelFlick()
{
    Q_D(QmlGraphicsFlickable);
    d->timeline.reset(d->_moveX);
    d->timeline.reset(d->_moveY);
    movementEnding();
}

void QmlGraphicsFlickablePrivate::data_removeAt(int)
{
    // ###
}

int QmlGraphicsFlickablePrivate::data_count() const
{
    // ###
    return 0;
}

void QmlGraphicsFlickablePrivate::data_append(QObject *o)
{
    Q_Q(QmlGraphicsFlickable);
    QmlGraphicsItem *i = qobject_cast<QmlGraphicsItem *>(o);
    if (i)
        viewport->fxChildren()->append(i);
    else
        o->setParent(q);
}

void QmlGraphicsFlickablePrivate::data_insert(int, QObject *)
{
    // ###
}

QObject *QmlGraphicsFlickablePrivate::data_at(int) const
{
    // ###
    return 0;
}

void QmlGraphicsFlickablePrivate::data_clear()
{
    // ###
}


QmlList<QObject *> *QmlGraphicsFlickable::flickableData()
{
    Q_D(QmlGraphicsFlickable);
    return &d->data;
}

QmlList<QmlGraphicsItem *> *QmlGraphicsFlickable::flickableChildren()
{
    Q_D(QmlGraphicsFlickable);
    return d->viewport->fxChildren();
}

/*!
    \qmlproperty bool Flickable::overShoot
    This property holds the number of pixels the surface may overshoot the
    Flickable's boundaries when flicked.

    If overShoot is non-zero the contents can be flicked beyond the boundary
    of the Flickable before being moved back to the boundary.  This provides
    the feeling that the edges of the view are soft, rather than a hard
    physical boundary.
*/
bool QmlGraphicsFlickable::overShoot() const
{
    Q_D(const QmlGraphicsFlickable);
    return d->overShoot;
}

void QmlGraphicsFlickable::setOverShoot(bool o)
{
    Q_D(QmlGraphicsFlickable);
    d->overShoot = o;
}

/*!
    \qmlproperty int Flickable::viewportWidth
    \qmlproperty int Flickable::viewportHeight

    The dimensions of the viewport (the surface controlled by Flickable). Typically this
    should be set to the combined size of the items placed in the Flickable.

    \code
    Flickable {
        width: 320; height: 480; viewportWidth: image.width; viewportHeight: image.height
        Image { id: image; source: "bigimage.png" }
    }
    \endcode
*/
qreal QmlGraphicsFlickable::viewportWidth() const
{
    Q_D(const QmlGraphicsFlickable);
    return d->vWidth;
}

void QmlGraphicsFlickable::setViewportWidth(qreal w)
{
    Q_D(QmlGraphicsFlickable);
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
    emit viewportWidthChanged();
    d->updateBeginningEnd();
}

void QmlGraphicsFlickable::widthChange()
{
    Q_D(QmlGraphicsFlickable);
    if (d->vWidth < 0) {
        d->viewport->setWidth(width());
        emit viewportWidthChanged();
    }
    d->updateBeginningEnd();
}

void QmlGraphicsFlickable::heightChange()
{
    Q_D(QmlGraphicsFlickable);
    if (d->vHeight < 0) {
        d->viewport->setHeight(height());
        emit viewportHeightChanged();
    }
    d->updateBeginningEnd();
}

qreal QmlGraphicsFlickable::viewportHeight() const
{
    Q_D(const QmlGraphicsFlickable);
    return d->vHeight;
}

void QmlGraphicsFlickable::setViewportHeight(qreal h)
{
    Q_D(QmlGraphicsFlickable);
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
    emit viewportHeightChanged();
    d->updateBeginningEnd();
}

qreal QmlGraphicsFlickable::vWidth() const
{
    Q_D(const QmlGraphicsFlickable);
    if (d->vWidth < 0)
        return width();
    else
        return d->vWidth;
}

qreal QmlGraphicsFlickable::vHeight() const
{
    Q_D(const QmlGraphicsFlickable);
    if (d->vHeight < 0)
        return height();
    else
        return d->vHeight;
}

bool QmlGraphicsFlickable::xflick() const
{
    Q_D(const QmlGraphicsFlickable);
    if (d->flickDirection == QmlGraphicsFlickable::AutoFlickDirection)
        return vWidth() != width();
    return d->flickDirection & QmlGraphicsFlickable::HorizontalFlick;
}

bool QmlGraphicsFlickable::yflick() const
{
    Q_D(const QmlGraphicsFlickable);
    if (d->flickDirection == QmlGraphicsFlickable::AutoFlickDirection)
        return vHeight() !=  height();
    return d->flickDirection & QmlGraphicsFlickable::VerticalFlick;
}

bool QmlGraphicsFlickable::sendMouseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QmlGraphicsFlickable);
    QGraphicsSceneMouseEvent mouseEvent(event->type());
    QRectF myRect = mapToScene(QRectF(0, 0, width(), height())).boundingRect();

    QGraphicsScene *s = scene();
    QmlGraphicsItem *grabber = s ? qobject_cast<QmlGraphicsItem*>(s->mouseGrabberItem()) : 0;
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
        grabber = qobject_cast<QmlGraphicsItem*>(s->mouseGrabberItem());
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

bool QmlGraphicsFlickable::sceneEventFilter(QGraphicsItem *i, QEvent *e)
{
    Q_D(QmlGraphicsFlickable);
    if (!isVisible() || !d->interactive)
        return QmlGraphicsItem::sceneEventFilter(i, e);
    switch (e->type()) {
    case QEvent::GraphicsSceneMousePress:
    case QEvent::GraphicsSceneMouseMove:
    case QEvent::GraphicsSceneMouseRelease:
        return sendMouseEvent(static_cast<QGraphicsSceneMouseEvent *>(e));
    default:
        break;
    }

    return QmlGraphicsItem::sceneEventFilter(i, e);
}

/*!
    \qmlproperty real Flickable::maximumFlickVelocity
    This property holds the maximum velocity that the user can flick the view in pixels/second.

    The default is 2000 pixels/s
*/
qreal QmlGraphicsFlickable::maximumFlickVelocity() const
{
    Q_D(const QmlGraphicsFlickable);
    return d->maxVelocity;
}

void QmlGraphicsFlickable::setMaximumFlickVelocity(qreal v)
{
    Q_D(QmlGraphicsFlickable);
    if (v == d->maxVelocity)
        return;
    d->maxVelocity = v;
}

/*!
    \qmlproperty real Flickable::flickDeceleration
    This property holds the rate at which a flick will decelerate.

    The default is 500.
*/
qreal QmlGraphicsFlickable::flickDeceleration() const
{
    Q_D(const QmlGraphicsFlickable);
    return d->deceleration;
}

void QmlGraphicsFlickable::setFlickDeceleration(qreal deceleration)
{
    Q_D(QmlGraphicsFlickable);
    d->deceleration = deceleration;
}

/*!
    \qmlproperty bool Flickable::flicking

    This property holds whether the view is currently moving due to
    the user flicking the view.
*/
bool QmlGraphicsFlickable::isFlicking() const
{
    Q_D(const QmlGraphicsFlickable);
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
int QmlGraphicsFlickable::pressDelay() const
{
    Q_D(const QmlGraphicsFlickable);
    return d->pressDelay;
}

void QmlGraphicsFlickable::setPressDelay(int delay)
{
    Q_D(QmlGraphicsFlickable);
    if (d->pressDelay == delay)
        return;
    d->pressDelay = delay;
}

qreal QmlGraphicsFlickable::reportedVelocitySmoothing() const
{
    Q_D(const QmlGraphicsFlickable);
    return d->reportedVelocitySmoothing;
}

void QmlGraphicsFlickable::setReportedVelocitySmoothing(qreal reportedVelocitySmoothing)
{
    Q_D(QmlGraphicsFlickable);
    Q_ASSERT(reportedVelocitySmoothing >= 0);
    if (reportedVelocitySmoothing == d->reportedVelocitySmoothing)
        return;
    d->reportedVelocitySmoothing = reportedVelocitySmoothing;
    emit reportedVelocitySmoothingChanged(reportedVelocitySmoothing);
}

/*!
    \qmlproperty bool Flickable::moving

    This property holds whether the view is currently moving due to
    the user either dragging or flicking the view.
*/
bool QmlGraphicsFlickable::isMoving() const
{
    Q_D(const QmlGraphicsFlickable);
    return d->moving;
}

void QmlGraphicsFlickable::movementStarting()
{
    Q_D(QmlGraphicsFlickable);
    if (!d->moving) {
        d->moving = true;
        emit movingChanged();
        emit movementStarted();
    }
}

void QmlGraphicsFlickable::movementEnding()
{
    Q_D(QmlGraphicsFlickable);
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

void QmlGraphicsFlickablePrivate::updateVelocity()
{
    Q_Q(QmlGraphicsFlickable);
    emit q->horizontalVelocityChanged();
    emit q->verticalVelocityChanged();
}

QT_END_NAMESPACE

QML_DECLARE_TYPE(QmlGraphicsFlickableVisibleArea)
QML_DEFINE_TYPE(Qt,4,6,VisibleArea,QmlGraphicsFlickableVisibleArea)

#include <qmlgraphicsflickable.moc>

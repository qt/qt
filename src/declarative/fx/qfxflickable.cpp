/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#include "qfxflickable.h"
#include "qfxflickable_p.h"

#include <QGraphicsSceneMouseEvent>
#include <QPointer>
#include <QTimer>

QT_BEGIN_NAMESPACE

class QFxFlickableVisibleArea : public QObject
{
    Q_OBJECT

    Q_PROPERTY(qreal xPosition READ xPosition NOTIFY pageChanged)
    Q_PROPERTY(qreal yPosition READ yPosition NOTIFY pageChanged)
    Q_PROPERTY(qreal widthRatio READ widthRatio NOTIFY pageChanged)
    Q_PROPERTY(qreal heightRatio READ heightRatio NOTIFY pageChanged)

public:
    QFxFlickableVisibleArea(QFxFlickable *parent=0);

    qreal xPosition() const;
    qreal widthRatio() const;
    qreal yPosition() const;
    qreal heightRatio() const;

    void updateVisible();

signals:
    void pageChanged();

private:
    QFxFlickable *flickable;
    qreal m_xPosition;
    qreal m_widthRatio;
    qreal m_yPosition;
    qreal m_heightRatio;
};

QFxFlickableVisibleArea::QFxFlickableVisibleArea(QFxFlickable *parent)
    : QObject(parent), flickable(parent), m_xPosition(0.), m_widthRatio(0.)
    , m_yPosition(0.), m_heightRatio(0.)
{
}

qreal QFxFlickableVisibleArea::widthRatio() const
{
    return m_widthRatio;
}

qreal QFxFlickableVisibleArea::xPosition() const
{
    return m_xPosition;
}

qreal QFxFlickableVisibleArea::heightRatio() const
{
    return m_heightRatio;
}

qreal QFxFlickableVisibleArea::yPosition() const
{
    return m_yPosition;
}

void QFxFlickableVisibleArea::updateVisible()
{
    QFxFlickablePrivate *p = static_cast<QFxFlickablePrivate *>(QGraphicsItemPrivate::get(flickable));
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


QFxFlickablePrivate::QFxFlickablePrivate()
  : viewport(new QFxItem), _moveX(viewport, &QFxItem::setX), _moveY(viewport, &QFxItem::setY)
    , vWidth(-1), vHeight(-1), overShoot(true), flicked(false), moving(false), stealMouse(false)
    , pressed(false), atXEnd(false), atXBeginning(true), atYEnd(false), atYBeginning(true)
    , interactive(true), maxVelocity(-1), reportedVelocitySmoothing(100)
    , delayedPressEvent(0), delayedPressTarget(0), pressDelay(0)
    , horizontalVelocity(this), verticalVelocity(this), vTime(0), visibleArea(0)
{
    fixupXEvent = QmlTimeLineEvent::timeLineEvent<QFxFlickablePrivate, &QFxFlickablePrivate::fixupX>(&_moveX, this);
    fixupYEvent = QmlTimeLineEvent::timeLineEvent<QFxFlickablePrivate, &QFxFlickablePrivate::fixupY>(&_moveY, this);
}

void QFxFlickablePrivate::init()
{
    Q_Q(QFxFlickable);
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

void QFxFlickablePrivate::flickX(qreal velocity)
{
    Q_Q(QFxFlickable);
    qreal maxDistance = -1;
    // -ve velocity means list is moving up
    if (velocity > 0) {
        if (_moveX.value() < q->minXExtent())
            maxDistance = qAbs(q->minXExtent() -_moveX.value() + (overShoot?30:0));
        flickTargetX = q->minXExtent();
    } else {
        if (_moveX.value() > q->maxXExtent())
            maxDistance = qAbs(q->maxXExtent() - _moveX.value()) + (overShoot?30:0);
        flickTargetX = q->maxXExtent();
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
        timeline.accel(_moveX, v, 500, maxDistance);
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

void QFxFlickablePrivate::flickY(qreal velocity)
{
    Q_Q(QFxFlickable);
    qreal maxDistance = -1;
    // -ve velocity means list is moving up
    if (velocity > 0) {
        if (_moveY.value() < q->minYExtent())
            maxDistance = qAbs(q->minYExtent() -_moveY.value() + (overShoot?30:0));
        flickTargetY = q->minYExtent();
    } else {
        if (_moveY.value() > q->maxYExtent())
            maxDistance = qAbs(q->maxYExtent() - _moveY.value()) + (overShoot?30:0);
        flickTargetY = q->maxYExtent();
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
        timeline.accel(_moveY, v, 500, maxDistance);
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

void QFxFlickablePrivate::fixupX()
{
    Q_Q(QFxFlickable);
    if (!q->xflick() || _moveX.timeLine())
        return;

    vTime = timeline.time();

    if (_moveX.value() > q->minXExtent() || (q->maxXExtent() > q->minXExtent())) {
        timeline.reset(_moveX);
        if (_moveX.value() != q->minXExtent())
            timeline.move(_moveX, q->minXExtent(), QEasingCurve(QEasingCurve::InOutQuad), 200);
        //emit flickingChanged();
    } else if (_moveX.value() < q->maxXExtent()) {
        timeline.reset(_moveX);
        timeline.move(_moveX,  q->maxXExtent(), QEasingCurve(QEasingCurve::InOutQuad), 200);
        //emit flickingChanged();
    } else {
        flicked = false;
    }
}

void QFxFlickablePrivate::fixupY()
{
    Q_Q(QFxFlickable);
    if (!q->yflick() || _moveY.timeLine())
        return;

    vTime = timeline.time();

    if (_moveY.value() > q->minYExtent() || (q->maxYExtent() > q->minYExtent())) {
        timeline.reset(_moveY);
        if (_moveY.value() != q->minYExtent())
            timeline.move(_moveY, q->minYExtent(), QEasingCurve(QEasingCurve::InOutQuad), 200);
        //emit flickingChanged();
    } else if (_moveY.value() < q->maxYExtent()) {
        timeline.reset(_moveY);
        timeline.move(_moveY,  q->maxYExtent(), QEasingCurve(QEasingCurve::InOutQuad), 200);
        //emit flickingChanged();
    } else {
        flicked = false;
    }
}

void QFxFlickablePrivate::updateBeginningEnd()
{
    Q_Q(QFxFlickable);
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

static const int FlickThreshold = 5;

QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,Flickable,QFxFlickable)

/*!
    \qmlclass Flickable
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
    \internal
    \class QFxFlickable
    \brief The QFxFlickable class provides a view that can be "flicked".

    \ingroup group_widgets

    QFxFlickable allows its children to be dragged and flicked.

\code
Flickable {
    width: 320; height: 480; viewportWidth: image.width; viewportHeight: image.height
    Image { id: image; source: "bigimage.png" }
}
\endcode

    Note that QFxFlickable does not automatically clip its contents. If
    it is not full-screen it is likely that QFxItem::clip should be set
    to true.

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

QFxFlickable::QFxFlickable(QFxItem *parent)
  : QFxItem(*(new QFxFlickablePrivate), parent)
{
    Q_D(QFxFlickable);
    d->init();
}

QFxFlickable::QFxFlickable(QFxFlickablePrivate &dd, QFxItem *parent)
  : QFxItem(dd, parent)
{
    Q_D(QFxFlickable);
    d->init();
}

QFxFlickable::~QFxFlickable()
{
}

/*!
    \qmlproperty int Flickable::viewportX
    \qmlproperty int Flickable::viewportY

    These properties hold the surface coordinate currently at the top-left
    corner of the Flickable. For example, if you flick an image up 100 pixels,
    \c yPosition will be 100.
*/
qreal QFxFlickable::viewportX() const
{
    Q_D(const QFxFlickable);
    return -d->_moveX.value();
}

void QFxFlickable::setViewportX(qreal pos)
{
    Q_D(QFxFlickable);
    pos = qRound(pos);
    d->timeline.reset(d->_moveX);
    if (-pos != d->_moveX.value()) {
        d->_moveX.setValue(-pos);
        viewportMoved();
    }
}

qreal QFxFlickable::viewportY() const
{
    Q_D(const QFxFlickable);
    return -d->_moveY.value();
}

void QFxFlickable::setViewportY(qreal pos)
{
    Q_D(QFxFlickable);
    pos = qRound(pos);
    d->timeline.reset(d->_moveY);
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
bool QFxFlickable::isInteractive() const
{
    Q_D(const QFxFlickable);
    return d->interactive;
}

void QFxFlickable::setInteractive(bool interactive)
{
    Q_D(QFxFlickable);
    d->interactive = interactive;
    if (!interactive && d->flicked) {
        d->timeline.clear();
        d->flicked = false;
        emit flickingChanged();
        emit flickEnded();
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
qreal QFxFlickable::horizontalVelocity() const
{
    Q_D(const QFxFlickable);
    return d->horizontalVelocity.value();
}

qreal QFxFlickable::verticalVelocity() const
{
    Q_D(const QFxFlickable);
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
bool QFxFlickable::isAtXEnd() const
{
    Q_D(const QFxFlickable);
    return d->atXEnd;
}

bool QFxFlickable::isAtXBeginning() const
{
    Q_D(const QFxFlickable);
    return d->atXBeginning;
}

bool QFxFlickable::isAtYEnd() const
{
    Q_D(const QFxFlickable);
    return d->atYEnd;
}

bool QFxFlickable::isAtYBeginning() const
{
    Q_D(const QFxFlickable);
    return d->atYBeginning;
}

void QFxFlickable::ticked()
{
    viewportMoved();
}

QFxItem *QFxFlickable::viewport()
{
    Q_D(QFxFlickable);
    return d->viewport;
}

qreal QFxFlickable::visibleX() const
{
    Q_D(const QFxFlickable);
    return -d->_moveX.value();
}

qreal QFxFlickable::visibleY() const
{
    Q_D(const QFxFlickable);
    return -d->_moveY.value();
}

QFxFlickableVisibleArea *QFxFlickable::visibleArea()
{
    Q_D(QFxFlickable);
    if (!d->visibleArea)
        d->visibleArea = new QFxFlickableVisibleArea(this);
    return d->visibleArea;
}

void QFxFlickablePrivate::handleMousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (interactive && timeline.isActive() && (qAbs(velocityX) > 10 || qAbs(velocityY) > 10))
        stealMouse = true; // If we've been flicked then steal the click.
    else
        stealMouse = false;
    pressed = true;
    timeline.clear();
    velocityX = -1;
    velocityY = -1;
    lastPos = QPoint();
    lastPosTime.start();
    pressPos = event->pos();
    pressX = _moveX.value();
    pressY = _moveY.value();
    flicked = false;
    pressTime.start();
    velocityTime.start();
}

void QFxFlickablePrivate::handleMouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    Q_Q(QFxFlickable);
    if (!interactive || lastPosTime.isNull())
        return;
    bool rejectY = false;
    bool rejectX = false;
    bool moved = false;

    if (q->yflick()) {
        int dy = int(event->pos().y() - pressPos.y());
        if (qAbs(dy) > FlickThreshold || pressTime.elapsed() > 200) {
            qreal newY = dy + pressY;
            const qreal minY = q->minYExtent();
            const qreal maxY = q->maxYExtent();
            if (newY > minY)
                newY = minY + (newY - minY) / 2;
            if (newY < maxY && maxY - minY < 0)
                newY = maxY + (newY - maxY) / 2;
            if (q->overShoot() || (newY <= minY && newY >= maxY)) {
                _moveY.setValue(newY);
                moved = true;
            } else if (!q->overShoot())
                rejectY = true;
            if (qAbs(dy) > FlickThreshold)
                stealMouse = true;
        }
    }

    if (q->xflick()) {
        int dx = int(event->pos().x() - pressPos.x());
        if (qAbs(dx) > FlickThreshold || pressTime.elapsed() > 200) {
            qreal newX = dx + pressX;
            const qreal minX = q->minXExtent();
            const qreal maxX = q->maxXExtent();
            if (newX > minX)
                newX = minX + (newX - minX) / 2;
            if (newX < maxX && maxX - minX < 0)
                newX = maxX + (newX - maxX) / 2;
            if (q->overShoot() || (newX <= minX && newX >= maxX)) {
                _moveX.setValue(newX);
                moved = true;
            } else if (!q->overShoot())
                rejectX = true;
            if (qAbs(dx) > FlickThreshold)
                stealMouse = true;
        }
    }

    if (!lastPos.isNull()) {
        qreal elapsed = qreal(lastPosTime.restart()) / 1000.;
        if (elapsed <= 0)
            elapsed = 1;
        if (q->yflick()) {
            qreal diff = event->pos().y() - lastPos.y();
            velocityY = diff / elapsed;
        }

        if (q->xflick()) {
            qreal diff = event->pos().x() - lastPos.x();
            velocityX = diff / elapsed;
        }
    }

    if (rejectY) velocityY = 0;
    if (rejectX) velocityX = 0;

    if (moved) {
        q->viewportMoved();
        q->movementStarting();
    }

    lastPos = event->pos();
}

void QFxFlickablePrivate::handleMouseReleaseEvent(QGraphicsSceneMouseEvent *)
{
    Q_Q(QFxFlickable);
    pressed = false;
    if (lastPosTime.isNull())
        return;

    vTime = timeline.time();
    if (qAbs(velocityY) > 10)
        flickY(velocityY);
    else
        fixupY();

    if (qAbs(velocityX) > 10)
        flickX(velocityX);
    else
        fixupX();

    stealMouse = false;
    lastPosTime = QTime();

    if (!timeline.isActive())
        q->movementEnding();
}

void QFxFlickable::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QFxFlickable);
    d->handleMousePressEvent(event);
    event->accept();
}

void QFxFlickable::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QFxFlickable);
    d->handleMouseMoveEvent(event);
    event->accept();
}

void QFxFlickable::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QFxFlickable);
    d->clearDelayedPress();
    d->handleMouseReleaseEvent(event);
    event->accept();
    ungrabMouse();
}

void QFxFlickablePrivate::captureDelayedPress(QGraphicsSceneMouseEvent *event)
{
    Q_Q(QFxFlickable);
    if (!q->scene() || pressDelay <= 0)
        return;
    delayedPressTarget = q->scene()->mouseGrabberItem();
    delayedPressEvent = new QGraphicsSceneMouseEvent(event->type());
    delayedPressEvent->setAccepted(false);
    for (int i = 0x1; i <= 0x10; i <<= 1) {
        if (event->buttons() & i) {
            Qt::MouseButton button = Qt::MouseButton(i);
            delayedPressEvent->setButtonDownPos(button, event->buttonDownPos(button));
        }
    }
    delayedPressEvent->setScenePos(event->scenePos());
    delayedPressEvent->setLastScenePos(event->lastScenePos());
    delayedPressEvent->setPos(event->pos());
    delayedPressEvent->setLastPos(event->lastPos());
    delayedPressTimer.start(pressDelay, q);
}

void QFxFlickablePrivate::clearDelayedPress()
{
    if (delayedPressEvent) {
        delayedPressTimer.stop();
        delete delayedPressEvent;
        delayedPressEvent = 0;
    }
}

void QFxFlickable::timerEvent(QTimerEvent *event)
{
    Q_D(QFxFlickable);
    if (event->timerId() == d->delayedPressTimer.timerId()) {
        d->delayedPressTimer.stop();
        if (d->delayedPressEvent) {
            QFxItem *grabber = scene() ? qobject_cast<QFxItem*>(scene()->mouseGrabberItem()) : 0;
            if (!grabber || grabber != this)
                scene()->sendEvent(d->delayedPressTarget, d->delayedPressEvent);
            delete d->delayedPressEvent;
            d->delayedPressEvent = 0;
        }
    }
}

qreal QFxFlickable::minYExtent() const
{
    return 0.0;
}

qreal QFxFlickable::minXExtent() const
{
    return 0.0;
}

/* returns -ve */
qreal QFxFlickable::maxXExtent() const
{
    return width() - vWidth();
}
/* returns -ve */
qreal QFxFlickable::maxYExtent() const
{
    return height() - vHeight();
}

void QFxFlickable::viewportMoved()
{
    Q_D(QFxFlickable);

    int elapsed = d->velocityTime.elapsed();

    if (elapsed) {
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
            if (d->timeline.time() != d->vTime) {
                qreal horizontalVelocity = (prevX - d->_moveX.value()) * 1000 / (d->timeline.time() - d->vTime);
                qreal verticalVelocity = (prevY - d->_moveY.value()) * 1000 / (d->timeline.time() - d->vTime);
                d->horizontalVelocity.setValue(horizontalVelocity);
                d->verticalVelocity.setValue(verticalVelocity);
            }
            d->vTime = d->timeline.time();
        }
    }

    d->lastFlickablePosition = QPointF(d->_moveY.value(), d->_moveX.value());
    d->velocityTime.restart();
    d->updateBeginningEnd();

    if (d->flicked) {
        // Near an end and it seems that the extent has changed?
        // Recalculate the flick so that we don't end up in an odd position.
        if (d->velocityY > 0) {
            const qreal minY = minYExtent();
            if (minY - d->_moveY.value() < height()/3 && minY != d->flickTargetY)
                d->flickY(-d->verticalVelocity.value());
        } else {
            const qreal maxY = maxYExtent();
            if (d->_moveY.value() - maxY < height()/3 && maxY != d->flickTargetY)
                d->flickY(-d->verticalVelocity.value());
        }

        if (d->velocityX > 0) {
            const qreal minX = minXExtent();
            if (minX - d->_moveX.value() < height()/3 && minX != d->flickTargetX)
                d->flickX(-d->horizontalVelocity.value());
        } else {
            const qreal maxX = maxXExtent();
            if (d->_moveX.value() - maxX < height()/3 && maxX != d->flickTargetX)
                d->flickX(-d->horizontalVelocity.value());
        }
    }
}

void QFxFlickablePrivate::data_removeAt(int)
{
    // ###
}

int QFxFlickablePrivate::data_count() const
{
    // ###
    return 0;
}

void QFxFlickablePrivate::data_append(QObject *o)
{
    Q_Q(QFxFlickable);
    QFxItem *i = qobject_cast<QFxItem *>(o);
    if (i)
        viewport->children()->append(i);
    else
        o->setParent(q);
}

void QFxFlickablePrivate::data_insert(int, QObject *)
{
    // ###
}

QObject *QFxFlickablePrivate::data_at(int) const
{
    // ###
    return 0;
}

void QFxFlickablePrivate::data_clear()
{
    // ###
}


QmlList<QObject *> *QFxFlickable::flickableData()
{
    Q_D(QFxFlickable);
    return &d->data;
}

QmlList<QFxItem *> *QFxFlickable::flickableChildren()
{
    Q_D(QFxFlickable);
    return d->viewport->children();
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
bool QFxFlickable::overShoot() const
{
    Q_D(const QFxFlickable);
    return d->overShoot;
}

void QFxFlickable::setOverShoot(bool o)
{
    Q_D(QFxFlickable);
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
qreal QFxFlickable::viewportWidth() const
{
    Q_D(const QFxFlickable);
    return d->vWidth;
}

void QFxFlickable::setViewportWidth(qreal w)
{
    Q_D(QFxFlickable);
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

void QFxFlickable::widthChange()
{
    Q_D(QFxFlickable);
    if (d->vWidth < 0) {
        d->viewport->setWidth(width());
        emit viewportWidthChanged();
    }
    d->updateBeginningEnd();
}

void QFxFlickable::heightChange()
{
    Q_D(QFxFlickable);
    if (d->vHeight < 0) {
        d->viewport->setHeight(height());
        emit viewportHeightChanged();
    }
    d->updateBeginningEnd();
}

qreal QFxFlickable::viewportHeight() const
{
    Q_D(const QFxFlickable);
    return d->vHeight;
}

void QFxFlickable::setViewportHeight(qreal h)
{
    Q_D(QFxFlickable);
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

qreal QFxFlickable::vWidth() const
{
    Q_D(const QFxFlickable);
    if (d->vWidth < 0)
        return width();
    else
        return d->vWidth;
}

qreal QFxFlickable::vHeight() const
{
    Q_D(const QFxFlickable);
    if (d->vHeight < 0)
        return height();
    else
        return d->vHeight;
}

bool QFxFlickable::xflick() const
{
    return vWidth() != width();
}

bool QFxFlickable::yflick() const
{
    return vHeight() !=  height();
}

bool QFxFlickable::sendMouseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QFxFlickable);
    QGraphicsSceneMouseEvent mouseEvent(event->type());
    QRectF myRect = mapToScene(QRectF(0, 0, width(), height())).boundingRect();

    QGraphicsScene *s = scene();
    QFxItem *grabber = s ? qobject_cast<QFxItem*>(s->mouseGrabberItem()) : 0;
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
        grabber = qobject_cast<QFxItem*>(s->mouseGrabberItem());
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

bool QFxFlickable::sceneEventFilter(QGraphicsItem *i, QEvent *e)
{
    if (!isVisible())
        return QFxItem::sceneEventFilter(i, e);
    switch (e->type()) {
    case QEvent::GraphicsSceneMousePress:
    case QEvent::GraphicsSceneMouseMove:
    case QEvent::GraphicsSceneMouseRelease:
        return sendMouseEvent(static_cast<QGraphicsSceneMouseEvent *>(e));
    default:
        break;
    }

    return QFxItem::sceneEventFilter(i, e);
}

/*!
    \qmlproperty int Flickable::maximumFlickVelocity
    This property holds the maximum velocity that the user can flick the view in pixels/second.
*/
qreal QFxFlickable::maximumFlickVelocity() const
{
    Q_D(const QFxFlickable);
    return d->maxVelocity;
}

void QFxFlickable::setMaximumFlickVelocity(qreal v)
{
    Q_D(QFxFlickable);
    if (v == d->maxVelocity)
        return;
    d->maxVelocity = v;
}

bool QFxFlickable::isFlicking() const
{
    Q_D(const QFxFlickable);
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
int QFxFlickable::pressDelay() const
{
    Q_D(const QFxFlickable);
    return d->pressDelay;
}

void QFxFlickable::setPressDelay(int delay)
{
    Q_D(QFxFlickable);
    if (d->pressDelay == delay)
        return;
    d->pressDelay = delay;
}

qreal QFxFlickable::reportedVelocitySmoothing() const
{
    Q_D(const QFxFlickable);
    return d->reportedVelocitySmoothing;
}

void QFxFlickable::setReportedVelocitySmoothing(qreal reportedVelocitySmoothing)
{
    Q_D(QFxFlickable);
    Q_ASSERT(reportedVelocitySmoothing >= 0);
    if (reportedVelocitySmoothing == d->reportedVelocitySmoothing)
        return;
    d->reportedVelocitySmoothing = reportedVelocitySmoothing;
    emit reportedVelocitySmoothingChanged(reportedVelocitySmoothing);
}

bool QFxFlickable::isMoving() const
{
    Q_D(const QFxFlickable);
    return d->moving;
}

void QFxFlickable::movementStarting()
{
    Q_D(QFxFlickable);
    if (!d->moving) {
        d->moving = true;
        emit movingChanged();
        emit movementStarted();
    }
}

void QFxFlickable::movementEnding()
{
    Q_D(QFxFlickable);
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
}

void QFxFlickablePrivate::updateVelocity()
{
    Q_Q(QFxFlickable);
    emit q->velocityChanged(q->horizontalVelocity(), q->verticalVelocity());
}

QT_END_NAMESPACE

QML_DECLARE_TYPE(QFxFlickableVisibleArea)
QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,VisibleArea,QFxFlickableVisibleArea)

#include "qfxflickable.moc"

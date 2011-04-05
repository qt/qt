// Commit: ee767e8c16742316068e83323374ea54f2b939cb
/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qsgflickable_p.h"
#include "qsgflickable_p_p.h"
#include "qsgcanvas.h"
#include "qsgcanvas_p.h"

#include <QtDeclarative/qdeclarativeinfo.h>
#include <QtGui/qgraphicssceneevent.h>
#include <QtGui/qapplication.h>

QT_BEGIN_NAMESPACE

// FlickThreshold determines how far the "mouse" must have moved
// before we perform a flick.
static const int FlickThreshold = 20;

// RetainGrabVelocity is the maxmimum instantaneous velocity that
// will ensure the Flickable retains the grab on consecutive flicks.
static const int RetainGrabVelocity = 15;

QSGFlickableVisibleArea::QSGFlickableVisibleArea(QSGFlickable *parent)
    : QObject(parent), flickable(parent), m_xPosition(0.), m_widthRatio(0.)
    , m_yPosition(0.), m_heightRatio(0.)
{
}

qreal QSGFlickableVisibleArea::widthRatio() const
{
    return m_widthRatio;
}

qreal QSGFlickableVisibleArea::xPosition() const
{
    return m_xPosition;
}

qreal QSGFlickableVisibleArea::heightRatio() const
{
    return m_heightRatio;
}

qreal QSGFlickableVisibleArea::yPosition() const
{
    return m_yPosition;
}

void QSGFlickableVisibleArea::updateVisible()
{
    QSGFlickablePrivate *p = QSGFlickablePrivate::get(flickable);

    bool changeX = false;
    bool changeY = false;
    bool changeWidth = false;
    bool changeHeight = false;

    // Vertical
    const qreal viewheight = flickable->height();
    const qreal maxyextent = -flickable->maxYExtent() + flickable->minYExtent();
    qreal pagePos = (-p->vData.move.value() + flickable->minYExtent()) / (maxyextent + viewheight);
    qreal pageSize = viewheight / (maxyextent + viewheight);

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
    const qreal maxxextent = -flickable->maxXExtent() + flickable->minXExtent();
    pagePos = (-p->hData.move.value() + flickable->minXExtent()) / (maxxextent + viewwidth);
    pageSize = viewwidth / (maxxextent + viewwidth);

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


QSGFlickablePrivate::QSGFlickablePrivate()
  : contentItem(new QSGItem)
    , hData(this, &QSGFlickablePrivate::setRoundedViewportX)
    , vData(this, &QSGFlickablePrivate::setRoundedViewportY)
    , flickingHorizontally(false), flickingVertically(false)
    , hMoved(false), vMoved(false)
    , movingHorizontally(false), movingVertically(false)
    , stealMouse(false), pressed(false), interactive(true), calcVelocity(false)
    , deceleration(500), maxVelocity(2000), reportedVelocitySmoothing(100)
    , delayedPressEvent(0), delayedPressTarget(0), pressDelay(0), fixupDuration(600)
    , fixupMode(Normal), vTime(0), visibleArea(0)
    , flickableDirection(QSGFlickable::AutoFlickDirection)
    , boundsBehavior(QSGFlickable::DragAndOvershootBounds)
{
}

void QSGFlickablePrivate::init()
{
    Q_Q(QSGFlickable);
    QDeclarative_setParent_noEvent(contentItem, q);
    contentItem->setParentItem(q);
    static int timelineUpdatedIdx = -1;
    static int timelineCompletedIdx = -1;
    static int flickableTickedIdx = -1;
    static int flickableMovementEndingIdx = -1;
    if (timelineUpdatedIdx == -1) {
        timelineUpdatedIdx = QDeclarativeTimeLine::staticMetaObject.indexOfSignal("updated()");
        timelineCompletedIdx = QDeclarativeTimeLine::staticMetaObject.indexOfSignal("completed()");
        flickableTickedIdx = QSGFlickable::staticMetaObject.indexOfSlot("ticked()");
        flickableMovementEndingIdx = QSGFlickable::staticMetaObject.indexOfSlot("movementEnding()");
    }
    QMetaObject::connect(&timeline, timelineUpdatedIdx,
                         q, flickableTickedIdx, Qt::DirectConnection);
    QMetaObject::connect(&timeline, timelineCompletedIdx,
                         q, flickableMovementEndingIdx, Qt::DirectConnection);
    q->setAcceptedMouseButtons(Qt::LeftButton);
    q->setFiltersChildMouseEvents(true);
    QSGItemPrivate *viewportPrivate = QSGItemPrivate::get(contentItem);
    viewportPrivate->addItemChangeListener(this, QSGItemPrivate::Geometry);
    lastPosTime.invalidate();
}

/*
    Returns the amount to overshoot by given a velocity.
    Will be roughly in range 0 - size/4
*/
qreal QSGFlickablePrivate::overShootDistance(qreal velocity, qreal size)
{
    if (maxVelocity <= 0)
        return 0.0;

    velocity = qAbs(velocity);
    if (velocity > maxVelocity)
        velocity = maxVelocity;
    qreal dist = size / 4 * velocity / maxVelocity;
    return dist;
}

void QSGFlickablePrivate::itemGeometryChanged(QSGItem *item, const QRectF &newGeom, const QRectF &oldGeom)
{
    Q_Q(QSGFlickable);
    if (item == contentItem) {
        if (newGeom.x() != oldGeom.x())
            emit q->contentXChanged();
        if (newGeom.y() != oldGeom.y())
            emit q->contentYChanged();
    }
}

void QSGFlickablePrivate::flickX(qreal velocity)
{
    Q_Q(QSGFlickable);
    flick(hData, q->minXExtent(), q->maxXExtent(), q->width(), fixupX_callback, velocity);
}

void QSGFlickablePrivate::flickY(qreal velocity)
{
    Q_Q(QSGFlickable);
    flick(vData, q->minYExtent(), q->maxYExtent(), q->height(), fixupY_callback, velocity);
}

void QSGFlickablePrivate::flick(AxisData &data, qreal minExtent, qreal maxExtent, qreal vSize,
                                         QDeclarativeTimeLineCallback::Callback fixupCallback, qreal velocity)
{
    Q_Q(QSGFlickable);
    qreal maxDistance = -1;
    data.fixingUp = false;
    bool overShoot = boundsBehavior == QSGFlickable::DragAndOvershootBounds;
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

void QSGFlickablePrivate::fixupY_callback(void *data)
{
    ((QSGFlickablePrivate *)data)->fixupY();
}

void QSGFlickablePrivate::fixupX_callback(void *data)
{
    ((QSGFlickablePrivate *)data)->fixupX();
}

void QSGFlickablePrivate::fixupX()
{
    Q_Q(QSGFlickable);
    fixup(hData, q->minXExtent(), q->maxXExtent());
}

void QSGFlickablePrivate::fixupY()
{
    Q_Q(QSGFlickable);
    fixup(vData, q->minYExtent(), q->maxYExtent());
}

void QSGFlickablePrivate::fixup(AxisData &data, qreal minExtent, qreal maxExtent)
{
    if (data.move.value() > minExtent || maxExtent > minExtent) {
        timeline.reset(data.move);
        if (data.move.value() != minExtent) {
            switch (fixupMode) {
            case Immediate:
                timeline.set(data.move, minExtent);
                break;
            case ExtentChanged:
                // The target has changed. Don't start from the beginning; just complete the
                // second half of the animation using the new extent.
                timeline.move(data.move, minExtent, QEasingCurve(QEasingCurve::OutExpo), 3*fixupDuration/4);
                data.fixingUp = true;
                break;
            default: {
                    qreal dist = minExtent - data.move;
                    timeline.move(data.move, minExtent - dist/2, QEasingCurve(QEasingCurve::InQuad), fixupDuration/4);
                    timeline.move(data.move, minExtent, QEasingCurve(QEasingCurve::OutExpo), 3*fixupDuration/4);
                    data.fixingUp = true;
                }
            }
        }
    } else if (data.move.value() < maxExtent) {
        timeline.reset(data.move);
        switch (fixupMode) {
        case Immediate:
            timeline.set(data.move, maxExtent);
            break;
        case ExtentChanged:
            // The target has changed. Don't start from the beginning; just complete the
            // second half of the animation using the new extent.
            timeline.move(data.move, maxExtent, QEasingCurve(QEasingCurve::OutExpo), 3*fixupDuration/4);
            data.fixingUp = true;
            break;
        default: {
                qreal dist = maxExtent - data.move;
                timeline.move(data.move, maxExtent - dist/2, QEasingCurve(QEasingCurve::InQuad), fixupDuration/4);
                timeline.move(data.move, maxExtent, QEasingCurve(QEasingCurve::OutExpo), 3*fixupDuration/4);
                data.fixingUp = true;
            }
        }
    }
    fixupMode = Normal;
    vTime = timeline.time();
}

void QSGFlickablePrivate::updateBeginningEnd()
{
    Q_Q(QSGFlickable);
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

QSGFlickable::QSGFlickable(QSGItem *parent)
  : QSGItem(*(new QSGFlickablePrivate), parent)
{
    Q_D(QSGFlickable);
    d->init();
}

QSGFlickable::QSGFlickable(QSGFlickablePrivate &dd, QSGItem *parent)
  : QSGItem(dd, parent)
{
    Q_D(QSGFlickable);
    d->init();
}

QSGFlickable::~QSGFlickable()
{
}

qreal QSGFlickable::contentX() const
{
    Q_D(const QSGFlickable);
    return -d->contentItem->x();
}

void QSGFlickable::setContentX(qreal pos)
{
    Q_D(QSGFlickable);
    d->timeline.reset(d->hData.move);
    d->vTime = d->timeline.time();
    movementXEnding();
    if (-pos != d->hData.move.value()) {
        d->hData.move.setValue(-pos);
        viewportMoved();
    }
}

qreal QSGFlickable::contentY() const
{
    Q_D(const QSGFlickable);
    return -d->contentItem->y();
}

void QSGFlickable::setContentY(qreal pos)
{
    Q_D(QSGFlickable);
    d->timeline.reset(d->vData.move);
    d->vTime = d->timeline.time();
    movementYEnding();
    if (-pos != d->vData.move.value()) {
        d->vData.move.setValue(-pos);
        viewportMoved();
    }
}

bool QSGFlickable::isInteractive() const
{
    Q_D(const QSGFlickable);
    return d->interactive;
}

void QSGFlickable::setInteractive(bool interactive)
{
    Q_D(QSGFlickable);
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

qreal QSGFlickable::horizontalVelocity() const
{
    Q_D(const QSGFlickable);
    return d->hData.smoothVelocity.value();
}

qreal QSGFlickable::verticalVelocity() const
{
    Q_D(const QSGFlickable);
    return d->vData.smoothVelocity.value();
}

bool QSGFlickable::isAtXEnd() const
{
    Q_D(const QSGFlickable);
    return d->hData.atEnd;
}

bool QSGFlickable::isAtXBeginning() const
{
    Q_D(const QSGFlickable);
    return d->hData.atBeginning;
}

bool QSGFlickable::isAtYEnd() const
{
    Q_D(const QSGFlickable);
    return d->vData.atEnd;
}

bool QSGFlickable::isAtYBeginning() const
{
    Q_D(const QSGFlickable);
    return d->vData.atBeginning;
}

void QSGFlickable::ticked()
{
    viewportMoved();
}

QSGItem *QSGFlickable::contentItem()
{
    Q_D(QSGFlickable);
    return d->contentItem;
}

QSGFlickableVisibleArea *QSGFlickable::visibleArea()
{
    Q_D(QSGFlickable);
    if (!d->visibleArea)
        d->visibleArea = new QSGFlickableVisibleArea(this);
    return d->visibleArea;
}

QSGFlickable::FlickableDirection QSGFlickable::flickableDirection() const
{
    Q_D(const QSGFlickable);
    return d->flickableDirection;
}

void QSGFlickable::setFlickableDirection(FlickableDirection direction)
{
    Q_D(QSGFlickable);
    if (direction != d->flickableDirection) {
        d->flickableDirection = direction;
        emit flickableDirectionChanged();
    }
}

void QSGFlickablePrivate::handleMousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_Q(QSGFlickable);
    if (interactive && timeline.isActive()
        && (qAbs(hData.smoothVelocity.value()) > RetainGrabVelocity
            || qAbs(vData.smoothVelocity.value()) > RetainGrabVelocity)) {
        stealMouse = true; // If we've been flicked then steal the click.
    } else {
        stealMouse = false;
    }
    q->setKeepMouseGrab(stealMouse);
    pressed = true;
    timeline.clear();
    hData.velocity = 0;
    vData.velocity = 0;
    hData.dragStartOffset = 0;
    vData.dragStartOffset = 0;
    hData.dragMinBound = q->minXExtent();
    vData.dragMinBound = q->minYExtent();
    hData.dragMaxBound = q->maxXExtent();
    vData.dragMaxBound = q->maxYExtent();
    hData.fixingUp = false;
    vData.fixingUp = false;
    lastPos = QPoint();
    QSGItemPrivate::start(lastPosTime);
    pressPos = event->pos();
    hData.pressPos = hData.move.value();
    vData.pressPos = vData.move.value();
    flickingHorizontally = false;
    flickingVertically = false;
    QSGItemPrivate::start(pressTime);
    QSGItemPrivate::start(velocityTime);
}

void QSGFlickablePrivate::handleMouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    Q_Q(QSGFlickable);
    if (!interactive || !lastPosTime.isValid())
        return;
    bool rejectY = false;
    bool rejectX = false;

    bool stealY = stealMouse;
    bool stealX = stealMouse;

    if (q->yflick()) {
        int dy = int(event->pos().y() - pressPos.y());
        if (qAbs(dy) > QApplication::startDragDistance() || QSGItemPrivate::elapsed(pressTime) > 200) {
            if (!vMoved)
                vData.dragStartOffset = dy;
            qreal newY = dy + vData.pressPos - vData.dragStartOffset;
            const qreal minY = vData.dragMinBound;
            const qreal maxY = vData.dragMaxBound;
            if (newY > minY)
                newY = minY + (newY - minY) / 2;
            if (newY < maxY && maxY - minY <= 0)
                newY = maxY + (newY - maxY) / 2;
            if (boundsBehavior == QSGFlickable::StopAtBounds && (newY > minY || newY < maxY)) {
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
                stealY = true;
        }
    }

    if (q->xflick()) {
        int dx = int(event->pos().x() - pressPos.x());
        if (qAbs(dx) > QApplication::startDragDistance() || QSGItemPrivate::elapsed(pressTime) > 200) {
            if (!hMoved)
                hData.dragStartOffset = dx;
            qreal newX = dx + hData.pressPos - hData.dragStartOffset;
            const qreal minX = hData.dragMinBound;
            const qreal maxX = hData.dragMaxBound;
            if (newX > minX)
                newX = minX + (newX - minX) / 2;
            if (newX < maxX && maxX - minX <= 0)
                newX = maxX + (newX - maxX) / 2;
            if (boundsBehavior == QSGFlickable::StopAtBounds && (newX > minX || newX < maxX)) {
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
                stealX = true;
        }
    }

    stealMouse = stealX || stealY;
    if (stealMouse)
        q->setKeepMouseGrab(true);

    if (!lastPos.isNull()) {
        qreal elapsed = qreal(QSGItemPrivate::restart(lastPosTime)) / 1000.;
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

void QSGFlickablePrivate::handleMouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_Q(QSGFlickable);
    stealMouse = false;
    q->setKeepMouseGrab(false);
    pressed = false;
    if (!lastPosTime.isValid())
        return;

    if (QSGItemPrivate::elapsed(lastPosTime) > 100) {
        // if we drag then pause before release we should not cause a flick.
        hData.velocity = 0.0;
        vData.velocity = 0.0;
    }

    vTime = timeline.time();
    if (qAbs(vData.velocity) > MinimumFlickVelocity && qAbs(event->pos().y() - pressPos.y()) > FlickThreshold)
        flickY(vData.velocity);
    else
        fixupY();

    if (qAbs(hData.velocity) > MinimumFlickVelocity && qAbs(event->pos().x() - pressPos.x()) > FlickThreshold)
        flickX(hData.velocity);
    else
        fixupX();

    lastPosTime.invalidate();

    if (!timeline.isActive())
        q->movementEnding();
}

void QSGFlickable::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QSGFlickable);
    if (d->interactive) {
        if (!d->pressed)
            d->handleMousePressEvent(event);
        event->accept();
    } else {
        QSGItem::mousePressEvent(event);
    }
}

void QSGFlickable::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QSGFlickable);
    if (d->interactive) {
        d->handleMouseMoveEvent(event);
        event->accept();
    } else {
        QSGItem::mouseMoveEvent(event);
    }
}

void QSGFlickable::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QSGFlickable);
    if (d->interactive) {
        d->clearDelayedPress();
        d->handleMouseReleaseEvent(event);
        event->accept();
        ungrabMouse();
    } else {
        QSGItem::mouseReleaseEvent(event);
    }
}

void QSGFlickable::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    Q_D(QSGFlickable);
    if (!d->interactive) {
        QSGItem::wheelEvent(event);
    } else if (yflick() && event->orientation() == Qt::Vertical) {
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
    } else if (xflick() && event->orientation() == Qt::Horizontal) {
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
        QSGItem::wheelEvent(event);
    }
}

bool QSGFlickablePrivate::isOutermostPressDelay() const
{
    Q_Q(const QSGFlickable);
    QSGItem *item = q->parentItem();
    while (item) {
        QSGFlickable *flick = qobject_cast<QSGFlickable*>(item);
        if (flick && flick->pressDelay() > 0 && flick->isInteractive())
            return false;
        item = item->parentItem();
    }

    return true;
}

void QSGFlickablePrivate::captureDelayedPress(QGraphicsSceneMouseEvent *event)
{
    Q_Q(QSGFlickable);
    if (!q->canvas() || pressDelay <= 0)
        return;
    if (!isOutermostPressDelay())
        return;
    delayedPressTarget = q->canvas()->mouseGrabberItem();
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

void QSGFlickablePrivate::clearDelayedPress()
{
    if (delayedPressEvent) {
        delayedPressTimer.stop();
        delete delayedPressEvent;
        delayedPressEvent = 0;
    }
}

void QSGFlickablePrivate::setRoundedViewportX(qreal x)
{
    contentItem->setX(qRound(x));
}

void QSGFlickablePrivate::setRoundedViewportY(qreal y)
{
    contentItem->setY(qRound(y));
}

void QSGFlickable::timerEvent(QTimerEvent *event)
{
    Q_D(QSGFlickable);
    if (event->timerId() == d->delayedPressTimer.timerId()) {
        d->delayedPressTimer.stop();
        if (d->delayedPressEvent) {
            QSGItem *grabber = canvas() ? canvas()->mouseGrabberItem() : 0;
            if (!grabber || grabber != this) {
                // We replay the mouse press but the grabber we had might not be interessted by the event (e.g. overlay)
                // so we reset the grabber
                if (canvas()->mouseGrabberItem() == d->delayedPressTarget)
                    d->delayedPressTarget->ungrabMouse();
                // Use the event handler that will take care of finding the proper item to propagate the event
                QSGCanvasPrivate::get(canvas())->deliverMouseEvent(d->delayedPressEvent);
            }
            delete d->delayedPressEvent;
            d->delayedPressEvent = 0;
        }
    }
}

qreal QSGFlickable::minYExtent() const
{
    return 0.0;
}

qreal QSGFlickable::minXExtent() const
{
    return 0.0;
}

/* returns -ve */
qreal QSGFlickable::maxXExtent() const
{
    return width() - vWidth();
}
/* returns -ve */
qreal QSGFlickable::maxYExtent() const
{
    return height() - vHeight();
}

void QSGFlickable::viewportMoved()
{
    Q_D(QSGFlickable);

    qreal prevX = d->lastFlickablePosition.x();
    qreal prevY = d->lastFlickablePosition.y();
    d->velocityTimeline.clear();
    if (d->pressed || d->calcVelocity) {
        int elapsed = QSGItemPrivate::restart(d->velocityTime);
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

    d->lastFlickablePosition = QPointF(d->hData.move.value(), d->vData.move.value());

    d->vTime = d->timeline.time();
    d->updateBeginningEnd();
}

void QSGFlickable::geometryChanged(const QRectF &newGeometry,
                             const QRectF &oldGeometry)
{
    Q_D(QSGFlickable);
    QSGItem::geometryChanged(newGeometry, oldGeometry);

    bool changed = false;
    if (newGeometry.width() != oldGeometry.width()) {
        if (xflick())
            changed = true;
        if (d->hData.viewSize < 0) {
            d->contentItem->setWidth(width());
            emit contentWidthChanged();
        }
        // Make sure that we're entirely in view.
        if (!d->pressed && !d->movingHorizontally && !d->movingVertically) {
            d->fixupMode = QSGFlickablePrivate::Immediate;
            d->fixupX();
        }
    }
    if (newGeometry.height() != oldGeometry.height()) {
        if (yflick())
            changed = true;
        if (d->vData.viewSize < 0) {
            d->contentItem->setHeight(height());
            emit contentHeightChanged();
        }
        // Make sure that we're entirely in view.
        if (!d->pressed && !d->movingHorizontally && !d->movingVertically) {
            d->fixupMode = QSGFlickablePrivate::Immediate;
            d->fixupY();
        }
    }

    if (changed)
        d->updateBeginningEnd();
}

void QSGFlickable::cancelFlick()
{
    Q_D(QSGFlickable);
    d->timeline.reset(d->hData.move);
    d->timeline.reset(d->vData.move);
    movementEnding();
}

void QSGFlickablePrivate::data_append(QDeclarativeListProperty<QObject> *prop, QObject *o)
{
    QSGItem *i = qobject_cast<QSGItem *>(o);
    if (i) {
        i->setParentItem(static_cast<QSGFlickablePrivate*>(prop->data)->contentItem);
    } else {
        o->setParent(prop->object); // XXX todo - do we want this?
    }
}

int QSGFlickablePrivate::data_count(QDeclarativeListProperty<QObject> *)
{
    // XXX todo
    return 0;
}

QObject *QSGFlickablePrivate::data_at(QDeclarativeListProperty<QObject> *, int)
{
    // XXX todo
    return 0;
}

void QSGFlickablePrivate::data_clear(QDeclarativeListProperty<QObject> *)
{
    // XXX todo
}

QDeclarativeListProperty<QObject> QSGFlickable::flickableData()
{
    Q_D(QSGFlickable);
    return QDeclarativeListProperty<QObject>(this, (void *)d, QSGFlickablePrivate::data_append,
                                             QSGFlickablePrivate::data_count,
                                             QSGFlickablePrivate::data_at,
                                             QSGFlickablePrivate::data_clear);
}

QDeclarativeListProperty<QSGItem> QSGFlickable::flickableChildren()
{
    Q_D(QSGFlickable);
    return QSGItemPrivate::get(d->contentItem)->children();
}

QSGFlickable::BoundsBehavior QSGFlickable::boundsBehavior() const
{
    Q_D(const QSGFlickable);
    return d->boundsBehavior;
}

void QSGFlickable::setBoundsBehavior(BoundsBehavior b)
{
    Q_D(QSGFlickable);
    if (b == d->boundsBehavior)
        return;
    d->boundsBehavior = b;
    emit boundsBehaviorChanged();
}

qreal QSGFlickable::contentWidth() const
{
    Q_D(const QSGFlickable);
    return d->hData.viewSize;
}

void QSGFlickable::setContentWidth(qreal w)
{
    Q_D(QSGFlickable);
    if (d->hData.viewSize == w)
        return;
    d->hData.viewSize = w;
    if (w < 0)
        d->contentItem->setWidth(width());
    else
        d->contentItem->setWidth(w);
    // Make sure that we're entirely in view.
    if (!d->pressed && !d->movingHorizontally && !d->movingVertically) {
        d->fixupMode = QSGFlickablePrivate::Immediate;
        d->fixupX();
    } else if (!d->pressed && d->hData.fixingUp) {
        d->fixupMode = QSGFlickablePrivate::ExtentChanged;
        d->fixupX();
    }
    emit contentWidthChanged();
    d->updateBeginningEnd();
}

qreal QSGFlickable::contentHeight() const
{
    Q_D(const QSGFlickable);
    return d->vData.viewSize;
}

void QSGFlickable::setContentHeight(qreal h)
{
    Q_D(QSGFlickable);
    if (d->vData.viewSize == h)
        return;
    d->vData.viewSize = h;
    if (h < 0)
        d->contentItem->setHeight(height());
    else
        d->contentItem->setHeight(h);
    // Make sure that we're entirely in view.
    if (!d->pressed && !d->movingHorizontally && !d->movingVertically) {
        d->fixupMode = QSGFlickablePrivate::Immediate;
        d->fixupY();
    } else if (!d->pressed && d->vData.fixingUp) {
        d->fixupMode = QSGFlickablePrivate::ExtentChanged;
        d->fixupY();
    }
    emit contentHeightChanged();
    d->updateBeginningEnd();
}

void QSGFlickable::resizeContent(qreal w, qreal h, QPointF center)
{
    Q_D(QSGFlickable);
    if (w != d->hData.viewSize) {
        qreal oldSize = d->hData.viewSize;
        setContentWidth(w);
        if (center.x() != 0) {
            qreal pos = center.x() * w / oldSize;
            setContentX(contentX() + pos - center.x());
        }
    }
    if (h != d->vData.viewSize) {
        qreal oldSize = d->vData.viewSize;
        setContentHeight(h);
        if (center.y() != 0) {
            qreal pos = center.y() * h / oldSize;
            setContentY(contentY() + pos - center.y());
        }
    }
}

void QSGFlickable::returnToBounds()
{
    Q_D(QSGFlickable);
    d->fixupX();
    d->fixupY();
}

qreal QSGFlickable::vWidth() const
{
    Q_D(const QSGFlickable);
    if (d->hData.viewSize < 0)
        return width();
    else
        return d->hData.viewSize;
}

qreal QSGFlickable::vHeight() const
{
    Q_D(const QSGFlickable);
    if (d->vData.viewSize < 0)
        return height();
    else
        return d->vData.viewSize;
}

bool QSGFlickable::xflick() const
{
    Q_D(const QSGFlickable);
    if (d->flickableDirection == QSGFlickable::AutoFlickDirection)
        return vWidth() != width();
    return d->flickableDirection & QSGFlickable::HorizontalFlick;
}

bool QSGFlickable::yflick() const
{
    Q_D(const QSGFlickable);
    if (d->flickableDirection == QSGFlickable::AutoFlickDirection)
        return vHeight() !=  height();
    return d->flickableDirection & QSGFlickable::VerticalFlick;
}

void QSGFlickable::mouseUngrabEvent()
{
    Q_D(QSGFlickable);
    if (d->pressed) {
        // if our mouse grab has been removed (probably by another Flickable),
        // fix our state
        d->pressed = false;
        d->stealMouse = false;
        setKeepMouseGrab(false);
    }
}

bool QSGFlickable::sendMouseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QSGFlickable);
    QGraphicsSceneMouseEvent mouseEvent(event->type());
    QRectF myRect = mapRectToScene(QRectF(0, 0, width(), height()));

    QSGCanvas *c = canvas();
    QSGItem *grabber = c ? c->mouseGrabberItem() : 0;
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
            if (d->pressed) // we are already pressed - this is a delayed replay
                return false;

            d->handleMousePressEvent(&mouseEvent);
            d->captureDelayedPress(event);
            stealThisEvent = d->stealMouse;   // Update stealThisEvent in case changed by function call above
            break;
        case QEvent::GraphicsSceneMouseRelease:
            if (d->delayedPressEvent) {
                // We replay the mouse press but the grabber we had might not be interessted by the event (e.g. overlay)
                // so we reset the grabber
                if (c->mouseGrabberItem() == d->delayedPressTarget)
                    d->delayedPressTarget->ungrabMouse();
                //Use the event handler that will take care of finding the proper item to propagate the event
                QSGCanvasPrivate::get(canvas())->deliverMouseEvent(d->delayedPressEvent);
                d->clearDelayedPress();
                // We send the release
                canvas()->sendEvent(c->mouseGrabberItem(), event);
                // And the event has been consumed
                d->stealMouse = false;
                d->pressed = false;
                return true;
            }
            d->handleMouseReleaseEvent(&mouseEvent);
            break;
        default:
            break;
        }
        grabber = qobject_cast<QSGItem*>(c->mouseGrabberItem());
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
        d->pressed = false;
    }
    return false;
}


bool QSGFlickable::childMouseEventFilter(QSGItem *i, QEvent *e)
{
    Q_D(QSGFlickable);
    if (!isVisible() || !d->interactive)
        return QSGItem::childMouseEventFilter(i, e);
    switch (e->type()) {
    case QEvent::GraphicsSceneMousePress:
    case QEvent::GraphicsSceneMouseMove:
    case QEvent::GraphicsSceneMouseRelease:
        return sendMouseEvent(static_cast<QGraphicsSceneMouseEvent *>(e));
    default:
        break;
    }

    return QSGItem::childMouseEventFilter(i, e);
}

qreal QSGFlickable::maximumFlickVelocity() const
{
    Q_D(const QSGFlickable);
    return d->maxVelocity;
}

void QSGFlickable::setMaximumFlickVelocity(qreal v)
{
    Q_D(QSGFlickable);
    if (v == d->maxVelocity)
        return;
    d->maxVelocity = v;
    emit maximumFlickVelocityChanged();
}

qreal QSGFlickable::flickDeceleration() const
{
    Q_D(const QSGFlickable);
    return d->deceleration;
}

void QSGFlickable::setFlickDeceleration(qreal deceleration)
{
    Q_D(QSGFlickable);
    if (deceleration == d->deceleration)
        return;
    d->deceleration = deceleration;
    emit flickDecelerationChanged();
}

bool QSGFlickable::isFlicking() const
{
    Q_D(const QSGFlickable);
    return d->flickingHorizontally ||  d->flickingVertically;
}

bool QSGFlickable::isFlickingHorizontally() const
{
    Q_D(const QSGFlickable);
    return d->flickingHorizontally;
}

bool QSGFlickable::isFlickingVertically() const
{
    Q_D(const QSGFlickable);
    return d->flickingVertically;
}

int QSGFlickable::pressDelay() const
{
    Q_D(const QSGFlickable);
    return d->pressDelay;
}

void QSGFlickable::setPressDelay(int delay)
{
    Q_D(QSGFlickable);
    if (d->pressDelay == delay)
        return;
    d->pressDelay = delay;
    emit pressDelayChanged();
}


bool QSGFlickable::isMoving() const
{
    Q_D(const QSGFlickable);
    return d->movingHorizontally || d->movingVertically;
}

bool QSGFlickable::isMovingHorizontally() const
{
    Q_D(const QSGFlickable);
    return d->movingHorizontally;
}

bool QSGFlickable::isMovingVertically() const
{
    Q_D(const QSGFlickable);
    return d->movingVertically;
}

void QSGFlickable::movementStarting()
{
    Q_D(QSGFlickable);
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

void QSGFlickable::movementEnding()
{
    Q_D(QSGFlickable);
    movementXEnding();
    movementYEnding();
    d->hData.smoothVelocity.setValue(0);
    d->vData.smoothVelocity.setValue(0);
}

void QSGFlickable::movementXEnding()
{
    Q_D(QSGFlickable);
    if (d->flickingHorizontally) {
        d->flickingHorizontally = false;
        emit flickingChanged();
        emit flickingHorizontallyChanged();
        if (!d->flickingVertically)
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
    }
    d->hData.fixingUp = false;
}

void QSGFlickable::movementYEnding()
{
    Q_D(QSGFlickable);
    if (d->flickingVertically) {
        d->flickingVertically = false;
        emit flickingChanged();
        emit flickingVerticallyChanged();
        if (!d->flickingHorizontally)
           emit flickEnded();
    }
    if (!d->pressed && !d->stealMouse) {
        if (d->movingVertically) {
            d->movingVertically = false;
            d->vMoved = false;
            emit movingChanged();
            emit movingVerticallyChanged();
            if (!d->movingHorizontally)
                emit movementEnded();
        }
    }
    d->vData.fixingUp = false;
}

void QSGFlickablePrivate::updateVelocity()
{
    Q_Q(QSGFlickable);
    emit q->horizontalVelocityChanged();
    emit q->verticalVelocityChanged();
}

QT_END_NAMESPACE

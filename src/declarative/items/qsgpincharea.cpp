// Commit: 2ec2dc55ddf424f5a7acd0a4729ddd9af2d7c398
/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtSG module of the Qt Toolkit.
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

#include "qsgpincharea_p_p.h"
#include "qsgcanvas.h"

#include <QtGui/qgraphicssceneevent.h>
#include <QtGui/qapplication.h>

#include <float.h>
#include <math.h>

QT_BEGIN_NAMESPACE

QSGPinch::QSGPinch()
    : m_target(0), m_minScale(1.0), m_maxScale(1.0)
    , m_minRotation(0.0), m_maxRotation(0.0)
    , m_axis(NoDrag), m_xmin(-FLT_MAX), m_xmax(FLT_MAX)
    , m_ymin(-FLT_MAX), m_ymax(FLT_MAX), m_active(false)
{
}

QSGPinchAreaPrivate::~QSGPinchAreaPrivate()
{
    delete pinch;
}

QSGPinchArea::QSGPinchArea(QSGItem *parent)
  : QSGItem(*(new QSGPinchAreaPrivate), parent)
{
    Q_D(QSGPinchArea);
    d->init();
}

QSGPinchArea::~QSGPinchArea()
{
}

bool QSGPinchArea::isEnabled() const
{
    Q_D(const QSGPinchArea);
    return d->absorb;
}

void QSGPinchArea::setEnabled(bool a)
{
    Q_D(QSGPinchArea);
    if (a != d->absorb) {
        d->absorb = a;
        emit enabledChanged();
    }
}

void QSGPinchArea::touchEvent(QTouchEvent *event)
{
    Q_D(QSGPinchArea);
    if (!d->absorb || !isVisible()) {
        QSGItem::event(event);
        return;
    }

    switch (event->type()) {
    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
        d->touchPoints.clear();
        for (int i = 0; i < event->touchPoints().count(); ++i) {
            if (!(event->touchPoints().at(i).state() & Qt::TouchPointReleased)) {
                d->touchPoints << event->touchPoints().at(i);
            }
        }
        updatePinch();
        break;
    case QEvent::TouchEnd:
        d->touchPoints.clear();
        updatePinch();
        break;
    default:
        QSGItem::event(event);
    }
}

void QSGPinchArea::updatePinch()
{
    Q_D(QSGPinchArea);
    if (d->touchPoints.count() != 2) {
        if (d->inPinch) {
            d->stealMouse = false;
            setKeepMouseGrab(false);
            d->inPinch = false;
            QPointF pinchCenter = mapFromScene(d->sceneLastCenter);
            QSGPinchEvent pe(pinchCenter, d->pinchLastScale, d->pinchLastAngle, d->pinchRotation);
            pe.setStartCenter(d->pinchStartCenter);
            pe.setPreviousCenter(pinchCenter);
            pe.setPreviousAngle(d->pinchLastAngle);
            pe.setPreviousScale(d->pinchLastScale);
            pe.setStartPoint1(mapFromScene(d->sceneStartPoint1));
            pe.setStartPoint2(mapFromScene(d->sceneStartPoint2));
            pe.setPoint1(d->lastPoint1);
            pe.setPoint2(d->lastPoint2);
            emit pinchFinished(&pe);
            d->pinchStartDist = 0;
            if (d->pinch && d->pinch->target())
                d->pinch->setActive(false);
        }
        return;
    }
    if (d->touchPoints.at(0).state() & Qt::TouchPointPressed
        || d->touchPoints.at(1).state() & Qt::TouchPointPressed) {
        d->sceneStartPoint1 = d->touchPoints.at(0).scenePos();
        d->sceneStartPoint2 = d->touchPoints.at(1).scenePos();
        d->inPinch = false;
        d->pinchRejected = false;
    } else if (!d->pinchRejected){
        QSGItem *grabber = canvas() ? canvas()->mouseGrabberItem() : 0;
        if (grabber == this || !grabber || !grabber->keepMouseGrab()) {
            const int dragThreshold = QApplication::startDragDistance();
            QPointF p1 = d->touchPoints.at(0).scenePos();
            QPointF p2 = d->touchPoints.at(1).scenePos();
            qreal dx = p1.x() - p2.x();
            qreal dy = p1.y() - p2.y();
            qreal dist = sqrt(dx*dx + dy*dy);
            QPointF sceneCenter = (p1 + p2)/2;
            qreal angle = QLineF(p1, p2).angle();
            if (angle > 180)
                angle -= 360;
            if (!d->inPinch) {
                if (qAbs(p1.x()-d->sceneStartPoint1.x()) > dragThreshold
                        || qAbs(p1.y()-d->sceneStartPoint1.y()) > dragThreshold
                        || qAbs(p2.x()-d->sceneStartPoint2.x()) > dragThreshold
                        || qAbs(p2.y()-d->sceneStartPoint2.y()) > dragThreshold) {
                    d->sceneStartCenter = sceneCenter;
                    d->sceneLastCenter = sceneCenter;
                    d->pinchStartCenter = mapFromScene(sceneCenter);
                    d->pinchStartDist = dist;
                    d->pinchStartAngle = angle;
                    d->pinchLastScale = 1.0;
                    d->pinchLastAngle = angle;
                    d->pinchRotation = 0.0;
                    d->lastPoint1 = d->touchPoints.at(0).pos();
                    d->lastPoint2 = d->touchPoints.at(1).pos();
                    QSGPinchEvent pe(d->pinchStartCenter, 1.0, angle, 0.0);
                    pe.setStartCenter(d->pinchStartCenter);
                    pe.setPreviousCenter(d->pinchStartCenter);
                    pe.setPreviousAngle(d->pinchLastAngle);
                    pe.setPreviousScale(d->pinchLastScale);
                    pe.setStartPoint1(mapFromScene(d->sceneStartPoint1));
                    pe.setStartPoint2(mapFromScene(d->sceneStartPoint2));
                    pe.setPoint1(d->lastPoint1);
                    pe.setPoint2(d->lastPoint2);
                    emit pinchStarted(&pe);
                    if (pe.accepted()) {
                        d->inPinch = true;
                        d->stealMouse = true;
                        QSGCanvas *c = canvas();
                        if (c && c->mouseGrabberItem() != this)
                            grabMouse();
                        setKeepMouseGrab(true);
                        if (d->pinch && d->pinch->target()) {
                            d->pinchStartPos = pinch()->target()->pos();
                            d->pinchStartScale = d->pinch->target()->scale();
                            d->pinchStartRotation = d->pinch->target()->rotation();
                            d->pinch->setActive(true);
                        }
                    } else {
                        d->pinchRejected = true;
                    }
                }
            } else if (d->pinchStartDist > 0) {
                qreal scale = dist / d->pinchStartDist;
                qreal da = d->pinchLastAngle - angle;
                if (da > 180)
                    da -= 360;
                else if (da < -180)
                    da += 360;
                d->pinchRotation += da;
                QPointF pinchCenter = mapFromScene(sceneCenter);
                QSGPinchEvent pe(pinchCenter, scale, angle, d->pinchRotation);
                pe.setStartCenter(d->pinchStartCenter);
                pe.setPreviousCenter(mapFromScene(d->sceneLastCenter));
                pe.setPreviousAngle(d->pinchLastAngle);
                pe.setPreviousScale(d->pinchLastScale);
                pe.setStartPoint1(mapFromScene(d->sceneStartPoint1));
                pe.setStartPoint2(mapFromScene(d->sceneStartPoint2));
                pe.setPoint1(d->touchPoints.at(0).pos());
                pe.setPoint2(d->touchPoints.at(1).pos());
                d->pinchLastScale = scale;
                d->sceneLastCenter = sceneCenter;
                d->pinchLastAngle = angle;
                d->lastPoint1 = d->touchPoints.at(0).pos();
                d->lastPoint2 = d->touchPoints.at(1).pos();
                emit pinchUpdated(&pe);
                if (d->pinch && d->pinch->target()) {
                    qreal s = d->pinchStartScale * scale;
                    s = qMin(qMax(pinch()->minimumScale(),s), pinch()->maximumScale());
                    pinch()->target()->setScale(s);
                    QPointF pos = sceneCenter - d->sceneStartCenter + d->pinchStartPos;
                    if (pinch()->axis() & QSGPinch::XAxis) {
                        qreal x = pos.x();
                        if (x < pinch()->xmin())
                            x = pinch()->xmin();
                        else if (x > pinch()->xmax())
                            x = pinch()->xmax();
                        pinch()->target()->setX(x);
                    }
                    if (pinch()->axis() & QSGPinch::YAxis) {
                        qreal y = pos.y();
                        if (y < pinch()->ymin())
                            y = pinch()->ymin();
                        else if (y > pinch()->ymax())
                            y = pinch()->ymax();
                        pinch()->target()->setY(y);
                    }
                    if (d->pinchStartRotation >= pinch()->minimumRotation()
                            && d->pinchStartRotation <= pinch()->maximumRotation()) {
                        qreal r = d->pinchRotation + d->pinchStartRotation;
                        r = qMin(qMax(pinch()->minimumRotation(),r), pinch()->maximumRotation());
                        pinch()->target()->setRotation(r);
                    }
                }
            }
        }
    }
}

void QSGPinchArea::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QSGPinchArea);
    d->stealMouse = false;
    if (!d->absorb)
        QSGItem::mousePressEvent(event);
    else {
        setKeepMouseGrab(false);
        event->setAccepted(true);
    }
}

void QSGPinchArea::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QSGPinchArea);
    if (!d->absorb) {
        QSGItem::mouseMoveEvent(event);
        return;
    }
}

void QSGPinchArea::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QSGPinchArea);
    d->stealMouse = false;
    if (!d->absorb) {
        QSGItem::mouseReleaseEvent(event);
    } else {
        QSGCanvas *c = canvas();
        if (c && c->mouseGrabberItem() == this)
            ungrabMouse();
        setKeepMouseGrab(false);
    }
}

void QSGPinchArea::mouseUngrabEvent()
{
    setKeepMouseGrab(false);
}

bool QSGPinchArea::sendMouseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QSGPinchArea);
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
            mouseMoveEvent(&mouseEvent);
            break;
        case QEvent::GraphicsSceneMousePress:
            mousePressEvent(&mouseEvent);
            break;
        case QEvent::GraphicsSceneMouseRelease:
            mouseReleaseEvent(&mouseEvent);
            break;
        default:
            break;
        }
        grabber = c->mouseGrabberItem();
        if (grabber && stealThisEvent && !grabber->keepMouseGrab() && grabber != this)
            grabMouse();

        return stealThisEvent;
    }
    if (mouseEvent.type() == QEvent::GraphicsSceneMouseRelease) {
        d->stealMouse = false;
        if (c && c->mouseGrabberItem() == this)
            ungrabMouse();
        setKeepMouseGrab(false);
    }
    return false;
}

bool QSGPinchArea::childMouseEventFilter(QSGItem *i, QEvent *e)
{
    Q_D(QSGPinchArea);
    if (!d->absorb || !isVisible())
        return QSGItem::childMouseEventFilter(i, e);
    switch (e->type()) {
    case QEvent::GraphicsSceneMousePress:
    case QEvent::GraphicsSceneMouseMove:
    case QEvent::GraphicsSceneMouseRelease:
        return sendMouseEvent(static_cast<QGraphicsSceneMouseEvent *>(e));
        break;
    case QEvent::TouchBegin:
    case QEvent::TouchUpdate: {
            QTouchEvent *touch = static_cast<QTouchEvent*>(e);
            d->touchPoints.clear();
            for (int i = 0; i < touch->touchPoints().count(); ++i)
                if (!(touch->touchPoints().at(i).state() & Qt::TouchPointReleased))
                    d->touchPoints << touch->touchPoints().at(i);
            updatePinch();
        }
        return d->inPinch;
    case QEvent::TouchEnd:
        d->touchPoints.clear();
        updatePinch();
        break;
    default:
        break;
    }

    return QSGItem::childMouseEventFilter(i, e);
}

void QSGPinchArea::geometryChanged(const QRectF &newGeometry,
                                            const QRectF &oldGeometry)
{
    QSGItem::geometryChanged(newGeometry, oldGeometry);
}

void QSGPinchArea::itemChange(ItemChange change, const ItemChangeData &value)
{
    QSGItem::itemChange(change, value);
}

QSGPinch *QSGPinchArea::pinch()
{
    Q_D(QSGPinchArea);
    if (!d->pinch)
        d->pinch = new QSGPinch;
    return d->pinch;
}


QT_END_NAMESPACE


/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "gesturerecognizerlinjazax.h"

#include <QRegExp>
#include <QDebug>

static const int SIZE = 20;

DirectionSimpleRecognizer::DirectionSimpleRecognizer()
{
}

Direction DirectionSimpleRecognizer::addPosition(const QPoint &pos)
{
    if (!directions.isEmpty()) {
        const QPoint tmp = pos - directions.back().point;
        if (tmp.manhattanLength() < 5)
            return Direction();
    }
    if (lastPoint.isNull()) {
        lastPoint = pos;
        return Direction();
    }
    int dx = pos.x() - lastPoint.x();
    int dy = pos.y() - lastPoint.y();
    QString direction;
    if (dx < 0) {
        if (-1*dx >= SIZE/2)
            direction = "4";
    } else {
        if (dx >= SIZE/2)
            direction = "6";
    }
    if (dy < 0) {
        if (-1*dy >= SIZE/2)
            direction = "8";
    } else {
        if (dy >= SIZE/2)
            direction = "2";
    }
    if (direction.isEmpty())
        return Direction();

    lastPoint = pos;
    directions.push_back(Direction(direction, pos));
    return Direction(direction, pos);
}


DirectionList DirectionSimpleRecognizer::getDirections() const
{
    return directions;
}

void DirectionSimpleRecognizer::reset()
{
    directions.clear();
    lastPoint = QPoint();
}

///////////////////////////////////////////////////////////////////////////

GestureRecognizerLinjaZax::GestureRecognizerLinjaZax()
    : QGestureRecognizer(QLatin1String("LinjaZax")), mousePressed(false), gestureFinished(false),
      zoomState(LinjaZaxGesture::NoZoom)
{
}

QGestureRecognizer::Result GestureRecognizerLinjaZax::filterEvent(const QEvent *event)
{
    if (zoomState != LinjaZaxGesture::NoZoom && !lastDirections.isEmpty()) {
        lastDirections = lastDirections.right(1);
        zoomState = LinjaZaxGesture::NoZoom;
    }

    if (event->type() == QEvent::MouseButtonPress) {
        if (!currentDirection.isEmpty()) {
            reset();
            return QGestureRecognizer::NotGesture;
        }
        mousePressed = true;
        const QMouseEvent *ev = static_cast<const QMouseEvent*>(event);
        pressedPos = lastPos = currentPos = ev->pos();
        return QGestureRecognizer::MaybeGesture;
    } else if (event->type() == QEvent::MouseButtonRelease) {
        const QMouseEvent *ev = static_cast<const QMouseEvent*>(event);
        if (mousePressed && !currentDirection.isEmpty()) {
            gestureFinished = true;
            currentPos = ev->pos();
            internalReset();
            return QGestureRecognizer::GestureFinished;
        }
        reset();
        return QGestureRecognizer::NotGesture;
    } else if (event->type() == QEvent::MouseMove) {
        if (!mousePressed)
            return QGestureRecognizer::NotGesture;
        lastPos = currentPos;
        const QMouseEvent *ev = static_cast<const QMouseEvent*>(event);
        currentPos = ev->pos();
        QString direction =
            simpleRecognizer.addPosition(ev->pos()).direction;
        QGestureRecognizer::Result result = QGestureRecognizer::NotGesture;
        if (currentDirection.isEmpty()) {
            if (direction.isEmpty())
                result = QGestureRecognizer::MaybeGesture;
            else
                result = QGestureRecognizer::GestureStarted;
        } else {
            result = QGestureRecognizer::GestureStarted;
        }
        if (!direction.isEmpty()) {
            lastDirections.append(direction);
            currentDirection = direction;
            if (lastDirections.length() > 5)
                lastDirections.remove(0, 1);
            if (lastDirections.contains("248") || lastDirections.contains("2448"))
                zoomState = LinjaZaxGesture::ZoomingIn;
            else if (lastDirections.contains("268") || lastDirections.contains("2668"))
                zoomState = LinjaZaxGesture::ZoomingOut;
        }
        return result;
    }
    return QGestureRecognizer::NotGesture;
}

static inline LinjaZaxGesture::DirectionType convertPanningDirection(const QString &direction)
{
    if (direction.length() == 1) {
        if (direction == "4")
            return LinjaZaxGesture::Left;
        else if (direction == "6")
            return LinjaZaxGesture::Right;
        else if (direction == "8")
            return LinjaZaxGesture::Up;
        else if (direction == "2")
            return LinjaZaxGesture::Down;
    }
    return LinjaZaxGesture::None;
}

QGesture* GestureRecognizerLinjaZax::getGesture()
{
    LinjaZaxGesture::DirectionType dir = convertPanningDirection(currentDirection);
    LinjaZaxGesture::DirectionType lastDir = convertPanningDirection(lastDirections.right(1));
    if (dir == LinjaZaxGesture::None)
        return 0;
    LinjaZaxGesture *g =
        new LinjaZaxGesture(this, pressedPos, lastPos, currentPos,
                            QRect(), pressedPos, QDateTime(), 0,
                            gestureFinished ? Qt::GestureFinished : Qt::GestureStarted);
    g->lastDirection_ = lastDir;
    g->direction_ = dir;
    g->zoomState_ = zoomState;

    return g;
}

void GestureRecognizerLinjaZax::reset()
{
    mousePressed = false;
    lastDirections.clear();
    currentDirection.clear();
    gestureFinished = false;
    simpleRecognizer.reset();
    zoomState = LinjaZaxGesture::NoZoom;
}

void GestureRecognizerLinjaZax::internalReset()
{
    mousePressed = false;
    simpleRecognizer.reset();
}

/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
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
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdirectionrecognizer_p.h"

#include <math.h>

QT_BEGIN_NAMESPACE

enum {
    DistanceDelta = 20
};

QDirectionSimpleRecognizer::QDirectionSimpleRecognizer()
{
}

Direction QDirectionSimpleRecognizer::addPosition(const QPoint &pos)
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
    Qt::DirectionType direction = Qt::NoDirection;
    if (dx < 0) {
        if (-1*dx >= DistanceDelta/2)
            direction = Qt::LeftDirection;
    } else {
        if (dx >= DistanceDelta/2)
            direction = Qt::RightDirection;
    }
    if (dy < 0) {
        if (-1*dy >= DistanceDelta/2)
            direction = Qt::UpDirection;
    } else {
        if (dy >= DistanceDelta/2)
            direction = Qt::DownDirection;
    }
    if (direction == Qt::NoDirection)
        return Direction();

    lastPoint = pos;
    directions.push_back(Direction(direction, pos));
    return Direction(direction, pos);
}


DirectionList QDirectionSimpleRecognizer::getDirections() const
{
    return directions;
}

void QDirectionSimpleRecognizer::reset()
{
    directions.clear();
    lastPoint = QPoint();
}


/// QDirectionDiagonalRecognizer

QDirectionDiagonalRecognizer::QDirectionDiagonalRecognizer()
{
}

Direction QDirectionDiagonalRecognizer::addPosition(const QPoint &pos)
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
    int distance = sqrt(static_cast<double>(dx*dx + dy*dy));
    if (distance < DistanceDelta/2)
        return Direction();

    Qt::DirectionType direction = Qt::NoDirection;
    double angle = atan(1.0*qAbs(lastPoint.y() - pos.y())/qAbs(pos.x() - lastPoint.x())) * 180. / M_PI;
    if (dx < 0 && dy <= 0) {
        angle = 180 - angle;
    } else if (dx <= 0 && dy > 0) {
        angle += 180;
    } else if (dx > 0 && dy > 0) {
        angle = 360-angle;
    }
    if (angle < 0)
        angle += 360;
    if (angle <= 20)
        direction = Qt::RightDirection;
    else if (angle <= 65)
        direction = Qt::RightUpDirection;
    else if (angle <= 110)
        direction = Qt::UpDirection;
    else if (angle <= 155)
        direction = Qt::LeftUpDirection;
    else if (angle <= 200)
        direction = Qt::LeftDirection;
    else if (angle <= 245)
        direction = Qt::LeftDownDirection;
    else if (angle <= 290)
        direction = Qt::DownDirection;
    else if (angle <= 335)
        direction = Qt::RightDownDirection;
    else
        direction = Qt::RightDirection;

    if (direction == Qt::NoDirection)
        return Direction();

    lastPoint = pos;
    directions.push_back(Direction(direction, pos));
    return Direction(direction, pos);
}


DirectionList QDirectionDiagonalRecognizer::getDirections() const
{
    return directions;
}

void QDirectionDiagonalRecognizer::reset()
{
    directions.clear();
    lastPoint = QPoint();
}

QT_END_NAMESPACE

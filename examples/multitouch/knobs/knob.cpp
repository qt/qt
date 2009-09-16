/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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

#include "knob.h"

#include <QBrush>
#include <QTouchEvent>

Knob::Knob()
    : QGraphicsEllipseItem(-50, -50, 100, 100)
{
    setAcceptTouchEvents(true);
    setBrush(Qt::lightGray);

    QGraphicsEllipseItem *leftItem = new QGraphicsEllipseItem(0, 0, 20, 20, this);
    leftItem->setPos(-40, -10);
    leftItem->setBrush(Qt::darkGreen);

    QGraphicsEllipseItem *rightItem = new QGraphicsEllipseItem(0, 0, 20, 20, this);
    rightItem->setPos(20, -10);
    rightItem->setBrush(Qt::darkRed);
}

bool Knob::sceneEvent(QEvent *event)
{
    switch (event->type()) {
    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
    case QEvent::TouchEnd:
    {
        QTouchEvent *touchEvent = static_cast<QTouchEvent *>(event);

        if (touchEvent->touchPoints().count() == 2) {
            const QTouchEvent::TouchPoint &touchPoint1 = touchEvent->touchPoints().first();
            const QTouchEvent::TouchPoint &touchPoint2 = touchEvent->touchPoints().last();

            QLineF line1(touchPoint1.lastScenePos(), touchPoint2.lastScenePos());
            QLineF line2(touchPoint1.scenePos(), touchPoint2.scenePos());

            rotate(line2.angleTo(line1));
        }

        break;
    }

    default:
        return QGraphicsItem::sceneEvent(event);
    }

    return true;
}

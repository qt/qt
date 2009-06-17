/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
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
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "rocketitem.h"
#include "tankitem.h"

#include <QPainter>
#include <QGraphicsScene>

#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

RocketItem::RocketItem(QObject *parent)
    : GameItem(parent), m_direction(0.0), m_distance(300.0)
{
}

QRectF RocketItem::boundingRect() const
{
    return QRectF(-1.0, -1.0, 2.0, 2.0);
}

void RocketItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setBrush(Qt::black);
    painter->drawEllipse(boundingRect());
}

void RocketItem::idle(qreal elapsed)
{
    qreal dist = elapsed * speed();

    m_distance -= dist;
    if (m_distance < 0.0) {
        scene()->removeItem(this);
        delete this;
        return;
    }

    qreal a = m_direction * M_PI / 180.0;

    qreal yd = dist * sin(a);
    qreal xd = dist * sin(M_PI / 2.0 - a);

    QPointF requestedPosition = pos() + QPointF(xd, yd);
    QGraphicsItem *collidedItem = 0;
    QPointF nextPosition = tryMove(requestedPosition, 0, &collidedItem);
    if (requestedPosition == nextPosition) {
        setPos(nextPosition);
    } else {
        if (GameItem *gameItem = qgraphicsitem_cast<GameItem *>(collidedItem)) {
            TankItem *tankItem = qobject_cast<TankItem *>(gameItem);
            if (tankItem != 0)
                tankItem->hitByRocket();
        }

        scene()->removeItem(this);
        delete this;
    }
}

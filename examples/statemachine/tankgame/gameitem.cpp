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

#include "gameitem.h"

#include <QGraphicsScene>
#include <QDebug>

GameItem::GameItem(QObject *parent) : QObject(parent)
{
}

QPointF GameItem::tryMove(const QPointF &requestedPosition, QLineF *collidedLine, 
                          QGraphicsItem **collidedItem) const
{
    QLineF movementPath(pos(), requestedPosition);    

    qreal cannonLength = 0.0;
    {
        QPointF p1 = boundingRect().center();
        QPointF p2 = QPointF(boundingRect().right() + 10.0, p1.y());

        cannonLength = QLineF(mapToScene(p1), mapToScene(p2)).length();
    }

    movementPath.setLength(movementPath.length() + cannonLength);

    QRectF boundingRectPath(QPointF(qMin(movementPath.x1(), movementPath.x2()), qMin(movementPath.y1(), movementPath.y2())),
                            QPointF(qMax(movementPath.x1(), movementPath.x2()), qMax(movementPath.y1(), movementPath.y2())));

    QList<QGraphicsItem *> itemsInRect = scene()->items(boundingRectPath, Qt::IntersectsItemBoundingRect);

    QPointF nextPoint = requestedPosition;
    QRectF sceneRect = scene()->sceneRect();

    foreach (QGraphicsItem *item, itemsInRect) {
        if (item == static_cast<const QGraphicsItem *>(this))
            continue; 

        QPolygonF mappedBoundingRect = item->mapToScene(item->boundingRect());
        for (int i=0; i<mappedBoundingRect.size(); ++i) {
            QPointF p1 = mappedBoundingRect.at(i == 0 ? mappedBoundingRect.size()-1 : i-1);
            QPointF p2 = mappedBoundingRect.at(i);

            QLineF rectLine(p1, p2);
            QPointF intersectionPoint;
            QLineF::IntersectType intersectType = movementPath.intersect(rectLine, &intersectionPoint);

            if (intersectType == QLineF::BoundedIntersection) {
                movementPath.setP2(intersectionPoint);
                movementPath.setLength(movementPath.length() - cannonLength);
                nextPoint = movementPath.p2();

                if (collidedLine != 0)
                    *collidedLine = rectLine;

                if (collidedItem != 0)
                    *collidedItem = item;
            }
        }
    }


    // Don't go outside of map
    if (nextPoint.x() < sceneRect.left()) {
        nextPoint.rx() = sceneRect.left();
        if (collidedLine != 0)
            *collidedLine = QLineF(scene()->sceneRect().topLeft(), scene()->sceneRect().bottomLeft());
    }
    
    if (nextPoint.x() > sceneRect.right()) {
        nextPoint.rx() = sceneRect.right();
        if (collidedLine != 0)
            *collidedLine = QLineF(scene()->sceneRect().topRight(), scene()->sceneRect().bottomRight());        
    }
    
    if (nextPoint.y() < sceneRect.top()) {
        nextPoint.ry() = sceneRect.top();
        if (collidedLine != 0)
            *collidedLine = QLineF(scene()->sceneRect().topLeft(), scene()->sceneRect().topRight());
    }
    
    if (nextPoint.y() > sceneRect.bottom()) {
        nextPoint.ry() = sceneRect.bottom();
        if (collidedLine != 0)
            *collidedLine = QLineF(scene()->sceneRect().bottomLeft(), scene()->sceneRect().bottomRight());
    }

    return nextPoint;
}


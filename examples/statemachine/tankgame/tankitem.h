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

#ifndef TANKITEM_H
#define TANKITEM_H

#include "gameitem.h"

#include <QColor>

class Action;
class TankItem: public GameItem
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled)
    Q_PROPERTY(qreal direction READ direction WRITE turnTo)
    Q_PROPERTY(qreal distanceToObstacle READ distanceToObstacle)
public:
    TankItem(QObject *parent = 0);

    void setColor(const QColor &color) { m_color = color; }
    QColor color() const { return m_color; }

    void idle(qreal elapsed);
    void setDirection(qreal newDirection);

    qreal speed() const { return 90.0; }
    qreal angularSpeed() const { return 90.0; }

    QRectF boundingRect() const;

    void hitByRocket();

    void setEnabled(bool b) { m_enabled = b; }
    bool enabled() const { return m_enabled; }

    qreal direction() const;
    qreal distanceToObstacle() const;
    qreal distanceToObstacle(QGraphicsItem **item) const;

//! [0]
signals:
    void tankSpotted(qreal direction, qreal distance);
    void collision(const QLineF &collidedLine);
    void actionCompleted();
    void cannonFired();
    void aboutToBeDestroyed();

public slots:
    void moveForwards(qreal length = 10.0);
    void moveBackwards(qreal length = 10.0);
    void turn(qreal degrees = 30.0);
    void turnTo(qreal degrees = 0.0);
    void stop();
    void fireCannon();
//! [0]

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value);

private:
    void setAction(Action *newAction);

    Action *m_currentAction;
    qreal m_currentDirection;
    QColor m_color;
    bool m_enabled;
};

#endif

/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#ifndef __BOAT__H__
#define __BOAT__H__

//Qt
#include <QtCore/QObject>
#include <QtGui/QKeyEvent>

#include <QtGui/QGraphicsWidget>

class PixmapItem;
class Bomb;
QT_BEGIN_NAMESPACE
class QVariantAnimation;
class QAbstractAnimation;
class QStateMachine;
QT_END_NAMESPACE

class Boat : public QGraphicsWidget
{
Q_OBJECT
public:
    enum Movement {
       None = 0,
       Left,
       Right
    };
    enum { Type = UserType + 2 };
    Boat(QGraphicsItem *parent = 0, Qt::WindowFlags wFlags = 0);
    void destroy();
    void run();
    void stop();

    int bombsLaunched() const;
    void setBombsLaunched(int number);

    int currentSpeed() const;
    void setCurrentSpeed(int speed);

    enum Movement currentDirection() const;
    void setCurrentDirection(Movement direction);

    void updateBoatMovement();

    virtual int type() const;

signals:
    void boatDestroyed();
    void boatExecutionFinished();

private:
    int speed;
    int bombsAlreadyLaunched;
    Movement direction;
    QVariantAnimation *movementAnimation;
    QAbstractAnimation *destroyAnimation;
    QStateMachine *machine;
    PixmapItem *pixmapItem;
};

#endif //__BOAT__H__

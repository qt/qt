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

#include "seek_ai.h"

QState *SeekAi::create(QState *parentState, QObject *tank)
{
    QState *topLevel = new QState(parentState);
    topLevel->setObjectName("topLevel");

    QState *seek = new QState(topLevel);
    seek->setObjectName("seek");
    topLevel->setInitialState(seek);

    QState *lookForNearestWall = new SearchState(tank, seek);
    lookForNearestWall->setObjectName("lookForNearestWall");
    seek->setInitialState(lookForNearestWall);

    QState *driveToFirstObstacle = new QState(seek);
    driveToFirstObstacle->setObjectName("driveToFirstObstacle");
    lookForNearestWall->addTransition(lookForNearestWall, SIGNAL(nearestObstacleStraightAhead()),
        driveToFirstObstacle);

    QState *drive = new QState(driveToFirstObstacle);
    drive->setObjectName("drive");
    driveToFirstObstacle->setInitialState(drive);
    connect(drive, SIGNAL(entered()), tank, SLOT(moveForwards()));
    connect(drive, SIGNAL(exited()), tank, SLOT(stop()));

    // Go in loop
    QState *finishedDriving = new QState(driveToFirstObstacle);
    finishedDriving->setObjectName("finishedDriving");
    drive->addTransition(tank, SIGNAL(actionCompleted()), finishedDriving);
    finishedDriving->addTransition(drive);

    QState *turnTo = new QState(seek);
    turnTo->setObjectName("turnTo");
    driveToFirstObstacle->addTransition(new CollisionTransition(tank, turnTo));

    turnTo->addTransition(tank, SIGNAL(actionCompleted()), driveToFirstObstacle);

    ChaseState *chase = new ChaseState(tank, topLevel);
    chase->setObjectName("chase");
    seek->addTransition(new TankSpottedTransition(tank, chase));
    chase->addTransition(chase, SIGNAL(finished()), driveToFirstObstacle);
    chase->addTransition(new TankSpottedTransition(tank, chase));

    return topLevel;
}

Q_EXPORT_PLUGIN2(seek_ai, SeekAi)

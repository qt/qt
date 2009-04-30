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

    return topLevel;
}

Q_EXPORT_PLUGIN2(seek_ai, SeekAi)

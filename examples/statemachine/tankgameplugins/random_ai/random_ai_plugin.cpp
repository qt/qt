#include "random_ai_plugin.h"

#include <QState>
#include <QtPlugin>

#include <time.h>

QState *RandomAiPlugin::create(QState *parentState, QObject *tank)
{
    qsrand(uint(time(NULL)));

    QState *topLevel = new QState(parentState);

    QState *selectNextActionState = new SelectActionState(topLevel);
    topLevel->setInitialState(selectNextActionState);

    QState *fireState = new RandomDistanceState(topLevel);
    connect(fireState, SIGNAL(distanceComputed(qreal)), tank, SLOT(fireCannon()));
    selectNextActionState->addTransition(selectNextActionState, SIGNAL(fireSelected()), fireState);

    QState *moveForwardsState = new RandomDistanceState(topLevel);
    connect(moveForwardsState, SIGNAL(distanceComputed(qreal)), tank, SLOT(moveForwards(qreal)));
    selectNextActionState->addTransition(selectNextActionState, SIGNAL(moveForwardsSelected()), moveForwardsState);

    QState *moveBackwardsState = new RandomDistanceState(topLevel);
    connect(moveBackwardsState, SIGNAL(distanceComputed(qreal)), tank, SLOT(moveBackwards(qreal)));
    selectNextActionState->addTransition(selectNextActionState, SIGNAL(moveBackwardsSelected()), moveBackwardsState);

    QState *turnState = new RandomDistanceState(topLevel);
    connect(turnState, SIGNAL(distanceComputed(qreal)), tank, SLOT(turn(qreal)));
    selectNextActionState->addTransition(selectNextActionState, SIGNAL(turnSelected()), turnState);

    topLevel->addTransition(tank, SIGNAL(actionCompleted()), selectNextActionState);

    return topLevel;
}

Q_EXPORT_PLUGIN2(random_ai, RandomAiPlugin)

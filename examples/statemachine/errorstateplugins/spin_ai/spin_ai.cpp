#include "spin_ai.h"

#include <QtPlugin>

QState *SpinAi::create(QState *parentState, QObject *tank)
{
    QState *topLevel = new QState(parentState);
    QState *spinState = new SpinState(tank, topLevel);    
    topLevel->setInitialState(spinState);

    // When tank is spotted, fire two times and go back to spin state
    QState *fireState = new QState(topLevel);

    QState *fireOnce = new QState(fireState);
    fireState->setInitialState(fireOnce); 
    connect(fireOnce, SIGNAL(entered()), tank, SLOT(fireCannon()));

    QState *fireTwice = new QState(fireState);
    connect(fireTwice, SIGNAL(entered()), tank, SLOT(fireCannon()));
    
    fireOnce->addTransition(tank, SIGNAL(actionCompleted()), fireTwice);
    fireTwice->addTransition(tank, SIGNAL(actionCompleted()), spinState);
    
    spinState->addTransition(tank, SIGNAL(tankSpotted(qreal,qreal)), fireState);
    
    return topLevel;
}

Q_EXPORT_PLUGIN2(spin_ai, SpinAi)

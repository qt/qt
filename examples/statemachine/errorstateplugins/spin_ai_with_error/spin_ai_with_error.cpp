#include "spin_ai_with_error.h"

#include <QtPlugin>

QState *SpinAiWithError::create(QState *parentState, QObject *tank)
{
    QState *topLevel = new QState(parentState);
    QState *spinState = new SpinState(tank, topLevel);    
    topLevel->setInitialState(spinState);

    // When tank is spotted, fire two times and go back to spin state
    // (no initial state set for fireState will lead to run-time error in machine)
    QState *fireState = new QState(topLevel);

    QState *fireOnce = new QState(fireState);
    connect(fireOnce, SIGNAL(entered()), tank, SLOT(fireCannon()));

    QState *fireTwice = new QState(fireState);
    connect(fireTwice, SIGNAL(entered()), tank, SLOT(fireCannon()));
    
    fireOnce->addTransition(tank, SIGNAL(actionCompleted()), fireTwice);
    fireTwice->addTransition(tank, SIGNAL(actionCompleted()), spinState);
    
    spinState->addTransition(tank, SIGNAL(tankSpotted(qreal,qreal)), fireState);
    
    return topLevel;
}

Q_EXPORT_PLUGIN2(spin_ai_with_error, SpinAiWithError)

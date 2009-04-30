#ifndef SPIN_AI_WITH_ERROR_H
#define SPIN_AI_WITH_ERROR_H

#include <errorstate/plugin.h>
#include <errorstate/tank.h>

#include <QObject>
#include <QState>

class SpinState: public QState
{
    Q_OBJECT
public:
    SpinState(Tank *tank, QState *parent) : QState(parent), m_tank(tank)
    {
    }

public slots:
    void spin() 
    {
        m_tank->turn(90.0);
    }

protected:
    void onEntry()
    {
        connect(m_tank, SIGNAL(actionCompleted()), this, SLOT(spin()));
        spin();        
    }

private:
    Tank *m_tank;

};

class SpinAiWithError: public QObject, public Plugin
{
    Q_OBJECT
    Q_INTERFACES(Plugin)
public:
    virtual QState *create(QState *parentState, Tank *tank);
};

#endif

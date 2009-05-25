#ifndef SPIN_AI_H
#define SPIN_AI_H

#include <tankgame/plugin.h>

#include <QObject>
#include <QState>
#include <QVariant>

class SpinState: public QState
{
    Q_OBJECT
public:
    SpinState(QObject *tank, QState *parent) : QState(parent), m_tank(tank)
    {
    }

public slots:
    void spin() 
    {
        m_tank->setProperty("direction", m_tank->property("direction").toDouble() + 90.0);
    }

protected:
    void onEntry(QEvent *)
    {
        connect(m_tank, SIGNAL(actionCompleted()), this, SLOT(spin()));
        spin();        
    }

    void onExit(QEvent *)
    {
        disconnect(m_tank, SIGNAL(actionCompleted()), this, SLOT(spin()));
    }

private:
    QObject *m_tank;

};

class SpinAi: public QObject, public Plugin
{
    Q_OBJECT
    Q_INTERFACES(Plugin)
public:
    SpinAi() { setObjectName("Spin and destroy"); }

    virtual QState *create(QState *parentState, QObject *tank);
};

#endif

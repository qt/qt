#ifndef SPIN_AI_WITH_ERROR_H
#define SPIN_AI_WITH_ERROR_H

#include <errorstate/plugin.h>

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
        m_tank->setProperty("direction", 90.0);
    }

protected:
    void onEntry(QEvent *)
    {
        connect(m_tank, SIGNAL(actionCompleted()), this, SLOT(spin()));
        spin();        
    }

private:
    QObject *m_tank;

};

class SpinAiWithError: public QObject, public Plugin
{
    Q_OBJECT
    Q_INTERFACES(Plugin)
public:
    virtual QState *create(QState *parentState, QObject *tank);
};

#endif

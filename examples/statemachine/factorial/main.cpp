/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
****************************************************************************/

#include <QtCore>
#include <stdio.h>
#ifdef QT_STATEMACHINE_SOLUTION
#include <qstatemachine.h>
#include <qstate.h>
#include <qsignaltransition.h>
#include <qfinalstate.h>
#endif

class Factorial : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int x READ x WRITE setX)
    Q_PROPERTY(int fac READ fac WRITE setFac)
public:
    Factorial(QObject *parent = 0)
        : QObject(parent)
    {
        m_fac = 1;
        m_x = -1;
    }

    int x() const
    {
        return m_x;
    }

    void setX(int x)
    {
        if (x == m_x)
            return;
        m_x = x;
        emit xChanged();
    }

    int fac() const
    {
        return m_fac;
    }

    void setFac(int fac)
    {
        m_fac = fac;
    }

Q_SIGNALS:
    void xChanged();
    
private:
    int m_x;
    int m_fac;
};

class FactorialLoopTransition : public QSignalTransition
{
public:
    FactorialLoopTransition(Factorial *fact)
        : QSignalTransition(fact, SIGNAL(xChanged())), m_fact(fact)
    {}

    virtual bool eventTest(QEvent *) const
    {
        return m_fact->property("x").toInt() > 1;
    }

    virtual void onTransition()
    {
        int x = m_fact->property("x").toInt();
        int fac = m_fact->property("fac").toInt();
        m_fact->setProperty("fac",  x * fac);
        m_fact->setProperty("x",  x - 1);
    }

private:
    Factorial *m_fact;
};

class FactorialDoneTransition : public QSignalTransition
{
public:
    FactorialDoneTransition(Factorial *fact)
        : QSignalTransition(fact, SIGNAL(xChanged())), m_fact(fact)
    {}

    virtual bool eventTest(QEvent *) const
    {
        return m_fact->property("x").toInt() <= 1;
    }

    virtual void onTransition()
    {
        fprintf(stdout, "%d\n", m_fact->property("fac").toInt());
    }

private:
    Factorial *m_fact;
};

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    Factorial factorial;

    QStateMachine machine;

    QState *computing = new QState(machine.rootState());
    computing->addTransition(new FactorialLoopTransition(&factorial));

    QFinalState *done = new QFinalState(machine.rootState());
    FactorialDoneTransition *doneTransition = new FactorialDoneTransition(&factorial);
    doneTransition->setTargetState(done);
    computing->addTransition(doneTransition);

    QState *initialize = new QState(machine.rootState());
    initialize->setPropertyOnEntry(&factorial, "x", 6);
    FactorialLoopTransition *enterLoopTransition = new FactorialLoopTransition(&factorial);
    enterLoopTransition->setTargetState(computing);
    initialize->addTransition(enterLoopTransition);

    QObject::connect(&machine, SIGNAL(finished()), &app, SLOT(quit()));

    machine.setInitialState(initialize);
    machine.start();

    return app.exec();
}

#include "main.moc"

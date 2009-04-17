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
#ifdef QT_STATEMACHINE_SOLUTION
#include <qstatemachine.h>
#include <qstate.h>
#include <qfinalstate.h>
#endif

class S0 : public QState
{
public:
    S0(QState *parent = 0)
        : QState(parent) {}

    virtual void onEntry()
    {
        fprintf(stdout, "Hello world!\n");
    }
};

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    QStateMachine machine;
    QState *s0 = new S0();
    QFinalState *s1 = new QFinalState();
    s0->addTransition(s1);

    machine.addState(s0);
    machine.addState(s1);
    machine.setInitialState(s0);

    QObject::connect(&machine, SIGNAL(finished()), QCoreApplication::instance(), SLOT(quit()));
    machine.start();

    return app.exec();
}

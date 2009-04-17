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

#include <QtGui>
#ifdef QT_STATEMACHINE_SOLUTION
#include <qstate.h>
#include <qstatemachine.h>
#endif

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QPushButton button;

    QStateMachine machine;
    QState *first = new QState();
    first->setObjectName("first");

    QState *off = new QState();
    off->setPropertyOnEntry(&button, "text", "Off");
    off->setObjectName("off");
    first->addTransition(off);

    QState *on = new QState();
    on->setObjectName("on");
    on->setPropertyOnEntry(&button, "text", "On");
    off->addTransition(&button, SIGNAL(clicked()), on);
    on->addTransition(&button, SIGNAL(clicked()), off);

    machine.addState(first);
    machine.addState(off);
    machine.addState(on);
    machine.setInitialState(first);
    machine.start();

    button.resize(100, 50);
    button.show();
    return app.exec();
}

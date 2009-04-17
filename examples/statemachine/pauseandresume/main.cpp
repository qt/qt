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
#include <qstatemachine.h>
#include <qstate.h>
#include <qfinalstate.h>
#include <qhistorystate.h>
#endif

class Window : public QWidget
{
public:
    Window(QWidget *parent = 0)
        : QWidget(parent)
    {
        QPushButton *pb = new QPushButton("Go");
        QPushButton *pauseButton = new QPushButton("Pause");
        QPushButton *quitButton = new QPushButton("Quit");
        QVBoxLayout *vbox = new QVBoxLayout(this);
        vbox->addWidget(pb);
        vbox->addWidget(pauseButton);
        vbox->addWidget(quitButton);

        QStateMachine *machine = new QStateMachine(this);

        QState *process = new QState(machine->rootState());
        process->setObjectName("process");

        QState *s1 = new QState(process);
        s1->setObjectName("s1");
        QState *s2 = new QState(process);
        s2->setObjectName("s2");
        s1->addTransition(pb, SIGNAL(clicked()), s2);
        s2->addTransition(pb, SIGNAL(clicked()), s1);

        QHistoryState *h = process->addHistoryState();
        h->setDefaultState(s1);

        QState *interrupted = new QState(machine->rootState());
        interrupted->setObjectName("interrupted");
        QFinalState *terminated = new QFinalState(machine->rootState());
        terminated->setObjectName("terminated");
        interrupted->addTransition(pauseButton, SIGNAL(clicked()), h);
        interrupted->addTransition(quitButton, SIGNAL(clicked()), terminated);

        process->addTransition(pauseButton, SIGNAL(clicked()), interrupted);
        process->addTransition(quitButton, SIGNAL(clicked()), terminated);

        process->setInitialState(s1);
        machine->setInitialState(process);
        QObject::connect(machine, SIGNAL(finished()), QApplication::instance(), SLOT(quit()));
        machine->start();
    }
};

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    Window win;
    win.show();
    return app.exec();
}

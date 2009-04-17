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
#include <qeventtransition.h>
#endif

class Window : public QWidget
{
public:
    Window(QWidget *parent = 0)
        : QWidget(parent)
    {
        QPushButton *button = new QPushButton(this);
        button->setGeometry(QRect(100, 100, 100, 100));

        QStateMachine *machine = new QStateMachine(this);

        QState *s1 = new QState();
        s1->setPropertyOnEntry(button, "text", "Outside");

        QState *s2 = new QState();
        s2->setPropertyOnEntry(button, "text", "Inside");

        QEventTransition *enterTransition = new QEventTransition(button, QEvent::Enter);
        enterTransition->setTargetState(s2);
        s1->addTransition(enterTransition);

        QEventTransition *leaveTransition = new QEventTransition(button, QEvent::Leave);
        leaveTransition->setTargetState(s1);
        s2->addTransition(leaveTransition);

        QState *s3 = new QState();
        s3->setPropertyOnEntry(button, "text", "Pressing...");

        QEventTransition *pressTransition = new QEventTransition(button, QEvent::MouseButtonPress);
        pressTransition->setTargetState(s3);
        s2->addTransition(pressTransition);

        QEventTransition *releaseTransition = new QEventTransition(button, QEvent::MouseButtonRelease);
        releaseTransition->setTargetState(s2);
        s3->addTransition(releaseTransition);

        machine->addState(s1);
        machine->addState(s2);
        machine->addState(s3);
        machine->setInitialState(s1);
        QObject::connect(machine, SIGNAL(finished()), qApp, SLOT(quit()));
        machine->start();
    }
};

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    Window window;
    window.resize(300, 300);
    window.show();

    return app.exec();
}

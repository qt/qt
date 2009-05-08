/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
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

        QHistoryState *h = new QHistoryState(process);
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

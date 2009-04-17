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
#endif

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QLabel label;
    label.setAlignment(Qt::AlignCenter);

    QStateMachine machine;

    QState *s1 = new QState();
    s1->setObjectName("s1");
    s1->assignProperty(&label, "text", "In S1, hang on...");
    s1->assignProperty(&label, "geometry", QRect(100, 100, 200, 100));

      QState *s1_timer = new QState(s1);
      s1_timer->setObjectName("s1_timer");
      QTimer t1;
      t1.setInterval(2000);
      s1_timer->invokeMethodOnEntry(&t1, "start");
      QFinalState *s1_done = new QFinalState(s1);
      s1_done->setObjectName("s1_done");
      s1_timer->addTransition(&t1, SIGNAL(timeout()), s1_done);
      s1->setInitialState(s1_timer);

    QState *s2 = new QState();
    s2->setObjectName("s2");
    s2->assignProperty(&label, "text", "In S2, I'm gonna quit on you...");
    s2->assignProperty(&label, "geometry", QRect(300, 300, 300, 100));
//    s2->invokeMethodOnEntry(&label, "setNum", QList<QVariant>() << 123);
//    s2->invokeMethodOnEntry(&label, "showMaximized");

      QState *s2_timer = new QState(s2);
      s2_timer->setObjectName("s2_timer");
      QTimer t2;
      t2.setInterval(2000);
      s2_timer->invokeMethodOnEntry(&t2, "start");
      QFinalState *s2_done = new QFinalState(s2);
      s2_done->setObjectName("s2_done");
      s2_timer->addTransition(&t2, SIGNAL(timeout()), s2_done);
      s2->setInitialState(s2_timer);

    s1->addFinishedTransition(s2);

    QFinalState *s3 = new QFinalState();
    s3->setObjectName("s3");
    s2->addFinishedTransition(s3);

    machine.addState(s1);
    machine.addState(s2);
    machine.addState(s3);
    machine.setInitialState(s1);
    QObject::connect(&machine, SIGNAL(finished()), &app, SLOT(quit()));
    machine.start();

    label.show();
    return app.exec();
}

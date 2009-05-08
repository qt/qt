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

    virtual void onTransition(QEvent *)
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

    virtual void onTransition(QEvent *)
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
    initialize->assignProperty(&factorial, "x", 6);
    FactorialLoopTransition *enterLoopTransition = new FactorialLoopTransition(&factorial);
    enterLoopTransition->setTargetState(computing);
    initialize->addTransition(enterLoopTransition);

    QObject::connect(&machine, SIGNAL(finished()), &app, SLOT(quit()));

    machine.setInitialState(initialize);
    machine.start();

    return app.exec();
}

#include "main.moc"

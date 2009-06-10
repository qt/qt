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

//! [0]
class Factorial : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int x READ x WRITE setX)
    Q_PROPERTY(int fac READ fac WRITE setFac)
public:
    Factorial(QObject *parent = 0)
        : QObject(parent), m_x(-1), m_fac(1)
    {
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
        emit xChanged(x);
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
    void xChanged(int value);
    
private:
    int m_x;
    int m_fac;
};
//! [0]

//! [1]
class FactorialLoopTransition : public QSignalTransition
{
public:
    FactorialLoopTransition(Factorial *fact)
        : QSignalTransition(fact, SIGNAL(xChanged(int))), m_fact(fact)
    {}

    virtual bool eventTest(QEvent *e)
    {
        if (!QSignalTransition::eventTest(e))
            return false;
        QSignalEvent *se = static_cast<QSignalEvent*>(e);
        return se->arguments().at(0).toInt() > 1;
    }

    virtual void onTransition(QEvent *e)
    {
        QSignalEvent *se = static_cast<QSignalEvent*>(e);
        int x = se->arguments().at(0).toInt();
        int fac = m_fact->property("fac").toInt();
        m_fact->setProperty("fac",  x * fac);
        m_fact->setProperty("x",  x - 1);
    }

private:
    Factorial *m_fact;
};
//! [1]

//! [2]
class FactorialDoneTransition : public QSignalTransition
{
public:
    FactorialDoneTransition(Factorial *fact)
        : QSignalTransition(fact, SIGNAL(xChanged(int))), m_fact(fact)
    {}

    virtual bool eventTest(QEvent *e)
    {
        if (!QSignalTransition::eventTest(e))
            return false;
        QSignalEvent *se = static_cast<QSignalEvent*>(e);
        return se->arguments().at(0).toInt() <= 1;
    }

    virtual void onTransition(QEvent *)
    {
        fprintf(stdout, "%d\n", m_fact->property("fac").toInt());
    }

private:
    Factorial *m_fact;
};
//! [2]

//! [3]
int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    Factorial factorial;
    QStateMachine machine;
//! [3]

//! [4]
    QState *compute = new QState(machine.rootState());
    compute->assignProperty(&factorial, "fac", 1);
    compute->assignProperty(&factorial, "x", 6);
    compute->addTransition(new FactorialLoopTransition(&factorial));
//! [4]

//! [5]
    QFinalState *done = new QFinalState(machine.rootState());
    FactorialDoneTransition *doneTransition = new FactorialDoneTransition(&factorial);
    doneTransition->setTargetState(done);
    compute->addTransition(doneTransition);
//! [5]

//! [6]
    machine.setInitialState(compute);
    QObject::connect(&machine, SIGNAL(finished()), &app, SLOT(quit()));
    machine.start();

    return app.exec();
}
//! [6]

#include "main.moc"

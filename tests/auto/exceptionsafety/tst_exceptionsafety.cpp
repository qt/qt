/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QtTest/QtTest>

QT_USE_NAMESPACE

#if defined(QT_NO_EXCEPTIONS)
    QTEST_NOOP_MAIN
#else
class tst_ExceptionSafety: public QObject
{
    Q_OBJECT
private slots:
    void exceptionInSlot();
};

class Emitter: public QObject
{
    Q_OBJECT
public:
    inline void emitTestSignal() { emit testSignal(); }
signals:
    void testSignal();
};

class ExceptionThrower: public QObject
{
    Q_OBJECT
public slots:
    void thrower() { throw 5; }
};

// connect a signal to a slot that throws an exception
// run this through valgrind to make sure it doesn't corrupt
void tst_ExceptionSafety::exceptionInSlot()
{
    Emitter emitter;
    ExceptionThrower thrower;

    connect(&emitter, SIGNAL(testSignal()), &thrower, SLOT(thrower()));

    try {
        emitter.emitTestSignal();
    } catch (int i) {
        QCOMPARE(i, 5);
    }
}

QTEST_MAIN(tst_ExceptionSafety)
#include "tst_exceptionsafety.moc"
#endif // QT_NO_EXCEPTIONS

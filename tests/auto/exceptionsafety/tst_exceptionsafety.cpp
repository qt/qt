/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
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

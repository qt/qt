/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <qtest.h>
#include <QObject>
#include <QDebug>

class LotsOfSignals : public QObject
{
    Q_OBJECT
public:
    LotsOfSignals() {}

signals:
    void extraSignal1();
    void extraSignal2();
    void extraSignal3();
    void extraSignal4();
    void extraSignal5();
    void extraSignal6();
    void extraSignal7();
    void extraSignal8();
    void extraSignal9();
    void extraSignal10();
    void extraSignal12();
    void extraSignal13();
    void extraSignal14();
    void extraSignal15();
    void extraSignal16();
    void extraSignal17();
    void extraSignal18();
    void extraSignal19();
    void extraSignal20();
    void extraSignal21();
    void extraSignal22();
    void extraSignal23();
    void extraSignal24();
    void extraSignal25();
    void extraSignal26();
    void extraSignal27();
    void extraSignal28();
    void extraSignal29();
    void extraSignal30();
    void extraSignal31();
    void extraSignal32();
    void extraSignal33();
    void extraSignal34();
    void extraSignal35();
    void extraSignal36();
    void extraSignal37();
    void extraSignal38();
    void extraSignal39();
    void extraSignal40();
    void extraSignal41();
    void extraSignal42();
    void extraSignal43();
    void extraSignal44();
    void extraSignal45();
    void extraSignal46();
    void extraSignal47();
    void extraSignal48();
    void extraSignal49();
    void extraSignal50();
    void extraSignal51();
    void extraSignal52();
    void extraSignal53();
    void extraSignal54();
    void extraSignal55();
    void extraSignal56();
    void extraSignal57();
    void extraSignal58();
    void extraSignal59();
    void extraSignal60();
    void extraSignal61();
    void extraSignal62();
    void extraSignal63();
    void extraSignal64();
    void extraSignal65();
    void extraSignal66();
    void extraSignal67();
    void extraSignal68();
    void extraSignal69();
    void extraSignal70();
};

class tst_signalemission : public QObject
{
    Q_OBJECT
public:
    tst_signalemission() {}

private slots:
    void unconnected_data();
    void unconnected();
};

void tst_signalemission::unconnected_data()
{
    QTest::addColumn<int>("signal_index");
    QTest::newRow("9") << 9;
    QTest::newRow("32") << 32;
    QTest::newRow("33") << 33;
    QTest::newRow("64") << 64;
    QTest::newRow("65") << 65;
    QTest::newRow("70") << 70;
}

void tst_signalemission::unconnected()
{
    LotsOfSignals *obj = new LotsOfSignals;
    QFETCH(int, signal_index);
    QVERIFY(obj->metaObject()->methodCount() == 73);
    void *v;
    QBENCHMARK {
        //+1 because QObject has one slot
        QMetaObject::metacall(obj, QMetaObject::InvokeMetaMethod, signal_index+1, &v);
    }
    delete obj;
}

QTEST_MAIN(tst_signalemission)
#include "tst_signalemission.moc"

/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QtTest/QtTest>
#include <qapplication.h>

#include <q3socketdevice.h>

#include "../network-settings.h"

//TESTED_CLASS=
//TESTED_FILES=

class tst_Q3SocketDevice : public QObject
{
    Q_OBJECT

public:
    tst_Q3SocketDevice();
    virtual ~tst_Q3SocketDevice();

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void readNull();
};

tst_Q3SocketDevice::tst_Q3SocketDevice()
{
}

tst_Q3SocketDevice::~tst_Q3SocketDevice()
{
}

void tst_Q3SocketDevice::initTestCase()
{
}

void tst_Q3SocketDevice::cleanupTestCase()
{
}

void tst_Q3SocketDevice::init()
{
}

void tst_Q3SocketDevice::cleanup()
{
}

void tst_Q3SocketDevice::readNull()
{
    Q3SocketDevice device;
    device.setBlocking(true);

    int attempts = 10;
    while (attempts--) {
        if (device.connect(QtNetworkSettings::serverIP(), 143))
            break;
    }

    // some static state checking
    QVERIFY(device.isValid());
    QCOMPARE(device.type(), Q3SocketDevice::Stream);
    QCOMPARE(device.protocol(), Q3SocketDevice::IPv4);
    QVERIFY(device.socket() != -1);
    QVERIFY(device.blocking());
#if defined Q_OS_IRIX
    // IRIX defaults to the opposite in Qt 3, so we won't fix
    // this in Qt 4.
    QVERIFY(device.addressReusable());
#else
    QVERIFY(!device.addressReusable());
#endif
    QCOMPARE(device.peerPort(), quint16(143));
    QCOMPARE(device.peerAddress().toString(),
            QtNetworkSettings::serverIP().toString());
    QCOMPARE(device.error(), Q3SocketDevice::NoError);

    // write a logout notice
    QCOMPARE(device.writeBlock("X LOGOUT\r\n", Q_ULONG(10)), Q_LONG(10));

    // expect three lines of response: greeting, bye-warning and
    // logout command completion.
    int ch;
    for (int i = 0; i < 3; ++i) {
        do {
            QVERIFY((ch = device.getch()) != -1);
        } while (char(ch) != '\n');
    }

    // here, read() will return 0.
    char c;
    QCOMPARE(device.readBlock(&c, 1), qint64(0));
    QVERIFY(!device.isValid());
}

QTEST_APPLESS_MAIN(tst_Q3SocketDevice)
#include "tst_q3socketdevice.moc"

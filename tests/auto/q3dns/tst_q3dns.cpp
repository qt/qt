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

#include <q3dns.h>
#include <qapplication.h>
#include <q3socket.h>

//TESTED_CLASS=
//TESTED_FILES=

class tst_Q3Dns : public QObject
{
    Q_OBJECT

public:
    tst_Q3Dns();
    virtual ~tst_Q3Dns();


public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
private slots:
    void destructor();
    void literals();
    void txtRecords();
    void longTxtRecord();
    void simpleLookup();

protected slots:
    void txtRecordAnswer();
    void longTxtRecordAnswer();
    void simpleLookupDone();
};

tst_Q3Dns::tst_Q3Dns()
{
}

tst_Q3Dns::~tst_Q3Dns()
{
}

void tst_Q3Dns::initTestCase()
{
}

void tst_Q3Dns::cleanupTestCase()
{
}

void tst_Q3Dns::init()
{
}

void tst_Q3Dns::cleanup()
{
}

void tst_Q3Dns::destructor()
{
    /*
    The following small program used to crash because of a bug in the Q3Dns
    constructor that should be fixed by change 67978:

	#include <qapplication.h>
	#include <qsocket.h>

	int main( int argc, char **argv )
	{
	    QApplication a( argc, argv );
	    Q3Socket *s = new Q3Socket( &a );
	    s->connectToHost( "ftp.qt.nokia.com", 21 );
	    return 0;
	}
    */
    int c = 0;
    char **v = 0;
    QCoreApplication a(c, v);
    Q3Socket *s = new Q3Socket(&a);
    s->connectToHost("ftp.qt.nokia.com", 21);

    // dummy verify since this test only makes shure that it does not crash
    QVERIFY( TRUE );
}

void tst_Q3Dns::literals()
{
    int c = 0;
    char **v = 0;
    QCoreApplication a(c, v);

    Q3Dns ip4literal1("4.2.2.1", Q3Dns::A);
    QCOMPARE((int) ip4literal1.addresses().count(), 1);
    QCOMPARE(ip4literal1.addresses().first().toString(), QString("4.2.2.1"));

    Q3Dns ip4literal2("4.2.2.1", Q3Dns::Aaaa);
    QCOMPARE((int) ip4literal2.addresses().count(), 0);

    Q3Dns ip6literal1("::1", Q3Dns::A);
    QCOMPARE((int) ip6literal1.addresses().count(), 0);

    Q3Dns ip6literal2("::1", Q3Dns::Aaaa);
    QCOMPARE(ip6literal2.addresses().first().toString(), QString("0:0:0:0:0:0:0:1"));
    QCOMPARE((int) ip6literal2.addresses().count(), 1);
}

void tst_Q3Dns::txtRecords()
{
    QSKIP("TXT record support is broken.", SkipAll);
    int argc = 0;
    char **argv = 0;
    QCoreApplication qapp(argc, argv);

    Q3Dns dns("Sales._ipp._tcp.dns-sd.org", Q3Dns::Txt);
    connect(&dns, SIGNAL(resultsReady()), SLOT(txtRecordAnswer()));
    QTestEventLoop::instance().enterLoop(10);
    if (QTestEventLoop::instance().timeout())
        QFAIL("Timed out while looking up TXT record for Sales._ipp._tcp.dns-sd.org");

    QStringList texts = dns.texts();
#if defined Q_OS_DARWIN
    QSKIP("TXT records in Q3Dns don't work for Mac OS X.", SkipSingle);
#endif
    QVERIFY(!texts.isEmpty());
    QCOMPARE(texts.at(0), QString("rp=lpt1"));
}

void tst_Q3Dns::txtRecordAnswer()
{
    QTestEventLoop::instance().exitLoop();
}

void tst_Q3Dns::longTxtRecord()
{
    QSKIP("Long TXT records in Q3Dns don't work.", SkipSingle);

    int c = 0;
    char **v = 0;
    QCoreApplication a(c, v);

    Q3Dns dns(QString::fromLatin1("andreas.hanssen.name"), Q3Dns::Txt);
    QObject::connect(&dns, SIGNAL(resultsReady()), this, SLOT(longTxtRecordAnswer()));

    QTestEventLoop::instance().enterLoop(30);
    if (QTestEventLoop::instance().timeout())
	QFAIL("Network operation timed out");

    QStringList list = dns.texts();

    QCOMPARE(list.count(), 1);
    QCOMPARE(list[0], QString::fromLatin1("I have a remarkable solution to Fermat's last theorem, but it doesn't fit into this TXT record"));
}

void tst_Q3Dns::longTxtRecordAnswer()
{
    QTestEventLoop::instance().exitLoop();
}

void tst_Q3Dns::simpleLookup()
{
    // Stuff
    int c = 0;
    char **v = 0;
    QCoreApplication a(c, v);
    Q3Dns dns("qt.nokia.com");

    QSignalSpy spy(&dns, SIGNAL(resultsReady()));
    connect(&dns, SIGNAL(resultsReady()), this, SLOT(simpleLookupDone()));
    QTestEventLoop::instance().enterLoop(5);
    if (QTestEventLoop::instance().timeout())
        QFAIL("Network operation timed out");
    QCOMPARE(spy.count(), 1);
}

void tst_Q3Dns::simpleLookupDone()
{
    QTestEventLoop::instance().exitLoop();
}


QTEST_APPLESS_MAIN(tst_Q3Dns)
#include "tst_q3dns.moc"


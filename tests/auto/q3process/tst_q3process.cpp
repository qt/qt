/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include <QtTest/QtTest>

#include <q3process.h>
#include <qregexp.h>
#include <qdebug.h>

QT_FORWARD_DECLARE_CLASS(Q3Process)

//TESTED_CLASS=
//TESTED_FILES=

class tst_Q3Process : public QObject
{
    Q_OBJECT

public:
    tst_Q3Process();
    virtual ~tst_Q3Process();


public slots:
    void init();
    void cleanup();
private slots:
    void readLineStdout_data();
    void readLineStdout();
    void readLineStderr_data();
    void readLineStderr();
    void communication_data();
    void communication();

    void canReadLineStdout_data();
    void canReadLineStdout();
    void canReadLineStderr_data();
    void canReadLineStderr();
    void startWithNoEnvironment();
    void startWithEmptyStringArgument();
    
protected slots:
    void processExited();
    void exitLoopSlot();

private:
    Q3Process *proc;
    QStringList linesReadStdout;
    QStringList linesReadStderr;
};

tst_Q3Process::tst_Q3Process()
    : proc(0)
{
}

tst_Q3Process::~tst_Q3Process()
{
}

void tst_Q3Process::init()
{
    linesReadStdout.clear();
    linesReadStderr.clear();
}

void tst_Q3Process::cleanup()
{
    if ( proc ) {
	proc->kill();
	delete proc;
	proc = 0;
    }
}

void tst_Q3Process::readLineStdout_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QStringList>("res");

    QStringList l;

    QTest::newRow( "unix_0" ) << QString("a\nbc\ndef\nghij\n")
	<< (QStringList)( l << "a" << "bc" << "def" << "ghij" );
    l.clear();
    QTest::newRow( "unix_1" ) << QString("a\nbc\ndef\nghij")
	<< (QStringList)( l << "a" << "bc" << "def" << "ghij" );
    l.clear();
    QTest::newRow( "unix_2" ) << QString("a\nbc\n\ndef\nghij\n")
	<< (QStringList)( l << "a" << "bc" << "" << "def" << "ghij" );
    l.clear();

    QTest::newRow( "windows_0" ) << QString("a\r\nbc\r\ndef\r\nghij\r\n")
	<< (QStringList)( l << "a" << "bc" << "def" << "ghij" );
    l.clear();
    QTest::newRow( "windows_1" ) << QString("a\r\nbc\r\ndef\r\nghij")
	<< (QStringList)( l << "a" << "bc" << "def" << "ghij" );
    l.clear();
    QTest::newRow( "windows_2" ) << QString("a\r\nbc\r\n\r\ndef\r\nghij\r\n")
	<< (QStringList)( l << "a" << "bc" << "" << "def" << "ghij" );
    l.clear();

    QTest::newRow( "mixed_0" ) << QString("a\r\nbc\ndef\r\nghij\n")
	<< (QStringList)( l << "a" << "bc" << "def" << "ghij" );
    l.clear();
    QTest::newRow( "mixed_1" ) << QString("a\nbc\r\ndef\nghij\r\n")
	<< (QStringList)( l << "a" << "bc" << "def" << "ghij" );
    l.clear();
    QTest::newRow( "mixed_2" ) << QString("a\nbc\r\r\ndef\r\r\r\nghij\r\r\r\r\n")
	<< (QStringList)( l << "a" << "bc\r" << "def\r\r" << "ghij\r\r\r" );
    l.clear();
}

void tst_Q3Process::readLineStdout()
{
    QFETCH( QString, input );

    if (proc) delete proc;
    proc = new Q3Process( QString("cat/cat") );
    connect( proc, SIGNAL(processExited()),
	     SLOT(processExited()) );

    QVERIFY( proc->launch( input ) );

    QTestEventLoop::instance().enterLoop( 29 );
    if ( QTestEventLoop::instance().timeout() )
	QFAIL( "Operation timed out" );

    QTEST( linesReadStdout, "res" );
}

void tst_Q3Process::readLineStderr_data()
{
    readLineStdout_data();
}

void tst_Q3Process::readLineStderr()
{
    QFETCH( QString, input );

    if (proc) delete proc;
    proc = new Q3Process( QString("cat/cat -stderr").split(' ') );
    connect( proc, SIGNAL(processExited()),
	     SLOT(processExited()) );

    QVERIFY( proc->launch( input ) );

    QTestEventLoop::instance().enterLoop( 29 );
    if ( QTestEventLoop::instance().timeout() )
	QFAIL( "Operation timed out" );

    QTEST( linesReadStderr, "res" );
}

void tst_Q3Process::communication_data()
{
    QTest::addColumn<QStringList>("command");
    QTest::addColumn<int>("commFlags");
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("resStdout");
    QTest::addColumn<QString>("resStderr");

    QTest::newRow( "no_dup_0" ) << QString("cat/cat").split(' ')
	<< ( Q3Process::Stdin | Q3Process::Stdout | Q3Process::Stderr )
	<< QString("12345")
	<< QString("12345")
	<< QString();
    QTest::newRow( "no_dup_1" ) << QString("cat/cat -stderr").split(' ')
	<< ( Q3Process::Stdin | Q3Process::Stdout | Q3Process::Stderr )
	<< QString("12345")
	<< QString()
	<< QString("12345");
    QTest::newRow( "no_dup_2" ) << QString("cat/cat -stdout_and_stderr").split(' ')
	<< ( Q3Process::Stdin | Q3Process::Stdout | Q3Process::Stderr )
	<< QString("12345")
	<< QString("12345")
	<< QString("23456");

    QTest::newRow( "dup_0" ) << QString("cat/cat").split(' ')
	<< ( Q3Process::Stdin | Q3Process::Stdout | Q3Process::Stderr | Q3Process::DupStderr )
	<< QString("12345")
	<< QString("12345")
	<< QString();
    QTest::newRow( "dup_1" ) << QString("cat/cat -stderr").split(' ')
	<< ( Q3Process::Stdin | Q3Process::Stdout | Q3Process::Stderr | Q3Process::DupStderr )
	<< QString("12345")
	<< QString("12345")
	<< QString();
    QTest::newRow( "dup_2" ) << QString("cat/cat -stdout_and_stderr").split(' ')
	<< ( Q3Process::Stdin | Q3Process::Stdout | Q3Process::Stderr | Q3Process::DupStderr )
	<< QString("12345")
	<< QString("1223344556")
	<< QString();
}

void tst_Q3Process::communication()
{
    for ( int i=0; i<2; i++ ) {
	cleanup();

	QFETCH( QStringList, command );
	QFETCH( int, commFlags );
	QFETCH( QString, input );

        if (proc) delete proc;
	proc = new Q3Process( command );
	proc->setCommunication( commFlags );
	connect( proc, SIGNAL(processExited()),
		SLOT(processExited()) );

	QVERIFY( proc->launch( input ) );

	QTestEventLoop::instance().enterLoop( 29 );
	if ( QTestEventLoop::instance().timeout() )
	    QFAIL( "Operation timed out" );

	if ( i == 0 ) {
	    QTEST( QString( proc->readStdout() ), "resStdout" );
	    QTEST( QString( proc->readStderr() ), "resStderr" );
	} else {
	    QTEST( QString( proc->readStdout() ), "resStdout" );
	    QTEST( QString( proc->readStderr() ), "resStderr" );
	}
    }
}

void tst_Q3Process::canReadLineStdout_data()
{
    readLineStdout_data();
}

void tst_Q3Process::canReadLineStdout()
{
    // This function tests the busy-loop abilities of the canReadLineStdout()
    // function.
    QFETCH( QString, input );
    QFETCH(QStringList, res);

    if (proc) delete proc;
    proc = new Q3Process( QString("cat/cat") );
    QVERIFY( proc->start() );
    proc->writeToStdin( input );
    proc->flushStdin();
    proc->closeStdin();

    do {
        connect(proc, SIGNAL(readyReadStdout()), this, SLOT(exitLoopSlot()));
        QTestEventLoop::instance().enterLoop(5);
        if (QTestEventLoop::instance().timeout()) {
            if (!proc->isRunning())
                break;
            QFAIL("Timed out while waiting for my kids");
        }

        // q3process can (actually!) emit readyRead() when
        // canReadLineStdout() is called.
        proc->disconnect();

        while ( proc->canReadLineStdout() )
            linesReadStdout << proc->readLineStdout();
    } while (linesReadStdout.size() < res.size() && proc->isRunning());

    // q3process can (actually!) emit readyRead() when
    // canReadLineStdout() is called.
    proc->disconnect();

    // if the last line is not terminated with a newline, we get it only after
    // we determined that the process is not running anymore
    if ( proc->canReadLineStdout() )
        linesReadStdout << proc->readLineStdout();

    QCOMPARE( linesReadStdout, res );
}

void tst_Q3Process::exitLoopSlot()
{
    QTestEventLoop::instance().exitLoop();
}

void tst_Q3Process::canReadLineStderr_data()
{
    readLineStdout_data();
}

void tst_Q3Process::canReadLineStderr()
{
    // This function tests the busy-loop abilities of the canReadLineStderr()
    // function.
    QFETCH( QString, input );
    QFETCH(QStringList, res);

    if (proc) delete proc;
    proc = new Q3Process( QString("cat/cat -stderr").split(' ') );
    QVERIFY( proc->start() );
    proc->writeToStdin( input );
    proc->flushStdin();
    proc->closeStdin();

    do {
        connect(proc, SIGNAL(readyReadStderr()), this, SLOT(exitLoopSlot()));
        QTestEventLoop::instance().enterLoop(5);
        if (QTestEventLoop::instance().timeout()) {
            if (!proc->isRunning())
                break;
            QFAIL("Timed out while waiting for my kids");
        }

        // q3process can (actually!) emit readyRead() when
        // canReadLineStdout() is called.
        proc->disconnect();

        while ( proc->canReadLineStderr() )
            linesReadStderr << proc->readLineStderr();
    } while (linesReadStderr.size() < res.size() && proc->isRunning());

    // q3process can (actually!) emit readyRead() when
    // canReadLineStdout() is called.
    proc->disconnect();

    // if the last line is not terminated with a newline, we get it only after
    // we determined that the process is not running anymore
    if ( proc->canReadLineStderr() )
	linesReadStderr << proc->readLineStderr();

    QCOMPARE( linesReadStderr, res );
}

void tst_Q3Process::processExited()
{
    if ( QTest::currentTestFunction() == QLatin1String("readLineStdout") ) {
	QVERIFY( proc != 0 );

        // q3process can (actually!) emit readyRead() when
        // canReadLineStdout() is called.
        proc->disconnect();

        while ( proc->canReadLineStdout() )
	    linesReadStdout << proc->readLineStdout();

	// if canReadLine...() returns FALSE, the readLine...() function should
	// return QString::null
	QVERIFY( proc->readLineStdout().isNull() );

	QTestEventLoop::instance().exitLoop();

    } else if ( QTest::currentTestFunction() == QLatin1String("readLineStderr") ) {
	QVERIFY( proc != 0 );

        // q3process can (actually!) emit readyRead() when
        // canReadLineStdout() is called.
        proc->disconnect();

        while ( proc->canReadLineStderr() )
	    linesReadStderr << proc->readLineStderr();

	// if canReadLine...() returns FALSE, the readLine...() function should
	// return QString::null
	QVERIFY( proc->readLineStderr().isNull() );

	QTestEventLoop::instance().exitLoop();
    } else if ( QTest::currentTestFunction() == QLatin1String("communication") ) {
	QTestEventLoop::instance().exitLoop();
    }
}

void tst_Q3Process::startWithNoEnvironment()
{
    QStringList args;
    QByteArray result;
#ifdef Q_OS_MACX
    args << "./echo/echo.app";
    result = "./echo/echo.app/Contents/MacOS/echo";
#elif defined Q_OS_WIN
    args << "./echo/echo";
    result = "echo";
#else
    args << "./echo/echo";
    result = args[0];
#endif
    args << "foo";
    proc = new Q3Process(args);
    QVERIFY(proc->start(/* environment = */ 0));
    connect(proc, SIGNAL(readyReadStdout()), this, SLOT(exitLoopSlot()));

    QTestEventLoop::instance().enterLoop(10);
    QVERIFY(!QTestEventLoop::instance().timeout());

#ifdef Q_OS_WIN
    // on different windows compilers the first arg is different, 
    // some just pass what was passed to createprocess while others 
    // expand the entire path.
    QVERIFY(proc->readStdout().contains(result));
#else
    QCOMPARE(proc->readStdout(), result);
#endif
}

void tst_Q3Process::startWithEmptyStringArgument()
{
    // Test that this doesn't assert (task 122353)
    Q3Process process("cmd");
    process.addArgument(QString());

    QStringList env;
    process.start(&env);
}

QTEST_MAIN(tst_Q3Process)
#include "tst_q3process.moc"

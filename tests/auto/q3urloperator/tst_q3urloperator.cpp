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

#include <q3urloperator.h>
#include <qtimer.h>
#include <qapplication.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qurlinfo.h>
#include <q3network.h>
#include <q3networkprotocol.h>
#include <qtimer.h>
#include <q3ptrlist.h>
#include <q3valuelist.h>
#include <qlist.h>

#include "../network-settings.h"

//TESTED_CLASS=
//TESTED_FILES=

QT_FORWARD_DECLARE_CLASS(Q3NetworkOperation)

class tst_Q3UrlOperator : public QObject
{
    Q_OBJECT

public:
    tst_Q3UrlOperator();
    virtual ~tst_Q3UrlOperator();



public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
private slots:
    void copy_data();
    void copy();
    void put_data();
    void put();
    void rename_data();
    void rename();
    void stop_data();
    void stop();
    void listChildren_data();
    void listChildren();

protected slots:
    void slotFinished_init( Q3NetworkOperation* );
    void slotFinished_cleanup( Q3NetworkOperation* );

    void slotFinished_copy( Q3NetworkOperation* );

    void slotFinished_put( Q3NetworkOperation* );
    void slotData_put( const QByteArray&, Q3NetworkOperation* );

    void slotFinished_rename( Q3NetworkOperation* );
    void slotItemChanged_rename( Q3NetworkOperation* );

    void slotFinished_stop( Q3NetworkOperation* );
    void slotDataTransferProgress_stop( int, int, Q3NetworkOperation* );

    void slotFinished_listChildren( Q3NetworkOperation* );
    void slotNewChildren_listChildren( const Q3ValueList<QUrlInfo> &, Q3NetworkOperation * );

    void stopOperation();

private:
    Q3UrlOperator *urlOp;
    QString ftpQtestUpload;
    bool doStop;
    bool finished;

    Q3PtrList<Q3NetworkOperation> pendingOperations;
    Q3NetworkProtocol::State finishedState;
    int	finishedErrorCode;
    QString finishedProtocolDetail;
    Q3ValueList<QUrlInfo> listChildrenInfo;
    QByteArray baData;
    uint timeout_count;
};

//#define DUMP_SIGNALS

tst_Q3UrlOperator::tst_Q3UrlOperator()
{
    q3InitNetworkProtocols();
    timeout_count = 0;

}

tst_Q3UrlOperator::~tst_Q3UrlOperator()
{
}

void tst_Q3UrlOperator::initTestCase()
{
    // create files for checking permissions
    QFile textFile("listData/readOnly");
    textFile.open(QIODevice::WriteOnly);
    textFile.close();
    textFile.setPermissions(QFile::ReadOwner | QFile::ReadUser | QFile::ReadGroup | QFile::ReadOther);
    QFile exe("listData/executable.exe");
    exe.open(QIODevice::WriteOnly);
    exe.close();
    exe.setPermissions(QFile::ReadOwner | QFile::ReadUser | QFile::ReadGroup | QFile::ReadOther |
    		QFile::ExeOwner | QFile::ExeUser | QFile::ExeGroup | QFile::ExeOther);

    // prepare: make sure that there is a unique directory for FTP upload
    // testing (to avoid parallel executed tests interfere with each other)
    ftpQtestUpload = QString("ftp://%1/").arg(QtNetworkSettings::serverLocalName());
    QString dir = QString( "qtest/upload/%1" ).arg( (ulong)this );

    Q3UrlOperator opMkdir( ftpQtestUpload );

    connect( &opMkdir, SIGNAL(finished(Q3NetworkOperation*)), SLOT(slotFinished_init(Q3NetworkOperation*)) );
    if ( opMkdir.mkdir( dir ) != 0 ) {
        QTestEventLoop::instance().enterLoop( 30 );
	if ( QTestEventLoop::instance().timeout() )
	    qWarning( "Prepare: Network operation timed out for create directory" );
    }
    ftpQtestUpload += dir;
}

void tst_Q3UrlOperator::slotFinished_init( Q3NetworkOperation *op )
{
    if ( op->state() == Q3NetworkProtocol::StFailed )
	qWarning( QString( "Prepare: Can't create directory: network operation failed ('%1'). " ).arg(op->protocolDetail()) );
    QTestEventLoop::instance().exitLoop();
}

void tst_Q3UrlOperator::cleanupTestCase()
{
	// remove the files created before
    QFile textFile("listData/readOnly");
    textFile.setPermissions(textFile.permissions() | QFile::WriteOwner);
    if (! textFile.remove())
    	qWarning() << "could not remove file:" << textFile.error();
    QFile exe("listData/executable.exe");
    exe.setPermissions(textFile.permissions() | QFile::WriteOwner);
    if (! exe.remove())
    	qWarning() << "could not remove file:" << exe.error();

    // cleanup: delete the unique directory for FTP upload testing
    QStringList tmp = QStringList::split( '/', ftpQtestUpload, TRUE );
    QString dir = tmp.last();
    tmp.pop_back();
    QString url = tmp.join( "/" );

    Q3UrlOperator opRemove( url );
    connect( &opRemove, SIGNAL(finished(Q3NetworkOperation*)), SLOT(slotFinished_cleanup(Q3NetworkOperation*)) );
    if ( opRemove.remove( dir ) != 0 ) {
	QTestEventLoop::instance().enterLoop( 30 );
	if ( QTestEventLoop::instance().timeout() )
	    qWarning( "Cleanup: Network operation timed out for removing directory" );
    }
}

void tst_Q3UrlOperator::slotFinished_cleanup( Q3NetworkOperation *op )
{
    if ( op->state() == Q3NetworkProtocol::StFailed )
	qWarning( QString( "Cleanup: Can't remove directory: network operation failed ('%1'). " ).arg(op->protocolDetail()) );
    QTestEventLoop::instance().exitLoop();
}

void tst_Q3UrlOperator::init()
{
    doStop = FALSE;
    finished = FALSE;
    pendingOperations.clear();
}

void tst_Q3UrlOperator::cleanup()
{
}

void tst_Q3UrlOperator::copy_data()
{
    const QString ftpQtest( QString("ftp://%1/qtest").arg(QtNetworkSettings::serverLocalName()) );
    const QString httpQtest( QString("http://%1/qtest").arg(QtNetworkSettings::serverLocalName()) );

    // argument for the constructor
    QTest::addColumn<QString>("url");
    // arguments for the copy()
    QTest::addColumn<QString>("from");
    QTest::addColumn<QString>("to");
    QTest::addColumn<bool>("move");
    QTest::addColumn<bool>("toPath");

    // FTP data
    // QTest::newRow( "ftp00" ) << QString() << QString("%1/rfc3252.txt").arg(ftpQtest)	<< QString(".")		    << (bool)FALSE << (bool)TRUE;
    QTest::newRow( "ftp01" ) << QString() << QString("%1/rfc3252").arg(ftpQtest)	<< QString("rfc3252.txt")    << (bool)FALSE << (bool)FALSE;

    QTest::newRow( "ftp02" ) << QString("%1/").arg(ftpQtest) << QString("rfc3252.txt") << QString(".")		<< (bool)FALSE << (bool)TRUE;
    QTest::newRow( "ftp03" ) << QString("%1/").arg(ftpQtest) << QString("rfc3252") << QString("rfc3252.txt")	<< (bool)FALSE << (bool)FALSE;

    QTest::newRow( "ftp04" ) << QString("%1").arg(ftpQtest) << QString("rfc3252.txt")  << QString(".")		<< (bool)FALSE << (bool)TRUE;
    QTest::newRow( "ftp05" ) << QString("%1").arg(ftpQtest) << QString("rfc3252")	    << QString("rfc3252.txt")	<< (bool)FALSE << (bool)FALSE;

    // the ftp.faqs.org host is down that often, that it does not make sense to
    // use it for automated tests
    // QTest::newRow( "ftp06" ) << QString() << QString("ftp://ftp.faqs.org/rfc/rfc3252.txt") << QString(".") << (bool)FALSE << (bool)TRUE;

    // HTTP data
    QTest::newRow( "http00" ) << QString() << QString("%1/rfc3252.txt").arg(httpQtest)    << QString(".")		<< (bool)FALSE << (bool)TRUE;
    QTest::newRow( "http01" ) << QString() << QString("%1/rfc3252").arg(httpQtest)	    << QString("rfc3252.txt")	<< (bool)FALSE << (bool)FALSE;
    QTest::newRow( "http02" ) << QString() << QString("%1/").arg(httpQtest)		    << QString("rfc3252.txt")	<< (bool)FALSE << (bool)FALSE;

    QTest::newRow( "http03" ) << QString("%1/").arg(httpQtest) << QString("rfc3252.txt")	<< QString(".")		    << (bool)FALSE << (bool)TRUE;
    QTest::newRow( "http04" ) << QString("%1/").arg(httpQtest) << QString("rfc3252")	<< QString("rfc3252.txt")    << (bool)FALSE << (bool)FALSE;
    QTest::newRow( "http05" ) << QString("%1/").arg(httpQtest) << QString("")		<< QString("rfc3252.txt")    << (bool)FALSE << (bool)FALSE;
    QTest::newRow( "http06" ) << QString("%1/").arg(httpQtest) << QString()	<< QString("rfc3252.txt")    << (bool)FALSE << (bool)FALSE;

    QTest::newRow( "http07" ) << QString("%1").arg(httpQtest) << QString("rfc3252.txt")	<< QString(".")		    << (bool)FALSE << (bool)TRUE;
    QTest::newRow( "http08" ) << QString("%1").arg(httpQtest) << QString("rfc3252")	<< QString("rfc3252.txt")    << (bool)FALSE << (bool)FALSE;
    QTest::newRow( "http09" ) << QString("%1").arg(httpQtest) << QString("")		<< QString("rfc3252.txt")    << (bool)FALSE << (bool)FALSE;
    QTest::newRow( "http10" ) << QString("%1").arg(httpQtest) << QString()	<< QString("rfc3252.txt")    << (bool)FALSE << (bool)FALSE;

    // Unstable host:
    // QTest::newRow( "http11" ) << QString() << QString("http://www.ietf.org/rfc/rfc3252.txt") << QString(".") << (bool)FALSE << (bool)TRUE;
}

void tst_Q3UrlOperator::copy()
{
    if (timeout_count >= 3) {
        QFAIL( "Too many timeout's. Aborting test prematurely." );
    }

    QFETCH( QString, url );
    QFETCH( QString, from );
    QFETCH( QString, to );
    QFETCH( bool, move );
    QFETCH( bool, toPath );

    if ( url.isNull() ) {
	urlOp = new Q3UrlOperator();
    } else {
	urlOp = new Q3UrlOperator( url );
    }
    pendingOperations = urlOp->copy( from, to, move, toPath );

    connect( urlOp, SIGNAL(finished(Q3NetworkOperation*)), SLOT(slotFinished_copy(Q3NetworkOperation*)) );

    QTestEventLoop::instance().enterLoop( 60 );
    delete urlOp;
    if ( QTestEventLoop::instance().timeout() )
	QFAIL( "Network operation timed out" );

    // test the results of the operation
    if ( finished ) {
	QVERIFY( finishedState==Q3NetworkProtocol::StDone || finishedState==Q3NetworkProtocol::StFailed );
	if ( finishedState == Q3NetworkProtocol::StDone ) {
	    QFile file( "rfc3252.txt" );
	    if ( !file.open( QIODevice::ReadOnly ) )
		QFAIL( "Could not open downloaded file" );

	    // ### this should move in the tesdata
	    QFile fileRes( "copy.res/rfc3252.txt" );
	    if ( !fileRes.open( QIODevice::ReadOnly ) )
		QFAIL( "Could not open result file" );

        QCOMPARE(file.readAll(), fileRes.readAll());

	    // delete the downloaded file
	    if ( !file.remove() )
		QFAIL( QString("Cleanup failed: could not remove the downloaded file '%1'").arg(file.name()) );
            timeout_count = 0;
	} else {
	    QFAIL( QString( "Network operation failed ('%1'). "
			"This could be due to a temporary network failure. "
			"If this test continues to fail, take a closer look").arg(finishedProtocolDetail) );
            timeout_count++;
	}
    }
}

void tst_Q3UrlOperator::slotFinished_copy( Q3NetworkOperation *op )
{
#if defined( DUMP_SIGNALS )
    qDebug( "finished( %p )", op );
#endif
    QVERIFY( op != 0 );
    pendingOperations.remove( op );
    // We are finished if either of this happens:
    // - both, the get and the put of the copy are finished
    // - we are in an error case (for the case that the get fails, no
    //   finished() signal for the put is emitted since it is never executed)
    if ( pendingOperations.isEmpty() || op->state()==Q3NetworkProtocol::StFailed ) {
	finished = TRUE;
	finishedState = op->state();
	finishedErrorCode = op->errorCode();
	finishedProtocolDetail = op->protocolDetail();

	QTestEventLoop::instance().exitLoop();
    }
}

void tst_Q3UrlOperator::put_data()
{
    const QString httpQtest( QString("http://%1/qtest/cgi-bin").arg(QtNetworkSettings::serverLocalName()) );
    const QString httpQtestGet( QString("http://%1/qtest/cgi-bin/retrieve_testfile.cgi").arg(QtNetworkSettings::serverLocalName()) );

    QByteArray putData_1( 5 );
    putData_1[0] = 'a';
    putData_1[1] = 'b';
    putData_1[2] = '\n';
    putData_1[3] = 'c';
    putData_1[4] = 'd';

    QTest::addColumn<QString>("url");
    QTest::addColumn<QString>("to");
    QTest::addColumn<QByteArray>("putData");
    QTest::addColumn<QString>("getUrl");
    QTest::addColumn<QString>("rmUrl");
    QTest::addColumn<QString>("rmFile");

    // FTP data
    QTest::newRow( "ftp00" )
	<< ftpQtestUpload << "put_ftp00" << putData_1
	<< QString("%1/put_ftp00").arg(ftpQtestUpload)
	<< ftpQtestUpload << "put_ftp00";
    QTest::newRow( "ftp01" )
	<< QString() << QString("%1/put_ftp01").arg(ftpQtestUpload) << putData_1
	<< QString("%1/put_ftp01").arg(ftpQtestUpload)
	<< ftpQtestUpload << "put_ftp01";

    QTest::newRow( "ftp02" )
	<< ftpQtestUpload << "put_ftp02" << QByteArray(0)
	<< QString("%1/put_ftp02").arg(ftpQtestUpload)
	<< ftpQtestUpload << "put_ftp02";
    QTest::newRow( "ftp03" )
	<< QString() << QString("%1/put_ftp03").arg(ftpQtestUpload) << QByteArray(0)
	<< QString("%1/put_ftp03").arg(ftpQtestUpload)
	<< ftpQtestUpload << "put_ftp03";
    // HTTP data
    QTest::newRow( "http00" )
	<< httpQtest << "store_testfile.cgi" << putData_1
	<< httpQtestGet
	<< httpQtest << QString();
    QTest::newRow( "http01" )
	<< QString() << QString("%1/store_testfile.cgi").arg(httpQtest) << putData_1
	<< httpQtestGet
	<< httpQtest << QString();
    QTest::newRow( "http02" )
	<< httpQtest     << "store_testfile.cgi" << QByteArray(0)
	<< httpQtestGet
	<< httpQtest << QString();
    QTest::newRow( "http03" ) << QString() << QString("%1/store_testfile.cgi").arg(httpQtest) << QByteArray(0)
	<< httpQtestGet
	<< httpQtest << QString();
}

void tst_Q3UrlOperator::put()
{
    {
	// do the put
	QFETCH( QString, url );
	QFETCH( QString, to );
	QFETCH( QByteArray, putData );

	if ( url.isNull() ) {
	    urlOp = new Q3UrlOperator();
	} else {
	    urlOp = new Q3UrlOperator( url );
	}

	pendingOperations.append( urlOp->put( putData, to) );

	connect( urlOp, SIGNAL(finished(Q3NetworkOperation*)), SLOT(slotFinished_put(Q3NetworkOperation*)) );

	QTestEventLoop::instance().enterLoop( 30 );
	delete urlOp;
	if ( QTestEventLoop::instance().timeout() )
	    QFAIL( "Network operation timed out" );
    }
    {
	// In order to test that the put was really successful, we have to get the
	// file from the server.
	QFETCH( QString, getUrl );
	baData.resize( 0 );
	Q3UrlOperator opGet( getUrl );
	opGet.get();
	connect( &opGet, SIGNAL(finished(Q3NetworkOperation*)), SLOT(slotFinished_put(Q3NetworkOperation*)) );
	connect( &opGet, SIGNAL(data(const QByteArray&,Q3NetworkOperation*)), SLOT(slotData_put(const QByteArray&,Q3NetworkOperation*)) );
	QTestEventLoop::instance().enterLoop( 30 );
	if ( QTestEventLoop::instance().timeout() )
	    QFAIL( "Network operation timed out" );

	QTEST( baData, "putData" );
    }
    {
	// cleanup: delete file (if possible)
	QFETCH( QString, rmUrl );
	QFETCH( QString, rmFile );
	Q3UrlOperator opRemove( rmUrl );
        connect( &opRemove, SIGNAL(finished(Q3NetworkOperation*)), SLOT(slotFinished_put(Q3NetworkOperation*)) );
	if ( opRemove.remove( rmFile ) != 0 ) {
            QTestEventLoop::instance().enterLoop( 30 );
	    if ( QTestEventLoop::instance().timeout() )
		QFAIL( "Network operation timed out" );
	    QVERIFY( finishedState==Q3NetworkProtocol::StDone || finishedState==Q3NetworkProtocol::StFailed );
	    if ( finishedState != Q3NetworkProtocol::StDone ) {
		QFAIL( QString( "Can't remove file: network operation failed ('%1'). " ).arg(finishedProtocolDetail) );
	    }
	}
    }
}

void tst_Q3UrlOperator::slotFinished_put( Q3NetworkOperation *op )
{
#if defined( DUMP_SIGNALS )
    qDebug( "finished( %p )", op );
#endif
    QVERIFY( op != 0 );
    pendingOperations.remove( op );
    QVERIFY( pendingOperations.isEmpty() );

    finished = TRUE;
    finishedState = op->state();
    finishedErrorCode = op->errorCode();
    finishedProtocolDetail = op->protocolDetail();

    if (op->state() != Q3NetworkProtocol::StFailed)
        QTestEventLoop::instance().exitLoop();
}

void tst_Q3UrlOperator::slotData_put( const QByteArray& ba, Q3NetworkOperation *op )
{
#if defined( DUMP_SIGNALS )
    qDebug( "data( %d, %p )", ba.size(), op );
#endif
    QVERIFY( op != 0 );
    int oldSize = baData.size();
    baData.resize( ba.size() + oldSize );
    memcpy( baData.data()+oldSize, ba.data(), ba.size() );
}

void tst_Q3UrlOperator::rename_data()
{
    QTest::addColumn<QString>("url");
    QTest::addColumn<QString>("oldname");
    QTest::addColumn<QString>("newname");

    QTest::newRow( "local00" ) << QString(".") << QString("foo") << QString("bar");
}

void tst_Q3UrlOperator::rename()
{
    {
	// create direcotry first
	QFETCH( QString, url );
	QFETCH( QString, oldname );

	if ( url.isNull() ) {
	    urlOp = new Q3UrlOperator();
	} else {
	    urlOp = new Q3UrlOperator( url );
	}

	pendingOperations.append( urlOp->mkdir( oldname ) );

	connect( urlOp, SIGNAL(finished(Q3NetworkOperation*)), SLOT(slotFinished_rename(Q3NetworkOperation*)) );

	QTestEventLoop::instance().enterLoop( 30 );
	delete urlOp;
	if ( QTestEventLoop::instance().timeout() )
	    QFAIL( "Network operation timed out" );

	QVERIFY( finishedState == Q3NetworkProtocol::StDone );
    }
    {
	// rename directory
	QFETCH( QString, url );
	QFETCH( QString, oldname );
	QFETCH( QString, newname );

	if ( url.isNull() ) {
	    urlOp = new Q3UrlOperator();
	} else {
	    urlOp = new Q3UrlOperator( url );
	}

	pendingOperations.append( urlOp->rename( oldname, newname ) );

	connect( urlOp, SIGNAL(finished(Q3NetworkOperation*)), SLOT(slotFinished_rename(Q3NetworkOperation*)) );
	connect( urlOp, SIGNAL(itemChanged(Q3NetworkOperation*)), SLOT(slotItemChanged_rename(Q3NetworkOperation*)) );

	QTestEventLoop::instance().enterLoop( 30 );
	delete urlOp;
	if ( QTestEventLoop::instance().timeout() )
	    QFAIL( "Network operation timed out" );

	QVERIFY( finishedState == Q3NetworkProtocol::StDone );
    }
    {
	// delete direcotry
	QFETCH( QString, url );
	QFETCH( QString, newname );

	if ( url.isNull() ) {
	    urlOp = new Q3UrlOperator();
	} else {
	    urlOp = new Q3UrlOperator( url );
	}

	pendingOperations.append( urlOp->remove( newname ) );

	connect( urlOp, SIGNAL(finished(Q3NetworkOperation*)), SLOT(slotFinished_rename(Q3NetworkOperation*)) );

	QTestEventLoop::instance().enterLoop( 30 );
	delete urlOp;
	if ( QTestEventLoop::instance().timeout() )
	    QFAIL( "Network operation timed out" );

	QVERIFY( finishedState == Q3NetworkProtocol::StDone );
    }
}

void tst_Q3UrlOperator::slotFinished_rename( Q3NetworkOperation *op )
{
#if defined( DUMP_SIGNALS )
    qDebug( "finished( %p )", op );
#endif
    QVERIFY( op != 0 );
    pendingOperations.remove( op );
    QVERIFY( pendingOperations.isEmpty() );

    finished = TRUE;
    finishedState = op->state();
    finishedErrorCode = op->errorCode();
    finishedProtocolDetail = op->protocolDetail();

    QTestEventLoop::instance().exitLoop();
}

void tst_Q3UrlOperator::slotItemChanged_rename( Q3NetworkOperation *op )
{
#if defined( DUMP_SIGNALS )
    qDebug( "itemChanged( %p )", op );
#endif
    QVERIFY( op != 0 );
    // ### what to do here?
}

void tst_Q3UrlOperator::stop_data()
{
    QTest::addColumn<QString>("from");
    QTest::addColumn<QString>("to");
    QTest::addColumn<QString>("rmUrl");
    QTest::addColumn<QString>("rmFile");

    // FTP data
    QTest::newRow( "ftp01" )
	<< "stop/bigfile" << ftpQtestUpload
	<< ftpQtestUpload << "bigfile";
}

void tst_Q3UrlOperator::stop()
{
    doStop = TRUE;
    urlOp = new Q3UrlOperator();
    connect( urlOp, SIGNAL(finished(Q3NetworkOperation*)),
	    SLOT(slotFinished_stop(Q3NetworkOperation*)) );
    connect( urlOp, SIGNAL(dataTransferProgress(int,int,Q3NetworkOperation*)),
	    SLOT(slotDataTransferProgress_stop(int,int,Q3NetworkOperation*)) );
    {
	// do the upload
	QFETCH( QString, from );
	QFETCH( QString, to );
	pendingOperations = urlOp->copy( from, to );

	QTestEventLoop::instance().enterLoop( 30 );
	if ( QTestEventLoop::instance().timeout() )
	    QFAIL( "Network operation timed out" );
    }
    doStop = FALSE;
    {
	// do the upload again (this time without stop) -- this used to fail
	// until change 71380
	QFETCH( QString, from );
	QFETCH( QString, to );
	pendingOperations = urlOp->copy( from, to );

	QTestEventLoop::instance().enterLoop( 30 );
	if ( QTestEventLoop::instance().timeout() )
	    QFAIL( "Network operation timed out" );
    }
    delete urlOp;

    {
	// cleanup: delete file (if possible)
	QFETCH( QString, rmUrl );
	QFETCH( QString, rmFile );
	Q3UrlOperator opRemove( rmUrl );
	connect( &opRemove, SIGNAL(finished(Q3NetworkOperation*)), SLOT(slotFinished_stop(Q3NetworkOperation*)) );
	if ( opRemove.remove( rmFile ) != 0 ) {
	    QTestEventLoop::instance().enterLoop( 30 );
	    if ( QTestEventLoop::instance().timeout() )
		QFAIL( "Network operation timed out" );
	    QVERIFY( finishedState==Q3NetworkProtocol::StDone || finishedState==Q3NetworkProtocol::StFailed );
	    if ( finishedState != Q3NetworkProtocol::StDone )
		QFAIL( QString( "Can't remove file: network operation failed ('%1'). " ).arg(finishedProtocolDetail) );
	}
    }
}

void tst_Q3UrlOperator::slotFinished_stop( Q3NetworkOperation *op )
{
#if defined( DUMP_SIGNALS )
    qDebug( "finished( %p )", op );
#endif
    QVERIFY( op != 0 );

    pendingOperations.remove( op );
    // We are finished if either of this happens:
    // - both, the get and the put of the copy are finished
    // - we are in an error case (for the case that the get fails, no
    //   finished() signal for the put is emitted since it is never executed)
    if ( pendingOperations.isEmpty() || op->state()==Q3NetworkProtocol::StFailed ) {
	finished = TRUE;
	finishedState = op->state();
	finishedErrorCode = op->errorCode();
	finishedProtocolDetail = op->protocolDetail();

	QTestEventLoop::instance().exitLoop();
    }
}

void tst_Q3UrlOperator::slotDataTransferProgress_stop( int done, int total, Q3NetworkOperation *op )
{
#if defined( DUMP_SIGNALS )
    qDebug( "dataTransferProgress( %d, %d, %p )", done, total, op );
#endif
    QVERIFY( op != 0 );
    QVERIFY( done <= total );

    if ( QTest::currentTestFunction() == QLatin1String("stop") ) {
	// ### it would be nice if we could specify in our testdata when to do
	// the stop
	if ( doStop && pendingOperations.count()==1 ) {
	    if ( done > 0 && done >= total/100000 ) {
		// it is not safe to call stop() in a slot connected to the
		// dataTransferProgress() signal (and it is not trivial to make
		// it work)
		QTimer::singleShot( 0, this, SLOT(stopOperation()) );
		doStop = FALSE;
	    }
	}
    }
}

void tst_Q3UrlOperator::stopOperation()
{
    if ( urlOp )
	urlOp->stop();
}

void tst_Q3UrlOperator::listChildren_data()
{
    QTest::addColumn<QString>("url");
    QTest::addColumn<QString>("nameFilter");
    QTest::addColumn<bool>("readable");
    QTest::addColumn<bool>("writable");
    QTest::addColumn<bool>("executable");
    QTest::addColumn<int>("permissions");

    QTest::newRow( "localfs00" )
	<< QString("listData") << QString("readOnly")
	<< (bool)TRUE << (bool)FALSE << (bool)FALSE << 0444;
    QTest::newRow( "localfs01" )
	<< QString("listData") << QString("executable.exe")
	<< (bool)TRUE << (bool)FALSE << (bool)TRUE << 0555;
    int permissions = 0755;
#ifdef Q_OS_WIN
    permissions = 0777;
#endif
    QTest::newRow( "localfs02" )
	<< QString("listData") << QString("readWriteExec.exe")
	<< (bool)TRUE << (bool)TRUE << (bool)TRUE << permissions;
}

void tst_Q3UrlOperator::listChildren()
{
    QFETCH( QString, url );
    QFETCH( QString, nameFilter );
    urlOp = new Q3UrlOperator( "listData" );
    urlOp->setNameFilter( nameFilter );

    pendingOperations.append( urlOp->listChildren() );

    connect( urlOp, SIGNAL(finished(Q3NetworkOperation*)),
	    SLOT(slotFinished_listChildren(Q3NetworkOperation*)) );
    connect( urlOp, SIGNAL(newChildren(const Q3ValueList<QUrlInfo>&, Q3NetworkOperation*)),
	    SLOT(slotNewChildren_listChildren(const Q3ValueList<QUrlInfo>&, Q3NetworkOperation*)) );

    QTestEventLoop::instance().enterLoop( 30 );
    delete urlOp;
    if ( QTestEventLoop::instance().timeout() )
	QFAIL( "Network operation timed out" );

    QVERIFY( finishedState == Q3NetworkProtocol::StDone );
    for ( int i=0; i<(int)listChildrenInfo.count(); i++ ) {
	if ( listChildrenInfo[i].name() == "." || listChildrenInfo[i].name() == ".." )
	    continue;

	QFETCH( bool, readable );
	QFETCH( bool, writable );
	QFETCH( bool, executable );
	QFETCH( int, permissions );
	QCOMPARE( listChildrenInfo[i].isReadable(),   readable );
	QCOMPARE( listChildrenInfo[i].isWritable(),   writable );

	QCOMPARE( listChildrenInfo[i].isExecutable(), executable);
	QCOMPARE( listChildrenInfo[i].permissions(), permissions );
    }
}

void tst_Q3UrlOperator::slotFinished_listChildren( Q3NetworkOperation *op )
{
#if defined( DUMP_SIGNALS )
    qDebug( "finished( %p )", op );
#endif
    QVERIFY( op != 0 );
    pendingOperations.remove( op );
    QVERIFY( pendingOperations.isEmpty() );

    finished = TRUE;
    finishedState = op->state();
    finishedErrorCode = op->errorCode();
    finishedProtocolDetail = op->protocolDetail();

    QTestEventLoop::instance().exitLoop();
}

void tst_Q3UrlOperator::slotNewChildren_listChildren( const Q3ValueList<QUrlInfo> &i, Q3NetworkOperation * )
{
    listChildrenInfo = i;
}

QTEST_MAIN(tst_Q3UrlOperator)
#include "tst_q3urloperator.moc"

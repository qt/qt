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
#include <qapplication.h>
#include <q3dragobject.h>

//TESTED_FILES=
QT_FORWARD_DECLARE_CLASS(Q3UriDrag)

class tst_Q3UriDrag : public QObject
{
    Q_OBJECT
public:
    tst_Q3UriDrag();
    virtual ~tst_Q3UriDrag();

public slots:
    void initTestCase();
    void cleanupTestCase();
private slots:
    void decodeLocalFiles_data();
    void decodeLocalFiles();
    void decodeToUnicodeUris_data();
    void decodeToUnicodeUris();
    void uriToLocalFile_data();
    void uriToLocalFile();
    void localFileToUri_data();
    void localFileToUri();

private:
    Q3UriDrag *uriDrag;
};

tst_Q3UriDrag::tst_Q3UriDrag()
{
}

tst_Q3UriDrag::~tst_Q3UriDrag()
{
}

void tst_Q3UriDrag::initTestCase()
{
    uriDrag = new Q3UriDrag();
}

void tst_Q3UriDrag::cleanupTestCase()
{
    delete uriDrag;
    uriDrag = 0;
}

void tst_Q3UriDrag::decodeLocalFiles_data()
{
    QTest::addColumn<QStringList>("localFiles");
    QTest::addColumn<QStringList>("decodedFiles");

#ifdef Q_WS_WIN
    QTest::newRow("singleFileWithAbsPath") << QStringList("c:/main.cpp") << QStringList("c:/main.cpp");
#else
    QTest::newRow("singleFileWithAbsPath") << QStringList("/main.cpp") << QStringList("/main.cpp");
#endif

    QStringList multipleFilesWithAbsPaths;
#ifdef Q_WS_WIN
    multipleFilesWithAbsPaths << "c:/main.cpp" << "c:/home/test.cpp" << "e:/andy/quridrag.cpp" << "//somehost/somfile";
#else
    multipleFilesWithAbsPaths << "/main.cpp" << "/home/test.cpp" << "/andy/quridrag.cpp";
#endif
    QTest::newRow("multipleFilesWithAbsPaths") << multipleFilesWithAbsPaths << multipleFilesWithAbsPaths;

    QTest::newRow("nonLocalFile") << QStringList("http://qt.nokia.com") << QStringList();
    
    QStringList mixOfLocalAndNonLocalFiles;
#ifdef Q_WS_WIN
    mixOfLocalAndNonLocalFiles << "http://qt.nokia.com" << "c:/main.cpp" << "ftp://qt.nokia.com/doc";
    QTest::newRow("mixOfLocalAndNonLocalFiles") << mixOfLocalAndNonLocalFiles << QStringList("c:/main.cpp");
#else
	mixOfLocalAndNonLocalFiles << "http://qt.nokia.com" << "/main.cpp" << "ftp://qt.nokia.com/doc";
    QTest::newRow("mixOfLocalAndNonLocalFiles") << mixOfLocalAndNonLocalFiles << QStringList("/main.cpp");
#endif
}

void tst_Q3UriDrag::decodeLocalFiles()
{
    QFETCH(QStringList, localFiles);
    QFETCH(QStringList, decodedFiles);
    
    uriDrag->setFileNames(localFiles);
    
    QStringList decodeList;
    QVERIFY(Q3UriDrag::decodeLocalFiles(uriDrag, decodeList));

    QCOMPARE(decodeList.join(";;"), decodedFiles.join(";;"));
    // Need to add the unicodeUris test separately
}

void tst_Q3UriDrag::decodeToUnicodeUris_data()
{
    QTest::addColumn<QStringList>("unicodeUris");
    QTest::addColumn<QStringList>("decodedFiles");

#ifdef Q_WS_WIN
    QTest::newRow("singleFileWithAbsPath") << QStringList("c:/main.cpp") << QStringList("c:/main.cpp");
#else
    QTest::newRow("singleFileWithAbsPath") << QStringList("/main.cpp") << QStringList("/main.cpp");
#endif

    QStringList multipleFiles;
    QStringList multipleFilesUU;
#ifdef Q_WS_WIN
    multipleFiles << "c:/main.cpp" << "c:/home/with space test.cpp" << "e:/andy/~quridrag.cpp";
    multipleFilesUU << "c:/main.cpp" << "c:/home/with space test.cpp" << "e:/andy/~quridrag.cpp";
#else
	multipleFiles << "/main.cpp" << "/home/with space test.cpp" << "/andy/~quridrag.cpp";
    multipleFilesUU << "/main.cpp" << "/home/with space test.cpp" << "/andy/~quridrag.cpp";
#endif
    QTest::newRow("multipleFiles") << multipleFiles << multipleFilesUU;

    QTest::newRow("nonLocalUris") << QStringList("http://qt.nokia.com") << QStringList("http://qt.nokia.com");
    
    QStringList mixOfLocalAndNonLocalUris;
	QStringList mixOfLocalAndNonLocalUrisUU;
#ifdef Q_WS_WIN
    mixOfLocalAndNonLocalUris << "http://qt.nokia.com" << "c:/with space main.cpp" << "ftp://qt.nokia.com/doc";
    mixOfLocalAndNonLocalUrisUU << "http://qt.nokia.com" << "c:/with space main.cpp" << "ftp://qt.nokia.com/doc";
#else
	mixOfLocalAndNonLocalUris << "http://qt.nokia.com" << "/main.cpp" << "ftp://qt.nokia.com/doc";
    mixOfLocalAndNonLocalUrisUU << "http://qt.nokia.com" << "/main.cpp" << "ftp://qt.nokia.com/doc";
#endif
	QTest::newRow("mixOfLocalAndNonLocalUris") << mixOfLocalAndNonLocalUris << mixOfLocalAndNonLocalUrisUU;
}

void tst_Q3UriDrag::decodeToUnicodeUris()
{
    // RFC 2396 used for reference
    //
    // Possible AbsoluteURIs are:
    //
    //	    http://qt.nokia.com
    //	    c:/main.cpp
    //

    QFETCH(QStringList, unicodeUris);
    QFETCH(QStringList, decodedFiles);
    
    uriDrag->setUnicodeUris(unicodeUris);
    
    QStringList decodeList;
    QVERIFY(Q3UriDrag::decodeToUnicodeUris(uriDrag, decodeList));
    QCOMPARE(decodeList.join(";;"), decodedFiles.join(";;"));

    // Need to test setFileNames separately
}

void tst_Q3UriDrag::uriToLocalFile_data()
{
    QTest::addColumn<QString>("uri");
    QTest::addColumn<QString>("localFile");

    //QTest::newRow("localFileNoEncoding") << QString("main.cpp") << QString("main.cpp");
    //QTest::newRow("localFileAbsPathNoEnc") << QString("c:/main.cpp") << QString("c:/main.cpp");
    //QTest::newRow("localFileAbsPathNoEnc-2") << QString("/main.cpp") << QString("/main.cpp");
    //QTest::newRow("localFileEncoding") << QString("Fran%c3%a7ois") << QString::fromUtf8("François");
    //QTest::newRow("localFileAbsPathEnc") << QString("c:/main.cpp") << QString("c:/main.cpp");
    //QTest::newRow("localFileAbsPathEnc-2") << QString("/main.cpp") << QString("/main.cpp");
    QTest::newRow("fileSchemelocalFileNoEncoding") << QString("file:///main.cpp") << QString("/main.cpp");
#ifdef Q_WS_WIN
    QTest::newRow("fileSchemelocalFileAbsPathNoEnc") << QString("file:///c:/main.cpp") << QString("c:/main.cpp");
    QTest::newRow("fileSchemelocalFileWindowsNetworkPath") << QString("file://somehost/somefile") << QString("//somehost/somefile");
#endif
    QTest::newRow("fileSchemelocalFileEncoding") << QString("file:///Fran%e7ois") << QString::fromUtf8("/François");
    QTest::newRow("nonLocalFile") << QString("http://qt.nokia.com") << QString();
}

void tst_Q3UriDrag::uriToLocalFile()
{
    QFETCH(QString, uri);
    QFETCH(QString, localFile);
    QCOMPARE(Q3UriDrag::uriToLocalFile(uri.utf8()), localFile);
}


void tst_Q3UriDrag::localFileToUri_data()
{
    QTest::addColumn<QString>("localFile");
    QTest::addColumn<QString>("uri");
#ifdef Q_WS_WIN
    QTest::newRow("fileSchemelocalFileWindowsNetworkPath") << QString("//somehost/somefile") << QString("file://somehost/somefile");
    QTest::newRow("hash in path") << QString("c:/tmp/p#d") << QString("file:///c:/tmp/p%23d");
#else
    QTest::newRow("fileSchemelocalFile") << QString("/somehost/somefile") << QString("file:///somehost/somefile");
    QTest::newRow("hash in path") << QString("/tmp/p#d") << QString("file:///tmp/p%23d");
#endif
    
}

void tst_Q3UriDrag::localFileToUri()
{
    QFETCH(QString, localFile);
    QFETCH(QString, uri);
    
    QCOMPARE(Q3UriDrag::localFileToUri(localFile), uri.toUtf8());
}


QTEST_MAIN(tst_Q3UriDrag)
#include "tst_q3uridrag.moc"

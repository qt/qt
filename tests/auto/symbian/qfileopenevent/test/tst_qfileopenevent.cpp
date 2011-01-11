/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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
#include <QEvent>

#ifdef Q_OS_SYMBIAN
#include <apgcli.h>
#include "private/qcore_symbian_p.h"

class tst_qfileopenevent : public QObject
{
    Q_OBJECT
public:
    tst_qfileopenevent(){}
    ~tst_qfileopenevent();

public slots:
    void initTestCase();

private slots:
    void constructor();
    void fileOpen();
    void handleLifetime();
    void multiOpen();
    void sendAndReceive();
    void external_data();
    void external();

private:
    RFile createRFile(const TDesC& filename, const TDesC8& content);
    void checkReadAndWrite(QFileOpenEvent& event, const QString& readContent, const QString& writeContent, bool writeOk);
    QString readFileContent(QFileOpenEvent& event);
    bool appendFileContent(QFileOpenEvent& event, const QString& writeContent);

    bool event(QEvent *);

private:
    RFs fsSession;
};

tst_qfileopenevent::~tst_qfileopenevent()
{
    fsSession.Close();
};

void tst_qfileopenevent::initTestCase()
{
    qt_symbian_throwIfError(fsSession.Connect());
    qt_symbian_throwIfError(fsSession.ShareProtected());
}

RFile tst_qfileopenevent::createRFile(const TDesC& filename, const TDesC8& content)
{
    RFile file;
    qt_symbian_throwIfError(file.Replace(fsSession, filename, EFileWrite));
    qt_symbian_throwIfError(file.Write(content));
    return file;
}

void tst_qfileopenevent::constructor()
{
    // check that filename get/set works
    QFileOpenEvent nameTest(QLatin1String("fileNameTest"));
    QCOMPARE(nameTest.file(), QLatin1String("fileNameTest"));

    // check that url get/set works
    QFileOpenEvent urlTest(QUrl(QLatin1String("file:///urlNameTest")));
    QCOMPARE(urlTest.url().toString(), QLatin1String("file:///urlNameTest"));

    // check that RFile construction works
    RFile rFile = createRFile(_L("testRFile"), _L8("test content"));
    QFileOpenEvent rFileTest(rFile);
    QString targetName(QLatin1String("testRFile"));
    QCOMPARE(rFileTest.file().right(targetName.size()), targetName);
    QCOMPARE(rFileTest.url().toString().right(targetName.size()), targetName);
    rFile.Close();
}

QString tst_qfileopenevent::readFileContent(QFileOpenEvent& event)
{
    QFile file;
    event.openFile(file, QFile::ReadOnly);
    file.seek(0);
    QByteArray data = file.readAll();
    return QString(data);
}

bool tst_qfileopenevent::appendFileContent(QFileOpenEvent& event, const QString& writeContent)
{
    QFile file;
    bool ok = event.openFile(file, QFile::Append | QFile::Unbuffered);
    if (ok)
        ok = file.write(writeContent.toUtf8()) == writeContent.size();
    return ok;
}

void tst_qfileopenevent::checkReadAndWrite(QFileOpenEvent& event, const QString& readContent, const QString& writeContent, bool writeOk)
{
    QCOMPARE(readFileContent(event), readContent);
    QCOMPARE(appendFileContent(event, writeContent), writeOk);
    QCOMPARE(readFileContent(event), writeOk ? readContent+writeContent : readContent);
}

void tst_qfileopenevent::fileOpen()
{
    // create writeable file
    {
        RFile rFile = createRFile(_L("testFileOpen"), _L8("test content"));
        QFileOpenEvent rFileTest(rFile);
        checkReadAndWrite(rFileTest, QLatin1String("test content"), QLatin1String("+RFileWrite"), true);
        rFile.Close();
    }

    // open read-only RFile
    {
        RFile rFile;
        int err = rFile.Open(fsSession, _L("testFileOpen"), EFileRead);
        QFileOpenEvent rFileTest(rFile);
        checkReadAndWrite(rFileTest, QLatin1String("test content+RFileWrite"), QLatin1String("+RFileRead"), false);
        rFile.Close();
    }

    // filename event
    QUrl fileUrl;   // need to get the URL during the file test, for use in the URL test
    {
        QFileOpenEvent nameTest(QLatin1String("testFileOpen"));
        fileUrl = nameTest.url();
        checkReadAndWrite(nameTest, QLatin1String("test content+RFileWrite"), QLatin1String("+nameWrite"), true);
    }

    // url event
    {
        QFileOpenEvent urlTest(fileUrl);
        checkReadAndWrite(urlTest, QLatin1String("test content+RFileWrite+nameWrite"), QLatin1String("+urlWrite"), true);
    }
}

void tst_qfileopenevent::handleLifetime()
{
    RFile rFile = createRFile(_L("testHandleLifetime"), _L8("test content"));
    QScopedPointer<QFileOpenEvent> event(new QFileOpenEvent(rFile));
    rFile.Close();

    // open a QFile after the original RFile is closed
    QFile qFile;
    QCOMPARE(event->openFile(qFile, QFile::Append | QFile::Unbuffered), true);
    event.reset(0);

    // write to the QFile after the event is closed
    QString writeContent(QLatin1String("+closed original handles"));
    QCOMPARE(int(qFile.write(writeContent.toUtf8())), writeContent.size());
    qFile.close();

    // check the content
    QFile check("testHandleLifetime");
    check.open(QFile::ReadOnly);
    QString content(check.readAll());
    QCOMPARE(content, QLatin1String("test content+closed original handles"));
}

void tst_qfileopenevent::multiOpen()
{
    RFile rFile = createRFile(_L("testMultiOpen"), _L8("itlum"));
    QFileOpenEvent event(rFile);
    rFile.Close();

    QFile files[5];
    for (int i=0; i<5; i++) {
        QCOMPARE(event.openFile(files[i], QFile::ReadOnly), true);
    }
    for (int i=0; i<5; i++)
        files[i].seek(i);
    QString str;
    for (int i=4; i>=0; i--) {
        char c;
        files[i].getChar(&c);
        str.append(c);
        files[i].close();
    }
    QCOMPARE(str, QLatin1String("multi"));
}

bool tst_qfileopenevent::event(QEvent *event)
{
    if (event->type() != QEvent::FileOpen)
        return QObject::event(event);
    QFileOpenEvent* fileOpenEvent = static_cast<QFileOpenEvent *>(event);
    appendFileContent(*fileOpenEvent, "+received");
    return true;
}

void tst_qfileopenevent::sendAndReceive()
{
    RFile rFile = createRFile(_L("testSendAndReceive"), _L8("sending"));
    QFileOpenEvent* event = new QFileOpenEvent(rFile);
    rFile.Close();
    QCoreApplication::instance()->postEvent(this, event);
    QCoreApplication::instance()->processEvents();

    // check the content
    QFile check("testSendAndReceive");
    QCOMPARE(check.open(QFile::ReadOnly), true);
    QString content(check.readAll());
    QCOMPARE(content, QLatin1String("sending+received"));
}

void tst_qfileopenevent::external_data()
{
    QTest::addColumn<QString>("filename");
    QTest::addColumn<QByteArray>("targetContent");
    QTest::addColumn<bool>("sendHandle");

    QString privateName(QLatin1String("tst_qfileopenevent_external"));
    QString publicName(QLatin1String("C:\\Data\\tst_qfileopenevent_external"));
    QByteArray writeSuccess("original+external");
    QByteArray writeFail("original");
    QTest::newRow("public name") << publicName << writeSuccess << false;
    QTest::newRow("data caged name") << privateName << writeFail << false;
    QTest::newRow("public handle") << publicName << writeSuccess << true;
    QTest::newRow("data caged handle") << privateName << writeSuccess << true;
}

void tst_qfileopenevent::external()
{
    QFETCH(QString, filename);
    QFETCH(QByteArray, targetContent);
    QFETCH(bool, sendHandle);

    RFile rFile = createRFile(qt_QString2TPtrC(filename), _L8("original"));

    // launch app with the file
    RApaLsSession apa;
    QCOMPARE(apa.Connect(), KErrNone);
    TThreadId threadId;
    TDataType type(_L8("application/x-tst_qfileopenevent"));
    if (sendHandle) {
        QCOMPARE(apa.StartDocument(rFile, type, threadId), KErrNone);
        rFile.Close();
    } else {
        TFileName fullName;
        rFile.FullName(fullName);
        rFile.Close();
        QCOMPARE(apa.StartDocument(fullName, type, threadId), KErrNone);
    }

    // wait for app exit
    RThread appThread;
    if (appThread.Open(threadId) == KErrNone) {
        TRequestStatus status;
        appThread.Logon(status);
        User::WaitForRequest(status);
    }

    // check the contents
    QFile check(filename);
    QCOMPARE(check.open(QFile::ReadOnly), true);
    QCOMPARE(check.readAll(), targetContent);
    bool ok = check.remove();
}

QTEST_MAIN(tst_qfileopenevent)
#include "tst_qfileopenevent.moc"
#else
QTEST_NOOP_MAIN
#endif

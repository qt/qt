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
    void viaApparc();
    void viasDocHandler();

private:
    RFile createRFile(const TDesC& filename, const TDesC8& content);
    void checkReadAndWrite(QFileOpenEvent& event, const QString& readContent, const QString& writeContent, bool writeOk);
    QString readFileContent(QFileOpenEvent& event);
    bool appendFileContent(QFileOpenEvent& event, const QString& writeContent);

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
        QFileOpenEvent rFileTest2(rFile);
        checkReadAndWrite(rFileTest2, QLatin1String("test content+RFileWrite"), QLatin1String("+RFileRead"), false);
        rFile.Close();
    }
    
    // test it with read and write
    // filename event
    // test it with read and write
    // url event
    // test it with read and write
}

void tst_qfileopenevent::handleLifetime()
{
}

void tst_qfileopenevent::multiOpen()
{
}

void tst_qfileopenevent::sendAndReceive()
{
}

void tst_qfileopenevent::viaApparc()
{
}

void tst_qfileopenevent::viasDocHandler()
{
}

QTEST_MAIN(tst_qfileopenevent)
#include "tst_qfileopenevent.moc"
#else
QTEST_NOOP_MAIN
#endif

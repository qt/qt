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

#include <QtCore/private/qfilesystementry_p.h>

#if defined(Q_OS_WIN) || defined(Q_OS_SYMBIAN)
#   define WIN_STUFF
#endif

//TESTED_CLASS=
//TESTED_FILES=

class tst_QFileSystemEntry : public QObject
{
    Q_OBJECT

private slots:
    void getSetCheck_data();
    void getSetCheck();
};

void tst_QFileSystemEntry::getSetCheck_data()
{
    QTest::addColumn<QByteArray>("nativeFilePath");
    QTest::addColumn<QString>("filepath");
    QTest::addColumn<QString>("filename");
    QTest::addColumn<QString>("suffix");
    QTest::addColumn<QString>("completeSuffix");
    QTest::addColumn<bool>("absolute");

    QTest::newRow("simple")
#if defined(WIN_STUFF)
        << QByteArray("A:\\home\\qt\\in\\a\\dir.tar.gz")
        << "A:/home/qt/in/a/dir.tar.gz"
#else
        << QByteArray("/home/qt/in/a/dir.tar.gz")
        << "/home/qt/in/a/dir.tar.gz"
#endif
        << "dir.tar.gz" << "gz" << "tar.gz" << true;

    QTest::newRow("relative")
#if defined(WIN_STUFF)
        << QByteArray("in\\a\\dir.tar.gz")
        << "in/a/dir.tar.gz"
#else
        << QByteArray("in/a/dir.tar.gz")
        << "in/a/dir.tar.gz"
#endif
        << "dir.tar.gz" << "gz" << "tar.gz" << false;

    QTest::newRow("noSuffix")
#if defined(WIN_STUFF)
        << QByteArray("myDir\\myfile")
#else
        << QByteArray("myDir/myfile")
#endif
        << "myDir/myfile" << "myfile" << "" << "" << false;

    QTest::newRow("noLongSuffix")
#if defined(WIN_STUFF)
        << QByteArray("myDir\\myfile.txt")
#else
        << QByteArray("myDir/myfile.txt")
#endif
        << "myDir/myfile.txt" << "myfile.txt" << "txt" << "txt" << false;

    QTest::newRow("endingSlash")
#if defined(WIN_STUFF)
        << QByteArray("myDir\\myfile.bla\\")
#else
        << QByteArray("myDir/myfile.bla/")
#endif
        << "myDir/myfile.bla/" << "" << "" << "" << false;

#if defined(WIN_STUFF)
    QTest::newRow("absolutePath")
        << QByteArray("A:dir\\without\\leading\\backslash.bat")
        << "A:dir/without/leading/backslash.bat" << "backslash.bat" << "bat" << "bat" << true;
#else
    QTest::newRow("relativePath")
        << QByteArray("A:dir/without/leading/backslash.bat")
        << "A:dir/without/leading/backslash.bat" << "backslash.bat" << "bat" << "bat" << false;
#endif
}

void tst_QFileSystemEntry::getSetCheck()
{
    QFETCH(QByteArray, nativeFilePath);
    QFETCH(QString, filepath);
    QFETCH(QString, filename);
    QFETCH(QString, suffix);
    QFETCH(QString, completeSuffix);
    QFETCH(bool, absolute);

    QFileSystemEntry entry1(filepath);
    QCOMPARE(entry1.filePath(), filepath);
    QCOMPARE(entry1.nativeFilePath(), nativeFilePath);
    QCOMPARE(entry1.fileName(), filename);
    QCOMPARE(entry1.suffix(), suffix);
    QCOMPARE(entry1.completeSuffix(), completeSuffix);
    QCOMPARE(entry1.isAbsolute(), absolute);
    QCOMPARE(entry1.isRelative(), !absolute);

    QFileSystemEntry entry2(nativeFilePath);
    QCOMPARE(entry2.suffix(), suffix);
    QCOMPARE(entry2.completeSuffix(), completeSuffix);
    QCOMPARE(entry2.isAbsolute(), absolute);
    QCOMPARE(entry2.isRelative(), !absolute);
    QCOMPARE(entry2.filePath(), filepath);
    QCOMPARE(entry2.nativeFilePath(), nativeFilePath);
    QCOMPARE(entry2.fileName(), filename);
}

QTEST_MAIN(tst_QFileSystemEntry)
#include <tst_qfilesystementry.moc>

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

#if defined(WIN_STUFF)
void tst_QFileSystemEntry::getSetCheck_data()
{
    QTest::addColumn<QString>("nativeFilePath");
    QTest::addColumn<QString>("internalnativeFilePath");
    QTest::addColumn<QString>("filepath");
    QTest::addColumn<QString>("filename");
    QTest::addColumn<QString>("baseName");
    QTest::addColumn<QString>("completeBasename");
    QTest::addColumn<QString>("suffix");
    QTest::addColumn<QString>("completeSuffix");
    QTest::addColumn<bool>("absolute");

    QString absPrefix = QLatin1String("\\\\?\\");
    QString relPrefix = absPrefix
                + QDir::toNativeSeparators(QDir::currentPath())
                + QLatin1String("\\");

    QTest::newRow("simple")
            << QString("A:\\home\\qt\\in\\a\\dir.tar.gz")
            << absPrefix +  QString("A:\\home\\qt\\in\\a\\dir.tar.gz")
            << "A:/home/qt/in/a/dir.tar.gz"
            << "dir.tar.gz" << "dir" << "dir.tar" << "gz" << "tar.gz" << true;

    QTest::newRow("relative")
            << QString("in\\a\\dir.tar.gz")
            << relPrefix +  QString("in\\a\\dir.tar.gz")
            << "in/a/dir.tar.gz"
            << "dir.tar.gz" << "dir" << "dir.tar" << "gz" << "tar.gz" << false;

    QTest::newRow("noSuffix")
            << QString("myDir\\myfile")
            << relPrefix + QString("myDir\\myfile")
            << "myDir/myfile" << "myfile" << "myfile" << "myfile" << "" << "" << false;

    QTest::newRow("noLongSuffix")
            << QString("myDir\\myfile.txt")
            << relPrefix + QString("myDir\\myfile.txt")
            << "myDir/myfile.txt" << "myfile.txt" << "myfile" << "myfile" << "txt" << "txt" << false;

    QTest::newRow("endingSlash")
            << QString("myDir\\myfile.bla\\")
            << relPrefix + QString("myDir\\myfile.bla\\")
            << "myDir/myfile.bla/" << "" << "" << "" << "" << "" << false;

    QTest::newRow("absolutePath")
            << QString("A:dir\\without\\leading\\backslash.bat")
            << absPrefix + QString("A:\\dir\\without\\leading\\backslash.bat")
            << "A:dir/without/leading/backslash.bat" << "backslash.bat" << << "backslash" << "backslash" << "bat" << "bat" << true;
}

void tst_QFileSystemEntry::getSetCheck()
{
    QFETCH(QString, nativeFilePath);
    QFETCH(QString, internalnativeFilePath);
    QFETCH(QString, filepath);
    QFETCH(QString, filename);
    QFETCH(QString, basename);
    QFETCH(QString, completeBasename);
    QFETCH(QString, suffix);
    QFETCH(QString, completeSuffix);
    QFETCH(bool, absolute);

    QFileSystemEntry entry1(filepath);
    QCOMPARE(entry1.filePath(), filepath);
    QCOMPARE(entry1.nativeFilePath().toLower(), internalnativeFilePath.toLower());
    QCOMPARE(entry1.fileName(), filename);
    QCOMPARE(entry1.suffix(), suffix);
    QCOMPARE(entry1.completeSuffix(), completeSuffix);
    QCOMPARE(entry1.isAbsolute(), absolute);
    QCOMPARE(entry1.isRelative(), !absolute);
    QCOMPARE(entry1.baseName(), basename);
    QCOMPARE(entry1.completeBaseName(), completeBasename);

    QFileSystemEntry entry2(nativeFilePath, QFileSystemEntry::FromNativePath());
    QCOMPARE(entry2.suffix(), suffix);
    QCOMPARE(entry2.completeSuffix(), completeSuffix);
    QCOMPARE(entry2.isAbsolute(), absolute);
    QCOMPARE(entry2.isRelative(), !absolute);
    QCOMPARE(entry2.filePath(), filepath);
    // Since this entry was created using the native path,
    // the object shouldnot change nativeFilePath.
    QCOMPARE(entry2.nativeFilePath(), nativeFilePath);
    QCOMPARE(entry2.fileName(), filename);
    QCOMPARE(entry2.baseName(), basename);
    QCOMPARE(entry2.completeBaseName(), completeBasename);
}

#else

void tst_QFileSystemEntry::getSetCheck_data()
{
    QTest::addColumn<QByteArray>("nativeFilePath");
    QTest::addColumn<QString>("filepath");
    QTest::addColumn<QString>("filename");
    QTest::addColumn<QString>("basename");
    QTest::addColumn<QString>("completeBasename");
    QTest::addColumn<QString>("suffix");
    QTest::addColumn<QString>("completeSuffix");
    QTest::addColumn<bool>("absolute");

    QTest::newRow("simple")
        << QByteArray("/home/qt/in/a/dir.tar.gz")
        << "/home/qt/in/a/dir.tar.gz"
        << "dir.tar.gz" << "dir" << "dir.tar" << "gz" << "tar.gz" << true;
    QTest::newRow("relative")
        << QByteArray("in/a/dir.tar.gz")
        << "in/a/dir.tar.gz"
        << "dir.tar.gz" << "dir" << "dir.tar" << "gz" << "tar.gz" << false;

    QTest::newRow("noSuffix")
        << QByteArray("myDir/myfile")
        << "myDir/myfile" << "myfile" << "myfile" << "myfile" << "" << "" << false;

    QTest::newRow("noLongSuffix")
        << QByteArray("myDir/myfile.txt")
        << "myDir/myfile.txt" << "myfile.txt" << "myfile" << "myfile" << "txt" << "txt" << false;

    QTest::newRow("endingSlash")
        << QByteArray("myDir/myfile.bla/")
        << "myDir/myfile.bla/" << "" << "" << "" << "" << "" << false;

    QTest::newRow("relativePath")
        << QByteArray("A:dir/without/leading/backslash.bat")
        << "A:dir/without/leading/backslash.bat" << "backslash.bat" << "backslash" << "backslash" << "bat" << "bat" << false;
}

void tst_QFileSystemEntry::getSetCheck()
{
    QFETCH(QByteArray, nativeFilePath);
    QFETCH(QString, filepath);
    QFETCH(QString, filename);
    QFETCH(QString, basename);
    QFETCH(QString, completeBasename);
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
    QCOMPARE(entry1.baseName(), basename);
    QCOMPARE(entry1.completeBaseName(), completeBasename);

    QFileSystemEntry entry2(nativeFilePath, QFileSystemEntry::FromNativePath());
    QCOMPARE(entry2.suffix(), suffix);
    QCOMPARE(entry2.completeSuffix(), completeSuffix);
    QCOMPARE(entry2.isAbsolute(), absolute);
    QCOMPARE(entry2.isRelative(), !absolute);
    QCOMPARE(entry2.filePath(), filepath);
    QCOMPARE(entry2.nativeFilePath(), nativeFilePath);
    QCOMPARE(entry2.fileName(), filename);
    QCOMPARE(entry2.baseName(), basename);
    QCOMPARE(entry2.completeBaseName(), completeBasename);
}
#endif

QTEST_MAIN(tst_QFileSystemEntry)
#include <tst_qfilesystementry.moc>

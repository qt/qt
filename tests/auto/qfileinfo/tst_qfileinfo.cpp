/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QtTest/QtTest>


#include <qfile.h>
#include <qdir.h>
#include <qcoreapplication.h>
#include <qtemporaryfile.h>
#include <qdir.h>
#include <qfileinfo.h>
#ifdef Q_OS_UNIX
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#endif
#ifdef Q_OS_WIN
#include <qt_windows.h>
#endif
#include <qplatformdefs.h>
#include <qdebug.h>

#include "../network-settings.h"
#include <private/qfileinfo_p.h>

//TESTED_CLASS=
//TESTED_FILES=

class tst_QFileInfo : public QObject
{
Q_OBJECT

public:
    tst_QFileInfo();
    ~tst_QFileInfo();

private slots:
    void getSetCheck();

    void copy();

    void isFile_data();
    void isFile();

    void isDir_data();
    void isDir();

    void isRoot_data();
    void isRoot();

    void exists_data();
    void exists();

    void absolutePath_data();
    void absolutePath();

    void absFilePath_data();
    void absFilePath();

    void canonicalPath();
    void canonicalFilePath();

    void fileName_data();
    void fileName();

    void bundleName_data();
    void bundleName();

    void dir_data();
    void dir();

    void suffix_data();
    void suffix();

    void completeSuffix_data();
    void completeSuffix();

    void baseName_data();
    void baseName();

    void completeBaseName_data();
    void completeBaseName();

    void permission_data();
    void permission();

    void size_data();
    void size();

    void systemFiles();

    void compare_data();
    void compare();

    void consistent_data();
    void consistent();

    void fileTimes_data();
    void fileTimes();
    void fileTimes_oldFile();

    void isSymLink();

    void isHidden_data();
    void isHidden();

    void isBundle_data();
    void isBundle();

    void refresh();

#if defined(Q_OS_WIN) && !defined(Q_OS_WINCE)
    void brokenShortcut();
#endif

#ifdef Q_OS_UNIX
    void isWritable();
#endif
    void isExecutable();
    void testDecomposedUnicodeNames_data();
    void testDecomposedUnicodeNames();

    void equalOperator() const;
    void equalOperatorWithDifferentSlashes() const;
    void notEqualOperator() const;
};

tst_QFileInfo::~tst_QFileInfo()
{
    QFile::remove("brokenlink.lnk");
    QFile::remove("link.lnk");
    QFile::remove("file1");
}

// Testing get/set functions
void tst_QFileInfo::getSetCheck()
{
    QFileInfo obj1;
    // bool QFileInfo::caching()
    // void QFileInfo::setCaching(bool)
    obj1.setCaching(false);
    QCOMPARE(false, obj1.caching());
    obj1.setCaching(true);
    QCOMPARE(true, obj1.caching());
}

static QFileInfoPrivate* getPrivate(QFileInfo &info)
{
    return (*reinterpret_cast<QFileInfoPrivate**>(&info));
}

void tst_QFileInfo::copy()
{
    QTemporaryFile *t;
    t = new QTemporaryFile;
    t->open();
    QFileInfo info(t->fileName());
    QVERIFY(info.exists());

    //copy constructor
    QFileInfo info2(info);
    QFileInfoPrivate *privateInfo = getPrivate(info);
    QFileInfoPrivate *privateInfo2 = getPrivate(info2);
    QCOMPARE(privateInfo->data, privateInfo2->data);

    //operator =
    QFileInfo info3 = info;
    QFileInfoPrivate *privateInfo3 = getPrivate(info3);
    QCOMPARE(privateInfo->data, privateInfo3->data);
    QCOMPARE(privateInfo2->data, privateInfo3->data);

    //refreshing info3 will detach it
    QFile file(info.absoluteFilePath());
    QVERIFY(file.open(QFile::WriteOnly));
    QCOMPARE(file.write("JAJAJAA"), qint64(7));
    file.flush();

    QTest::qWait(250);
#if defined(Q_OS_WIN) || defined(Q_OS_WINCE)
    if (QSysInfo::windowsVersion() & QSysInfo::WV_VISTA ||
                QSysInfo::windowsVersion() & QSysInfo::WV_CE_based)
        file.close();
#endif
#if defined(Q_OS_WINCE)
    // On Windows CE we need to close the file.
    // Otherwise the content will be cached and not
    // flushed to the storage, although we flushed it
    // manually!!! CE has interim cache, we cannot influence.
    QTest::qWait(5000);
#endif
    info3.refresh();
    QVERIFY(privateInfo->data != privateInfo3->data);
    QVERIFY(privateInfo2->data != privateInfo3->data);
    QCOMPARE(privateInfo->data, privateInfo2->data);


}

tst_QFileInfo::tst_QFileInfo()
{
}

void tst_QFileInfo::isFile_data()
{
    QTest::addColumn<QString>("path");
    QTest::addColumn<bool>("expected");

    QTest::newRow("data0") << QDir::currentPath() << false;
    QTest::newRow("data1") << "tst_qfileinfo.cpp" << true;
    QTest::newRow("data2") << ":/tst_qfileinfo/resources/" << false;
    QTest::newRow("data3") << ":/tst_qfileinfo/resources/file1" << true;
}

void tst_QFileInfo::isFile()
{
    QFETCH(QString, path);
    QFETCH(bool, expected);

    QFileInfo fi(path);
    QCOMPARE(fi.isFile(), expected);
}


void tst_QFileInfo::isDir_data()
{
    // create a broken symlink
    QFile::remove("brokenlink.lnk");
    QFile file3("dummyfile");
    file3.open(QIODevice::WriteOnly);
    if (file3.link("brokenlink.lnk")) {
        file3.remove();
        QFileInfo info3("brokenlink.lnk");
        QVERIFY( info3.isSymLink() );
    }

    QTest::addColumn<QString>("path");
    QTest::addColumn<bool>("expected");

    QTest::newRow("data0") << QDir::currentPath() << true;
    QTest::newRow("data1") << "tst_qfileinfo.cpp" << false;
    QTest::newRow("data2") << ":/tst_qfileinfo/resources/" << true;
    QTest::newRow("data3") << ":/tst_qfileinfo/resources/file1" << false;

    QTest::newRow("simple dir") << "resources" << true;
    QTest::newRow("simple dir with slash") << "resources/" << true;

    QTest::newRow("broken link") << "brokenlink.lnk" << false;

#if defined(Q_OS_WIN) && !defined(Q_OS_WINCE)
    QTest::newRow("drive 1") << "c:" << true;
    QTest::newRow("drive 2") << "c:/" << true;
    //QTest::newRow("drive 2") << "t:s" << false;
    QTest::newRow("unc 1") << "//"  + QtNetworkSettings::winServerName() << true;
    QTest::newRow("unc 2") << "//"  + QtNetworkSettings::winServerName() + "/" << true;
    QTest::newRow("unc 3") << "//"  + QtNetworkSettings::winServerName() + "/testshare" << true;
    QTest::newRow("unc 4") << "//"  + QtNetworkSettings::winServerName() + "/testshare/" << true;
    QTest::newRow("unc 5") << "//"  + QtNetworkSettings::winServerName() + "/testshare/tmp" << true;
    QTest::newRow("unc 6") << "//"  + QtNetworkSettings::winServerName() + "/testshare/tmp/" << true;
    QTest::newRow("unc 7") << "//"  + QtNetworkSettings::winServerName() + "/testshare/adirthatshouldnotexist" << false;
#endif
}

void tst_QFileInfo::isDir()
{
    QFETCH(QString, path);
    QFETCH(bool, expected);

    QFileInfo fi(path);
    QCOMPARE(fi.isDir(), expected);
}

void tst_QFileInfo::isRoot_data()
{
    QTest::addColumn<QString>("path");
    QTest::addColumn<bool>("expected");

    QTest::newRow("data0") << QDir::currentPath() << false;
    QTest::newRow("data1") << "/" << true;
    QTest::newRow("data2") << ":/tst_qfileinfo/resources/" << false;
    QTest::newRow("data3") << ":/" << true;

    QTest::newRow("simple dir") << "resources" << false;
    QTest::newRow("simple dir with slash") << "resources/" << false;

#if defined(Q_OS_WIN) && !defined(Q_OS_WINCE)
    QTest::newRow("drive 1") << "c:" << false;
    QTest::newRow("drive 2") << "c:/" << true;
    QTest::newRow("unc 1") << "//"  + QtNetworkSettings::winServerName() << true;
    QTest::newRow("unc 2") << "//"  + QtNetworkSettings::winServerName() + "/" << true;
    QTest::newRow("unc 3") << "//"  + QtNetworkSettings::winServerName() + "/testshare" << false;
    QTest::newRow("unc 4") << "//"  + QtNetworkSettings::winServerName() + "/testshare/" << false;
    QTest::newRow("unc 7") << "//ahostthatshouldnotexist" << false;
#endif
}

void tst_QFileInfo::isRoot()
{
    QFETCH(QString, path);
    QFETCH(bool, expected);

    QFileInfo fi(path);
    QCOMPARE(fi.isRoot(), expected);
}

void tst_QFileInfo::exists_data()
{
    QTest::addColumn<QString>("path");
    QTest::addColumn<bool>("expected");

    QTest::newRow("data0") << QDir::currentPath() << true;
    QTest::newRow("data1") << "tst_qfileinfo.cpp" << true;
    QTest::newRow("data2") << "/I/do_not_expect_this_path_to_exist/" << false;
    QTest::newRow("data3") << ":/tst_qfileinfo/resources/" << true;
    QTest::newRow("data4") << ":/tst_qfileinfo/resources/file1" << true;
    QTest::newRow("data5") << ":/I/do_not_expect_this_path_to_exist/" << false;
    QTest::newRow("data6") << "resources/*" << false;
    QTest::newRow("data7") << "resources/*.foo" << false;
    QTest::newRow("data8") << "resources/*.ext1" << false;
    QTest::newRow("data9") << "resources/file?.ext1" << false;
    QTest::newRow("data10") << "." << true;
    QTest::newRow("data11") << ". " << false;

    QTest::newRow("simple dir") << "resources" << true;
    QTest::newRow("simple dir with slash") << "resources/" << true;

#if defined(Q_OS_WIN) && !defined(Q_OS_WINCE)
    QTest::newRow("unc 1") << "//"  + QtNetworkSettings::winServerName() << true;
    QTest::newRow("unc 2") << "//"  + QtNetworkSettings::winServerName() + "/" << true;
    QTest::newRow("unc 3") << "//"  + QtNetworkSettings::winServerName() + "/testshare" << true;
    QTest::newRow("unc 4") << "//"  + QtNetworkSettings::winServerName() + "/testshare/" << true;
    QTest::newRow("unc 5") << "//"  + QtNetworkSettings::winServerName() + "/testshare/tmp" << true;
    QTest::newRow("unc 6") << "//"  + QtNetworkSettings::winServerName() + "/testshare/tmp/" << true;
    QTest::newRow("unc 7") << "//"  + QtNetworkSettings::winServerName() + "/testshare/adirthatshouldnotexist" << false;
    QTest::newRow("unc 8") << "//"  + QtNetworkSettings::winServerName() + "/asharethatshouldnotexist" << false;
    QTest::newRow("unc 9") << "//ahostthatshouldnotexist" << false;
#endif
}

void tst_QFileInfo::exists()
{
    QFETCH(QString, path);
    QFETCH(bool, expected);

    QFileInfo fi(path);
    QCOMPARE(fi.exists(), expected);
}

void tst_QFileInfo::absolutePath_data()
{
    QTest::addColumn<QString>("file");
    QTest::addColumn<QString>("path");
    QTest::addColumn<QString>("filename");

    QString drivePrefix;
#if defined(Q_OS_WIN) && !defined(Q_OS_WINCE)
    drivePrefix = QDir::currentPath().left(2);
#endif
    QTest::newRow("0") << "/machine/share/dir1/" << drivePrefix + "/machine/share/dir1" << "";
    QTest::newRow("1") << "/machine/share/dir1" << drivePrefix + "/machine/share" << "dir1";
    QTest::newRow("2") << "/usr/local/bin" << drivePrefix + "/usr/local" << "bin";
    QTest::newRow("3") << "/usr/local/bin/" << drivePrefix + "/usr/local/bin" << "";
    QTest::newRow("/test") << "/test" << drivePrefix + "/" << "test";

#if defined(Q_OS_WIN) && !defined(Q_OS_WINCE)
    // see task 102898
    QTest::newRow("c:\\autoexec.bat") << "c:\\autoexec.bat" << "C:/"
                                      << "autoexec.bat";
#endif
}

void tst_QFileInfo::absolutePath()
{
    QFETCH(QString, file);
    QFETCH(QString, path);
    QFETCH(QString, filename);

    QFileInfo fi(file);

    QCOMPARE(fi.absolutePath(), path);
    QCOMPARE(fi.fileName(), filename);
}

void tst_QFileInfo::absFilePath_data()
{
    QTest::addColumn<QString>("file");
    QTest::addColumn<QString>("expected");

    QTest::newRow("relativeFile") << "tmp.txt" << QDir::currentPath() + "/tmp.txt";
    QTest::newRow("relativeFileInSubDir") << "temp/tmp.txt" << QDir::currentPath() + "/" + "temp/tmp.txt";
#if defined(Q_OS_WIN) && !defined(Q_OS_WINCE)
    QString curr = QDir::currentPath();
    curr.remove(0, 2);   // Make it a absolute path with no drive specifier: \depot\qt-4.2\tests\auto\qfileinfo
    QTest::newRow(".")            << curr << QDir::currentPath();
    QTest::newRow("absFilePath") << "c:\\home\\andy\\tmp.txt" << "C:/home/andy/tmp.txt";
#else
    QTest::newRow("absFilePath") << "/home/andy/tmp.txt" << "/home/andy/tmp.txt";
#endif
}

void tst_QFileInfo::absFilePath()
{
    QFETCH(QString, file);
    QFETCH(QString, expected);

    QFileInfo fi(file);
    QCOMPARE(fi.absoluteFilePath(), expected);
}

void tst_QFileInfo::canonicalPath()
{
    QTemporaryFile tempFile;
    tempFile.setAutoRemove(true);
    tempFile.open();
    QFileInfo fi(tempFile.fileName());
    QCOMPARE(fi.canonicalPath(), QFileInfo(QDir::tempPath()).canonicalFilePath());
}

void tst_QFileInfo::canonicalFilePath()
{
    const QString fileName("tmp.canon");
    QFile tempFile(fileName);
    QVERIFY(tempFile.open(QFile::WriteOnly));
    QFileInfo fi(tempFile.fileName());
    QCOMPARE(fi.canonicalFilePath(), QDir::currentPath() + "/" + fileName);
    tempFile.remove();

    // This used to crash on Mac, verify that it doesn't anymore.
    QFileInfo info("/tmp/../../../../../../../../../../../../../../../../../");
    info.canonicalFilePath();

#ifndef Q_OS_WIN
    // test symlinks
    QFile::remove("link.lnk");
    {
        QFile file("tst_qfileinfo.cpp");
        if (file.link("link.lnk")) {
            QFileInfo info1(file);
            QFileInfo info2("link.lnk");
            QCOMPARE(info1.canonicalFilePath(), info2.canonicalFilePath());
        }
    }
    {
        const QString link(QDir::tempPath() + QDir::separator() + "tst_qfileinfo");
        QFile::remove(link);
        QFile file(QDir::currentPath());
        if (file.link(link)) {
            QFileInfo info1("tst_qfileinfo.cpp");
            QFileInfo info2(link + QDir::separator() + "tst_qfileinfo.cpp");
            QCOMPARE(info1.canonicalFilePath(), info2.canonicalFilePath());

            QFileInfo info3(link + QDir::separator() + "link.lnk");
            QVERIFY(!info3.canonicalFilePath().isEmpty());
            QCOMPARE(info1.canonicalFilePath(), info3.canonicalFilePath());
        }
    }
    {
        QString link(QDir::tempPath() + QDir::separator() + "tst_qfileinfo"
                     + QDir::separator() + "link_to_tst_qfileinfo");
        QFile::remove(link);

        QFile file(QDir::tempPath() + QDir::separator() + "tst_qfileinfo"
                   + QDir::separator() + "tst_qfileinfo.cpp");
        if (file.link(link))
        {
            QFileInfo info1("tst_qfileinfo.cpp");
            QFileInfo info2(link);
            QCOMPARE(info1.canonicalFilePath(), info2.canonicalFilePath());
        }
    }
#endif

}

void tst_QFileInfo::fileName_data()
{
    QTest::addColumn<QString>("file");
    QTest::addColumn<QString>("expected");

    QTest::newRow("relativeFile") << "tmp.txt" << "tmp.txt";
    QTest::newRow("relativeFileInSubDir") << "temp/tmp.txt" << "tmp.txt";
#if defined(Q_OS_WIN) && !defined(Q_OS_WINCE)
    QTest::newRow("absFilePath") << "c:\\home\\andy\\tmp.txt" << "tmp.txt";
#else
    QTest::newRow("absFilePath") << "/home/andy/tmp.txt" << "tmp.txt";
#endif
    QTest::newRow("resource1") << ":/tst_qfileinfo/resources/file1.ext1" << "file1.ext1";
    QTest::newRow("resource2") << ":/tst_qfileinfo/resources/file1.ext1.ext2" << "file1.ext1.ext2";

    QTest::newRow("ending slash [small]") << QString::fromLatin1("/a/") << QString::fromLatin1("");
    QTest::newRow("no ending slash [small]") << QString::fromLatin1("/a") << QString::fromLatin1("a");

    QTest::newRow("ending slash") << QString::fromLatin1("/somedir/") << QString::fromLatin1("");
    QTest::newRow("no ending slash") << QString::fromLatin1("/somedir") << QString::fromLatin1("somedir");
}

void tst_QFileInfo::fileName()
{
    QFETCH(QString, file);
    QFETCH(QString, expected);

    QFileInfo fi(file);
    QCOMPARE(fi.fileName(), expected);
}

void tst_QFileInfo::bundleName_data()
{
    QTest::addColumn<QString>("file");
    QTest::addColumn<QString>("expected");

    QTest::newRow("root") << "/" << "";
    QTest::newRow("etc") << "/etc" << "";
#ifdef Q_OS_MAC
    QTest::newRow("safari") << "/Applications/Safari.app" << "Safari";
#endif
}

void tst_QFileInfo::bundleName()
{
    QFETCH(QString, file);
    QFETCH(QString, expected);

    QFileInfo fi(file);
    QCOMPARE(fi.bundleName(), expected);
}

void tst_QFileInfo::dir_data()
{
    QTest::addColumn<QString>("file");
    QTest::addColumn<bool>("absPath");
    QTest::addColumn<QString>("expected");

    QTest::newRow("relativeFile") << "tmp.txt" << false << ".";
    QTest::newRow("relativeFileAbsPath") << "tmp.txt" << true << QDir::currentPath();
    QTest::newRow("relativeFileInSubDir") << "temp/tmp.txt" << false << "temp";
    QTest::newRow("relativeFileInSubDirAbsPath") << "temp/tmp.txt" << true << QDir::currentPath() + "/temp";
    QTest::newRow("absFilePath") << QDir::currentPath() + "/tmp.txt" << false << QDir::currentPath();
    QTest::newRow("absFilePathAbsPath") << QDir::currentPath() + "/tmp.txt" << true << QDir::currentPath();
    QTest::newRow("resource1") << ":/tst_qfileinfo/resources/file1.ext1" << true << ":/tst_qfileinfo/resources";
}

void tst_QFileInfo::dir()
{
    QFETCH(QString, file);
    QFETCH(bool, absPath);
    QFETCH(QString, expected);

    QFileInfo fi(file);
    if (absPath)
        QCOMPARE(fi.absolutePath(), expected);
    else
        QCOMPARE(fi.path(), expected);
}


void tst_QFileInfo::suffix_data()
{
    QTest::addColumn<QString>("file");
    QTest::addColumn<QString>("expected");

    QTest::newRow("noextension0") << "file" << "";
    QTest::newRow("noextension1") << "/path/to/file" << "";
    QTest::newRow("data0") << "file.tar" << "tar";
    QTest::newRow("data1") << "file.tar.gz" << "gz";
    QTest::newRow("data2") << "/path/file/file.tar.gz" << "gz";
    QTest::newRow("data3") << "/path/file.tar" << "tar";
    QTest::newRow("resource1") << ":/tst_qfileinfo/resources/file1.ext1" << "ext1";
    QTest::newRow("resource2") << ":/tst_qfileinfo/resources/file1.ext1.ext2" << "ext2";
    QTest::newRow("hidden1") << ".ext1" << "ext1";
    QTest::newRow("hidden1") << ".ext" << "ext";
    QTest::newRow("hidden1") << ".ex" << "ex";
    QTest::newRow("hidden1") << ".e" << "e";
    QTest::newRow("hidden2") << ".ext1.ext2" << "ext2";
    QTest::newRow("hidden2") << ".ext.ext2" << "ext2";
    QTest::newRow("hidden2") << ".ex.ext2" << "ext2";
    QTest::newRow("hidden2") << ".e.ext2" << "ext2";
    QTest::newRow("hidden2") << "..ext2" << "ext2";
}

void tst_QFileInfo::suffix()
{
    QFETCH(QString, file);
    QFETCH(QString, expected);

    QFileInfo fi(file);
    QCOMPARE(fi.suffix(), expected);
}


void tst_QFileInfo::completeSuffix_data()
{
    QTest::addColumn<QString>("file");
    QTest::addColumn<QString>("expected");

    QTest::newRow("noextension0") << "file" << "";
    QTest::newRow("noextension1") << "/path/to/file" << "";
    QTest::newRow("data0") << "file.tar" << "tar";
    QTest::newRow("data1") << "file.tar.gz" << "tar.gz";
    QTest::newRow("data2") << "/path/file/file.tar.gz" << "tar.gz";
    QTest::newRow("data3") << "/path/file.tar" << "tar";
    QTest::newRow("resource1") << ":/tst_qfileinfo/resources/file1.ext1" << "ext1";
    QTest::newRow("resource2") << ":/tst_qfileinfo/resources/file1.ext1.ext2" << "ext1.ext2";
}

void tst_QFileInfo::completeSuffix()
{
    QFETCH(QString, file);
    QFETCH(QString, expected);

    QFileInfo fi(file);
    QCOMPARE(fi.completeSuffix(), expected);
}

void tst_QFileInfo::baseName_data()
{
    QTest::addColumn<QString>("file");
    QTest::addColumn<QString>("expected");

    QTest::newRow("data0") << "file.tar" << "file";
    QTest::newRow("data1") << "file.tar.gz" << "file";
    QTest::newRow("data2") << "/path/file/file.tar.gz" << "file";
    QTest::newRow("data3") << "/path/file.tar" << "file";
    QTest::newRow("data4") << "/path/file" << "file";
    QTest::newRow("resource1") << ":/tst_qfileinfo/resources/file1.ext1" << "file1";
    QTest::newRow("resource2") << ":/tst_qfileinfo/resources/file1.ext1.ext2" << "file1";
}

void tst_QFileInfo::baseName()
{
    QFETCH(QString, file);
    QFETCH(QString, expected);

    QFileInfo fi(file);
    QCOMPARE(fi.baseName(), expected);
}

void tst_QFileInfo::completeBaseName_data()
{
    QTest::addColumn<QString>("file");
    QTest::addColumn<QString>("expected");

    QTest::newRow("data0") << "file.tar" << "file";
    QTest::newRow("data1") << "file.tar.gz" << "file.tar";
    QTest::newRow("data2") << "/path/file/file.tar.gz" << "file.tar";
    QTest::newRow("data3") << "/path/file.tar" << "file";
    QTest::newRow("data4") << "/path/file" << "file";
    QTest::newRow("resource1") << ":/tst_qfileinfo/resources/file1.ext1" << "file1";
    QTest::newRow("resource2") << ":/tst_qfileinfo/resources/file1.ext1.ext2" << "file1.ext1";
}

void tst_QFileInfo::completeBaseName()
{
    QFETCH(QString, file);
    QFETCH(QString, expected);

    QFileInfo fi(file);
    QCOMPARE(fi.completeBaseName(), expected);
}

void tst_QFileInfo::permission_data()
{
    QTest::addColumn<QString>("file");
    QTest::addColumn<int>("perms");
    QTest::addColumn<bool>("expected");

    QTest::newRow("data0") << QCoreApplication::instance()->applicationFilePath() << int(QFile::ExeUser) << true;
    QTest::newRow("data1") << "tst_qfileinfo.cpp" << int(QFile::ReadUser) << true;
//    QTest::newRow("data2") << "tst_qfileinfo.cpp" << int(QFile::WriteUser) << false;
    QTest::newRow("resource1") << ":/tst_qfileinfo/resources/file1.ext1" << int(QFile::ReadUser) << true;
    QTest::newRow("resource2") << ":/tst_qfileinfo/resources/file1.ext1" << int(QFile::WriteUser) << false;
    QTest::newRow("resource3") << ":/tst_qfileinfo/resources/file1.ext1" << int(QFile::ExeUser) << false;

}

void tst_QFileInfo::permission()
{
    QFETCH(QString, file);
    QFETCH(int, perms);
    QFETCH(bool, expected);
    QFileInfo fi(file);
    QCOMPARE(fi.permission((QFile::Permissions)perms), expected);
}

void tst_QFileInfo::size_data()
{
    QTest::addColumn<QString>("file");
    QTest::addColumn<int>("size");

    QTest::newRow("resource1") << ":/tst_qfileinfo/resources/file1.ext1" << 0;
    QFile::remove("file1");
    QFile file("file1");
    QVERIFY(file.open(QFile::WriteOnly));
    QCOMPARE(file.write("JAJAJAA"), qint64(7));
    QTest::newRow("created-file") << "file1" << 7;

    QTest::newRow("resource2") << ":/tst_qfileinfo/resources/file1.ext1.ext2" << 0;
}

void tst_QFileInfo::size()
{
    QFETCH(QString, file);

    QFileInfo fi(file);
    (void)fi.permissions();     // see task 104198
    QTEST(int(fi.size()), "size");
}

void tst_QFileInfo::systemFiles()
{
#ifndef Q_OS_WIN
    QSKIP("This is a Windows only test", SkipAll);
#endif
    QFileInfo fi("c:\\pagefile.sys");
    QVERIFY(fi.exists());      // task 167099
    QVERIFY(fi.size() > 0);    // task 189202
    QVERIFY(fi.lastModified().isValid());
}

void tst_QFileInfo::compare_data()
{
    QTest::addColumn<QString>("file1");
    QTest::addColumn<QString>("file2");
    QTest::addColumn<bool>("same");

    QTest::newRow("data0") << QString::fromLatin1("tst_qfileinfo.cpp") << QString::fromLatin1("tst_qfileinfo.cpp") << true;
    QTest::newRow("data1") << QString::fromLatin1("tst_qfileinfo.cpp") << QString::fromLatin1("/tst_qfileinfo.cpp") << false;
    QTest::newRow("data2") << QString::fromLatin1("tst_qfileinfo.cpp")
                        << QDir::currentPath() + QString::fromLatin1("/tst_qfileinfo.cpp") << true;
    QTest::newRow("casesense1") << QString::fromLatin1("tst_qfileInfo.cpp")
                        << QDir::currentPath() + QString::fromLatin1("/tst_qfileinfo.cpp")
#ifdef Q_OS_WIN
                        << true;
#else
                        << false;
#endif

}
void tst_QFileInfo::compare()
{
    QFETCH(QString, file1);
    QFETCH(QString, file2);
    QFETCH(bool, same);
    QFileInfo fi1(file1), fi2(file2);
    QCOMPARE(same, fi1 == fi2);
}

void tst_QFileInfo::consistent_data()
{
    QTest::addColumn<QString>("file");
    QTest::addColumn<QString>("expected");

#ifdef Q_OS_WIN
    QTest::newRow("slashes") << QString::fromLatin1("\\a\\a\\a\\a") << QString::fromLatin1("/a/a/a/a");
#endif
    QTest::newRow("ending slash") << QString::fromLatin1("/a/somedir/") << QString::fromLatin1("/a/somedir/");
    QTest::newRow("no ending slash") << QString::fromLatin1("/a/somedir") << QString::fromLatin1("/a/somedir");
}

void tst_QFileInfo::consistent()
{
    QFETCH(QString, file);
    QFETCH(QString, expected);

    QFileInfo fi(file);
    QCOMPARE(fi.filePath(), expected);
    QCOMPARE(fi.dir().path() + "/" + fi.fileName(), expected);
}


void tst_QFileInfo::fileTimes_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::newRow("simple") << QString::fromLatin1("simplefile.txt");
    QTest::newRow( "longfile" ) << QString::fromLatin1("longFileNamelongFileNamelongFileNamelongFileName"
                                                     "longFileNamelongFileNamelongFileNamelongFileName"
                                                     "longFileNamelongFileNamelongFileNamelongFileName"
                                                     "longFileNamelongFileNamelongFileNamelongFileName"
                                                     "longFileNamelongFileNamelongFileNamelongFileName.txt");
    QTest::newRow( "longfile absolutepath" ) << QFileInfo(QString::fromLatin1("longFileNamelongFileNamelongFileNamelongFileName"
                                                     "longFileNamelongFileNamelongFileNamelongFileName"
                                                     "longFileNamelongFileNamelongFileNamelongFileName"
                                                     "longFileNamelongFileNamelongFileNamelongFileName"
                                                     "longFileNamelongFileNamelongFileNamelongFileName.txt")).absoluteFilePath();
}

void tst_QFileInfo::fileTimes()
{
#if defined(Q_OS_WINCE)
    int sleepTime = 3000;
#else
    int sleepTime = 2000;
#endif
    QFETCH(QString, fileName);
    if (QFile::exists(fileName)) {
        QVERIFY(QFile::remove(fileName));
    }
    QTest::qSleep(sleepTime);
    {
        QFile file(fileName);
#if defined(Q_OS_WINCE)
        QEXPECT_FAIL("longfile", "No long filenames on WinCE", Abort);
        QEXPECT_FAIL("longfile absolutepath", "No long filenames on WinCE", Abort);
#endif
        QVERIFY(file.open(QFile::WriteOnly | QFile::Text));
        QTextStream ts(&file);
        ts << fileName << endl;
    }
    QTest::qSleep(sleepTime);
    QDateTime beforeWrite = QDateTime::currentDateTime();
    QTest::qSleep(sleepTime);
    {
        QFileInfo fileInfo(fileName);
        QVERIFY(fileInfo.created() < beforeWrite);
        QFile file(fileName);
        QVERIFY(file.open(QFile::ReadWrite | QFile::Text));
        QTextStream ts(&file);
        ts << fileName << endl;
    }
    QTest::qSleep(sleepTime);
    QDateTime beforeRead = QDateTime::currentDateTime();
    QTest::qSleep(sleepTime);
    {
        QFileInfo fileInfo(fileName);
// On unix created() returns the same as lastModified().
#if !defined(Q_OS_UNIX) && !defined(Q_OS_WINCE)
        QVERIFY(fileInfo.created() < beforeWrite);
#endif
        QVERIFY(fileInfo.lastModified() > beforeWrite);
        QFile file(fileName);
        QVERIFY(file.open(QFile::ReadOnly | QFile::Text));
        QTextStream ts(&file);
        QString line = ts.readLine();
        QCOMPARE(line, fileName);
    }

    QFileInfo fileInfo(fileName);
#if !defined(Q_OS_UNIX) && !defined(Q_OS_WINCE)
    QVERIFY(fileInfo.created() < beforeWrite);
#endif
    //In Vista the last-access timestamp is not updated when the file is accessed/touched (by default).
    //To enable this the HKLM\SYSTEM\CurrentControlSet\Control\FileSystem\NtfsDisableLastAccessUpdate
    //is set to 0, in the test machine.
#ifdef Q_OS_WINCE
    QEXPECT_FAIL("simple", "WinCE only stores date of access data, not the time", Continue);
#endif
    QVERIFY(fileInfo.lastRead() > beforeRead);
    QVERIFY(fileInfo.lastModified() > beforeWrite);
    QVERIFY(fileInfo.lastModified() < beforeRead);
}

void tst_QFileInfo::fileTimes_oldFile()
{
    // This is not supported on WinCE
#if defined(Q_OS_WIN) && !defined(Q_OS_WINCE)
    // All files are opened in share mode (both read and write).
    DWORD shareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;

    // All files on Windows can be read; there's no such thing as an
    // unreadable file. Add GENERIC_WRITE if WriteOnly is passed.
    int accessRights = GENERIC_READ | GENERIC_WRITE;

    SECURITY_ATTRIBUTES securityAtts = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };

    // Regular file mode. In Unbuffered mode, pass the no-buffering flag.
    DWORD flagsAndAtts = FILE_ATTRIBUTE_NORMAL;

    // WriteOnly can create files, ReadOnly cannot.
    DWORD creationDisp = OPEN_ALWAYS;

    // Create the file handle.
    HANDLE fileHandle = CreateFile(L"oldfile.txt",
        accessRights,
        shareMode,
        &securityAtts,
        creationDisp,
        flagsAndAtts,
        NULL);

    // Set file times back to 1601.
    FILETIME ctime;
    ctime.dwLowDateTime = 1;
    ctime.dwHighDateTime = 0;
    FILETIME atime = ctime;
    FILETIME mtime = atime;
    QVERIFY(fileHandle);
    QVERIFY(SetFileTime(fileHandle, &ctime, &atime, &mtime) != 0);

    QFileInfo info("oldfile.txt");
    QCOMPARE(info.lastModified(), QDateTime(QDate(1601, 1, 1), QTime(1, 0)));

    CloseHandle(fileHandle);
#endif
}

void tst_QFileInfo::isSymLink()
{
    QFile::remove("link.lnk");
    QFile::remove("brokenlink.lnk");

    QFileInfo info1("tst_qfileinfo.cpp");
    QVERIFY( !info1.isSymLink() );

    QFile file2("tst_qfileinfo.cpp");
    if (file2.link("link.lnk")) {
        QFileInfo info2("link.lnk");
        QVERIFY( info2.isSymLink() );
    }

    QFile file3("dummyfile");
    file3.open(QIODevice::WriteOnly);
    if (file3.link("brokenlink.lnk")) {
        file3.remove();
        QFileInfo info3("brokenlink.lnk");
        QVERIFY( info3.isSymLink() );
    }

}

void tst_QFileInfo::isHidden_data()
{
    QTest::addColumn<QString>("path");
    QTest::addColumn<bool>("isHidden");
    foreach (QFileInfo info, QDir::drives()) {
	QTest::newRow(qPrintable("drive." + info.path())) << info.path() << false;
    }
#ifdef Q_OS_MAC
    QTest::newRow("mac_etc") << QString::fromLatin1("/etc") << true;
    QTest::newRow("mac_private_etc") << QString::fromLatin1("/private/etc") << false;
    QTest::newRow("mac_Applications") << QString::fromLatin1("/Applications") << false;
#endif
}

void tst_QFileInfo::isHidden()
{
    QFETCH(QString, path);
    QFETCH(bool, isHidden);
    QFileInfo fi(path);
    QCOMPARE(fi.isHidden(), isHidden);
}

void tst_QFileInfo::isBundle_data()
{
    QTest::addColumn<QString>("path");
    QTest::addColumn<bool>("isBundle");
    QTest::newRow("root") << QString::fromLatin1("/") << false;
#ifdef Q_OS_MAC
    QTest::newRow("mac_Applications") << QString::fromLatin1("/Applications") << false;
    QTest::newRow("mac_Applications") << QString::fromLatin1("/Applications/Safari.app") << true;
#endif
}

void tst_QFileInfo::isBundle()
{
    QFETCH(QString, path);
    QFETCH(bool, isBundle);
    QFileInfo fi(path);
    QCOMPARE(fi.isBundle(), isBundle);
}

void tst_QFileInfo::refresh()
{
#if defined(Q_OS_WINCE)
    int sleepTime = 3000;
#else
    int sleepTime = 2000;
#endif

    QFile::remove("file1");
    QFile file("file1");
    QVERIFY(file.open(QFile::WriteOnly));
    QCOMPARE(file.write("JAJAJAA"), qint64(7));
    file.flush();

    QFileInfo info(file);
    QDateTime lastModified = info.lastModified();
    QCOMPARE(info.size(), qint64(7));

    QTest::qSleep(sleepTime);

    QCOMPARE(file.write("JOJOJO"), qint64(6));
    file.flush();
    QVERIFY(info.lastModified() == lastModified);

    QCOMPARE(info.size(), qint64(7));
#if defined(Q_OS_WIN) || defined(Q_OS_WINCE)
    if (QSysInfo::windowsVersion() & QSysInfo::WV_VISTA ||
                QSysInfo::windowsVersion() & QSysInfo::WV_CE_based)
        file.close();
#endif
#if defined(Q_OS_WINCE)
    // On Windows CE we need to close the file.
    // Otherwise the content will be cached and not
    // flushed to the storage, although we flushed it
    // manually!!! CE has interim cache, we cannot influence.
    QTest::qWait(5000);
#endif
    info.refresh();
    QCOMPARE(info.size(), qint64(13));
    QVERIFY(info.lastModified() > lastModified);

    QFileInfo info2 = info;
    QCOMPARE(info2.size(), info.size());

    info2.refresh();
    QCOMPARE(info2.size(), info.size());
}

#if defined(Q_OS_WIN) && !defined(Q_OS_WINCE)
void tst_QFileInfo::brokenShortcut()
{
    QString linkName("borkenlink.lnk");
    QFile::remove(linkName);
    QFile file(linkName);
    file.open(QFile::WriteOnly);
    file.write("b0rk");
    file.close();

    QFileInfo info(linkName);
    QVERIFY(info.isSymLink());
    QVERIFY(!info.exists());
    QFile::remove(linkName);
}
#endif

#ifdef Q_OS_UNIX
void tst_QFileInfo::isWritable()
{
    if (::getuid() == 0)
        QVERIFY(QFileInfo("/etc/passwd").isWritable());
    else
        QVERIFY(!QFileInfo("/etc/passwd").isWritable());
}
#endif

void tst_QFileInfo::isExecutable()
{
    QString appPath = QCoreApplication::applicationDirPath();
    appPath += "/tst_qfileinfo";
#if defined(Q_OS_WIN)
    appPath += ".exe";
#endif
    QFileInfo fi(appPath);
    QCOMPARE(fi.isExecutable(), true);

    QCOMPARE(QFileInfo("qfileinfo.pro").isExecutable(), false);
}


void tst_QFileInfo::testDecomposedUnicodeNames_data()
{
    QTest::addColumn<QString>("filePath");
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<bool>("exists");
    QString currPath = QDir::currentPath();
    QTest::newRow("latin-only") << currPath + "/4.pdf" << "4.pdf" << true;
    QTest::newRow("one-decomposed uni") << currPath + QString::fromUtf8("/4 ä.pdf") << QString::fromUtf8("4 ä.pdf") << true;
    QTest::newRow("many-decomposed uni") << currPath + QString::fromUtf8("/4 äääcopy.pdf") << QString::fromUtf8("4 äääcopy.pdf") << true;
    QTest::newRow("no decomposed") << currPath + QString::fromUtf8("/4 øøøcopy.pdf") << QString::fromUtf8("4 øøøcopy.pdf") << true;
}

static void createFileNative(const QString &filePath)
{
#ifdef Q_OS_UNIX
    int fd = open(filePath.normalized(QString::NormalizationForm_D).toUtf8().constData(), O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
    if (fd < 0) {
        QFAIL("couldn't create file");
    } else {
        close(fd);
    }
#else
    Q_UNUSED(filePath);
#endif
}

static void removeFileNative(const QString &filePath)
{
#ifdef Q_OS_UNIX
    unlink(filePath.normalized(QString::NormalizationForm_D).toUtf8().constData());
#else
    Q_UNUSED(filePath);
#endif
}

void tst_QFileInfo::testDecomposedUnicodeNames()
{
#ifndef Q_OS_MAC
    QSKIP("This is a OS X only test (unless you know more about filesystems, then maybe you should try it ;)", SkipAll);
#endif
    QFETCH(QString, filePath);
    createFileNative(filePath);

    QFileInfo file(filePath);
    QTEST(file.fileName(), "fileName");
    QTEST(file.exists(), "exists");
    removeFileNative(filePath);
}

void tst_QFileInfo::equalOperator() const
{
    /* Compare two default constructed values. Yes, to me it seems it should be the opposite too, but
     * this is how the code was written. */
    QVERIFY(!(QFileInfo() == QFileInfo()));
}


void tst_QFileInfo::equalOperatorWithDifferentSlashes() const
{
    const QFileInfo fi1("/usr");
    const QFileInfo fi2("/usr/");

    QCOMPARE(fi1, fi2);
}

void tst_QFileInfo::notEqualOperator() const
{
    /* Compare two default constructed values. Yes, to me it seems it should be the opposite too, but
     * this is how the code was written. */
    QVERIFY(QFileInfo() != QFileInfo());
}

QTEST_MAIN(tst_QFileInfo)
#include "tst_qfileinfo.moc"

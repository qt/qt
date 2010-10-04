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


#include <QtCore/QtCore>
#include <QtTest/QtTest>

#ifdef QT_NO_PROCESS
QTEST_NOOP_MAIN
#else

#include "qbic.h"

#include <stdlib.h>

class tst_Bic: public QObject
{
    Q_OBJECT

public:
    tst_Bic();
    QBic::Info getCurrentInfo(const QString &libName);

    QHash<QString, QBic::Info> cachedCurrentInfo;

private slots:
    void initTestCase_data();
    void initTestCase();

    void sizesAndVTables_data();
    void sizesAndVTables();

private:
    QBic bic;
};

typedef QPair<QString, QString> QStringPair;

tst_Bic::tst_Bic()
{
    bic.addBlacklistedClass(QLatin1String("std::*"));
    bic.addBlacklistedClass(QLatin1String("qIsNull*"));
    bic.addBlacklistedClass(QLatin1String("_*"));
    bic.addBlacklistedClass(QLatin1String("<anonymous*"));

    /* some system stuff we don't care for */
    bic.addBlacklistedClass(QLatin1String("timespec"));
    bic.addBlacklistedClass(QLatin1String("itimerspec"));
    bic.addBlacklistedClass(QLatin1String("sched_param"));
    bic.addBlacklistedClass(QLatin1String("timeval"));
    bic.addBlacklistedClass(QLatin1String("drand"));
    bic.addBlacklistedClass(QLatin1String("lconv"));
    bic.addBlacklistedClass(QLatin1String("random"));
    bic.addBlacklistedClass(QLatin1String("wait"));
    bic.addBlacklistedClass(QLatin1String("tm"));
    bic.addBlacklistedClass(QLatin1String("sigcontext"));
    bic.addBlacklistedClass(QLatin1String("ucontext"));
    bic.addBlacklistedClass(QLatin1String("ucontext64"));
    bic.addBlacklistedClass(QLatin1String("sigaltstack"));

    /* QtOpenGL includes qt_windows.h, and some SDKs dont have these structs present */
    bic.addBlacklistedClass(QLatin1String("tagTITLEBARINFO"));
    bic.addBlacklistedClass(QLatin1String("tagMENUITEMINFOA"));
    bic.addBlacklistedClass(QLatin1String("tagMENUITEMINFOW"));
    bic.addBlacklistedClass(QLatin1String("tagENHMETAHEADER"));

    /* some bug in gcc also reported template instanciations */
    bic.addBlacklistedClass(QLatin1String("QTypeInfo<*>"));
    bic.addBlacklistedClass(QLatin1String("QMetaTypeId<*>"));
    bic.addBlacklistedClass(QLatin1String("QVector<QGradientStop>*"));

    /* this guy is never instantiated, just for compile-time checking */
    bic.addBlacklistedClass(QLatin1String("QMap<*>::PayloadNode"));

    /* QFileEngine was removed in 4.1 */
    bic.addBlacklistedClass(QLatin1String("QFileEngine"));
    bic.addBlacklistedClass(QLatin1String("QFileEngineHandler"));
    bic.addBlacklistedClass(QLatin1String("QFlags<QFileEngine::FileFlag>"));

    /* Private classes */
    bic.addBlacklistedClass(QLatin1String("QBrushData"));
    bic.addBlacklistedClass(QLatin1String("QObjectData"));
    bic.addBlacklistedClass(QLatin1String("QAtomic"));
    bic.addBlacklistedClass(QLatin1String("QBasicAtomic"));
    bic.addBlacklistedClass(QLatin1String("QRegion::QRegionData"));
    bic.addBlacklistedClass(QLatin1String("QtConcurrent::ThreadEngineSemaphore"));
    bic.addBlacklistedClass(QLatin1String("QDrawPixmaps::Data"));
    bic.addBlacklistedClass(QLatin1String("QS60Style"));

    /* Jambi-related classes in Designer */
    bic.addBlacklistedClass(QLatin1String("QDesignerLanguageExtension"));

    /* Harald says it's undocumented and private :) */
    bic.addBlacklistedClass(QLatin1String("QAccessibleInterfaceEx"));
    bic.addBlacklistedClass(QLatin1String("QAccessibleObjectEx"));
    bic.addBlacklistedClass(QLatin1String("QAccessibleWidgetEx"));

    /* This structure is semi-private and should never shrink */
    bic.addBlacklistedClass(QLatin1String("QVFbHeader"));
}

void tst_Bic::initTestCase_data()
{
    QTest::addColumn<QString>("libName");

    QTest::newRow("QtCore") << "QtCore";
    QTest::newRow("QtGui") << "QtGui";

    QTest::newRow("Qt3Support") << "Qt3Support";
#ifndef QT_NO_DBUS
    QTest::newRow("QtDBus") << "QtDBus";
#endif
    QTest::newRow("QtDesigner") << "QtDesigner";
    QTest::newRow("QtDeclarative") << "QtDeclarative";
    QTest::newRow("QtMultimedia") << "QtMultimedia";
    QTest::newRow("QtNetwork") << "QtNetwork";
    QTest::newRow("QtOpenGL") << "QtOpenGL";
    QTest::newRow("QtScript") << "QtScript";
    QTest::newRow("QtScriptTools") << "QtScriptTools";
    QTest::newRow("QtSql") << "QtSql";
    QTest::newRow("QtSvg") << "QtSvg";
    QTest::newRow("QtTest") << "QtTest";
    QTest::newRow("QtWebKit") << "QtWebKit";
    QTest::newRow("QtXml") << "QtXml";
    QTest::newRow("QtXmlPatterns") << "QtXmlPatterns";
    QTest::newRow("phonon") << "phonon";
}

void tst_Bic::initTestCase()
{
    QString qtDir = QString::fromLocal8Bit(qgetenv("QTDIR"));
    QVERIFY2(!qtDir.isEmpty(), "This test needs $QTDIR");

    if (qgetenv("PATH").contains("teambuilder"))
        QTest::qWarn("This test might not work with teambuilder, consider switching it off.");
}

void tst_Bic::sizesAndVTables_data()
{
#if !defined(Q_CC_GNU) || defined(Q_CC_INTEL)
    QSKIP("Test not implemented for this compiler/platform", SkipAll);
#else

#if defined Q_OS_LINUX && defined Q_WS_X11
# if defined(__powerpc__) && !defined(__powerpc64__)
#  define FILESUFFIX "linux-gcc-ppc32"
# elif defined(__amd64__)
#  define FILESUFFIX "linux-gcc-amd64"
# elif defined(__i386__)
#  define FILESUFFIX "linux-gcc-ia32"
# endif
#elif defined Q_OS_MAC && defined(__powerpc__)
#  define FILESUFFIX "macx-gcc-ppc32"
#elif defined Q_OS_MAC && defined(__i386__)
#  define FILESUFFIX "macx-gcc-ia32"
#elif defined Q_OS_MAC && defined(__amd64__)
#  define FILESUFFIX "macx-gcc-amd64"
#elif defined Q_OS_WIN && defined Q_CC_GNU
#  define FILESUFFIX "win32-gcc-ia32"
#else
#  define FILESUFFIX "nonsuch"
    QSKIP("No reference files found for this platform", SkipAll);
#endif

    QTest::addColumn<QString>("oldLib");
    QTest::addColumn<bool>("isPatchRelease");

    int minor = (QT_VERSION >> 8) & 0xFF;
    int patch = QT_VERSION & 0xFF;
    for (int i = 0; i <= minor; ++i) {
        if (i != minor || patch)
            QTest::newRow("4." + QByteArray::number(i))
                << (QString(SRCDIR "data/%1.4.")
                    + QString::number(i)
                    + QString(".0." FILESUFFIX ".txt"))
                << (i == minor && patch);
    }
#endif
}

QBic::Info tst_Bic::getCurrentInfo(const QString &libName)
{
    QBic::Info &inf = cachedCurrentInfo[libName];
    if (!inf.classSizes.isEmpty())
        return inf;

    QTemporaryFile tmpQFile;
    tmpQFile.open();
    QString tmpFileName = tmpQFile.fileName();

    QByteArray tmpFileContents = "#include<" + libName.toLatin1() + "/" + libName.toLatin1() + ">\n";
    tmpQFile.write(tmpFileContents);
    tmpQFile.flush();

    QString qtDir = QString::fromLocal8Bit(qgetenv("QTDIR"));
#ifdef Q_OS_WIN
    qtDir.replace('\\', '/');
#endif
    QString compilerName = "g++";

    QStringList args;
    args << "-c"
         << "-I" + qtDir + "/include"
#ifdef Q_OS_MAC
        << "-arch" << "i386" // Always use 32-bit data on Mac.
#endif
#ifndef Q_OS_WIN
         << "-I/usr/X11R6/include/"
#endif
         << "-DQT_NO_STL" << "-DQT3_SUPPORT"
         << "-xc++"
#if !defined(Q_OS_AIX) && !defined(Q_OS_WIN)
         << "-o" << "/dev/null"
#endif
         << "-fdump-class-hierarchy"
         << tmpFileName;

    QProcess proc;
    proc.start(compilerName, args, QIODevice::ReadOnly);
    if (!proc.waitForFinished(6000000)) {
        qWarning() << "gcc didn't finish" << proc.errorString();
        return QBic::Info();
    }
    if (proc.exitCode() != 0) {
        qWarning() << "gcc returned with" << proc.exitCode();
        qDebug() << proc.readAllStandardError();
        return QBic::Info();
    }

    QString errs = QString::fromLocal8Bit(proc.readAllStandardError().constData());
    if (!errs.isEmpty()) {
        qDebug() << "Arguments:" << args << "Warnings:" << errs;
        return QBic::Info();
    }

    // See if we find the gcc output file, which seems to change
    // from release to release
    QStringList files = QDir().entryList(QStringList() << "*.class");
    if (files.isEmpty()) {
        qFatal("Could not locate the GCC output file, update this test");
        return QBic::Info();
    } else if (files.size() > 1) {
        qDebug() << files;
        qFatal("Located more than one output file, please clean up before running this test");
        return QBic::Info();
    }

    QString resultFileName = files.first();
    inf = bic.parseFile(resultFileName);

    QFile::remove(resultFileName);
    tmpQFile.close();

    return inf;
}

void tst_Bic::sizesAndVTables()
{
#if !defined(Q_CC_GNU) || defined(Q_CC_INTEL)
    QSKIP("Test not implemented for this compiler/platform", SkipAll);
#else

    QFETCH_GLOBAL(QString, libName);
    QFETCH(QString, oldLib);
    QFETCH(bool, isPatchRelease);

    bool isFailed = false;

    //qDebug() << oldLib.arg(libName);
    if (oldLib.isEmpty() || !QFile::exists(oldLib.arg(libName)))
        QSKIP("No platform spec found for this platform/version.", SkipSingle);

    const QBic::Info oldLibInfo = bic.parseFile(oldLib.arg(libName));
    QVERIFY(!oldLibInfo.classVTables.isEmpty());

    const QBic::Info currentLibInfo = getCurrentInfo(libName);
    QVERIFY(!currentLibInfo.classVTables.isEmpty());

    QBic::VTableDiff diff = bic.diffVTables(oldLibInfo, currentLibInfo);

    if (!diff.removedVTables.isEmpty()) {
        qWarning() << "VTables for the following classes were removed" << diff.removedVTables;
        isFailed = true;
    }

    if (!diff.modifiedVTables.isEmpty()) {
        if (diff.modifiedVTables.size() != 1 ||
            strcmp(QTest::currentDataTag(), "4.4") != 0 ||
            diff.modifiedVTables.at(0).first != "QGraphicsProxyWidget") {
            foreach(QStringPair entry, diff.modifiedVTables)
                qWarning() << "modified VTable:\n    Old: " << entry.first
                           << "\n    New: " << entry.second;
            isFailed = true;
        }
    }

    if (isPatchRelease && !diff.addedVTables.isEmpty()) {
        qWarning() << "VTables for the following classes were added in a patch release:"
                   << diff.addedVTables;
        isFailed = true;
    }

    if (isPatchRelease && !diff.reimpMethods.isEmpty()) {
        foreach(QStringPair entry, diff.reimpMethods)
            qWarning() << "reimplemented virtual in patch release:\n    Old: " << entry.first
                       << "\n    New: " << entry.second;
        isFailed = true;
    }

    QBic::SizeDiff sizeDiff = bic.diffSizes(oldLibInfo, currentLibInfo);
    if (!sizeDiff.mismatch.isEmpty()) {
        foreach (QString className, sizeDiff.mismatch)
            qWarning() << "size mismatch for" << className
                       << "old" << oldLibInfo.classSizes.value(className)
                       << "new" << currentLibInfo.classSizes.value(className);
        isFailed = true;
    }

#ifdef Q_CC_MINGW
    /**
     * These symbols are from Windows' imm.h header, and is available
     * conditionally depending on the value of the WINVER define. We pull
     * them out since they're not relevant to the testing done.
     */
    sizeDiff.removed.removeAll(QLatin1String("tagIMECHARPOSITION"));
    sizeDiff.removed.removeAll(QLatin1String("tagRECONVERTSTRING"));
#endif

    if (!sizeDiff.removed.isEmpty()) {
        qWarning() << "the following classes were removed:" << sizeDiff.removed;
        isFailed = true;
    }

    if (isPatchRelease && !sizeDiff.added.isEmpty()) {
        qWarning() << "the following classes were added in a patch release:" << sizeDiff.added;
        isFailed = true;
    }

    if (isFailed)
        QFAIL("Test failed, read warnings above.");
#endif
}

QTEST_APPLESS_MAIN(tst_Bic)

#include "tst_bic.moc"
#endif

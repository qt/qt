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
#include <qtest.h>
#include <QDeclarativeView>
#include <QApplication>
#include <QLibraryInfo>
#include <QDir>
#include <QDebug>
#include <QProcess>
#include <QFile>

enum Mode { Record, RecordNoVisuals, RecordSnapshot, Play, TestVisuals, RemoveVisuals, UpdateVisuals, UpdatePlatformVisuals, Test };

static QString testdir;
class tst_qmlvisual : public QObject
{
    Q_OBJECT
public:
    tst_qmlvisual();

    static QString toTestScript(const QString &, Mode=Test);
    static QString viewer();

    static QStringList findQmlFiles(const QDir &d);
private slots:
    void visual_data();
    void visual();

private:
    QString qmlruntime;
};


tst_qmlvisual::tst_qmlvisual()
{
    qmlruntime = viewer();
}

QString tst_qmlvisual::viewer()
{
    QString binaries = QLibraryInfo::location(QLibraryInfo::BinariesPath);

    QString qmlruntime;

#if defined(Q_WS_MAC)
    qmlruntime = QDir(binaries).absoluteFilePath("QMLViewer.app/Contents/MacOS/QMLViewer");
#elif defined(Q_WS_WIN) || defined(Q_WS_S60)
    qmlruntime = QDir(binaries).absoluteFilePath("qmlviewer.exe");
#else
    qmlruntime = QDir(binaries).absoluteFilePath("qmlviewer");
#endif

    return qmlruntime;
}

void tst_qmlvisual::visual_data()
{
    QTest::addColumn<QString>("file");
    QTest::addColumn<QString>("testdata");

    QStringList files;
    if (qgetenv("QMLVISUAL_ALL") != "")
        files << findQmlFiles(QDir(QT_TEST_SOURCE_DIR));
    else {
        //these are newly added tests we want to try out in CI (then move to the stable list)
        files << QT_TEST_SOURCE_DIR "/animation/qtbug10586/qtbug10586.qml";
        files << QT_TEST_SOURCE_DIR "/qdeclarativeborderimage/animated.qml";
        files << QT_TEST_SOURCE_DIR "/qdeclarativeflipable/test-flipable.qml";
        files << QT_TEST_SOURCE_DIR "/qdeclarativepositioners/usingRepeater.qml";
        files << QT_TEST_SOURCE_DIR "/animation/parentAnimation2/parentAnimation2.qml";

        //these are tests we think are stable and useful enough to be run by the CI system
        files << QT_TEST_SOURCE_DIR "/animation/bindinganimation/bindinganimation.qml";
        files << QT_TEST_SOURCE_DIR "/animation/loop/loop.qml";
        files << QT_TEST_SOURCE_DIR "/animation/parallelAnimation/parallelAnimation-visual.qml";
        files << QT_TEST_SOURCE_DIR "/animation/parentAnimation/parentAnimation-visual.qml";
        files << QT_TEST_SOURCE_DIR "/animation/reanchor/reanchor.qml";
        files << QT_TEST_SOURCE_DIR "/animation/scriptAction/scriptAction-visual.qml";
        files << QT_TEST_SOURCE_DIR "/qdeclarativemousearea/drag.qml";
        files << QT_TEST_SOURCE_DIR "/fillmode/fillmode.qml";

        // new tests
        files << QT_TEST_SOURCE_DIR "/qdeclarativemousearea/mousearea-flickable.qml";

        //these reliably fail in CI, for unknown reasons
        //files << QT_TEST_SOURCE_DIR "/animation/easing/easing.qml";
        //files << QT_TEST_SOURCE_DIR "/animation/pauseAnimation/pauseAnimation-visual.qml";
        //files << QT_TEST_SOURCE_DIR "/qdeclarativeborderimage/borders.qml";
        //files << QT_TEST_SOURCE_DIR "/qdeclarativeborderimage/animated-smooth.qml";

        //these reliably fail on Linux because of color interpolation (different float rounding)
#if !defined(Q_WS_X11) && !defined(Q_WS_QWS)
        files << QT_TEST_SOURCE_DIR "/animation/colorAnimation/colorAnimation-visual.qml";
        files << QT_TEST_SOURCE_DIR "/animation/propertyAction/propertyAction-visual.qml";
#endif

        //this is unstable because the MouseArea press-and-hold timer is not synchronized to the animation framework.
        //files << QT_TEST_SOURCE_DIR "/qdeclarativemousearea/mousearea-visual.qml";
    }

    foreach (const QString &file, files) {
        QString testdata = toTestScript(file);
        if (testdata.isEmpty())
            continue;

        QTest::newRow(file.toLatin1().constData()) << file << testdata;
    }
}

void tst_qmlvisual::visual()
{
    QFETCH(QString, file);
    QFETCH(QString, testdata);

    QStringList arguments;
    arguments << "-script" << testdata
              << "-scriptopts" << "play,testimages,testerror,exitoncomplete,exitonfailure" 
              << file;
#ifdef Q_WS_QWS
    arguments << "-qws";
#endif

    QProcess p;
    p.start(qmlruntime, arguments);
    QVERIFY(p.waitForFinished());
    if (p.exitCode() != 0)
        qDebug() << p.readAllStandardError();
    QCOMPARE(p.exitStatus(), QProcess::NormalExit);
    QCOMPARE(p.exitCode(), 0);
}

QString tst_qmlvisual::toTestScript(const QString &file, Mode mode)
{
    if (!file.endsWith(".qml"))
        return QString();

    int index = file.lastIndexOf(QDir::separator());
    if (index == -1)
        index = file.lastIndexOf('/');
    if (index == -1)
        return QString();

    const char* platformsuffix=0; // platforms with different fonts
#if defined(Q_WS_MACX)
    platformsuffix = "-MAC";
#elif defined(Q_WS_X11)
    platformsuffix = "-X11";
#elif defined(Q_WS_WIN32)
    platformsuffix = "-WIN";
#elif defined(Q_WS_QWS)
    platformsuffix = "-QWS";
#elif defined(Q_WS_S60)
    platformsuffix = "-S60";
#endif

    QString testdata = file.left(index + 1) + 
                       QString("data");
    QString testname = file.mid(index + 1, file.length() - index - 5);

    if (platformsuffix && (mode == UpdatePlatformVisuals || QFile::exists(testdata+QLatin1String(platformsuffix)+QDir::separator()+testname+".qml"))) {
        QString platformdir = testdata + QLatin1String(platformsuffix);
        if (mode == UpdatePlatformVisuals) {
            Q_ASSERT(QDir().mkpath(platformdir));
            // Copy from base
            QDir dir(testdata,testname+".*");
            dir.setFilter(QDir::Files);
            QFileInfoList list = dir.entryInfoList();
            for (int i = 0; i < list.size(); ++i) {
                QFile in(list.at(i).filePath());
                Q_ASSERT(in.open(QIODevice::ReadOnly));
                QFile out(platformdir + QDir::separator() + list.at(i).fileName());
                Q_ASSERT(out.open(QIODevice::WriteOnly));
                out.write(in.readAll());
            }
        }
        testdata = platformdir;
    }

    testdata += QDir::separator() + testname;

    return testdata;
}

QStringList tst_qmlvisual::findQmlFiles(const QDir &d)
{
    QStringList rv;

    QStringList files = d.entryList(QStringList() << QLatin1String("*.qml"), 
                                    QDir::Files);
    foreach (const QString &file, files) {
        if (file.at(0).isLower()) {
            rv << d.absoluteFilePath(file);
        }
    }

    QStringList dirs = d.entryList(QDir::Dirs | QDir::NoDotAndDotDot | 
                                   QDir::NoSymLinks);
    foreach (const QString &dir, dirs) {
        if (dir.left(4) == "data")
            continue;

        QDir sub = d;
        sub.cd(dir);
        rv << findQmlFiles(sub);
    }

    return rv;
}

void action(Mode mode, const QString &file)
{
    Q_ASSERT(mode != Test);

    QString testdata = tst_qmlvisual::toTestScript(file,mode);

    QStringList arguments;
    switch (mode) {
        case Test:
            // Don't run qml
            break;
        case Record:
            arguments << "-script" << testdata
                  << "-scriptopts" << "record,testimages,saveonexit"
                  << file;
            break;
        case RecordNoVisuals:
            arguments << "-script" << testdata
                  << "-scriptopts" << "record,saveonexit"
                  << file;
            break;
        case RecordSnapshot:
            arguments << "-script" << testdata
                  << "-scriptopts" << "record,testimages,snapshot,saveonexit"
                  << file;
            break;
        case Play:
            arguments << "-script" << testdata
                  << "-scriptopts" << "play,testimages,testerror,exitoncomplete"
                  << file;
            break;
        case TestVisuals:
        arguments << "-script" << testdata
                  << "-scriptopts" << "play"
                  << file;
            break;
        case UpdateVisuals:
        case UpdatePlatformVisuals:
            arguments << "-script" << testdata
                  << "-scriptopts" << "play,record,testimages,exitoncomplete,saveonexit"
                  << file;
            break;
        case RemoveVisuals:
            arguments << "-script" << testdata
                  << "-scriptopts" << "play,record,exitoncomplete,saveonexit"
                  << file;
            break;
    }
    if (!arguments.isEmpty()) {
        QProcess p;
        p.setProcessChannelMode(QProcess::ForwardedChannels);
        p.start(tst_qmlvisual::viewer(), arguments);
        p.waitForFinished();
    }
}

void usage()
{
    fprintf(stderr, "\n");
    fprintf(stderr, "QML related options\n");
    fprintf(stderr, " -listtests                  : list all the tests seen by tst_qmlvisual, and then exit immediately\n");
    fprintf(stderr, " -record file                : record new test data for file\n");
    fprintf(stderr, " -recordnovisuals file       : record new test data for file, but ignore visuals\n");
    fprintf(stderr, " -recordsnapshot file        : record new snapshot for file (like record but only records a single frame and then exits)\n");
    fprintf(stderr, " -play file                  : playback test data for file, printing errors\n");
    fprintf(stderr, " -testvisuals file           : playback test data for file, without errors\n");
    fprintf(stderr, " -updatevisuals file         : playback test data for file, accept new visuals for file\n");
    fprintf(stderr, " -updateplatformvisuals file : playback test data for file, accept new visuals for file only on current platform (MacOSX/Win32/X11/QWS/S60)\n");
    fprintf(stderr, "\n"
        "Visual tests are recordings of manual interactions with a QML test,\n"
        "that can then be run automatically. To record a new test, run:\n"
        "\n"
        "  tst_qmlvisual -record yourtestdir/yourtest.qml\n"
        "\n"
        "This records everything you do (try to keep it short).\n"
        "To play back a test, run:\n"
        "\n"
        "  tst_qmlvisual -play yourtestdir/yourtest.qml\n"
        "\n"
        "Your test may include QML code to test itself, reporting any error to an\n"
        "'error' property on the root object - the test will fail if this property\n"
        "gets set to anything non-empty.\n"
        "\n"
        "If your test changes slightly but is still correct (check with -play), you\n"
        "can update the visuals by running:\n"
        "\n"
        "  tst_qmlvisual -updatevisuals yourtestdir/yourtest.qml\n"
        "\n"
        "If your test includes platform-sensitive visuals (eg. text in system fonts),\n"
        "you should create platform-specific visuals, using -updateplatformvisuals\n"
        "instead.\n"
        "\n"
        "If you ONLY wish to use the 'error' property, you can record your test with\n"
        "-recordnovisuals, or discard existing visuals with -removevisuals; the test\n"
        "will then only fail on a syntax error, crash, or non-empty 'error' property.\n"
    );
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    Mode mode = Test;
    QString file;
    bool showHelp = false;

    int newArgc = 1;
    char **newArgv = new char*[argc];
    newArgv[0] = argv[0];

    for (int ii = 1; ii < argc; ++ii) {
        QString arg(argv[ii]);
        if (arg == "-play" && (ii + 1) < argc) {
            mode = Play;
            file = argv[++ii];
        } else if (arg == "-record" && (ii + 1) < argc) {
            mode = Record;
            file = argv[++ii];
        } else if (arg == "-recordnovisuals" && (ii + 1) < argc) {
            mode = RecordNoVisuals;
            file = argv[++ii];
        }  else if (arg == "-recordsnapshot" && (ii + 1) < argc) {
            mode = RecordSnapshot;
            file = argv[++ii];
        } else if (arg == "-testvisuals" && (ii + 1) < argc) {
            mode = TestVisuals;
            file = argv[++ii];
        } else if (arg == "-removevisuals" && (ii + 1) < argc) {
            mode = RemoveVisuals;
            file = argv[++ii];
        } else if (arg == "-updatevisuals" && (ii + 1) < argc) {
            mode = UpdateVisuals;
            file = argv[++ii];
        } else if (arg == "-updateplatformvisuals" && (ii + 1) < argc) {
            mode = UpdatePlatformVisuals;
            file = argv[++ii];
        } else {
            newArgv[newArgc++] = argv[ii];
        }
        
        if (arg == "-help" || arg == "-?" || arg == "--help") {
            atexit(usage);
            showHelp = true;
        }

        if (arg == "-listtests") {
            QStringList list = tst_qmlvisual::findQmlFiles(QDir(QT_TEST_SOURCE_DIR));
            foreach (QString test, list) {
                qWarning() << qPrintable(test);
            }
            return 0;
        }
    }

    if (mode == Test || showHelp) {
        tst_qmlvisual tc;
        return QTest::qExec(&tc, newArgc, newArgv);
    } else {
        if (!file.endsWith(QLatin1String(".qml"))) {
            qWarning() << "Error: Invalid file name (must end in .qml)";
            return -1;
        }
        QDir d = QDir::current();
        QString absFile = d.absoluteFilePath(file);
        if (!QFile::exists(absFile)) {
            qWarning() << "Error: File does not exist";
            return -1;
        }

        action(mode, absFile);
        return 0;
    }
}

#include "tst_qmlvisual.moc"

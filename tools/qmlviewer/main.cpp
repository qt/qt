/****************************************************************************
**
** Copyright (C) 1992-$THISYEAR$ $TROLLTECH$. All rights reserved.
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "qml.h"
#include "qmlviewer.h"
#include <QWidget>
#include <QDir>
#include "qfxtestengine.h"
#include <QApplication>
#include <QTranslator>
#include <QDebug>

void usage()
{
    qWarning("Usage: qmlviewer [options] <filename>");
    qWarning(" ");
    qWarning(" options:");
    qWarning("  -v, -version ............................. display version");
    qWarning("  -frameless ............................... run with no window frame");
    qWarning("  -skin <qvfbskindir> ...................... run with a skin window frame");
    qWarning("  -recordfile <output> ..................... set video recording file");
    qWarning("                                              - ImageMagick 'convert' for GIF)");
    qWarning("                                              - png file for raw frames");
    qWarning("                                              - 'ffmpeg' for other formats");
    qWarning("  -recorddither ordered|threshold|floyd .... set GIF dither recording mode");
    qWarning("  -recordperiod <milliseconds> ............. set time between recording frames");
    qWarning("  -record arg .............................. add a recording process argument");
    qWarning("  -autorecord [from-]<tomilliseconds> ...... set recording to start and stop");
    qWarning("  -devicekeys .............................. use numeric keys (see F1)");
    qWarning("  -cache ................................... disk cache remote content");
    qWarning("  -recordtest <directory> .................. record an autotest");
    qWarning("  -runtest <directory> ..................... run a previously recorded test");
    qWarning("  -translation <translationfile> ........... set the language to run in");
    qWarning("  -L <directory> ........................... prepend to the library search path");
    qWarning(" ");
    qWarning(" Press F1 for interactive help");
    exit(1);
}

int main(int argc, char ** argv)
{
    //### default to using raster graphics backend for now
    bool gsSpecified = false;
    for (int i = 0; i < argc; ++i) {
        QString arg = argv[i];
        if (arg == "-graphicssystem") {
            gsSpecified = true;
            break;
        }
    }
    if (!gsSpecified)
        QApplication::setGraphicsSystem("raster");

    QApplication app(argc, argv);
    app.setApplicationName("viewer");

    bool frameless = false;
    QString fileName;
    int period = 0;
    int autorecord_from = 0;
    int autorecord_to = 0;
    QString dither = "none";
    QString recordfile;
    QStringList recordargs;
    QStringList libraries;
    QString skin;
    bool devkeys = false;
    bool cache = false;
    QFxTestEngine::TestMode testMode = QFxTestEngine::NoTest;
    QString testDir;
    QString translationFile;

    for (int i = 1; i < argc; ++i) {
        QString arg = argv[i];
        if (arg == "-frameless") {
            frameless = true;
        } else if (arg == "-skin") {
            skin = QString(argv[++i]);
        } else if (arg == "-cache") {
            cache = true;
        } else if (arg == "-recordperiod") {
            period = QString(argv[++i]).toInt();
        } else if (arg == "-recordfile") {
            recordfile = QString(argv[++i]);
        } else if (arg == "-record") {
            recordargs << QString(argv[++i]);
        } else if (arg == "-recorddither") {
            dither = QString(argv[++i]);
        } else if (arg == "-autorecord") {
            QString range = QString(argv[++i]);
            int dash = range.indexOf('-');
            if (dash > 0)
                autorecord_from = range.left(dash).toInt();
            autorecord_to = range.mid(dash+1).toInt();
        } else if (arg == "-devicekeys") {
            devkeys = true;
        } else if (arg == "-recordtest") {
            testMode = QFxTestEngine::RecordTest;
            if(i + 1 >= argc)
                usage();
            testDir = argv[i + 1];
            ++i;
        } else if (arg == "-runtest") {
            testMode = QFxTestEngine::PlaybackTest;
            if(i + 1 >= argc)
                usage();
            testDir = argv[i + 1];
            ++i;
        } else if (arg == QLatin1String("-v") || arg == QLatin1String("-version")) {
            fprintf(stderr, "Qt Declarative UI Viewer version %s\n", QT_VERSION_STR);
            return 0;
        } else if (arg == "-translation") {
            if(i + 1 >= argc)
                usage();
            translationFile = argv[i + 1];
            ++i;
        } else if (arg == "-L") {
            libraries << QString(argv[++i]);
        } else if (arg[0] != '-') {
            fileName = arg;
        } else if (1 || arg == "-help") {
            usage();
        }
    }

    QTranslator qmlTranslator;
    if (!translationFile.isEmpty()) {
        qmlTranslator.load(translationFile);
        app.installTranslator(&qmlTranslator);
    }

    QmlViewer viewer(testMode, testDir, 0, frameless ? Qt::FramelessWindowHint : Qt::Widget);
    foreach (QString lib, libraries)
        viewer.addLibraryPath(lib);
    viewer.setCacheEnabled(cache);
    viewer.setRecordFile(recordfile);
    if (period>0)
        viewer.setRecordPeriod(period);
    if (autorecord_to)
        viewer.setAutoRecord(autorecord_from,autorecord_to);
    if (!skin.isEmpty() && QDir(skin).exists())
        viewer.setSkin(skin);
    if (devkeys)
        viewer.setDeviceKeys(true);
    viewer.setRecordDither(dither);
    viewer.setRecordArgs(recordargs);
    if (!fileName.isEmpty()) {
        viewer.openQml(fileName);
        viewer.show();
    } else {
        viewer.show();
        viewer.open();
    }

    return app.exec();
}


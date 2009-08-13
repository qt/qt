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
    qWarning("                                             \"list\" for a list of built-ins");
    qWarning("  -resizeview .............................. resize the view, not the skin");
    qWarning("  -recordfile <output> ..................... set video recording file");
    qWarning("                                              - ImageMagick 'convert' for GIF)");
    qWarning("                                              - png file for raw frames");
    qWarning("                                              - 'ffmpeg' for other formats");
    qWarning("  -recorddither ordered|threshold|floyd .... set GIF dither recording mode");
    qWarning("  -recordrate <fps> ........................ set recording frame rate");
    qWarning("  -record arg .............................. add a recording process argument");
    qWarning("  -autorecord [from-]<tomilliseconds> ...... set recording to start and stop");
    qWarning("  -devicekeys .............................. use numeric keys (see F1)");
    qWarning("  -netcache <size> ......................... set disk cache to size bytes");
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
    app.setOrganizationName("Nokia");
    app.setOrganizationDomain("nokia.com");

    bool frameless = false;
    bool resizeview = false;
    QString fileName;
    double fps = 0;
    int autorecord_from = 0;
    int autorecord_to = 0;
    QString dither = "none";
    QString recordfile;
    QStringList recordargs;
    QStringList libraries;
    QString skin;
    bool devkeys = false;
    int cache = 0;
    QString translationFile;

    for (int i = 1; i < argc; ++i) {
        QString arg = argv[i];
        if (arg == "-frameless") {
            frameless = true;
        } else if (arg == "-skin") {
            skin = QString(argv[++i]);
        } else if (arg == "-resizeview") {
            resizeview = true;
        } else if (arg == "-netcache") {
            cache = QString(argv[++i]).toInt();
        } else if (arg == "-recordrate") {
            fps = QString(argv[++i]).toDouble();
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

    QmlViewer viewer(0, frameless ? Qt::FramelessWindowHint : Qt::Widget);
    foreach (QString lib, libraries)
        viewer.addLibraryPath(lib);
    viewer.setNetworkCacheSize(cache);
    viewer.setRecordFile(recordfile);
    if (resizeview)
        viewer.setScaleView();
    if (fps>0)
        viewer.setRecordRate(fps);
    if (autorecord_to)
        viewer.setAutoRecord(autorecord_from,autorecord_to);
    if (!skin.isEmpty()) {
        if (skin == "list") {
            foreach (QString s, viewer.builtinSkins())
                qWarning(s.toUtf8());
            exit(0);
        } else {
            viewer.setSkin(skin);
        }
    }
    if (devkeys)
        viewer.setDeviceKeys(true);
    viewer.setRecordDither(dither);
    if (recordargs.count())
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

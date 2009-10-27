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
    qWarning("  -maximized................................ run maximized");
    qWarning("  -fullscreen............................... run fullscreen");
    qWarning("  -stayontop................................ keep viewer window on top");
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
    qWarning("  -opengl .................................. use a QGLWidget for the viewport");
    qWarning("  -script <path> ........................... set the script to use");
    qWarning("  -scriptopts <options>|help ............... set the script options to use");

    qWarning(" ");
    qWarning(" Press F1 for interactive help");
    exit(1);
}

void scriptOptsUsage()
{
    qWarning("Usage: qmlviewer -scriptopts <option>[,<option>...] ...");
    qWarning(" options:");
    qWarning("  record ................................... record a new script");
    qWarning("  play ..................................... playback an existing script");
    qWarning("  testimages ............................... compare images on playback");
    qWarning("  exitoncomplete ........................... cleanly exit the viewer on script completion");
    qWarning("  exitonfailure ............................ immediately exit the viewer on script failure");
    qWarning("  saveonexit ............................... save recording on viewer exit");
    qWarning(" ");
    qWarning(" One of record, play or both must be specified.");
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
    QString script;
    QString scriptopts;
    bool runScript = false;
    bool devkeys = false;
    int cache = 0;
    QString translationFile;
    bool useGL = false;
    bool fullScreen = false;
    bool stayOnTop = false;
    bool maximized = false;

    for (int i = 1; i < argc; ++i) {
        bool lastArg = (i == argc - 1);
        QString arg = argv[i];
        if (arg == "-frameless") {
            frameless = true;
        } else if (arg == "-maximized") {
            maximized = true;
        } else if (arg == "-fullscreen") {
            fullScreen = true;
        } else if (arg == "-stayontop") {
            stayOnTop = true;
        } else if (arg == "-skin") {
            if (lastArg) usage();
            skin = QString(argv[++i]);
        } else if (arg == "-resizeview") {
            resizeview = true;
        } else if (arg == "-netcache") {
            if (lastArg) usage();
            cache = QString(argv[++i]).toInt();
        } else if (arg == "-recordrate") {
            if (lastArg) usage();
            fps = QString(argv[++i]).toDouble();
        } else if (arg == "-recordfile") {
            if (lastArg) usage();
            recordfile = QString(argv[++i]);
        } else if (arg == "-record") {
            if (lastArg) usage();
            recordargs << QString(argv[++i]);
        } else if (arg == "-recorddither") {
            if (lastArg) usage();
            dither = QString(argv[++i]);
        } else if (arg == "-autorecord") {
            if (lastArg) usage();
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
            if (lastArg) usage();
            translationFile = argv[++i];
        } else if (arg == "-opengl") {
            useGL = true;
        } else if (arg == "-L") {
            if (lastArg) usage();
            libraries << QString(argv[++i]);
        } else if (arg == "-script") {
            if (lastArg) usage();
            script = QString(argv[++i]);
        } else if (arg == "-scriptopts") {
            if (lastArg) usage();
            scriptopts = QString(argv[++i]);
        } else if (arg == "-savescript") {
            if (lastArg) usage();
            script = QString(argv[++i]);
            runScript = false;
        } else if (arg == "-playscript") {
            if (lastArg) usage();
            script = QString(argv[++i]);
            runScript = true;
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

    Qt::WFlags wflags = (frameless ? Qt::FramelessWindowHint : Qt::Widget);
    if (stayOnTop)
        wflags |= Qt::WindowStaysOnTopHint;
        
    QmlViewer viewer(0, wflags);
    if (!scriptopts.isEmpty()) {
        QStringList options = 
            scriptopts.split(QLatin1Char(','), QString::SkipEmptyParts);

        QmlViewer::ScriptOptions scriptOptions = 0;
        for (int i = 0; i < options.count(); ++i) {
            const QString &option = options.at(i);
            if (option == QLatin1String("help")) {
                scriptOptsUsage();
            } else if (option == QLatin1String("play")) {
                scriptOptions |= QmlViewer::Play;
            } else if (option == QLatin1String("record")) {
                scriptOptions |= QmlViewer::Record;
            } else if (option == QLatin1String("testimages")) {
                scriptOptions |= QmlViewer::TestImages;
            } else if (option == QLatin1String("exitoncomplete")) {
                scriptOptions |= QmlViewer::ExitOnComplete;
            } else if (option == QLatin1String("exitonfailure")) {
                scriptOptions |= QmlViewer::ExitOnFailure;
            } else if (option == QLatin1String("saveonexit")) {
                scriptOptions |= QmlViewer::SaveOnExit;
            } else {
                scriptOptsUsage();
            }
        }

        if (script.isEmpty())
            usage();

        if (!(scriptOptions & QmlViewer::Record) && !(scriptOptions & QmlViewer::Play))
            scriptOptsUsage();
        viewer.setScriptOptions(scriptOptions);
        viewer.setScript(script);
    }  else if (!script.isEmpty()) {
        usage();
    }

    viewer.setUseGL(useGL);
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
    if (fullScreen && maximized)
        qWarning() << "Both -fullscreen and -maximized specified. Using -fullscreen.";
    if (!fileName.isEmpty()) {
        viewer.openQml(fileName);
        fullScreen ? viewer.showFullScreen() : maximized ? viewer.showMaximized() : viewer.show();
    } else {
        fullScreen ? viewer.showFullScreen() : maximized ? viewer.showMaximized() : viewer.show();
        viewer.open();
    }
    viewer.raise();

    return app.exec();
}

/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "windowmanager.h"

#include <QtCore/QDir>
#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QProcess>
#include <QtCore/QByteArray>
#include <QtCore/QLibraryInfo>
#include <QtCore/QVariant>
#include <QtCore/QDateTime>
#include <QtCore/QMap>

// AppLaunch: Launch gui applications, keep them running a while
// (grabbing their top level from the window manager) and send
// them a Close event via window manager. Verify that they do not
// not crash nor produces unexpected error output.
// Note: Do not play with the machine while it is running as otherwise
// the top-level find algorithm might get confused (especially on Windows).
// Environment variables are checked to turned off some tests
// It is currently implemented for X11 and Windows, pending an
// implementation of the WindowManager class and deployment on
// the other platforms.

enum  { defaultUpTimeMS = 3000, defaultTopLevelWindowTimeoutMS = 30000,
        defaultTerminationTimeoutMS = 35000 };

// List the examples to test (Gui examples only).
struct Example {
    const char *name;
    const char *directory;
    const char *binary;
    unsigned priority; // 0-highest
    int upTimeMS;
};

const struct Example examples[] = {
    {"animation/animatedtiles Example", "animation/animatedtiles", "animatedtiles", 0, -1},
    {"animation/appchooser Example", "animation/appchooser", "appchooser", 10, -1},
    {"animation/easing Example", "animation/easing", "easing", 10, -1},
    {"animation/moveblocks Example", "animation/moveblocks", "moveblocks", 10, -1},
    {"animation/states Example", "animation/states", "states", 10, -1},
    {"animation/stickman Example", "animation/stickman", "stickman", 10, -1},
    {"designer/calculatorbuilder Example", "designer/calculatorbuilder", "calculatorbuilder", 10, -1},
    {"dialogs/standarddialogs Example", "dialogs/standarddialogs", "standarddialogs", 10, -1},
    {"draganddrop/dropsite Example", "draganddrop/dropsite", "dropsite", 10, -1},
    {"draganddrop/fridgemagnets Example", "draganddrop/fridgemagnets", "fridgemagnets", 10, -1},
    {"draganddrop/puzzle Example", "draganddrop/puzzle", "puzzle", 10, -1},
    {"effects/blurpicker Example", "effects/blurpicker", "blurpicker", 10, -1},
    {"effects/customshader Example", "effects/customshader", "customshader", 10, -1},
    {"effects/fademessage Example", "effects/fademessage", "fademessage", 10, -1},
    {"effects/lighting Example", "effects/lighting", "lighting", 10, -1},
    {"graphicsview/anchorlayout Example", "graphicsview/anchorlayout", "anchorlayout", 10, -1},
    {"graphicsview/basicgraphicslayouts Example", "graphicsview/basicgraphicslayouts", "basicgraphicslayouts", 0, -1},
    {"graphicsview/collidingmice Example", "graphicsview/collidingmice", "collidingmice", 10, -1},
    {"graphicsview/diagramscene Example", "graphicsview/diagramscene", "diagramscene", 10, -1},
    {"graphicsview/dragdroprobot Example", "graphicsview/dragdroprobot", "dragdroprobot", 10, -1},
    {"graphicsview/elasticnodes Example", "graphicsview/elasticnodes", "elasticnodes", 10, -1},
    {"graphicsview/flowlayout Example", "graphicsview/flowlayout", "flowlayout", 10, -1},
    {"graphicsview/padnavigator Example", "graphicsview/padnavigator", "padnavigator", 0, -1},
    {"graphicsview/portedasteroids Example", "graphicsview/portedasteroids", "portedasteroids", 10, -1},
    {"graphicsview/portedcanvas Example", "graphicsview/portedcanvas", "portedcanvas", 10, -1},
    {"graphicsview/weatheranchorlayout Example", "graphicsview/weatheranchorlayout", "weatheranchorlayout", 10, -1},
    {"itemviews/addressbook Example", "itemviews/addressbook", "addressbook", 0, -1},
    {"itemviews/basicsortfiltermodel Example", "itemviews/basicsortfiltermodel", "basicsortfiltermodel", 10, -1},
    {"itemviews/chart Example", "itemviews/chart", "chart", 0, -1},
    {"itemviews/coloreditorfactory Example", "itemviews/coloreditorfactory", "coloreditorfactory", 10, -1},
    {"itemviews/combowidgetmapper Example", "itemviews/combowidgetmapper", "combowidgetmapper", 6, -1},
    {"itemviews/customsortfiltermodel Example", "itemviews/customsortfiltermodel", "customsortfiltermodel", 6, -1},
    {"itemviews/dirview Example", "itemviews/dirview", "dirview", 0, -1},
    {"itemviews/editabletreemodel Example", "itemviews/editabletreemodel", "editabletreemodel", 0, -1},
    {"itemviews/fetchmore Example", "itemviews/fetchmore", "fetchmore", 10, -1},
    {"itemviews/frozencolumn Example", "itemviews/frozencolumn", "frozencolumn", 10, -1},
    {"itemviews/pixelator Example", "itemviews/pixelator", "pixelator", 10, -1},
    {"itemviews/puzzle Example", "itemviews/puzzle", "puzzle", 10, -1},
    {"itemviews/simpledommodel Example", "itemviews/simpledommodel", "simpledommodel", 10, -1},
    {"itemviews/simpletreemodel Example", "itemviews/simpletreemodel", "simpletreemodel", 10, -1},
    {"itemviews/simplewidgetmapper Example", "itemviews/simplewidgetmapper", "simplewidgetmapper", 10, -1},
    {"itemviews/spinboxdelegate Example", "itemviews/spinboxdelegate", "spinboxdelegate", 0, -1},
    {"itemviews/stardelegate Example", "itemviews/stardelegate", "stardelegate", 10, -1},
    {"layouts/basiclayouts Example", "layouts/basiclayouts", "basiclayouts", 0, -1},
    {"layouts/borderlayout Example", "layouts/borderlayout", "borderlayout", 10, -1},
    {"layouts/dynamiclayouts Example", "layouts/dynamiclayouts", "dynamiclayouts", 10, -1},
    {"layouts/flowlayout Example", "layouts/flowlayout", "flowlayout", 10, -1},
    {"mainwindows/application Example", "mainwindows/application", "application", 6, -1},
    {"mainwindows/dockwidgets Example", "mainwindows/dockwidgets", "dockwidgets", 0, -1},
    {"mainwindows/mdi Example", "mainwindows/mdi", "mdi", 0, -1},
    {"mainwindows/menus Example", "mainwindows/menus", "menus", 10, -1},
    {"mainwindows/recentfiles Example", "mainwindows/recentfiles", "recentfiles", 10, -1},
    {"mainwindows/sdi Example", "mainwindows/sdi", "sdi", 10, -1},
    {"touch/dials Example", "touch/dials", "dials", 10, -1},
    {"touch/fingerpaint Example", "touch/fingerpaint", "fingerpaint", 10, -1},
    {"touch/knobs Example", "touch/knobs", "knobs", 10, -1},
    {"touch/pinchzoom Example", "touch/pinchzoom", "pinchzoom", 10, -1},
    {"opengl/2dpainting Example", "opengl/2dpainting", "2dpainting", 10, -1},
    {"opengl/grabber Example", "opengl/grabber", "grabber", 10, -1},
    {"opengl/hellogl Example", "opengl/hellogl", "hellogl", 10, -1},
    {"opengl/overpainting Example", "opengl/overpainting", "overpainting", 10, -1},
    {"opengl/samplebuffers Example", "opengl/samplebuffers", "samplebuffers", 10, -1},
    {"opengl/textures Example", "opengl/textures", "textures", 10, -1},
    {"painting/basicdrawing Example", "painting/basicdrawing", "basicdrawing", 10, -1},
    {"painting/concentriccircles Example", "painting/concentriccircles", "concentriccircles", 0, -1},
    {"painting/fontsampler Example", "painting/fontsampler", "fontsampler", 0, -1},
    {"painting/imagecomposition Example", "painting/imagecomposition", "imagecomposition", 10, -1},
    {"painting/painterpaths Example", "painting/painterpaths", "painterpaths", 10, -1},
    {"painting/svggenerator Example", "painting/svggenerator", "svggenerator", 10, -1},
    {"painting/svgviewer Example", "painting/svgviewer", "svgviewer", 0, -1},
    {"painting/transformations Example", "painting/transformations", "transformations", 0, -1},
    {"qtconcurrent/imagescaling Example", "qtconcurrent/imagescaling", "imagescaling", 10, -1},
    {"richtext/calendar Example", "richtext/calendar", "calendar", 0, -1},
    {"richtext/orderform Example", "richtext/orderform", "orderform", 10, -1},
    {"richtext/syntaxhighlighter Example", "richtext/syntaxhighlighter", "syntaxhighlighter", 0, -1},
    {"richtext/textobject Example", "richtext/textobject", "textobject", 10, -1},
    {"script/calculator Example", "script/calculator", "calculator", 6, -1},
    {"script/qstetrix Example", "script/qstetrix", "qstetrix", 0, -1},
    {"statemachine/eventtransitions Example", "statemachine/eventtransitions", "eventtransitions", 10, -1},
    {"statemachine/rogue Example", "statemachine/rogue", "rogue", 10, -1},
    {"statemachine/trafficlight Example", "statemachine/trafficlight", "trafficlight", 0, -1},
    {"statemachine/twowaybutton Example", "statemachine/twowaybutton", "twowaybutton", 10, -1},
    {"tutorials/addressbook/part7 Example", "tutorials/addressbook/part7", "part7", 0, -1},
    {"webkit/fancybrowser Example", "webkit/fancybrowser", "fancybrowser", 0, 7000},
    {"widgets/analogclock Example", "widgets/analogclock", "analogclock", 6, -1},
    {"widgets/calculator Example", "widgets/calculator", "calculator", 6, -1},
    {"widgets/calendarwidget Example", "widgets/calendarwidget", "calendarwidget", 10, -1},
    {"widgets/charactermap Example", "widgets/charactermap", "charactermap", 10, -1},
    {"widgets/codeeditor Example", "widgets/codeeditor", "codeeditor", 0, -1},
    {"widgets/digitalclock Example", "widgets/digitalclock", "digitalclock", 10, -1},
    {"widgets/groupbox Example", "widgets/groupbox", "groupbox", 10, -1},
    {"widgets/icons Example", "widgets/icons", "icons", 10, -1},
    {"widgets/imageviewer Example", "widgets/imageviewer", "imageviewer", 10, -1},
    {"widgets/lineedits Example", "widgets/lineedits", "lineedits", 10, -1},
    {"widgets/scribble Example", "widgets/scribble", "scribble", 10, -1},
    {"widgets/sliders Example", "widgets/sliders", "sliders", 10, -1},
    {"widgets/spinboxes Example", "widgets/spinboxes", "spinboxes", 10, -1},
    {"widgets/styles Example", "widgets/styles", "styles", 0, -1},
    {"widgets/stylesheet Example", "widgets/stylesheet", "stylesheet", 0, -1},
    {"widgets/tablet Example", "widgets/tablet", "tablet", 10, -1},
    {"widgets/tetrix Example", "widgets/tetrix", "tetrix", 0, -1},
    {"widgets/tooltips Example", "widgets/tooltips", "tooltips", 10, -1},
    {"widgets/validators Example", "widgets/validators", "validators", 10, -1},
    {"widgets/wiggly Example", "widgets/wiggly", "wiggly", 10, -1}
};

const struct Example demos[] = {
    {"Affine Demo", "affine", "affine", 0, -1},
    {"Books Demo", "books", "books", 0, -1},
    {"Browser Demo", "browser", "browser", 0, 0000},
    {"Chip Demo", "chip", "chip", 0, -1},
    {"Composition Demo", "composition", "composition", 0, -1},
    {"Deform Demo", "deform", "deform", 0, -1},
    {"Embeddeddialogs Demo", "embeddeddialogs", "embeddeddialogs", 0, -1},
    {"Gradients Demo", "gradients", "gradients", 0, -1},
    {"Interview Demo", "interview", "interview", 0, -1},
    {"Mainwindow Demo", "mainwindow", "mainwindow", 0, -1},
    {"PathStroke Demo", "pathstroke", "pathstroke", 0, -1},
    {"Spreadsheet Demo", "spreadsheet", "spreadsheet", 0, -1},
    {"Sub-Attac Demo", "sub-attaq", "sub-attaq", 0, -1},
    {"TextEdit Demo", "textedit", "textedit", 0, -1},
    {"Undo Demo", "undo", "undo", 0, -1}
};

// Data struct used in tests, specifying paths and timeouts
struct AppLaunchData {
    AppLaunchData();
    void clear();

    QString binary;
    QStringList args;    
    QString workingDirectory;
    int upTimeMS;
    int topLevelWindowTimeoutMS;
    int terminationTimeoutMS;
    bool splashScreen;
};

AppLaunchData::AppLaunchData() :
    upTimeMS(defaultUpTimeMS),
    topLevelWindowTimeoutMS(defaultTopLevelWindowTimeoutMS),
    terminationTimeoutMS(defaultTerminationTimeoutMS),
    splashScreen(false)
{
}

void AppLaunchData::clear()
{
    binary.clear();
    args.clear();
    workingDirectory.clear();
    upTimeMS = defaultUpTimeMS;
    topLevelWindowTimeoutMS = defaultTopLevelWindowTimeoutMS;
    terminationTimeoutMS = defaultTerminationTimeoutMS;
    splashScreen = false;
}

Q_DECLARE_METATYPE(AppLaunchData)


class tst_GuiAppLauncher : public QObject
{
    Q_OBJECT

public:
    // Test name (static const char title!) + data
    typedef QPair<const char*, AppLaunchData> TestDataEntry;
    typedef QList<TestDataEntry> TestDataEntries;

    enum { TestTools = 0x1, TestDemo = 0x2, TestExamples = 0x4,
           TestAll = TestTools|TestDemo|TestExamples };

    tst_GuiAppLauncher();

private Q_SLOTS:
    void initTestCase();

    void run();
    void run_data();

    void cleanupTestCase();

private:
    QString workingDir() const;

private:
    bool runApp(const AppLaunchData &data, QString *errorMessage) const;
    TestDataEntries testData() const;

    const unsigned m_testMask;
    const unsigned m_examplePriority;
    const QString m_dir;
    const QSharedPointer<WindowManager> m_wm;
};

// Test mask from enviroment as test lib does not allow options.
static inline unsigned testMask()
{
    unsigned testMask = tst_GuiAppLauncher::TestAll;
    if (!qgetenv("QT_TEST_NOTOOLS").isEmpty())
        testMask &= ~ tst_GuiAppLauncher::TestTools;
    if (!qgetenv("QT_TEST_NOEXAMPLES").isEmpty())
        testMask &= ~tst_GuiAppLauncher::TestExamples;
    if (!qgetenv("QT_TEST_NODEMOS").isEmpty())
        testMask &= ~tst_GuiAppLauncher::TestDemo;
    return testMask;
}

static inline unsigned testExamplePriority()
{
    const QByteArray priorityD = qgetenv("QT_TEST_EXAMPLE_PRIORITY");
    if (!priorityD.isEmpty()) {
        bool ok;
        const unsigned rc = priorityD.toUInt(&ok);
        if (ok)
            return rc;
    }
    return 5;
}

tst_GuiAppLauncher::tst_GuiAppLauncher() :
    m_testMask(testMask()),
    m_examplePriority(testExamplePriority()),
    m_dir(QLatin1String(SRCDIR)),
    m_wm(WindowManager::create())
{
}

void tst_GuiAppLauncher::initTestCase()
{   
    QString message = QString::fromLatin1("### App Launcher test on %1 in %2 (%3)").
                      arg(QDateTime::currentDateTime().toString(), QDir::currentPath()).
                      arg(QLibraryInfo::buildKey());
    qDebug("%s", qPrintable(message));
    qWarning("### PLEASE LEAVE THE MACHINE UNATTENDED WHILE THIS TEST IS RUNNING\n");

    // Does a window manager exist on the platform?
    if (!m_wm->openDisplay(&message)) {
        QSKIP(message.toLatin1().constData(), SkipAll);
    }

    // Paranoia: Do we have our test file?
    const QDir workDir(m_dir);
    if (!workDir.exists()) {
        message = QString::fromLatin1("Invalid working directory %1").arg(m_dir);
        QFAIL(message.toLocal8Bit().constData());
    }
}

void tst_GuiAppLauncher::run()
{
    QString errorMessage;
    QFETCH(AppLaunchData, data);
    const bool rc = runApp(data, &errorMessage);
    if (!rc) // Wait for windows to disappear after kill
        WindowManager::sleepMS(500);
    QVERIFY2(rc, qPrintable(errorMessage));
}

// Cross platform galore!
static inline QString guiBinary(QString in)
{
#ifdef Q_OS_MAC
    return in + QLatin1String(".app/Contents/MacOS/") + in;
#endif
    in[0] = in.at(0).toLower();
#ifdef Q_OS_WIN
    in += QLatin1String(".exe");
#endif
    return in;
}

void tst_GuiAppLauncher::run_data()
{
    QTest::addColumn<AppLaunchData>("data");
    foreach(const TestDataEntry &data, testData())
        QTest::newRow(data.first) << data.second;
}

// Read out the examples array structures and convert to test data.
static tst_GuiAppLauncher::TestDataEntries exampleData(unsigned priority,
                                                       const QString &path,
                                                       bool debug,
                                                       const Example *exArray,
                                                       unsigned n)
{
    Q_UNUSED(debug)
    tst_GuiAppLauncher::TestDataEntries rc;
    const QChar slash = QLatin1Char('/');
    AppLaunchData data;
    for (unsigned e = 0; e < n; e++) {
        const Example &example = exArray[e];
        if (example.priority <= priority) {
            data.clear();
            const QString examplePath = path + slash + QLatin1String(example.directory);
            data.binary = examplePath + slash;
#ifdef Q_OS_WIN
            data.binary += debug? QLatin1String("debug/") : QLatin1String("release/");
#endif
            data.binary += guiBinary(QLatin1String(example.binary));
            data.workingDirectory = examplePath;
            if (example.upTimeMS > 0)
                data.upTimeMS = example.upTimeMS;
            rc.append(tst_GuiAppLauncher::TestDataEntry(example.name, data));
        }
    }
    return rc;
}

tst_GuiAppLauncher::TestDataEntries tst_GuiAppLauncher::testData() const
{
    TestDataEntries rc;
    const QChar slash = QLatin1Char('/');
    const QString binPath = QLibraryInfo::location(QLibraryInfo::BinariesPath) + slash;
    const bool debug = QLibraryInfo::buildKey().contains(QLatin1String("debug"));
    Q_UNUSED(debug)

    AppLaunchData data;

    if (m_testMask & TestTools) {
        data.binary = binPath + guiBinary(QLatin1String("Designer"));
        data.args.append(m_dir + QLatin1String("test.ui"));
        rc.append(TestDataEntry("Qt Designer", data));

        data.clear();
        data.binary = binPath + guiBinary(QLatin1String("Linguist"));
        data.splashScreen = true;
        data.upTimeMS = 5000; // Slow loading
        data.args.append(m_dir + QLatin1String("test.ts"));
        rc.append(TestDataEntry("Qt Linguist", data));
    }

    if (m_testMask & TestDemo) {
        data.clear();
        data.upTimeMS = 5000; // Startup animation
        data.binary = binPath + guiBinary(QLatin1String("qtdemo"));
        rc.append(TestDataEntry("Qt Demo", data));

        const QString demosPath = QLibraryInfo::location(QLibraryInfo::DemosPath);
        if (!demosPath.isEmpty())
            rc += exampleData(m_examplePriority, demosPath, debug, demos, sizeof(demos)/sizeof(Example));
    }

    if (m_testMask & TestExamples) {
        const QString examplesPath = QLibraryInfo::location(QLibraryInfo::ExamplesPath);
        if (!examplesPath.isEmpty())
            rc += exampleData(m_examplePriority, examplesPath, debug, examples, sizeof(examples)/sizeof(Example));
    }
    qDebug("Running %d tests...", rc.size());
    return rc;
}

static inline void ensureTerminated(QProcess *p)
{
    if (p->state() != QProcess::Running)
        return;
    p->terminate();
    if (p->waitForFinished(300))
        return;
    p->kill();
    if (!p->waitForFinished(500))
        qWarning("Unable to terminate process");
}

static const QStringList &stderrWhiteList()
{
    static QStringList rc;
    if (rc.empty()) {
        rc << QLatin1String("QPainter::begin: Paint device returned engine == 0, type: 2")
           << QLatin1String("QPainter::setRenderHint: Painter must be active to set rendering hints")
           << QLatin1String("QPainter::setPen: Painter not active")
           << QLatin1String("QPainter::setBrush: Painter not active")
           << QLatin1String("QPainter::end: Painter not active, aborted");
    }
    return rc;
}

bool tst_GuiAppLauncher::runApp(const AppLaunchData &data, QString *errorMessage) const
{
    qDebug("Launching: %s\n", qPrintable(data.binary));
    QProcess process;
    process.setProcessChannelMode(QProcess::MergedChannels);
    if (!data.workingDirectory.isEmpty())
        process.setWorkingDirectory(data.workingDirectory);
    process.start(data.binary, data.args);
    process.closeWriteChannel();
    if (!process.waitForStarted()) {
        *errorMessage = QString::fromLatin1("Unable to execute %1: %2").arg(data.binary, process.errorString());
        return false;
    }
    // Get window id.
    const QString winId = m_wm->waitForTopLevelWindow(data.splashScreen ? 2 : 1, process.pid(), data.topLevelWindowTimeoutMS, errorMessage);
    if (winId.isEmpty()) {
        ensureTerminated(&process);
        return false;
    }
    qDebug("Window: %s\n", qPrintable(winId));
    // Wait a bit, then send close
    WindowManager::sleepMS(data.upTimeMS);
    if (m_wm->sendCloseEvent(winId, process.pid(), errorMessage)) {
        qDebug("Sent close to window: %s\n", qPrintable(winId));
    } else {        
        ensureTerminated(&process);
        return false;
    }
    // Terminate
    if (!process.waitForFinished(data.terminationTimeoutMS)) {
        *errorMessage = QString::fromLatin1("%1: Timeout %2ms").arg(data.binary).arg(data.terminationTimeoutMS);
        ensureTerminated(&process);
        return false;
    }
    if (process.exitStatus() != QProcess::NormalExit) {
        *errorMessage = QString::fromLatin1("%1: Startup crash").arg(data.binary);
        return false;
    }

    const int exitCode = process.exitCode();
    // check stderr
    const QStringList stderrOutput = QString::fromLocal8Bit(process.readAllStandardOutput()).split(QLatin1Char('\n'));
    foreach(const QString &stderrLine, stderrOutput) {
        // Skip expected QPainter warnings from oxygen.
        if (stderrWhiteList().contains(stderrLine)) {
            qWarning("%s: stderr: %s\n", qPrintable(data.binary), qPrintable(stderrLine));
        } else {
            if (!stderrLine.isEmpty()) { // Split oddity gives empty messages
                *errorMessage = QString::fromLatin1("%1: Unexpected output (ex=%2): '%3'").arg(data.binary).arg(exitCode).arg(stderrLine);
                return false;
            }
        }
    }

    if (exitCode != 0) {
        *errorMessage = QString::fromLatin1("%1: Exit code %2").arg(data.binary).arg(exitCode);
        return false;
    }
    return true;
}

void tst_GuiAppLauncher::cleanupTestCase()
{
}

#if defined(Q_OS_WINCE) || defined(Q_OS_SYMBIAN)
QTEST_NOOP_MAIN
#else
QTEST_APPLESS_MAIN(tst_GuiAppLauncher)
#endif

#include "tst_guiapplauncher.moc"

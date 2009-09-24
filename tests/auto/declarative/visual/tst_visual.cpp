#include <qtest.h>
#include <QmlView>
#include <QApplication>
#include <QLibraryInfo>
#include <QDir>
#include <QDebug>
#include <QProcess>
#include <QFile>

static QString testdir;
class tst_visual : public QObject
{
    Q_OBJECT
public:
    tst_visual();

    static QString toTestScript(const QString &);
    static QString viewer();

private slots:
    void visual_data();
    void visual();

private:
    QString qmlviewer;
    QStringList findQmlFiles(const QDir &d);
};


tst_visual::tst_visual() 
{
    qmlviewer = viewer();
}

QString tst_visual::viewer()
{
    QString binaries = QLibraryInfo::location(QLibraryInfo::BinariesPath);

    QString qmlviewer;

#if defined(Q_WS_MAC)
    qmlviewer = QDir(binaries).absoluteFilePath("qmlviewer.app/Contents/MacOS/qmlviewer");
#elif defined(Q_WS_WIN)
    qmlviewer = QDir(binaries).absoluteFilePath("qmlviewer.exe");
#else
    qmlviewer = QDir(binaries).absoluteFilePath("qmlviewer");
#endif

    return qmlviewer;
}

void tst_visual::visual_data()
{
    QTest::addColumn<QString>("file");
    QTest::addColumn<QString>("testdata");

    QStringList files;
    files << findQmlFiles(QDir(QT_TEST_SOURCE_DIR));

    foreach (const QString &file, files) {
        QString testdata = toTestScript(file);
        if (testdata.isEmpty())
            continue;
        
        QTest::newRow(file.toLatin1().constData()) << file << testdata;
    }
}

void tst_visual::visual()
{
    QFETCH(QString, file);
    QFETCH(QString, testdata);

    QStringList arguments;
    arguments << "-script" << testdata
              << "-scriptopts" << "play,testimages,exitoncomplete,exitonfailure" 
              << file;
    QProcess p;
    p.start(qmlviewer, arguments);
    QVERIFY(p.waitForFinished());
    QCOMPARE(p.exitStatus(), QProcess::NormalExit);
    QCOMPARE(p.exitCode(), 0);
}

QString tst_visual::toTestScript(const QString &file)
{
    if (!file.endsWith(".qml"))
        return QString();

    int index = file.lastIndexOf(QDir::separator());
    if (index == -1)
        return QString();

    QString testdata = file.left(index + 1) + 
                       QString("data") + QDir::separator() +
                       file.mid(index + 1, file.length() - index - 5);

    return testdata;
}

QStringList tst_visual::findQmlFiles(const QDir &d)
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
        if (dir == "data")
            continue;

        QDir sub = d;
        sub.cd(dir);
        rv << findQmlFiles(sub);
    }

    return rv;
}

enum Mode { Record, Play, TestVisuals, UpdateVisuals, Test };

void action(Mode mode, const QString &file)
{
    Q_ASSERT(mode != Test);

    QString testdata = tst_visual::toTestScript(file);

    if (Record == mode) {
        QStringList arguments;
        arguments << "-script" << testdata
                  << "-scriptopts" << "record,saveonexit"
                  << file;
        QProcess p;
        p.setProcessChannelMode(QProcess::ForwardedChannels);
        p.start(tst_visual::viewer(), arguments);
        p.waitForFinished();
    } else if (Play == mode) {
        QStringList arguments;
        arguments << "-script" << testdata
                  << "-scriptopts" << "play,testimages,exitoncomplete"
                  << file;
        QProcess p;
        p.setProcessChannelMode(QProcess::ForwardedChannels);
        p.start(tst_visual::viewer(), arguments);
        p.waitForFinished();
    } else if (TestVisuals == mode) {
        QStringList arguments;
        arguments << "-script" << testdata
                  << "-scriptopts" << "play"
                  << file;
        QProcess p;
        p.setProcessChannelMode(QProcess::ForwardedChannels);
        p.start(tst_visual::viewer(), arguments);
        p.waitForFinished();
    } else if (UpdateVisuals == mode) {
        QStringList arguments;
        arguments << "-script" << testdata
                  << "-scriptopts" << "play,record,exitoncomplete,saveonexit"
                  << file;
        QProcess p;
        p.setProcessChannelMode(QProcess::ForwardedChannels);
        p.start(tst_visual::viewer(), arguments);
        p.waitForFinished();
    }
}

void usage()
{
    fprintf(stderr, "\n");
    fprintf(stderr, "QML related options\n");
    fprintf(stderr, " -record file        : record new test data for file\n");
    fprintf(stderr, " -play file          : playback test data for file, printing errors\n");
    fprintf(stderr, " -testvisuals file   : playback test data for file, without errors\n");
    fprintf(stderr, " -updatevisuals file : playback test data for file, accept new visuals for file\n");
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
        } else if (arg == "-testvisuals" && (ii + 1) < argc) {
            mode = TestVisuals;
            file = argv[++ii];
        } else if (arg == "-updatevisuals" && (ii + 1) < argc) {
            mode = UpdateVisuals;
            file = argv[++ii];
        } else {
            newArgv[newArgc++] = argv[ii];
        }
        
        if (arg == "-help") {
            atexit(usage);
            showHelp = true;
        }
    }

    if (mode == Test || showHelp) {
        tst_visual tc;
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

#include "tst_visual.moc"

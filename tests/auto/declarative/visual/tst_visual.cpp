#include <qtest.h>
#include <QmlView>
#include <QApplication>
#include <QFxTestView>
#include <QFile>

static QString testdir;
class tst_visual : public QObject
{
    Q_OBJECT
public:
    tst_visual() {}

private slots:
    void visual();
};

#ifdef QT_OPENGL_ES
#define RASTER_TESTDIR "/data/opengl"
#else
#define RASTER_TESTDIR "/data/raster"
#endif

void tst_visual::visual()
{
    QVERIFY(!testdir.isEmpty());

    QString filename = QLatin1String(QT_TEST_SOURCE_DIR) + "/" + testdir + "/test";
    qWarning() << "Using config:" << filename;
    QFile configFile(filename);
    QVERIFY(configFile.open(QIODevice::ReadOnly));

    QString testfile = configFile.readLine().trimmed();
    QVERIFY(!testfile.isEmpty());

    if(testfile.startsWith("QTDIR/")) {
        testfile.remove(0, 6);
        testfile.prepend(QLatin1String(QT_TEST_SOURCE_DIR) + "/../../../../");
    } else {
        testfile.prepend(QLatin1String(QT_TEST_SOURCE_DIR) + "/" + testdir + "/");
    }

    QFxTestView *view = new QFxTestView(testfile, QLatin1String(QT_TEST_SOURCE_DIR) + "/" + testdir + RASTER_TESTDIR);
    QVERIFY(view->runTest());
}

#include <QtTest/qtest_gui.h>

int main(int argc, char *argv[]) 
{ 
    int newArgc = 1;
    char **newArgv = new char*[argc];

    newArgv[0] = argv[0];

    QApplication app(newArgc, newArgv); 

    for(int ii = 1; ii < argc; ii++) {
        if(QLatin1String(argv[ii]) == "-testdir" && (ii + 1) < argc) {
            testdir = QLatin1String(argv[ii + 1]);
            ii++;
        } else {
            newArgv[newArgc++] = argv[ii];
        }
    }

    tst_visual tc; 
    int rv = QTest::qExec(&tc, newArgc, newArgv); 

    delete [] newArgv;
    return rv;
}

#include "tst_visual.moc"

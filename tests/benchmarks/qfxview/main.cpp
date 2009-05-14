#include <QFxView>
#include <QmlBindContext>
#include <QtGui>
#include <QTest>
#include <gfxtimeline.h>

class tst_qfxview : public QObject
{
    Q_OBJECT

private slots:
    void render();
    void render_data();

private:
    void waitForUpdates(QObject* obj, int howmany, int timeout = 10000);
    bool eventFilter(QObject*, QEvent*);

    QString testdata(QString const&);
    QString readFile(QString const&);

    int         m_updateCount;
    QEventLoop* m_eventLoop;
    int         m_updateLimit;
};

bool tst_qfxview::eventFilter(QObject*, QEvent* e)
{
    if (!m_eventLoop) {
        return false;
    }

    if (e->type() == QEvent::UpdateRequest) {
        m_updateCount++;
        if (m_updateCount >= m_updateLimit) {
            m_eventLoop->quit();
        }
    }

    return false;
}

void tst_qfxview::waitForUpdates(QObject* obj, int howmany, int timeout)
{
    m_updateLimit = howmany;
    m_updateCount = 0;

    QEventLoop loop;
    m_eventLoop = &loop;
    if (timeout > 0)
        QTimer::singleShot(timeout, &loop, SLOT(quit()));

    obj->installEventFilter(this);
    loop.exec();
    obj->removeEventFilter(this);

    m_eventLoop = 0;

    if (m_updateCount < howmany) {
        QFAIL(qPrintable(
            QString("Didn't receive %1 updates within %2 milliseconds (only got %3)")
            .arg(howmany).arg(timeout).arg(m_updateCount)
        ));
    }
}

void tst_qfxview::render()
{
    QFETCH(QString,         xml);
    QFETCH(QString,         filename);
    QFETCH(int,             mode);

    static const int FRAMES = 100;

    QPixmap pm(800, 600);

    QFxView view((QSimpleCanvas::CanvasMode)mode);
    view.setFixedSize(pm.size());
    if (!filename.isEmpty()) {
        filename = testdata(filename);
        xml = readFile(filename);
    }
    view.setXml(xml, filename);
    QFileInfo fi(filename);
    view.setPath(fi.path());

    /* So we can reuse duiviewer examples... */
    QObject *dummyData = QML::newInstance(fi.path() + "/editor/data.xml");
    if (dummyData) {
        QmlBindContext *ctxt = view.bindContext();
        ctxt->setProperty("obj", dummyData);
        ctxt->addDefaultObject(dummyData);
        dummyData->setParent(&view);
    }

    view.show();
    view.raise();
    QTest::qWait(100);
    view.activateWindow();
    QTest::qWait(100);
    QVERIFY(view.isActiveWindow());

    view.execute();

    /* Set internal GfxAppClock to consistent timing (every tick is 5 ms) */
    {
        GfxTimeLine tl;
        tl.setMode(GfxTimeLine::Consistent);
    }

    /* Render FRAMES amount of updates */
    QBENCHMARK {
        waitForUpdates(&view, FRAMES);
        if (QTest::currentTestFailed()) return;
    }

    //view.dumpTiming();
}

void tst_qfxview::render_data()
{
    QTest::addColumn<QString>        ("xml");
    QTest::addColumn<QString>        ("filename");
    QTest::addColumn<int>            ("mode");

    QTest::newRow("tatwheel-simplecanvas")
        << QString()
        << QString("tatwheel/tat-wheel.xml")
        << int(QSimpleCanvas::SimpleCanvas)
    ;

    QTest::newRow("tatwheel-graphicsview")
        << QString()
        << QString("tatwheel/tat-wheel.xml")
        << int(QSimpleCanvas::GraphicsView)
    ;

}

QString tst_qfxview::readFile(QString const& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        qFatal("Could not open %s: %s", qPrintable(filename), qPrintable(file.errorString()));
    }
    QByteArray ba = file.readAll();
    return QString::fromLatin1(ba);
}

QString tst_qfxview::testdata(QString const& name)
{
#define _STR(X) #X
#define STR(X) _STR(X)
    return QString("%1/testdata/%2").arg(STR(SRCDIR)).arg(name);
}

int main(int argc, char** argv)
{
    /* Remove this when -graphicssystem raster is the default. */
    bool explicit_graphicssystem = false;

    QVector<const char*> args;
    for (int i = 0; i < argc; ++i) {
        if (!strcmp(argv[i], "-graphicssystem"))
            explicit_graphicssystem = true;
        args << argv[i];
    }

    if (!explicit_graphicssystem) {
        args << "-graphicssystem" << "raster";
    }

    argc = args.count();
    argv = const_cast<char**>(args.data());

    QApplication app(argc, argv);

    tst_qfxview test;
    return QTest::qExec(&test, argc, argv);
}

#include "main.moc"

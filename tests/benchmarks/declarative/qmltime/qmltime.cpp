#include <QmlEngine>
#include <QmlComponent>
#include <QDebug>
#include <QApplication>
#include <QTime>
#include <QmlContext>
#include <QGraphicsScene>
#include <QGraphicsRectItem>

class Timer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QmlComponent *component READ component WRITE setComponent);

public:
    Timer();

    QmlComponent *component() const;
    void setComponent(QmlComponent *);

    static Timer *timerInstance();

    void run(uint);

    bool willParent() const;
    void setWillParent(bool p);

private:
    void runTest(QmlContext *, uint);

    QmlComponent *m_component;
    static Timer *m_timer;

    bool m_willparent;
    QGraphicsScene m_scene;
    QGraphicsRectItem m_item;
};
QML_DECLARE_TYPE(Timer);
QML_DEFINE_TYPE(QmlTime, 1, 0, Timer, Timer);

Timer *Timer::m_timer = 0;

Timer::Timer()
: m_component(0), m_willparent(false)
{
    if (m_timer)
        qWarning("Timer: Timer already registered");
    m_timer = this;

    m_scene.setItemIndexMethod(QGraphicsScene::NoIndex);
    m_scene.addItem(&m_item);
}

QmlComponent *Timer::component() const
{
    return m_component;
}

void Timer::setComponent(QmlComponent *c)
{
    m_component = c;
}

Timer *Timer::timerInstance()
{
    return m_timer;
}

void Timer::run(uint iterations)
{
    QmlContext context(qmlContext(this));

    QObject *o = m_component->create(&context);
    QGraphicsObject *go = qobject_cast<QGraphicsObject *>(o);
    if (m_willparent && go) 
        go->setParentItem(&m_item);
    delete o;
    
    runTest(&context, iterations);
}

bool Timer::willParent() const
{
    return m_willparent;
}

void Timer::setWillParent(bool p)
{
    m_willparent = p;
}

void Timer::runTest(QmlContext *context, uint iterations)
{
    QTime t;
    t.start();
    for (uint ii = 0; ii < iterations; ++ii) {
        QObject *o = m_component->create(context);
        QGraphicsObject *go = qobject_cast<QGraphicsObject *>(o);
        if (m_willparent && go) 
            go->setParentItem(&m_item);
        delete o;
    }

    int e = t.elapsed();

    qWarning() << "Total:" << e << "ms, Per iteration:" << qreal(e) / qreal(iterations) << "ms";

}

void usage(const char *name)
{
    qWarning("Usage: %s [-iterations <count>] [-parent] <qml file>", name);
    exit(-1);
}

int main(int argc, char ** argv)
{
    QApplication app(argc, argv);

    uint iterations = 1024;
    QString filename;
    bool willParent = false;

    for (int ii = 1; ii < argc; ++ii) {
        QByteArray arg(argv[ii]);

        if (arg == "-iterations") {
            if (ii + 1 < argc) {
                ++ii;
                QByteArray its(argv[ii]);
                bool ok = false;
                iterations = its.toUInt(&ok);
                if (!ok)
                    usage(argv[0]);
            } else {
                usage(argv[0]);
            }
        } else if (arg == "-parent") {
            willParent = true;
        } else {
            filename = QLatin1String(argv[ii]);
        }
    }

    if (filename.isEmpty())
        usage(argv[0]);

    QmlEngine engine;
    QmlComponent component(&engine, filename);
    if (component.isError()) {
        qWarning() << component.errors();
        return -1;
    }

    QObject *obj = component.create();
    if (!obj) {
        qWarning() << component.errors();
        return -1;
    }

    Timer *timer = Timer::timerInstance();
    if (!timer) {
        qWarning() << "A Tester.Timer instance is required.";
        return -1;
    }

    timer->setWillParent(willParent);

    if (!timer->component()) {
        qWarning() << "The timer has no component";
        return -1;
    }

    timer->run(iterations);

    return 0;
}

#include "qmltime.moc"

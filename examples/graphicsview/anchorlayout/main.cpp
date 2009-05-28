#include <QGraphicsWidget>
#include <QGraphicsProxyWidget>
#include <QGraphicsAnchorLayout>
#include <QtGui>

//#define BENCHMARK_RUN 1
#define CALLGRIND_DEBUG 1

#if defined(BENCHMARK_RUN)

// Callgrind command line:
//   valgrind --tool=callgrind --instr-atstart=no ./anchorlayout >/dev/null 2>&1

#if defined(CALLGRIND_DEBUG)
#include <valgrind/callgrind.h>
#else
#define CALLGRIND_START_INSTRUMENTATION do { } while (0)
#define CALLGRIND_STOP_INSTRUMENTATION do { } while (0)
#endif

#endif


static QGraphicsProxyWidget *createItem(const QSizeF &minimum = QSizeF(100.0, 100.0),
                                   const QSizeF &preferred = QSize(150.0, 100.0),
                                   const QSizeF &maximum = QSizeF(200.0, 100.0),
                                   const QString &name = "0")
{
    QGraphicsProxyWidget *w = new QGraphicsProxyWidget;
    w->setWidget(new QPushButton(name));
    w->setData(0, name);
    w->setMinimumSize(minimum);
    w->setPreferredSize(preferred);
    w->setMaximumSize(maximum);

    return w;
}

class Test : public QObject
{
    Q_OBJECT;
public:
    Test(QGraphicsWidget *wi, QGraphicsLayout* la)
        : QObject(), w(wi), l(la), first(true) {}

    QGraphicsWidget *w;
    QGraphicsLayout *l;
    bool first;

public slots:
    void onTimer() {
        if (first) {
            first = false;
            w->setContentsMargins(10, 10, 10, 10);
        } else {
            l->setContentsMargins(10, 10, 10, 10);
        }
    }
};

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QGraphicsScene *scene = new QGraphicsScene();
    scene->setSceneRect(0, 0, 800, 480);

    QSizeF min(30, 100);
    QSizeF pref(210, 100);
    QSizeF max(300, 100);

    QGraphicsProxyWidget *a = createItem(min, pref, max, "A");
    QGraphicsProxyWidget *b = createItem(min, pref, max, "B");
    QGraphicsProxyWidget *c = createItem(min, pref, max, "C");
    QGraphicsProxyWidget *d = createItem(min, pref, max, "D");
    QGraphicsProxyWidget *e = createItem(min, pref, max, "E");
    QGraphicsProxyWidget *f = createItem(QSizeF(30, 50), QSizeF(150, 50), max, "F");
    QGraphicsProxyWidget *g = createItem(QSizeF(30, 50), QSizeF(30, 100), max, "G");

    QGraphicsAnchorLayout *l = new QGraphicsAnchorLayout;

    QGraphicsWidget *w = new QGraphicsWidget(0, Qt::Window);
    w->setPos(20, 20);
    w->setLayout(l);

    // vertical
    l->anchor(a, QGraphicsAnchorLayout::Top, l, QGraphicsAnchorLayout::Top);
    l->anchor(b, QGraphicsAnchorLayout::Top, l, QGraphicsAnchorLayout::Top);

    l->anchor(c, QGraphicsAnchorLayout::Top, a, QGraphicsAnchorLayout::Bottom);
    l->anchor(c, QGraphicsAnchorLayout::Top, b, QGraphicsAnchorLayout::Bottom);
    l->anchor(c, QGraphicsAnchorLayout::Bottom, d, QGraphicsAnchorLayout::Top);
    l->anchor(c, QGraphicsAnchorLayout::Bottom, e, QGraphicsAnchorLayout::Top);

    l->anchor(d, QGraphicsAnchorLayout::Bottom, l, QGraphicsAnchorLayout::Bottom);
    l->anchor(e, QGraphicsAnchorLayout::Bottom, l, QGraphicsAnchorLayout::Bottom);

    l->anchor(c, QGraphicsAnchorLayout::Top, f, QGraphicsAnchorLayout::Top);
    l->anchor(c, QGraphicsAnchorLayout::VCenter, f, QGraphicsAnchorLayout::Bottom);
    l->anchor(f, QGraphicsAnchorLayout::Bottom, g, QGraphicsAnchorLayout::Top);
    l->anchor(c, QGraphicsAnchorLayout::Bottom, g, QGraphicsAnchorLayout::Bottom);

    // horizontal
    l->anchor(l, QGraphicsAnchorLayout::Left, a, QGraphicsAnchorLayout::Left);
    l->anchor(l, QGraphicsAnchorLayout::Left, d, QGraphicsAnchorLayout::Left);
    l->anchor(a, QGraphicsAnchorLayout::Right, b, QGraphicsAnchorLayout::Left);

    l->anchor(a, QGraphicsAnchorLayout::Right, c, QGraphicsAnchorLayout::Left);
    l->anchor(c, QGraphicsAnchorLayout::Right, e, QGraphicsAnchorLayout::Left);

    l->anchor(b, QGraphicsAnchorLayout::Right, l, QGraphicsAnchorLayout::Right);
    l->anchor(e, QGraphicsAnchorLayout::Right, l, QGraphicsAnchorLayout::Right);
    l->anchor(d, QGraphicsAnchorLayout::Right, e, QGraphicsAnchorLayout::Left);

    l->anchor(l, QGraphicsAnchorLayout::Left, f, QGraphicsAnchorLayout::Left);
    l->anchor(l, QGraphicsAnchorLayout::Left, g, QGraphicsAnchorLayout::Left);
    l->anchor(f, QGraphicsAnchorLayout::Right, g, QGraphicsAnchorLayout::Right);

#ifdef BENCHMARK_RUN
    CALLGRIND_START_INSTRUMENTATION;
    for (int i = 0; i < 100; i++) {
        l->invalidate();
        l->d_ptr->calculateGraphs();
    }
    CALLGRIND_STOP_INSTRUMENTATION;
    return 0;
#endif

    l->dumpGraph();

    scene->addItem(w);
    scene->setBackgroundBrush(Qt::darkGreen);
    QGraphicsView *view = new QGraphicsView(scene);
    view->show();

    Test test(w, l);
    QTimer timer;
    test.connect(&timer, SIGNAL(timeout()), SLOT(onTimer()));
    timer.start(5000);

    int rc = app.exec();

    delete scene;
    delete view;

    return rc;
}

#include "main.moc"

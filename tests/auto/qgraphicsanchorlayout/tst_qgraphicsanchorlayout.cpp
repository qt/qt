#include <QtTest/QtTest>
#include <qgraphicsanchorlayout.h>
#include <qgraphicswidget.h>
#include <qgraphicsproxywidget.h>
#include <QtGui/qgraphicsview.h>

class tst_QGraphicsAnchorLayout : public QObject {
    Q_OBJECT;

private slots:
    void simple();
    void diagonal();
    void parallel();
    void parallel2();
    void snake();
    void snakeOppositeDirections();
    void fairDistribution();
    void fairDistributionOppositeDirections();
    void proportionalPreferred();
    void example();
};

class RectWidget : public QGraphicsWidget
{
public:
    RectWidget(QGraphicsItem *parent = 0) : QGraphicsWidget(parent){}

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
    {
        Q_UNUSED(option);
        Q_UNUSED(widget);
        painter->drawRoundRect(rect());
        painter->drawLine(rect().topLeft(), rect().bottomRight());
        painter->drawLine(rect().bottomLeft(), rect().topRight());
    }
};

static QGraphicsWidget *createItem(const QSizeF &minimum = QSizeF(100.0, 100.0),
                                   const QSizeF &preferred = QSize(150.0, 100.0),
                                   const QSizeF &maximum = QSizeF(200.0, 100.0),
                                   const QString &name = QString())
{
    QGraphicsWidget *w = new RectWidget;
    w->setMinimumSize(minimum);
    w->setPreferredSize(preferred);
    w->setMaximumSize(maximum);
    w->setData(0, name);
    return w;
}

void tst_QGraphicsAnchorLayout::simple()
{
    QGraphicsWidget *w1 = createItem();
    QGraphicsWidget *w2 = createItem();

    QGraphicsAnchorLayout *l = new QGraphicsAnchorLayout;
    l->setContentsMargins(0, 0, 0, 0);
    l->anchor(w1, QGraphicsAnchorLayout::Right, w2, QGraphicsAnchorLayout::Left);

    QGraphicsWidget p;
    p.setLayout(l);

    QCOMPARE(l->count(), 2);
}

void tst_QGraphicsAnchorLayout::diagonal()
{
    QSizeF min(10, 100);
    QSizeF pref(70, 100);
    QSizeF max(100, 100);

    QGraphicsWidget *a = createItem(min, pref, max);
    QGraphicsWidget *b = createItem(min, pref, max);
    QGraphicsWidget *c = createItem(min, pref, max);
    QGraphicsWidget *d = createItem(min, pref, max);
    QGraphicsWidget *e = createItem(min, pref, max);

    QGraphicsAnchorLayout *l = new QGraphicsAnchorLayout;
    l->setContentsMargins(0, 0, 0, 0);

    // vertical
    l->anchor(a, QGraphicsAnchorLayout::Top, l, QGraphicsAnchorLayout::Top, 0);
    l->anchor(b, QGraphicsAnchorLayout::Top, l, QGraphicsAnchorLayout::Top, 0);

    l->anchor(c, QGraphicsAnchorLayout::Top, a, QGraphicsAnchorLayout::Bottom, 0);
    l->anchor(c, QGraphicsAnchorLayout::Top, b, QGraphicsAnchorLayout::Bottom, 0);
    l->anchor(c, QGraphicsAnchorLayout::Bottom, d, QGraphicsAnchorLayout::Top, 0);
    l->anchor(c, QGraphicsAnchorLayout::Bottom, e, QGraphicsAnchorLayout::Top, 0);

    l->anchor(d, QGraphicsAnchorLayout::Bottom, l, QGraphicsAnchorLayout::Bottom, 0);
    l->anchor(e, QGraphicsAnchorLayout::Bottom, l, QGraphicsAnchorLayout::Bottom, 0);

    // horizontal
    l->anchor(l, QGraphicsAnchorLayout::Left, a, QGraphicsAnchorLayout::Left, 0);
    l->anchor(l, QGraphicsAnchorLayout::Left, d, QGraphicsAnchorLayout::Left, 0);
    l->anchor(a, QGraphicsAnchorLayout::Right, b, QGraphicsAnchorLayout::Left, 0);

    l->anchor(a, QGraphicsAnchorLayout::Right, c, QGraphicsAnchorLayout::Left, 0);
    l->anchor(c, QGraphicsAnchorLayout::Right, e, QGraphicsAnchorLayout::Left, 0);

    l->anchor(b, QGraphicsAnchorLayout::Right, l, QGraphicsAnchorLayout::Right, 0);
    l->anchor(e, QGraphicsAnchorLayout::Right, l, QGraphicsAnchorLayout::Right, 0);
    l->anchor(d, QGraphicsAnchorLayout::Right, e, QGraphicsAnchorLayout::Left, 0);

    QCOMPARE(l->count(), 5);

    QGraphicsWidget p;
    p.setLayout(l);

    QSizeF layoutMinimumSize = l->effectiveSizeHint(Qt::MinimumSize);
    QSizeF layoutMaximumSize = l->effectiveSizeHint(Qt::MaximumSize);
    QSizeF layoutPreferredSize = l->effectiveSizeHint(Qt::PreferredSize);

    QCOMPARE(layoutMinimumSize, QSizeF(30.0, 300.0));
    QCOMPARE(layoutPreferredSize, QSizeF(170.0, 300.0));
    QCOMPARE(layoutMaximumSize, QSizeF(190.0, 300.0));

    p.resize(layoutMinimumSize);
    QCOMPARE(a->geometry(), QRectF(0.0, 0.0, 10.0, 100.0));
    QCOMPARE(b->geometry(), QRectF(10.0, 0.0, 20.0, 100.0));
    QCOMPARE(c->geometry(), QRectF(10.0, 100.0, 10.0, 100.0));
    QCOMPARE(d->geometry(), QRectF(0.0, 200.0, 20.0, 100.0));
    QCOMPARE(e->geometry(), QRectF(20.0, 200.0, 10.0, 100.0));
    QCOMPARE(p.size(), layoutMinimumSize);

    p.resize(layoutPreferredSize);
    QCOMPARE(a->geometry(), QRectF(0.0, 0.0, 70.0, 100.0));
    QCOMPARE(b->geometry(), QRectF(70.0, 0.0, 100.0, 100.0));
    QCOMPARE(c->geometry(), QRectF(70.0, 100.0, 30.0, 100.0));
    QCOMPARE(d->geometry(), QRectF(0.0, 200.0, 100.0, 100.0));
    QCOMPARE(e->geometry(), QRectF(100.0, 200.0, 70.0, 100.0));
    QCOMPARE(p.size(), layoutPreferredSize);

    p.resize(layoutMaximumSize);
    QCOMPARE(a->geometry(), QRectF(0.0, 0.0, 90.0, 100.0));
    QCOMPARE(b->geometry(), QRectF(90.0, 0.0, 100.0, 100.0));
    QCOMPARE(c->geometry(), QRectF(90.0, 100.0, 10.0, 100.0));
    QCOMPARE(d->geometry(), QRectF(0.0, 200.0, 100.0, 100.0));
    QCOMPARE(e->geometry(), QRectF(100.0, 200.0, 90.0, 100.0));
    QCOMPARE(p.size(), layoutMaximumSize);

    QSizeF testA(175.0, 300.0);
    p.resize(testA);
    QCOMPARE(a->geometry(), QRectF(0.0, 0.0, 75.0, 100.0));
    QCOMPARE(b->geometry(), QRectF(75.0, 0.0, 100.0, 100.0));
    QCOMPARE(c->geometry(), QRectF(75.0, 100.0, 25.0, 100.0));
    QCOMPARE(d->geometry(), QRectF(0.0, 200.0, 100.0, 100.0));
    QCOMPARE(e->geometry(), QRectF(100.0, 200.0, 75.0, 100.0));
    QCOMPARE(p.size(), testA);
}

void tst_QGraphicsAnchorLayout::parallel()
{
    QGraphicsWidget *a = createItem(QSizeF(100, 100),
                                    QSizeF(150, 100),
                                    QSizeF(200, 100), "A");

    QGraphicsWidget *b = createItem(QSizeF(100, 100),
                                    QSizeF(150, 100),
                                    QSizeF(300, 100), "B");

    QGraphicsWidget *c = createItem(QSizeF(100, 100),
                                    QSizeF(200, 100),
                                    QSizeF(350, 100), "C");

    QGraphicsWidget *d = createItem(QSizeF(100, 100),
                                    QSizeF(170, 100),
                                    QSizeF(200, 100), "D");

    QGraphicsWidget *e = createItem(QSizeF(150, 100),
                                    QSizeF(150, 100),
                                    QSizeF(200, 100), "E");

    QGraphicsWidget *f = createItem(QSizeF(100, 100),
                                    QSizeF(150, 100),
                                    QSizeF(200, 100), "F");

    QGraphicsAnchorLayout *l = new QGraphicsAnchorLayout;
    l->setContentsMargins(0, 0, 0, 0);

    l->anchor(l, QGraphicsAnchorLayout::Top, a, QGraphicsAnchorLayout::Top, 0);
    l->anchor(a, QGraphicsAnchorLayout::Bottom, b, QGraphicsAnchorLayout::Top, 0);
    l->anchor(b, QGraphicsAnchorLayout::Bottom, c, QGraphicsAnchorLayout::Top, 0);
    l->anchor(c, QGraphicsAnchorLayout::Bottom, d, QGraphicsAnchorLayout::Top, 0);
    l->anchor(d, QGraphicsAnchorLayout::Bottom, e, QGraphicsAnchorLayout::Top, 0);
    l->anchor(e, QGraphicsAnchorLayout::Bottom, f, QGraphicsAnchorLayout::Top, 0);
    l->anchor(f, QGraphicsAnchorLayout::Bottom, l, QGraphicsAnchorLayout::Bottom, 0);

    l->anchor(l, QGraphicsAnchorLayout::Left, a, QGraphicsAnchorLayout::Left, 0);
    l->anchor(a, QGraphicsAnchorLayout::Right, b, QGraphicsAnchorLayout::Left, 0);
    l->anchor(a, QGraphicsAnchorLayout::Right, c, QGraphicsAnchorLayout::Left, 0);
    l->anchor(b, QGraphicsAnchorLayout::Right, d, QGraphicsAnchorLayout::Left, 0);
    l->anchor(b, QGraphicsAnchorLayout::Right, e, QGraphicsAnchorLayout::Left, 0);
    l->anchor(c, QGraphicsAnchorLayout::Right, f, QGraphicsAnchorLayout::Left, 0);
    l->anchor(d, QGraphicsAnchorLayout::Right, f, QGraphicsAnchorLayout::Left, 0);
    l->anchor(e, QGraphicsAnchorLayout::Right, f, QGraphicsAnchorLayout::Left, 0);
    l->anchor(f, QGraphicsAnchorLayout::Right, l, QGraphicsAnchorLayout::Right, 0);

    QCOMPARE(l->count(), 6);

    QGraphicsWidget p;
    p.setLayout(l);

    QSizeF layoutMinimumSize = l->effectiveSizeHint(Qt::MinimumSize);
    QSizeF layoutPreferredSize = l->effectiveSizeHint(Qt::PreferredSize);
    QSizeF layoutMaximumSize = l->effectiveSizeHint(Qt::MaximumSize);

    QCOMPARE(layoutMinimumSize, QSizeF(450, 600));
    QCOMPARE(layoutPreferredSize, QSizeF(620, 600));
    QCOMPARE(layoutMaximumSize, QSizeF(750, 600));

    p.resize(layoutMinimumSize);
    QCOMPARE(a->geometry(), QRectF(0, 0, 100, 100));
    QCOMPARE(b->geometry(), QRectF(100, 100, 100, 100));
    QCOMPARE(c->geometry(), QRectF(100, 200, 250, 100));
    QCOMPARE(d->geometry(), QRectF(200, 300, 150, 100));
    QCOMPARE(e->geometry(), QRectF(200, 400, 150, 100));
    QCOMPARE(f->geometry(), QRectF(350, 500, 100, 100));
    QCOMPARE(p.size(), layoutMinimumSize);

    p.resize(layoutPreferredSize);
    QCOMPARE(a->geometry(), QRectF(0, 0, 150, 100));
    QCOMPARE(b->geometry(), QRectF(150, 100, 150, 100));
    QCOMPARE(c->geometry(), QRectF(150, 200, 320, 100));
    QCOMPARE(d->geometry(), QRectF(300, 300, 170, 100));
    QCOMPARE(e->geometry(), QRectF(300, 400, 170, 100));
    QCOMPARE(f->geometry(), QRectF(470, 500, 150, 100));
    QCOMPARE(p.size(), layoutPreferredSize);

    // Maximum size depends on simplification / fair distribution
    // Without that, test may or may not pass, depending on the
    // solution found by the solver at runtime.
    p.resize(layoutMaximumSize);
    QCOMPARE(a->geometry(), QRectF(0, 0, 200, 100));
    QEXPECT_FAIL("", "see commit 23441f49a23cbf936b60140c5c8a6d5cb3ca00a7 for explanation", Abort);
    QCOMPARE(b->geometry(), QRectF(200, 100, 175, 100));
    QCOMPARE(c->geometry(), QRectF(200, 200, 350, 100));
    QCOMPARE(d->geometry(), QRectF(375, 300, 175, 100));
    QCOMPARE(e->geometry(), QRectF(375, 400, 175, 100));
    QCOMPARE(f->geometry(), QRectF(550, 500, 200, 100));
    QCOMPARE(p.size(), layoutMaximumSize);
}

void tst_QGraphicsAnchorLayout::parallel2()
{
    QGraphicsWidget *a = createItem(QSizeF(70.0, 100.0),
                                    QSizeF(100.0, 100.0),
                                    QSizeF(200.0, 100.0));

    QGraphicsWidget *b = createItem(QSizeF(100.0, 100.0),
                                    QSizeF(150.0, 100.0),
                                    QSizeF(190.0, 100.0));

    QGraphicsAnchorLayout *l = new QGraphicsAnchorLayout;
    l->setContentsMargins(0, 0, 0, 0);

    l->anchor(l, QGraphicsAnchorLayout::Top, a, QGraphicsAnchorLayout::Top, 0);
    l->anchor(a, QGraphicsAnchorLayout::Bottom, b, QGraphicsAnchorLayout::Top, 0);
    l->anchor(b, QGraphicsAnchorLayout::Bottom, l, QGraphicsAnchorLayout::Bottom, 0);

    l->anchorWidth(l, a);
    l->anchor(l, QGraphicsAnchorLayout::Left, b, QGraphicsAnchorLayout::Left, 0);
    l->anchor(b, QGraphicsAnchorLayout::Right, a, QGraphicsAnchorLayout::Right, 0);

    QCOMPARE(l->count(), 2);

    QGraphicsWidget p;
    p.setLayout(l);

    QSizeF layoutMinimumSize = l->effectiveSizeHint(Qt::MinimumSize);
    QSizeF layoutPreferredSize = l->effectiveSizeHint(Qt::PreferredSize);
    QSizeF layoutMaximumSize = l->effectiveSizeHint(Qt::MaximumSize);

    QCOMPARE(layoutMinimumSize, QSizeF(100.0, 200.0));
    QCOMPARE(layoutPreferredSize, QSizeF(150.0, 200.0));
    QCOMPARE(layoutMaximumSize, QSizeF(190.0, 200.0));

    p.resize(layoutMinimumSize);
    QCOMPARE(p.size(), layoutMinimumSize);

    p.resize(layoutPreferredSize);
    QCOMPARE(p.size(), layoutPreferredSize);

    p.resize(layoutMaximumSize);
    QCOMPARE(p.size(), layoutMaximumSize);
}

void tst_QGraphicsAnchorLayout::snake()
{
    QGraphicsWidget *a = createItem(QSizeF(50.0, 100.0),
                                    QSizeF(70.0, 100.0),
                                    QSizeF(100.0, 100.0));

    QGraphicsWidget *b = createItem(QSizeF(10.0, 100.0),
                                    QSizeF(20.0, 100.0),
                                    QSizeF(40.0, 100.0));

    QGraphicsWidget *c = createItem(QSizeF(50.0, 100.0),
                                    QSizeF(70.0, 100.0),
                                    QSizeF(100.0, 100.0));

    QGraphicsAnchorLayout *l = new QGraphicsAnchorLayout;
    l->setContentsMargins(0, 0, 0, 0);

    l->anchor(l, QGraphicsAnchorLayout::Top, a, QGraphicsAnchorLayout::Top, 0);
    l->anchor(a, QGraphicsAnchorLayout::Bottom, b, QGraphicsAnchorLayout::Top, 0);
    l->anchor(b, QGraphicsAnchorLayout::Bottom, c, QGraphicsAnchorLayout::Top, 0);
    l->anchor(c, QGraphicsAnchorLayout::Bottom, l, QGraphicsAnchorLayout::Bottom, 0);

    l->anchor(l, QGraphicsAnchorLayout::Left, a, QGraphicsAnchorLayout::Left, 0);
    l->anchor(a, QGraphicsAnchorLayout::Right, b, QGraphicsAnchorLayout::Right, 0);
    l->anchor(b, QGraphicsAnchorLayout::Left, c, QGraphicsAnchorLayout::Left, 0);
    l->anchor(c, QGraphicsAnchorLayout::Right, l, QGraphicsAnchorLayout::Right, 0);

    QCOMPARE(l->count(), 3);

    QGraphicsWidget p;
    p.setLayout(l);

    QSizeF layoutMinimumSize = l->effectiveSizeHint(Qt::MinimumSize);
    QSizeF layoutMaximumSize = l->effectiveSizeHint(Qt::MaximumSize);
    QSizeF layoutPreferredSize = l->effectiveSizeHint(Qt::PreferredSize);

    QCOMPARE(layoutMinimumSize, QSizeF(60.0, 300.0));
    QCOMPARE(layoutPreferredSize, QSizeF(120.0, 300.0));
    QCOMPARE(layoutMaximumSize, QSizeF(190.0, 300.0));

    p.resize(layoutMinimumSize);
    QCOMPARE(a->geometry(), QRectF(0.0, 0.0, 50.0, 100.0));
    QCOMPARE(b->geometry(), QRectF(10.0, 100.0, 40.0, 100.0));
    QCOMPARE(c->geometry(), QRectF(10.0, 200.0, 50.0, 100.0));
    QCOMPARE(p.size(), layoutMinimumSize);

    p.resize(layoutPreferredSize);
    QCOMPARE(a->geometry(), QRectF(0.0, 0.0, 70.0, 100.0));
    QCOMPARE(b->geometry(), QRectF(50.0, 100.0, 20.0, 100.0));
    QCOMPARE(c->geometry(), QRectF(50.0, 200.0, 70.0, 100.0));
    QCOMPARE(p.size(), layoutPreferredSize);

    p.resize(layoutMaximumSize);
    QCOMPARE(a->geometry(), QRectF(0.0, 0.0, 100.0, 100.0));
    QCOMPARE(b->geometry(), QRectF(90.0, 100.0, 10.0, 100.0));
    QCOMPARE(c->geometry(), QRectF(90.0, 200.0, 100.0, 100.0));
    QCOMPARE(p.size(), layoutMaximumSize);
}

void tst_QGraphicsAnchorLayout::snakeOppositeDirections()
{
    QGraphicsWidget *a = createItem(QSizeF(50.0, 100.0),
                                    QSizeF(70.0, 100.0),
                                    QSizeF(100.0, 100.0));

    QGraphicsWidget *b = createItem(QSizeF(10.0, 100.0),
                                    QSizeF(20.0, 100.0),
                                    QSizeF(40.0, 100.0));

    QGraphicsWidget *c = createItem(QSizeF(50.0, 100.0),
                                    QSizeF(70.0, 100.0),
                                    QSizeF(100.0, 100.0));

    QGraphicsAnchorLayout *l = new QGraphicsAnchorLayout;
    l->setContentsMargins(0, 0, 0, 0);

    l->anchor(l, QGraphicsAnchorLayout::Top, a, QGraphicsAnchorLayout::Top, 0);
    l->anchor(a, QGraphicsAnchorLayout::Bottom, b, QGraphicsAnchorLayout::Top, 0);
    l->anchor(b, QGraphicsAnchorLayout::Bottom, c, QGraphicsAnchorLayout::Top, 0);
    l->anchor(c, QGraphicsAnchorLayout::Bottom, l, QGraphicsAnchorLayout::Bottom, 0);

    l->anchor(l, QGraphicsAnchorLayout::Left, a, QGraphicsAnchorLayout::Left, 0);

    // Both a and c are 'pointing' to b
    l->anchor(a, QGraphicsAnchorLayout::Right, b, QGraphicsAnchorLayout::Right, 0);
    l->anchor(c, QGraphicsAnchorLayout::Left, b, QGraphicsAnchorLayout::Left, 0);

    l->anchor(c, QGraphicsAnchorLayout::Right, l, QGraphicsAnchorLayout::Right, 0);

    QCOMPARE(l->count(), 3);

    QGraphicsWidget p;
    p.setLayout(l);

    QSizeF layoutMinimumSize = l->effectiveSizeHint(Qt::MinimumSize);
    QSizeF layoutMaximumSize = l->effectiveSizeHint(Qt::MaximumSize);
    QSizeF layoutPreferredSize = l->effectiveSizeHint(Qt::PreferredSize);

    QCOMPARE(layoutMinimumSize, QSizeF(60.0, 300.0));
    QCOMPARE(layoutPreferredSize, QSizeF(120.0, 300.0));
    QCOMPARE(layoutMaximumSize, QSizeF(190.0, 300.0));

    p.resize(layoutMinimumSize);
    QCOMPARE(a->geometry(), QRectF(0.0, 0.0, 50.0, 100.0));
    QCOMPARE(b->geometry(), QRectF(10.0, 100.0, 40.0, 100.0));
    QCOMPARE(c->geometry(), QRectF(10.0, 200.0, 50.0, 100.0));
    QCOMPARE(p.size(), layoutMinimumSize);

    p.resize(layoutPreferredSize);
    QCOMPARE(a->geometry(), QRectF(0.0, 0.0, 70.0, 100.0));
    QCOMPARE(b->geometry(), QRectF(50.0, 100.0, 20.0, 100.0));
    QCOMPARE(c->geometry(), QRectF(50.0, 200.0, 70.0, 100.0));
    QCOMPARE(p.size(), layoutPreferredSize);

    p.resize(layoutMaximumSize);
    QCOMPARE(a->geometry(), QRectF(0.0, 0.0, 100.0, 100.0));
    QCOMPARE(b->geometry(), QRectF(90.0, 100.0, 10.0, 100.0));
    QCOMPARE(c->geometry(), QRectF(90.0, 200.0, 100.0, 100.0));
    QCOMPARE(p.size(), layoutMaximumSize);
}

void tst_QGraphicsAnchorLayout::fairDistribution()
{
    QGraphicsWidget *a = createItem(QSizeF(10.0, 100.0),
                                    QSizeF(50.0, 100.0),
                                    QSizeF(100.0, 100.0));

    QGraphicsWidget *b = createItem(QSizeF(10.0, 100.0),
                                    QSizeF(50.0, 100.0),
                                    QSizeF(100.0, 100.0));

    QGraphicsWidget *c = createItem(QSizeF(10.0, 100.0),
                                    QSizeF(50.0, 100.0),
                                    QSizeF(100.0, 100.0));

    QGraphicsWidget *d = createItem(QSizeF(60.0, 100.0),
                                    QSizeF(165.0, 100.0),
                                    QSizeF(600.0, 100.0));


    QGraphicsAnchorLayout *l = new QGraphicsAnchorLayout;
    l->setContentsMargins(0, 0, 0, 0);

    l->anchor(l, QGraphicsAnchorLayout::Top, a, QGraphicsAnchorLayout::Top, 0);
    l->anchor(a, QGraphicsAnchorLayout::Bottom, b, QGraphicsAnchorLayout::Top, 0);
    l->anchor(b, QGraphicsAnchorLayout::Bottom, c, QGraphicsAnchorLayout::Top, 0);
    l->anchor(c, QGraphicsAnchorLayout::Bottom, d, QGraphicsAnchorLayout::Top, 0);
    l->anchor(d, QGraphicsAnchorLayout::Bottom, l, QGraphicsAnchorLayout::Bottom, 0);

    l->anchor(l, QGraphicsAnchorLayout::Left, a, QGraphicsAnchorLayout::Left, 0);
    l->anchor(a, QGraphicsAnchorLayout::Right, b, QGraphicsAnchorLayout::Left, 0);
    l->anchor(b, QGraphicsAnchorLayout::Right, c, QGraphicsAnchorLayout::Left, 0);
    l->anchor(c, QGraphicsAnchorLayout::Right, l, QGraphicsAnchorLayout::Right, 0);
    l->anchor(l, QGraphicsAnchorLayout::Left, d, QGraphicsAnchorLayout::Left, 0);
    l->anchor(d, QGraphicsAnchorLayout::Right, l, QGraphicsAnchorLayout::Right, 0);

    QCOMPARE(l->count(), 4);

    QGraphicsWidget p;
    p.setLayout(l);

    QSizeF layoutMinimumSize = l->effectiveSizeHint(Qt::MinimumSize);
    QSizeF layoutMaximumSize = l->effectiveSizeHint(Qt::MaximumSize);
    QSizeF layoutPreferredSize = l->effectiveSizeHint(Qt::PreferredSize);

    QCOMPARE(layoutMinimumSize, QSizeF(60.0, 400.0));
    QCOMPARE(layoutPreferredSize, QSizeF(165.0, 400.0));
    QCOMPARE(layoutMaximumSize, QSizeF(300.0, 400.0));

    p.resize(layoutMinimumSize);
    QCOMPARE(a->geometry(), QRectF(0.0, 0.0, 20.0, 100.0));
    QCOMPARE(b->geometry(), QRectF(20.0, 100.0, 20.0, 100.0));
    QCOMPARE(c->geometry(), QRectF(40.0, 200.0, 20.0, 100.0));
    QCOMPARE(d->geometry(), QRectF(0.0, 300.0, 60.0, 100.0));
    QCOMPARE(p.size(), layoutMinimumSize);

    p.resize(layoutPreferredSize);
    QCOMPARE(a->geometry(), QRectF(0.0, 0.0, 55.0, 100.0));
    QCOMPARE(b->geometry(), QRectF(55.0, 100.0, 55.0, 100.0));
    QCOMPARE(c->geometry(), QRectF(110.0, 200.0, 55.0, 100.0));
    QCOMPARE(d->geometry(), QRectF(0.0, 300.0, 165.0, 100.0));
    QCOMPARE(p.size(), layoutPreferredSize);

    p.resize(layoutMaximumSize);
    QCOMPARE(a->geometry(), QRectF(0.0, 0.0, 100.0, 100.0));
    QCOMPARE(b->geometry(), QRectF(100.0, 100.0, 100.0, 100.0));
    QCOMPARE(c->geometry(), QRectF(200.0, 200.0, 100.0, 100.0));
    QCOMPARE(d->geometry(), QRectF(0.0, 300.0, 300.0, 100.0));
    QCOMPARE(p.size(), layoutMaximumSize);
}

void tst_QGraphicsAnchorLayout::fairDistributionOppositeDirections()
{
    QGraphicsWidget *a = createItem(QSizeF(10.0, 100.0),
                                    QSizeF(50.0, 100.0),
                                    QSizeF(100.0, 100.0));

    QGraphicsWidget *b = createItem(QSizeF(10.0, 100.0),
                                    QSizeF(50.0, 100.0),
                                    QSizeF(100.0, 100.0));

    QGraphicsWidget *c = createItem(QSizeF(10.0, 100.0),
                                    QSizeF(50.0, 100.0),
                                    QSizeF(100.0, 100.0));

    QGraphicsWidget *d = createItem(QSizeF(10.0, 100.0),
                                    QSizeF(50.0, 100.0),
                                    QSizeF(100.0, 100.0));

    QGraphicsWidget *e = createItem(QSizeF(60.0, 100.0),
                                    QSizeF(220.0, 100.0),
                                    QSizeF(600.0, 100.0));


    QGraphicsAnchorLayout *l = new QGraphicsAnchorLayout;
    l->setContentsMargins(0, 0, 0, 0);

    l->anchor(l, QGraphicsAnchorLayout::Top, a, QGraphicsAnchorLayout::Top, 0);
    l->anchor(a, QGraphicsAnchorLayout::Bottom, b, QGraphicsAnchorLayout::Top, 0);
    l->anchor(b, QGraphicsAnchorLayout::Bottom, c, QGraphicsAnchorLayout::Top, 0);
    l->anchor(c, QGraphicsAnchorLayout::Bottom, d, QGraphicsAnchorLayout::Top, 0);
    l->anchor(d, QGraphicsAnchorLayout::Bottom, e, QGraphicsAnchorLayout::Top, 0);
    l->anchor(e, QGraphicsAnchorLayout::Bottom, l, QGraphicsAnchorLayout::Bottom, 0);

    l->anchor(a, QGraphicsAnchorLayout::Left, l, QGraphicsAnchorLayout::Left, 0);
    l->anchor(b, QGraphicsAnchorLayout::Left, a, QGraphicsAnchorLayout::Right, 0);
    l->anchor(c, QGraphicsAnchorLayout::Left, b, QGraphicsAnchorLayout::Right, 0);
    l->anchor(d, QGraphicsAnchorLayout::Left, c, QGraphicsAnchorLayout::Right, 0);
    l->anchor(d, QGraphicsAnchorLayout::Right, l, QGraphicsAnchorLayout::Right, 0);
    l->anchorWidth(l, e, 0);

    QCOMPARE(l->count(), 5);

    QGraphicsWidget p;
    p.setLayout(l);

    QSizeF layoutMinimumSize = l->effectiveSizeHint(Qt::MinimumSize);
    QSizeF layoutMaximumSize = l->effectiveSizeHint(Qt::MaximumSize);
    QSizeF layoutPreferredSize = l->effectiveSizeHint(Qt::PreferredSize);

    QCOMPARE(layoutMinimumSize, QSizeF(60.0, 500.0));
    QCOMPARE(layoutPreferredSize, QSizeF(220.0, 500.0));
    QCOMPARE(layoutMaximumSize, QSizeF(400.0, 500.0));

    p.resize(layoutMinimumSize);
    QCOMPARE(a->size(), b->size());
    QCOMPARE(a->size(), c->size());
    QCOMPARE(a->size(), d->size());
    QCOMPARE(e->size().width(), 4 * a->size().width());
    QCOMPARE(p.size(), layoutMinimumSize);

    p.resize(layoutPreferredSize);
    QCOMPARE(a->size(), b->size());
    QCOMPARE(a->size(), c->size());
    QCOMPARE(a->size(), d->size());
    QCOMPARE(e->size().width(), 4 * a->size().width());
    QCOMPARE(p.size(), layoutPreferredSize);

    p.resize(layoutMaximumSize);
    QCOMPARE(a->size(), b->size());
    QCOMPARE(a->size(), c->size());
    QCOMPARE(a->size(), d->size());
    QCOMPARE(e->size().width(), 4 * a->size().width());
    QCOMPARE(p.size(), layoutMaximumSize);
}

void tst_QGraphicsAnchorLayout::proportionalPreferred()
{
    QSizeF min(0, 100);

    QGraphicsWidget *a = createItem(min, QSizeF(10, 100), QSizeF(20, 100));
    QGraphicsWidget *b = createItem(min, QSizeF(20, 100), QSizeF(30, 100));
    QGraphicsWidget *c = createItem(min, QSizeF(14, 100), QSizeF(20, 100));
    QGraphicsWidget *d = createItem(min, QSizeF(10, 100), QSizeF(20, 100));

    QGraphicsAnchorLayout *l = new QGraphicsAnchorLayout;
    l->setContentsMargins(0, 0, 0, 0);

    l->anchor(l, QGraphicsAnchorLayout::Top, a, QGraphicsAnchorLayout::Top, 0);
    l->anchor(a, QGraphicsAnchorLayout::Bottom, b, QGraphicsAnchorLayout::Top, 0);
    l->anchor(b, QGraphicsAnchorLayout::Bottom, c, QGraphicsAnchorLayout::Top, 0);
    l->anchor(c, QGraphicsAnchorLayout::Bottom, d, QGraphicsAnchorLayout::Top, 0);
    l->anchor(d, QGraphicsAnchorLayout::Bottom, l, QGraphicsAnchorLayout::Bottom, 0);

    l->anchor(l, QGraphicsAnchorLayout::Left, a, QGraphicsAnchorLayout::Left, 0);
    l->anchor(l, QGraphicsAnchorLayout::Left, b, QGraphicsAnchorLayout::Left, 0);
    l->anchor(a, QGraphicsAnchorLayout::Right, c, QGraphicsAnchorLayout::Left, 0);
    l->anchor(a, QGraphicsAnchorLayout::Right, d, QGraphicsAnchorLayout::Left, 0);
    l->anchor(b, QGraphicsAnchorLayout::Right, l, QGraphicsAnchorLayout::Right, 0);
    l->anchor(c, QGraphicsAnchorLayout::Right, l, QGraphicsAnchorLayout::Right, 0);
    l->anchor(d, QGraphicsAnchorLayout::Right, l, QGraphicsAnchorLayout::Right, 0);

    QCOMPARE(l->count(), 4);

    QGraphicsWidget p;
    p.setLayout(l);

    QSizeF layoutMinimumSize = l->effectiveSizeHint(Qt::MinimumSize);
    QSizeF layoutPreferredSize = l->effectiveSizeHint(Qt::PreferredSize);
    QSizeF layoutMaximumSize = l->effectiveSizeHint(Qt::MaximumSize);

    QCOMPARE(layoutMinimumSize, QSizeF(0, 400));
    QCOMPARE(layoutPreferredSize, QSizeF(24, 400));
    QCOMPARE(layoutMaximumSize, QSizeF(30, 400));

    p.resize(layoutMinimumSize);
    QCOMPARE(p.size(), layoutMinimumSize);

    p.resize(layoutPreferredSize);
    QCOMPARE(c->size().width(), d->size().width());
    QCOMPARE(p.size(), layoutPreferredSize);

    p.resize(layoutMaximumSize);
    QCOMPARE(p.size(), layoutMaximumSize);

    p.resize(QSizeF(12, 400));

    // Proportionality between size given and preferred size, this
    // should be respected in this graph for (a) and (b)|(c).
    qreal factor = 12.0 / 24.0;

    QCOMPARE(c->size().width(), d->size().width());
    QEXPECT_FAIL("", "see commit 23441f49a23cbf936b60140c5c8a6d5cb3ca00a7 for explanation", Abort);
    QCOMPARE(a->size().width() * factor, 10 * factor);
    QCOMPARE(c->size().width() * factor, 14 * factor);
    QCOMPARE(p.size(), QSizeF(12, 400));
}

void tst_QGraphicsAnchorLayout::example()
{
    QSizeF min(30, 100);
    QSizeF pref(210, 100);
    QSizeF max(300, 100);

    QGraphicsWidget *a = createItem(min, pref, max);
    QGraphicsWidget *b = createItem(min, pref, max);
    QGraphicsWidget *c = createItem(min, pref, max);
    QGraphicsWidget *d = createItem(min, pref, max);
    QGraphicsWidget *e = createItem(min, pref, max);
    QGraphicsWidget *f = createItem(QSizeF(30, 50), QSizeF(150, 50), max);
    QGraphicsWidget *g = createItem(QSizeF(30, 50), QSizeF(30, 100), max);

    QGraphicsAnchorLayout *l = new QGraphicsAnchorLayout;
    l->setContentsMargins(0, 0, 0, 0);

    // vertical
    l->anchor(a, QGraphicsAnchorLayout::Top, l, QGraphicsAnchorLayout::Top, 0);
    l->anchor(b, QGraphicsAnchorLayout::Top, l, QGraphicsAnchorLayout::Top, 0);

    l->anchor(c, QGraphicsAnchorLayout::Top, a, QGraphicsAnchorLayout::Bottom, 0);
    l->anchor(c, QGraphicsAnchorLayout::Top, b, QGraphicsAnchorLayout::Bottom, 0);
    l->anchor(c, QGraphicsAnchorLayout::Bottom, d, QGraphicsAnchorLayout::Top, 0);
    l->anchor(c, QGraphicsAnchorLayout::Bottom, e, QGraphicsAnchorLayout::Top, 0);

    l->anchor(d, QGraphicsAnchorLayout::Bottom, l, QGraphicsAnchorLayout::Bottom, 0);
    l->anchor(e, QGraphicsAnchorLayout::Bottom, l, QGraphicsAnchorLayout::Bottom, 0);

    l->anchor(c, QGraphicsAnchorLayout::Top, f, QGraphicsAnchorLayout::Top, 0);
    l->anchor(c, QGraphicsAnchorLayout::VCenter, f, QGraphicsAnchorLayout::Bottom, 0);
    l->anchor(f, QGraphicsAnchorLayout::Bottom, g, QGraphicsAnchorLayout::Top, 0);
    l->anchor(c, QGraphicsAnchorLayout::Bottom, g, QGraphicsAnchorLayout::Bottom, 0);

    // horizontal
    l->anchor(l, QGraphicsAnchorLayout::Left, a, QGraphicsAnchorLayout::Left, 0);
    l->anchor(l, QGraphicsAnchorLayout::Left, d, QGraphicsAnchorLayout::Left, 0);
    l->anchor(a, QGraphicsAnchorLayout::Right, b, QGraphicsAnchorLayout::Left, 0);

    l->anchor(a, QGraphicsAnchorLayout::Right, c, QGraphicsAnchorLayout::Left, 0);
    l->anchor(c, QGraphicsAnchorLayout::Right, e, QGraphicsAnchorLayout::Left, 0);

    l->anchor(b, QGraphicsAnchorLayout::Right, l, QGraphicsAnchorLayout::Right, 0);
    l->anchor(e, QGraphicsAnchorLayout::Right, l, QGraphicsAnchorLayout::Right, 0);
    l->anchor(d, QGraphicsAnchorLayout::Right, e, QGraphicsAnchorLayout::Left, 0);

    l->anchor(l, QGraphicsAnchorLayout::Left, f, QGraphicsAnchorLayout::Left, 0);
    l->anchor(l, QGraphicsAnchorLayout::Left, g, QGraphicsAnchorLayout::Left, 0);
    l->anchor(f, QGraphicsAnchorLayout::Right, g, QGraphicsAnchorLayout::Right, 0);

    QCOMPARE(l->count(), 7);

    QGraphicsWidget p;
    p.setLayout(l);

    QSizeF layoutMinimumSize = l->effectiveSizeHint(Qt::MinimumSize);
    QSizeF layoutMaximumSize = l->effectiveSizeHint(Qt::MaximumSize);
    QSizeF layoutPreferredSize = l->effectiveSizeHint(Qt::PreferredSize);

    QCOMPARE(layoutMinimumSize, QSizeF(90.0, 300.0));
    QCOMPARE(layoutPreferredSize, QSizeF(510.0, 300.0));
    QCOMPARE(layoutMaximumSize, QSizeF(570.0, 300.0));

    p.resize(layoutMinimumSize);
    QCOMPARE(p.size(), layoutMinimumSize);
    QCOMPARE(a->size(), e->size());
    QCOMPARE(b->size(), d->size());
    QEXPECT_FAIL("", "please fix this test", Abort);
    QCOMPARE(f->size(), g->size());

    p.resize(layoutPreferredSize);
    QCOMPARE(p.size(), layoutPreferredSize);
    QCOMPARE(a->size(), e->size());
    QCOMPARE(b->size(), d->size());
    QCOMPARE(f->size(), g->size());

    p.resize(layoutMaximumSize);
    QCOMPARE(p.size(), layoutMaximumSize);
    QCOMPARE(a->size(), e->size());
    QCOMPARE(b->size(), d->size());
    QCOMPARE(f->size(), g->size());
}

QTEST_MAIN(tst_QGraphicsAnchorLayout)
#include "tst_qgraphicsanchorlayout.moc"

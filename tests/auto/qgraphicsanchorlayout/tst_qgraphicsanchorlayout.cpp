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
    void snake();
    void fairDistribution();
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
                                   const QSizeF &maximum = QSizeF(200.0, 100.0))
{
    QGraphicsWidget *w = new RectWidget;
    w->setMinimumSize(minimum);
    w->setPreferredSize(preferred);
    w->setMaximumSize(maximum);
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
    QGraphicsWidget *a = createItem(QSizeF(100.0, 100.0),
                                    QSizeF(150.0, 100.0),
                                    QSizeF(200.0, 100.0));

    QGraphicsWidget *b = createItem(QSizeF(100.0, 100.0),
                                    QSizeF(150.0, 100.0),
                                    QSizeF(200.0, 100.0));

    QGraphicsWidget *c = createItem(QSizeF(100.0, 100.0),
                                    QSizeF(200.0, 100.0),
                                    QSizeF(300.0, 100.0));

    QGraphicsWidget *d = createItem(QSizeF(100.0, 100.0),
                                    QSizeF(170.0, 100.0),
                                    QSizeF(200.0, 100.0));

    QGraphicsWidget *e = createItem(QSizeF(150.0, 100.0),
                                    QSizeF(150.0, 100.0),
                                    QSizeF(200.0, 100.0));

    QGraphicsWidget *f = createItem(QSizeF(100.0, 100.0),
                                    QSizeF(150.0, 100.0),
                                    QSizeF(200.0, 100.0));

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

    QCOMPARE(layoutMinimumSize, QSizeF(450.0, 600.0));
    QCOMPARE(layoutPreferredSize, QSizeF(600.0, 600.0));
    QCOMPARE(layoutMaximumSize, QSizeF(700.0, 600.0));

    p.resize(layoutMinimumSize);
    QCOMPARE(a->geometry(), QRectF(0.0, 0.0, 100.0, 100.0));
    QCOMPARE(b->geometry(), QRectF(100.0, 100.0, 100.0, 100.0));
    QCOMPARE(c->geometry(), QRectF(100.0, 200.0, 250.0, 100.0));
    QCOMPARE(d->geometry(), QRectF(200.0, 300.0, 150.0, 100.0));
    QCOMPARE(e->geometry(), QRectF(200.0, 400.0, 150.0, 100.0));
    QCOMPARE(f->geometry(), QRectF(350.0, 500.0, 100.0, 100.0));
    QCOMPARE(p.size(), layoutMinimumSize);

    p.resize(layoutPreferredSize);
    QCOMPARE(a->geometry(), QRectF(0.0, 0.0, 150.0, 100.0));
    QCOMPARE(b->geometry(), QRectF(150.0, 100.0, 150.0, 100.0));
    QCOMPARE(c->geometry(), QRectF(150.0, 200.0, 300.0, 100.0));
    QCOMPARE(d->geometry(), QRectF(300.0, 300.0, 150.0, 100.0));
    QCOMPARE(e->geometry(), QRectF(300.0, 400.0, 150.0, 100.0));
    QCOMPARE(f->geometry(), QRectF(450.0, 500.0, 150.0, 100.0));
    QCOMPARE(p.size(), layoutPreferredSize);

    // Maximum size depends on simplification / fair distribution
    // Without that, test may or may not pass, depending on the
    // solution found by the solver at runtime.
    p.resize(layoutMaximumSize);
    QCOMPARE(a->geometry(), QRectF(0.0, 0.0, 200.0, 100.0));
    QCOMPARE(b->geometry(), QRectF(200.0, 100.0, 150.0, 100.0));
    QCOMPARE(c->geometry(), QRectF(200.0, 200.0, 300.0, 100.0));
    QCOMPARE(d->geometry(), QRectF(350.0, 300.0, 150.0, 100.0));
    QCOMPARE(e->geometry(), QRectF(350.0, 400.0, 150.0, 100.0));
    QCOMPARE(f->geometry(), QRectF(500.0, 500.0, 200.0, 100.0));
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


QTEST_MAIN(tst_QGraphicsAnchorLayout)
#include "tst_qgraphicsanchorlayout.moc"

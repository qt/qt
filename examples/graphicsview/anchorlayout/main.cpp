#include <QGraphicsWidget>
#include <QGraphicsProxyWidget>
#include <QGraphicsAnchorLayout>
#include <QtGui>

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

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QGraphicsScene scene;
    scene.setSceneRect(0, 0, 800, 480);

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

    scene.addItem(w);
    scene.setBackgroundBrush(Qt::darkGreen);
    QGraphicsView *view = new QGraphicsView(&scene);
    view->show();

    return app.exec();
}

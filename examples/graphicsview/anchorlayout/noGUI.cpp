#include <QGraphicsWidget>
#include "qgraphicsanchorlayout.h"

// DEBUG
#include "qgraphicsanchorlayout_p.h"

static QGraphicsWidget *createItem(const QSizeF &minimum = QSizeF(100.0, 100.0),
                                   const QSizeF &preferred = QSize(150.0, 100.0),
                                   const QSizeF &maximum = QSizeF(200.0, 100.0))
{
    QGraphicsWidget *w = new QGraphicsWidget;
    w->setMinimumSize(minimum);
    w->setPreferredSize(preferred);
    w->setMaximumSize(maximum);
    return w;
}


int main(int argc, char **argv)
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);


    QGraphicsWidget *a = createItem(QSizeF(50.0, 100.0),
                                    QSizeF(70.0, 100.0),
                                    QSizeF(100.0, 200.0));
    a->setData(0, "A");

    QGraphicsWidget *b = createItem(QSizeF(10.0, 100.0),
                                    QSizeF(20.0, 100.0),
                                    QSizeF(40.0, 200.0));
    b->setData(0, "B");

    QGraphicsWidget *c = createItem(QSizeF(50.0, 100.0),
                                    QSizeF(70.0, 100.0),
                                    QSizeF(100.0, 200.0));
    c->setData(0, "C");

    QGraphicsAnchorLayout *l = new QGraphicsAnchorLayout;

    l->anchor(l, QGraphicsAnchorLayout::Top, a, QGraphicsAnchorLayout::Top);
    l->anchor(a, QGraphicsAnchorLayout::Bottom, b, QGraphicsAnchorLayout::Top);
    l->anchor(b, QGraphicsAnchorLayout::Bottom, c, QGraphicsAnchorLayout::Top);
    l->anchor(c, QGraphicsAnchorLayout::Bottom, l, QGraphicsAnchorLayout::Bottom);

    l->anchor(l, QGraphicsAnchorLayout::Left, a, QGraphicsAnchorLayout::Left);
    l->anchor(a, QGraphicsAnchorLayout::Right, b, QGraphicsAnchorLayout::Right);
    l->anchor(b, QGraphicsAnchorLayout::Left, c, QGraphicsAnchorLayout::Left);
    l->anchor(c, QGraphicsAnchorLayout::Right, l, QGraphicsAnchorLayout::Right);

    l->dumpGraph();

    QSizeF layoutMinimumSize = l->effectiveSizeHint(Qt::MinimumSize);
    QSizeF layoutMaximumSize = l->effectiveSizeHint(Qt::MaximumSize);
    QSizeF layoutPreferredSize = l->effectiveSizeHint(Qt::PreferredSize);


    qWarning() << "Layout Min/Pref/Max: " << layoutMinimumSize
               << " " << layoutPreferredSize
               << " " << layoutMaximumSize;


    qWarning() << "\nSetting minimum size";
    l->setGeometry(QRectF(QPointF(0,0), layoutMinimumSize));
    qWarning() << "A: " << a->geometry();
    qWarning() << "B: " << b->geometry();
    qWarning() << "C: " << c->geometry();

    qWarning() << "\nSetting maximum size";
    l->setGeometry(QRectF(QPointF(0,0), layoutMaximumSize));
    qWarning() << "A: " << a->geometry();
    qWarning() << "B: " << b->geometry();
    qWarning() << "C: " << c->geometry();

    qWarning() << "\nSetting size 85.0";
    l->setGeometry(QRectF(QPointF(0,0), QSizeF(85.0, 200.0)));
    qWarning() << "A: " << a->geometry();
    qWarning() << "B: " << b->geometry();
    qWarning() << "C: " << c->geometry();

    return 0;
}

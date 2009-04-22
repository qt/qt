#include <QtGui>
#ifdef QT_EXPERIMENTAL_SOLUTION
# include "qgraphicswidget.h"
# include "qstate.h"
# include "qstatemachine.h"
# include "qgraphicswidget.h"
# include "qparallelanimationgroup.h"
# include "qpropertyanimation.h"
#else
#include <QtCore/qstate.h>
#endif

class Pixmap : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)
public:
    Pixmap(const QPixmap &pix)
        : QObject(), QGraphicsPixmapItem(pix)
    {
        setCacheMode(DeviceCoordinateCache);
    }
};

class Button : public QGraphicsWidget
{
    Q_OBJECT
public:
    Button(const QPixmap &pixmap, QGraphicsItem *parent = 0)
        : QGraphicsWidget(parent), _pix(pixmap)
    {
        setAcceptHoverEvents(true);
        setCacheMode(DeviceCoordinateCache);
    }

    QRectF boundingRect() const
    {
        return QRectF(-65, -65, 130, 130);
    }

    QPainterPath shape() const
    {
        QPainterPath path;
        path.addEllipse(boundingRect());
        return path;
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
    {
        bool down = option->state & QStyle::State_Sunken;
        QRectF r = boundingRect();
        QLinearGradient grad(r.topLeft(), r.bottomRight());
        grad.setColorAt(down ? 1 : 0, option->state & QStyle::State_MouseOver ? Qt::white : Qt::lightGray);
        grad.setColorAt(down ? 0 : 1, Qt::darkGray);
        painter->setPen(Qt::darkGray);
        painter->setBrush(grad);
        painter->drawEllipse(r);
        QLinearGradient grad2(r.topLeft(), r.bottomRight());
        grad.setColorAt(down ? 1 : 0, Qt::darkGray);
        grad.setColorAt(down ? 0 : 1, Qt::lightGray);
        painter->setPen(Qt::NoPen);
        painter->setBrush(grad);
        if (down)
            painter->translate(2, 2);
        painter->drawEllipse(r.adjusted(5, 5, -5, -5));
        painter->drawPixmap(-_pix.width()/2, -_pix.height()/2, _pix);
    }

signals:
    void pressed();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *)
    {
        emit pressed();
        update();
    }

    void mouseReleaseEvent(QGraphicsSceneMouseEvent *)
    {
        update();
    }

private:
    QPixmap _pix;
};

class View : public QGraphicsView
{
public:
    View(QGraphicsScene *scene) : QGraphicsView(scene) { }

protected:
    void resizeEvent(QResizeEvent *event)
    {
        QGraphicsView::resizeEvent(event);
        fitInView(sceneRect(), Qt::KeepAspectRatio);
    }
};

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QPixmap kineticPix(":/images/kinetic.png");
    QPixmap bgPix(":/images/Time-For-Lunch-2.jpg");

    QGraphicsScene scene(-350, -350, 700, 700);

    QList<Pixmap *> items;
    for (int i = 0; i < 64; ++i) {
        Pixmap *item = new Pixmap(kineticPix);
        item->setOffset(-kineticPix.width()/2, -kineticPix.height()/2);
        item->setZValue(i);
        items << item;
        scene.addItem(item);
    }

    // Buttons
    QGraphicsItem *buttonParent = new QGraphicsRectItem;
    Button *ellipseButton = new Button(QPixmap(":/images/ellipse.png"), buttonParent);
    Button *figure8Button = new Button(QPixmap(":/images/figure8.png"), buttonParent);
    Button *randomButton = new Button(QPixmap(":/images/random.png"), buttonParent);
    Button *tiledButton = new Button(QPixmap(":/images/tile.png"), buttonParent);
    Button *centeredButton = new Button(QPixmap(":/images/centered.png"), buttonParent);

    ellipseButton->setPos(-100, -100);
    figure8Button->setPos(100, -100);
    randomButton->setPos(0, 0);
    tiledButton->setPos(-100, 100);
    centeredButton->setPos(100, 100);

    scene.addItem(buttonParent);
    buttonParent->scale(0.75, 0.75);
    buttonParent->setPos(200, 200);
    buttonParent->setZValue(65);

    // States
    QState *rootState = new QState;
    QState *ellipseState = new QState(rootState);
    QState *figure8State = new QState(rootState);
    QState *randomState = new QState(rootState);
    QState *tiledState = new QState(rootState);
    QState *centeredState = new QState(rootState);

    // Values
    for (int i = 0; i < 64; ++i) {
        Pixmap *item = items.at(i);
        // Ellipse
        ellipseState->setPropertyOnEntry(item, "pos",
                                         QPointF(cos((i / 63.0) * 6.28) * 250,
                                                 sin((i / 63.0) * 6.28) * 250));

        // Figure 8
        figure8State->setPropertyOnEntry(item, "pos",
                                         QPointF(sin((i / 63.0) * 6.28) * 250,
                                                 sin(((i * 2)/63.0) * 6.28) * 250));

        // Random
        randomState->setPropertyOnEntry(item, "pos",
                                        QPointF(-250 + qrand() % 500,
                                                -250 + qrand() % 500));

        // Tiled
        tiledState->setPropertyOnEntry(item, "pos",
                                       QPointF(((i % 8) - 4) * kineticPix.width() + kineticPix.width() / 2,
                                               ((i / 8) - 4) * kineticPix.height() + kineticPix.height() / 2));

        // Centered
        centeredState->setPropertyOnEntry(item, "pos", QPointF());
    }

    // Ui
    View *view = new View(&scene);
    view->setWindowTitle(QT_TRANSLATE_NOOP(QGraphicsView, "Animated Tiles"));
    view->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    view->setBackgroundBrush(bgPix);
    view->setCacheMode(QGraphicsView::CacheBackground);
    view->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    view->show();

    QStateMachine states;
    states.addState(rootState);
    states.setInitialState(rootState);
    rootState->setInitialState(centeredState);

    // rootState->addTransition(ellipseButton, SIGNAL(pressed()), ellipseState);
    QParallelAnimationGroup *group = new QParallelAnimationGroup;
    for (int i = 0; i < 64; ++i) {
        QPropertyAnimation *anim = new QPropertyAnimation(items[i], "pos");
        anim->setDuration(750 + i * 25);
        anim->setEasingCurve(QEasingCurve::InOutBack);
        group->addAnimation(anim);
    }
    rootState->addAnimatedTransition(ellipseButton, SIGNAL(pressed()), ellipseState, group);

    group = new QParallelAnimationGroup;
    for (int i = 0; i < 64; ++i) {
        QPropertyAnimation *anim = new QPropertyAnimation(items[i], "pos");
        anim->setDuration(750 + i * 25);
        anim->setEasingCurve(QEasingCurve::InOutBack);
        group->addAnimation(anim);
    }
    rootState->addAnimatedTransition(figure8Button, SIGNAL(pressed()), figure8State, group);

    group = new QParallelAnimationGroup;
    for (int i = 0; i < 64; ++i) {
        QPropertyAnimation *anim = new QPropertyAnimation(items[i], "pos");
        anim->setDuration(750 + i * 25);
        anim->setEasingCurve(QEasingCurve::InOutBack);
        group->addAnimation(anim);
    }
    rootState->addAnimatedTransition(randomButton, SIGNAL(pressed()), randomState, group);

    group = new QParallelAnimationGroup;
    for (int i = 0; i < 64; ++i) {
        QPropertyAnimation *anim = new QPropertyAnimation(items[i], "pos");
        anim->setDuration(750 + i * 25);
        anim->setEasingCurve(QEasingCurve::InOutBack);
        group->addAnimation(anim);
    }
    rootState->addAnimatedTransition(tiledButton, SIGNAL(pressed()), tiledState, group);

    group = new QParallelAnimationGroup;
    for (int i = 0; i < 64; ++i) {
        QPropertyAnimation *anim = new QPropertyAnimation(items[i], "pos");
        anim->setDuration(750 + i * 25);
        anim->setEasingCurve(QEasingCurve::InOutBack);
        group->addAnimation(anim);
    }
    rootState->addAnimatedTransition(centeredButton, SIGNAL(pressed()), centeredState, group);

    states.start();
    QTimer timer;
    timer.start(125);
    timer.setSingleShot(true);
    rootState->addAnimatedTransition(&timer, SIGNAL(timeout()), ellipseState, group);

    return app.exec();
}

#include "main.moc"

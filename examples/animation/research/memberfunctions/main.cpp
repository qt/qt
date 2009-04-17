#include <QtGui>
#include "qvalueanimation.h"

AbstractProperty *qGraphicsItemProperty(QGraphicsItem *item, const char *property)
{
    if (qstrcmp(property, "pos") == 0) {
        return new MemberFunctionProperty<QGraphicsItem, QPointF>(item, &QGraphicsItem::pos, &QGraphicsItem::setPos);
    }
    return 0;
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QGraphicsScene scene;
    QGraphicsView view(&scene);

    QGraphicsItem *item = new QGraphicsRectItem(QRectF(0,0, 200, 100));
    scene.addItem(item);

    QValueAnimation *posAnim = new QValueAnimation;
    posAnim->setStartValue(QPointF(0,0));
    posAnim->setEndValue(QPointF(400, 0));
    posAnim->setDuration(1000);
    // Alternative 1
    //posAnim->setMemberFunction(item, &QGraphicsItem::pos, &QGraphicsItem::setPos);

    // Alternative 2
    //posAnim->setProperty(qMemberFunctionProperty(item, &QGraphicsItem::pos, &QGraphicsItem::setPos));
    
    // Alternative 3
    posAnim->setProperty(qGraphicsItemProperty(item, "pos"));
    
    // Alternative 4, (by implementing the qGraphicsItemProperty  QGraphicsItem::property())
    //posAnim->setProperty(item->property("pos"));

    // can also do this, which abstracts away the whole property thing.
    // i.e. this interface can also be used for QObject-properties:
    //posAnim->setAnimationProperty(animationProperty);

    posAnim->start();

    view.resize(800,600);
    view.show();
    return app.exec();
}


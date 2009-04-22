#include <QtGui>

#include "qpropertytransform.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    
    QGraphicsItem *item = new QGraphicsRectItem(QRectF(0,0, 200, 100));
    scene.addItem(item);
    QPropertyTransform transform;
    transform.setTargetItem(item);

    QAnimationGroup *group = new QAnimationGroup(QAnimationGroup::Parallel, &scene);
    QPropertyAnimation *scaleAnim = new QPropertyAnimation(&transform, "scaleX");
    scaleAnim->setStartValue(1.0);
    scaleAnim->setTargetValue(2.0);
    scaleAnim->setDuration(10000);
    group->add(scaleAnim);

    QPropertyAnimation *scaleAnim2 = new QPropertyAnimation(&transform, "scaleY");
    scaleAnim2->setStartValue(.0);
    scaleAnim2->setTargetValue(2.0);
    scaleAnim2->setDuration(10000);
    QEasingCurve curve(QEasingCurve::InElastic);
    curve.setPeriod(2);
    curve.setAmplitude(2);

    //scaleAnim2->setEasingCurve(curve);
    //scaleAnim2->setEasingCurve(QEasingCurve(QEasingCurve::OutElastic , 2, 2 ));
    group->add(scaleAnim2);
    
    QPropertyAnimation *rotAnim = new QPropertyAnimation(&transform, "rotation");
    rotAnim->setStartValue(0);
    rotAnim->setTargetValue(90);
    rotAnim->setDuration(10000);
    group->add(rotAnim);
    
    group->start();

    view.resize(800,600);
    view.show();
    return app.exec();
}

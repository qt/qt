#include "animation.h"
#include "node.h"
#include "lifecycle.h"
#include "stickman.h"
#include "graphicsview.h"

#include <QtCore>
#include <QtGui>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    StickMan *stickMan = new StickMan;
    stickMan->setDrawSticks(false);

    QGraphicsTextItem *textItem = new QGraphicsTextItem();
    textItem->setHtml("<font color=\"white\"><b>Stickman</b>"
        "<p>"
        "Tell the stickman what to do!"
        "</p>"
        "<p><i>"
        "<li>Press <font color=\"purple\">J</font> to make the stickman jump.</li>"
        "<li>Press <font color=\"purple\">D</font> to make the stickman dance.</li>"
        "<li>Press <font color=\"purple\">C</font> to make him chill out.</li>"
        "<li>Press <font color=\"purple\">Return</font> to make him return to his original position.</li>"
        "<li>When you are done, press <font color=\"purple\">Escape</font>.</li>"
        "</i></p>"
        "<p>If he is unlucky, the stickman will get struck by lightning, and never jump, dance or chill out again."
        "</p></font>");
    qreal w = textItem->boundingRect().width();
    QRectF stickManBoundingRect = stickMan->mapToScene(stickMan->boundingRect()).boundingRect();
    textItem->setPos(-w / 2.0, stickManBoundingRect.bottom() + 25.0);

    QGraphicsScene *scene = new QGraphicsScene();
    scene->addItem(stickMan);
    scene->addItem(textItem);
    scene->setBackgroundBrush(Qt::black);

    GraphicsView *view = new GraphicsView();
    view->setRenderHints(QPainter::Antialiasing);
    view->setTransformationAnchor(QGraphicsView::NoAnchor);
    view->setScene(scene);
    view->showFullScreen();
    view->setFocus();    
    view->setSceneRect(scene->sceneRect());
    
    LifeCycle *cycle = new LifeCycle(stickMan, view);
    cycle->setResetKey(Qt::Key_Return);
    cycle->setDeathAnimation("animations/dead");
    
    cycle->addActivity("animations/jumping", Qt::Key_J);
    cycle->addActivity("animations/dancing", Qt::Key_D);
    cycle->addActivity("animations/chilling", Qt::Key_C);
    cycle->start();

    return app.exec();
}

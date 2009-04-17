#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>

#include "clock.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QGraphicsScene scene;
    
    Clock *clock = new Clock;
    clock->initializeUi();
    clock->initializeStateMachine();
    scene.addItem(clock);

    QGraphicsView view(&scene);
    view.setRenderHint(QPainter::Antialiasing);
    view.show();

    return app.exec();
}

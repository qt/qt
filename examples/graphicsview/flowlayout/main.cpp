#include <QtGui>
#include "window.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QGraphicsScene scene;
    QGraphicsView *view = new QGraphicsView(&scene);
    Window *w = new Window;
    scene.addItem(w);
    view->show();
    return app.exec();
}
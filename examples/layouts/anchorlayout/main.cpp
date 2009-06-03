#include "window.h"
#include <QtGui/qapplication.h>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    Window *w = new Window;
    w->show();
    return app.exec();
}
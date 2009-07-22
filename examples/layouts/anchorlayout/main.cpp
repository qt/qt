#include "window.h"
#include <QtGui/qapplication.h>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    Window *w = new Window(app.arguments());
    w->show();
    return app.exec();
}

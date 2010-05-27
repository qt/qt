#include <QtGui/QApplication>
#include "modelview.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ModelView w;
    w.show();
    return a.exec();
}

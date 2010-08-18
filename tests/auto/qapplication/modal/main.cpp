#include <QtGui>

#include <QApplication>
#include "base.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    base *b = new base();
    return app.exec();
}

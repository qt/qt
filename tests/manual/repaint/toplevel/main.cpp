#include <QtGui>
#include "../shared/shared.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    StaticWidget widget;
    widget.show();
    return app.exec();
}


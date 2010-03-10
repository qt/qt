#include <QtGui>
#include "../shared/shared.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QSplitter splitter;

    splitter.addWidget(new StaticWidget());
    splitter.addWidget(new StaticWidget());

    splitter.resize(600, 400);
    splitter.show();
    
    return app.exec();
}

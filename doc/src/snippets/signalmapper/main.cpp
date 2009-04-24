#include <QtGui>
#include "filereader.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    FileReader *reader = new FileReader;
    reader->show();

    return app.exec();
}


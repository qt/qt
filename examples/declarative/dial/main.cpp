#include <QApplication>
#include <QUrl>
#include <qmlview.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QmlView *canvas = new QmlView;
    canvas->setUrl(QUrl("qrc:/dial.qml"));
    canvas->execute();
    canvas->resize(210,240);
    canvas->show();

    return app.exec();
}



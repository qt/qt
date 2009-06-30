#include <QApplication>
#include <QUrl>
#include <qfxview.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QFxView *canvas = new QFxView;
    canvas->setUrl(QUrl("qrc:/loader.qml"));
    canvas->execute();
    canvas->resize(210,240);
    canvas->show();

    return app.exec();
}



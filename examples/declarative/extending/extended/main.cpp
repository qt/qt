#include <QApplication>
#include <QmlEngine>
#include <QmlComponent>
#include <QDebug>
#include <QLineEdit>

int main(int argc, char ** argv)
{
    QApplication app(argc, argv);

    QmlEngine engine;
    QmlComponent component(&engine, ":example.qml");
    QLineEdit *edit = qobject_cast<QLineEdit *>(component.create());

    if (edit) {
        edit->show();
        return app.exec();
    } else {
        qWarning() << "An error occured";
        return 0;
    }
}

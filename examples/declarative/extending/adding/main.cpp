#include <QCoreApplication>
#include <QmlEngine>
#include <QmlComponent>
#include <QDebug>
#include "person.h"

int main(int argc, char ** argv)
{
    QCoreApplication app(argc, argv);

    QmlEngine engine;
    QmlComponent component(&engine, QUrl::fromLocalFile(":example.qml"));
    Person *person = qobject_cast<Person *>(component.create());
    if (person) {
        qWarning() << "The person's name is" << person->name();
        qWarning() << "They wear a" << person->shoeSize() << "sized shoe";
    } else {
        qWarning() << "An error occured";
    }

    return 0;
}

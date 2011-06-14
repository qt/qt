#include <QApplication>
#include <QDebug>
#include <QDeclarativeComponent>
#include <QDeclarativeEngine>
#include <QStringList>
#include <QtDeclarative/private/qdeclarativedebughelper_p.h>
#include <QtDeclarative/private/qdeclarativedebugservice_p.h>

int main(int argc, char *argv[])
{
    QDeclarativeDebugHelper::enableDebugging();

    QApplication app(argc, argv);

    const QUrl path = QUrl::fromLocalFile(app.arguments().last());

    QDeclarativeEngine engine;
    QDeclarativeComponent component(&engine, path);

    if (!component.isReady()) {
        qWarning() << component.errorString();
        return -1;
    }

    QObject *obj = component.create();

//    printf("%u\n", QDeclarativeDebugService::idForObject(obj));
//    fflush(stdout);

    return app.exec();
}

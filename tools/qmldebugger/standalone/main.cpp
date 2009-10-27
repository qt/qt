#include <QtGui/qapplication.h>

#include "qmldebugger.h"

int main(int argc, char ** argv)
{
    QApplication app(argc, argv);

    QStringList args = app.arguments();

    QmlDebugger win;
    if (args.contains("--engine"))
        win.showEngineTab();

    for (int i=0; i<args.count(); i++) {
        if (!args[i].contains(':'))
            continue;
        QStringList hostAndPort = args[i].split(':');
        bool ok = false;
        quint16 port = hostAndPort.value(1).toInt(&ok);
        if (ok) {
            qWarning() << "qmldebugger connecting to"
                    << hostAndPort[0] << port << "...";
            win.setHost(hostAndPort[0]);
            win.setPort(port);
            win.connectToHost();
            break;
        }
    }

    win.show();

    return app.exec();
}

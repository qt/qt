#include <QApplication>
#include <QUrl>
#include <QFileInfo>
#include <QDir>
#include <QmlContext>
#include <QmlComponent>
#include <qfxview.h>

QFxView *canvas = 0;

class QmlLauncher : public QObject
{
    Q_OBJECT
public:
    QmlLauncher() {}

    Q_INVOKABLE void launch(const QString &fileName)
    {
        QUrl url(fileName);
        QFileInfo fi(fileName);
        if (fi.exists()) {
            url = QUrl::fromLocalFile(fi.absoluteFilePath());
            QmlContext *ctxt = canvas->rootContext();
            QDir dir(fi.path()+"/dummydata", "*.qml");
            QStringList list = dir.entryList();
            for (int i = 0; i < list.size(); ++i) {
                QString qml = list.at(i);
                QFile f(dir.filePath(qml));
                f.open(QIODevice::ReadOnly);
                QByteArray data = f.readAll();
                QmlComponent comp(canvas->engine());
                comp.setData(data, QUrl());
                QObject *dummyData = comp.create();

                if(comp.isError()) {
                    QList<QmlError> errors = comp.errors();
                    foreach (const QmlError &error, errors) {
                        qWarning() << error;
                    }
                }

                if (dummyData) {
                    qml.truncate(qml.length()-4);
                    ctxt->setContextProperty(qml, dummyData);
                    dummyData->setParent(this);
                }
            }
        }

        canvas->setUrl(fileName);
        canvas->execute();
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QmlLauncher *launcher = new QmlLauncher;

    canvas = new QFxView;
    QmlContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("qmlLauncher", launcher);
    canvas->setUrl(QUrl("qrc:/loader.qml"));
    canvas->execute();
    canvas->show();

    return app.exec();
}

#include "main.moc"

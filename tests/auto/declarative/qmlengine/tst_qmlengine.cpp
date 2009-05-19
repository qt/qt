#include <qtest.h>
#include <QtDeclarative/QmlEngine>

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QUrl>

class tst_qmlengine : public QObject
{
    Q_OBJECT
public:
    tst_qmlengine() {}

private slots:
    void componentSearchPath();
};


void tst_qmlengine::componentSearchPath()
{
    QFile file(SRCDIR  "/imports.qml");
    QVERIFY(file.open(QIODevice::ReadOnly));

    QmlEngine engine;

    QList<QUrl> searchPath = engine.componentSearchPath(file.readAll(),
                                                        QUrl::fromLocalFile(file.fileName()));

    QList<QUrl> expected;
    expected << QUrl::fromLocalFile(SRCDIR);
    expected << QUrl::fromLocalFile(file.fileName()).resolved(QUrl("import1"));
    expected << QUrl::fromLocalFile(file.fileName()).resolved(QUrl("import2"));

    QCOMPARE(searchPath.size(), expected.size());
    for (int i = 0; i < expected.size(); ++i) {
        QCOMPARE(searchPath.at(i).toString(QUrl::StripTrailingSlash),
                 expected.at(i).toString(QUrl::StripTrailingSlash));
    }
}

QTEST_MAIN(tst_qmlengine)

#include "tst_qmlengine.moc"

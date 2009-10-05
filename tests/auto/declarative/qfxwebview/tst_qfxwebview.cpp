#include <qtest.h>
#include "../../../shared/util.h"
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcomponent.h>
#include <QtDeclarative/qfxwebview.h>
#include <QtWebKit/qwebpage.h>
#include <QtWebKit/qwebframe.h>

class tst_qfxwebview : public QObject
{
    Q_OBJECT
public:
    tst_qfxwebview() {}

private slots:
    void testQmlFiles_data();
    void testQmlFiles();

private:
    void checkNoErrors(const QmlComponent& component);
    QmlEngine engine;
};


void tst_qfxwebview::checkNoErrors(const QmlComponent& component)
{
    if (component.isError()) {
        QList<QmlError> errors = component.errors();
        for (int ii = 0; ii < errors.count(); ++ii) {
            const QmlError &error = errors.at(ii);
            QByteArray errorStr = QByteArray::number(error.line()) + ":" +
                                  QByteArray::number(error.column()) + ":" +
                                  error.description().toUtf8();
            qWarning() << errorStr;
        }
    }
    QVERIFY(!component.isError());
}

void tst_qfxwebview::testQmlFiles_data()
{
    QTest::addColumn<QUrl>("qmlfile"); // The input file

    QTest::newRow("creation") << QUrl::fromLocalFile(SRCDIR "/data/creation.qml");
}

void tst_qfxwebview::testQmlFiles()
{
    QFETCH(QUrl, qmlfile);

    QmlComponent component(&engine, qmlfile);
    checkNoErrors(component);
    QFxWebView *wv = qobject_cast<QFxWebView*>(component.create());
    QVERIFY(wv != 0);
}

QTEST_MAIN(tst_qfxwebview)

#include "tst_qfxwebview.moc"

#include <qtest.h>
#include "../../../shared/util.h"
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcomponent.h>
#include <QtDeclarative/qfxwebview.h>
#include <QtWebKit/qwebpage.h>
#include <QtWebKit/qwebframe.h>
#include <QtCore/qdir.h>
#include <QtCore/qfile.h>

class tst_qfxwebview : public QObject
{
    Q_OBJECT
public:
    tst_qfxwebview() {}

private slots:
    void testBasicProperties();
    void cleanupTestCase();


private:
    void checkNoErrors(const QmlComponent& component);
    QmlEngine engine;
    QString tmpDir() const
    {
        static QString tmpd = QDir::tempPath()+"/tst_qfxwebview-"
            + QDateTime::currentDateTime().toString(QLatin1String("yyyyMMddhhmmss"));
        return tmpd;
    }
};

void removeRecursive(const QString& dirname)
{
    QDir dir(dirname);
    QFileInfoList entries(dir.entryInfoList(QDir::Dirs|QDir::Files|QDir::NoDotAndDotDot));
    for (int i = 0; i < entries.count(); ++i)
        if (entries[i].isDir())
            removeRecursive(entries[i].filePath());
        else
            dir.remove(entries[i].fileName());
    QDir().rmdir(dirname);
}

void tst_qfxwebview::cleanupTestCase()
{
    removeRecursive(tmpDir());
}

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

void tst_qfxwebview::testBasicProperties()
{
    QmlComponent component(&engine, QUrl::fromLocalFile(SRCDIR "/data/basic.qml"));
    checkNoErrors(component);
    QWebSettings::enablePersistentStorage(tmpDir());

    QFxWebView *wv = qobject_cast<QFxWebView*>(component.create());
    QVERIFY(wv != 0);
    QTRY_COMPARE(wv->progress(), 1.0);
    QCOMPARE(wv->title(),QString("Basic"));
    QTRY_COMPARE(wv->icon().width(), 48);
    QCOMPARE(wv->icon(),QPixmap(SRCDIR "/data/basic.png"));
    QCOMPARE(wv->statusText(),QString(""));
    QFile htmlfile(SRCDIR "/data/basic.html");
    QVERIFY(htmlfile.open(QIODevice::ReadOnly));
    QString actualhtml____ = wv->html(); // "____" is to make errors line up for easier reading
    QString expectedhtml = htmlfile.readAll();
    actualhtml____.replace(QRegExp("\\s+"),"");
    expectedhtml.replace(QRegExp("\\s+"),"");
    QCOMPARE(actualhtml____,expectedhtml); // same, ignoring whitespace
    QCOMPARE(wv->width(), 123.0);
    QCOMPARE(wv->webPageWidth(), 0);
    QCOMPARE(wv->preferredWidth(), 0);
    QCOMPARE(wv->zoomFactor(), 1.0);
    QCOMPARE(wv->url(), QUrl::fromLocalFile(SRCDIR "/data/basic.html"));
    QCOMPARE(wv->status(), QFxWebView::Ready);
    QVERIFY(wv->reloadAction());
    QVERIFY(wv->reloadAction()->isEnabled());
    QVERIFY(wv->backAction());
    QVERIFY(!wv->backAction()->isEnabled());
    QVERIFY(wv->forwardAction());
    QVERIFY(!wv->forwardAction()->isEnabled());
    QVERIFY(wv->stopAction());
    QVERIFY(!wv->stopAction()->isEnabled());
}

QTEST_MAIN(tst_qfxwebview)

#include "tst_qfxwebview.moc"

#include <qtest.h>
#include "../../../shared/util.h"
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcomponent.h>
#include <QtDeclarative/qfxtext.h>
#include <QtWebKit/qwebpage.h>
#include <QtWebKit/qwebframe.h>
#include <QtWebKit/qwebdatabase.h>
#include <QtWebKit/qwebsecurityorigin.h>
#include <QtCore/qdir.h>
#include <QtCore/qfile.h>

class tst_sql : public QObject
{
    Q_OBJECT
public:
    tst_sql() {}

private slots:
    void verifyAgainstWebKit_data();
    void verifyAgainstWebKit();

private:
    QmlEngine engine;
};

class QWebPageWithJavaScriptConsoleMessages : public QWebPage {
public:
    void javaScriptConsoleMessage(const QString& message, int lineNumber, const QString& sourceID)
    {
qDebug() << sourceID << ":" << lineNumber << ":" << message;
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


void tst_sql::verifyAgainstWebKit_data()
{
    QTest::addColumn<QString>("jsfile"); // The input file
    QTest::addColumn<QString>("result"); // The required output from the js test() function
    QTest::addColumn<int>("databases");  // The number of databases that should have been created

    QTest::newRow("basic creation")     << "data/test1.js" << "passed" << 1;
}

void tst_sql::verifyAgainstWebKit()
{
    // Tests QML SQL Database support, and tests the same thing against
    // WebKit (HTML5) support to validate the test.
    //
    // WebKit SQL is asynchronous, so tests are divided into code plus a test()
    // function which is executed "later" (via QTRY_).
    //
    QFETCH(QString, jsfile);
    QFETCH(QString, result);
    QFETCH(int, databases);

    QString tmpdir = QString(SRCDIR)+"/output";

    // QML...
    QString qml=
        "import Qt 4.6\n"
        "Text { Script { source: \""+jsfile+"\" } text: test() }";

    // Check default storage path (we can't use it since we don't want to mess with user's data)
    QVERIFY(engine.offlineStoragePath().contains("Nokia"));
    QVERIFY(engine.offlineStoragePath().contains("OfflineStorage"));
    engine.setOfflineStoragePath(tmpdir);
    QmlComponent component(&engine, qml.toUtf8(), QUrl::fromLocalFile(SRCDIR "/empty.qml")); // just a file for relative local imports
    QFxText *text = qobject_cast<QFxText*>(component.create());
    QVERIFY(text != 0);
    QCOMPARE(text->text(),result);
    QCOMPARE(QDir(tmpdir+"/Databases").entryInfoList(QDir::Files|QDir::NoDotAndDotDot).count(), databases*2); // *2 = .ini file + .sqlite file

    // WebKit...
    QFile f(jsfile);
    QVERIFY(f.open(QIODevice::ReadOnly));
    QString js=f.readAll();

    QWebPageWithJavaScriptConsoleMessages webpage;
    QDir().mkpath(tmpdir);
    webpage.settings()->setOfflineStoragePath(tmpdir);
    webpage.mainFrame()->evaluateJavaScript(js);
    QTest::qWait(200); // WebKit db access is asynchronous
    QTRY_COMPARE(webpage.mainFrame()->evaluateJavaScript("test()").toString(),result);

    QWebSecurityOrigin origin = webpage.mainFrame()->securityOrigin();
    QList<QWebDatabase> dbs = origin.databases();
    QCOMPARE(dbs.count(), databases);


    removeRecursive(tmpdir);
}

QTEST_MAIN(tst_sql)

#include "tst_sql.moc"

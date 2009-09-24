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
    void initTestCase();

    void checkDatabasePath();

    void validateAgainstWebkit_data();
    void validateAgainstWebkit();

    void testQml_data();
    void testQml();

    void cleanupTestCase();

private:
    QString dbDir() const;
    QmlEngine engine;
};

class QWebPageWithJavaScriptConsoleMessages : public QWebPage {
public:
    void javaScriptConsoleMessage(const QString& message, int lineNumber, const QString& sourceID)
    {
        qWarning() << sourceID << ":" << lineNumber << ":" << message;
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

void tst_sql::initTestCase()
{
    removeRecursive(dbDir());
    QDir().mkpath(dbDir());
}

void tst_sql::cleanupTestCase()
{
    removeRecursive(dbDir());
}

QString tst_sql::dbDir() const
{
    return QString(SRCDIR)+"/output";
}

void tst_sql::checkDatabasePath()
{
    // Check default storage path (we can't use it since we don't want to mess with user's data)
    QVERIFY(engine.offlineStoragePath().contains("Nokia"));
    QVERIFY(engine.offlineStoragePath().contains("OfflineStorage"));
}

void tst_sql::testQml_data()
{
    QTest::addColumn<QString>("jsfile"); // The input file
    QTest::addColumn<QString>("result"); // The required output from the js test() function
    QTest::addColumn<int>("databases");  // The number of databases that should have been created
    QTest::addColumn<bool>("qmlextension"); // Things WebKit can't do

    QTest::newRow("creation") << "data/1-creation.js" << "passed" << 1 << false;
    QTest::newRow("selection") << "data/2-selection.js" << "passed" << 1 << false;
    QTest::newRow("iteration-item-function") << "data/3-iteration-item-function.js" << "passed" << 1 << false;
    QTest::newRow("iteration-index") << "data/4-iteration-index.js" << "passed" << 1 << true;
    QTest::newRow("iteration-iterator") << "data/5-iteration-iterator.js" << "passed" << 1 << true;
    QTest::newRow("iteration-efficient") << "data/6-iteration-efficient.js" << "passed" << 1 << true;
}

void tst_sql::validateAgainstWebkit_data()
{
    testQml_data();
}

void tst_sql::validateAgainstWebkit()
{
    // Validates tests against WebKit (HTML5) support.
    //
    // WebKit SQL is asynchronous, so tests are divided into code plus a test()
    // function which is executed "later" (via QTRY_).
    //
    QFETCH(QString, jsfile);
    QFETCH(QString, result);
    QFETCH(int, databases);
    QFETCH(bool, qmlextension);

    if (qmlextension) // WebKit can't do it (yet?)
        return;

    QFile f(jsfile);
    QVERIFY(f.open(QIODevice::ReadOnly));
    QString js=f.readAll();

    QWebPageWithJavaScriptConsoleMessages webpage;
    webpage.settings()->setOfflineStoragePath(dbDir());
    webpage.settings()->setAttribute(QWebSettings::OfflineStorageDatabaseEnabled, true);

    webpage.mainFrame()->evaluateJavaScript(js);
    QTest::qWait(200); // WebKit db access is asynchronous
    QTRY_COMPARE(webpage.mainFrame()->evaluateJavaScript("test()").toString(),result);

    QWebSecurityOrigin origin = webpage.mainFrame()->securityOrigin();
    QList<QWebDatabase> dbs = origin.databases();
    QCOMPARE(dbs.count(), databases);
}

void tst_sql::testQml()
{
    // Tests QML SQL Database support with tests
    // that have been validated against Webkit.
    //
    QFETCH(QString, jsfile);
    QFETCH(QString, result);
    QFETCH(int, databases);

    QString qml=
        "import Qt 4.6\n"
        "Text { Script { source: \""+jsfile+"\" } text: test() }";

    engine.setOfflineStoragePath(dbDir());
    QmlComponent component(&engine, qml.toUtf8(), QUrl::fromLocalFile(SRCDIR "/empty.qml")); // just a file for relative local imports
    QFxText *text = qobject_cast<QFxText*>(component.create());
    QVERIFY(text != 0);
    QCOMPARE(text->text(),result);
    QCOMPARE(QDir(dbDir()+"/Databases").entryInfoList(QDir::Files|QDir::NoDotAndDotDot).count(), databases*2); // *2 = .ini file + .sqlite file
}

QTEST_MAIN(tst_sql)

#include "tst_sql.moc"

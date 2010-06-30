/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include <qtest.h>
#include "../../../shared/util.h"
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativecomponent.h>
#include <private/qdeclarativetext_p.h>
#include <private/qdeclarativeengine_p.h>
#include <QtCore/qcryptographichash.h>
#include <QtWebKit/qwebpage.h>
#include <QtWebKit/qwebframe.h>
#include <QtWebKit/qwebdatabase.h>
#include <QtWebKit/qwebsecurityorigin.h>
#include <QtSql/qsqldatabase.h>
#include <QtCore/qdir.h>
#include <QtCore/qfile.h>

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
#define SRCDIR "."
#endif

class tst_qdeclarativesqldatabase : public QObject
{
    Q_OBJECT
public:
    tst_qdeclarativesqldatabase()
    {
        qApp->setApplicationName("tst_qdeclarativesqldatabase");
        qApp->setOrganizationName("Nokia");
        qApp->setOrganizationDomain("nokia.com");
        engine = new QDeclarativeEngine;
    }

    ~tst_qdeclarativesqldatabase()
    {
        delete engine;
    }

private slots:
    void initTestCase();

    void checkDatabasePath();

    void testQml_data();
    void testQml();
    void testQml_cleanopen_data();
    void testQml_cleanopen();
    void totalDatabases();

    void cleanupTestCase();

private:
    QString dbDir() const;
    QDeclarativeEngine *engine;
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

void tst_qdeclarativesqldatabase::initTestCase()
{
    removeRecursive(dbDir());
    QDir().mkpath(dbDir());
}

void tst_qdeclarativesqldatabase::cleanupTestCase()
{
    removeRecursive(dbDir());
}

QString tst_qdeclarativesqldatabase::dbDir() const
{
    static QString tmpd = QDir::tempPath()+"/tst_qdeclarativesqldatabase_output-"
        + QDateTime::currentDateTime().toString(QLatin1String("yyyyMMddhhmmss"));
    return tmpd;
}

void tst_qdeclarativesqldatabase::checkDatabasePath()
{
    // Check default storage path (we can't use it since we don't want to mess with user's data)
    QVERIFY(engine->offlineStoragePath().contains("tst_qdeclarativesqldatabase"));
    QVERIFY(engine->offlineStoragePath().contains("OfflineStorage"));
}

static const int total_databases_created_by_tests = 12;
void tst_qdeclarativesqldatabase::testQml_data()
{
    QTest::addColumn<QString>("jsfile"); // The input file

    // Each test should use a newly named DB to avoid inter-test dependencies
    QTest::newRow("creation") << "data/creation.js";
    QTest::newRow("creation-a") << "data/creation-a.js";
    QTest::newRow("creation") << "data/creation.js";
    QTest::newRow("error-creation") << "data/error-creation.js"; // re-uses above DB
    QTest::newRow("changeversion") << "data/changeversion.js";
    QTest::newRow("readonly") << "data/readonly.js";
    QTest::newRow("readonly-error") << "data/readonly-error.js";
    QTest::newRow("selection") << "data/selection.js";
    QTest::newRow("selection-bindnames") << "data/selection-bindnames.js";
    QTest::newRow("iteration") << "data/iteration.js";
    QTest::newRow("iteration-forwardonly") << "data/iteration-forwardonly.js";
    QTest::newRow("error-a") << "data/error-a.js";
    QTest::newRow("error-notransaction") << "data/error-notransaction.js";
    QTest::newRow("error-outsidetransaction") << "data/error-outsidetransaction.js"; // reuse above
    QTest::newRow("reopen1") << "data/reopen1.js";
    QTest::newRow("reopen2") << "data/reopen2.js"; // re-uses above DB

    // If you add a test, you should usually use a new database in the
    // test - in which case increment total_databases_created_by_tests above.
}

/*
void tst_qdeclarativesqldatabase::validateAgainstWebkit()
{
    // Validates tests against WebKit (HTML5) support.
    //
    QFETCH(QString, jsfile);
    QFETCH(QString, result);
    QFETCH(int, databases);

    QFile f(jsfile);
    QVERIFY(f.open(QIODevice::ReadOnly));
    QString js=f.readAll();

    QWebPageWithJavaScriptConsoleMessages webpage;
    webpage.settings()->setOfflineStoragePath(dbDir());
    webpage.settings()->setAttribute(QWebSettings::OfflineStorageDatabaseEnabled, true);

    QEXPECT_FAIL("","WebKit doesn't support openDatabaseSync yet", Continue);
    QCOMPARE(webpage.mainFrame()->evaluateJavaScript(js).toString(),result);

    QTest::qWait(100); // WebKit crashes if you quit it too fast

    QWebSecurityOrigin origin = webpage.mainFrame()->securityOrigin();
    QList<QWebDatabase> dbs = origin.databases();
    QCOMPARE(dbs.count(), databases);
}
*/

void tst_qdeclarativesqldatabase::testQml()
{
    // Tests QML SQL Database support with tests
    // that have been validated against Webkit.
    //
    QFETCH(QString, jsfile);

    QString qml=
        "import Qt 4.7\n"
        "import \""+jsfile+"\" as JS\n"
        "Text { text: JS.test() }";

    engine->setOfflineStoragePath(dbDir());
    QDeclarativeComponent component(engine);
    component.setData(qml.toUtf8(), QUrl::fromLocalFile(SRCDIR "/empty.qml")); // just a file for relative local imports
    QVERIFY(!component.isError());
    QDeclarativeText *text = qobject_cast<QDeclarativeText*>(component.create());
    QVERIFY(text != 0);
    QCOMPARE(text->text(),QString("passed"));
}

void tst_qdeclarativesqldatabase::testQml_cleanopen_data()
{
    QTest::addColumn<QString>("jsfile"); // The input file
    QTest::newRow("reopen1") << "data/reopen1.js";
    QTest::newRow("reopen2") << "data/reopen2.js";
    QTest::newRow("error-creation") << "data/error-creation.js"; // re-uses creation DB
}

void tst_qdeclarativesqldatabase::testQml_cleanopen()
{
    // Same as testQml, but clean connections between tests,
    // making it more like the tests are running in new processes.
    testQml();

    QDeclarativeEnginePrivate::getScriptEngine(engine)->collectGarbage(); // close databases
    foreach (QString dbname, QSqlDatabase::connectionNames()) {
        QSqlDatabase::removeDatabase(dbname);
    }
}

void tst_qdeclarativesqldatabase::totalDatabases()
{
    QCOMPARE(QDir(dbDir()+"/Databases").entryInfoList(QDir::Files|QDir::NoDotAndDotDot).count(), total_databases_created_by_tests*2);
}

QTEST_MAIN(tst_qdeclarativesqldatabase)

#include "tst_qdeclarativesqldatabase.moc"

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
#include <QDeclarativeEngine>
#include <QDeclarativeContext>
#include <QNetworkAccessManager>
#include <QPointer>
#include <QDir>
#include <QDesktopServices>
#include <QDebug>
#include <QDeclarativeComponent>
#include <QDeclarativeNetworkAccessManagerFactory>

class tst_qdeclarativeengine : public QObject
{
    Q_OBJECT
public:
    tst_qdeclarativeengine() {}

private slots:
    void rootContext();
    void networkAccessManager();
    void baseUrl();
    void contextForObject();
    void offlineStoragePath();
    void clearComponentCache();
};

void tst_qdeclarativeengine::rootContext()
{
    QDeclarativeEngine engine;

    QVERIFY(engine.rootContext());

    QCOMPARE(engine.rootContext()->engine(), &engine);
    QVERIFY(engine.rootContext()->parentContext() == 0);
}

class NetworkAccessManagerFactory : public QDeclarativeNetworkAccessManagerFactory
{
public:
    NetworkAccessManagerFactory() : manager(0) {}

    QNetworkAccessManager *create(QObject *parent) {
        manager = new QNetworkAccessManager(parent);
        return manager;
    }

    QNetworkAccessManager *manager;
};

void tst_qdeclarativeengine::networkAccessManager()
{
    QDeclarativeEngine *engine = new QDeclarativeEngine;

    // Test QDeclarativeEngine created manager
    QPointer<QNetworkAccessManager> manager = engine->networkAccessManager();
    QVERIFY(manager != 0);
    delete engine;

    // Test factory created manager
    engine = new QDeclarativeEngine;
    NetworkAccessManagerFactory factory;
    engine->setNetworkAccessManagerFactory(&factory);
    QVERIFY(engine->networkAccessManager() == factory.manager);
    delete engine;
}

void tst_qdeclarativeengine::baseUrl()
{
    QDeclarativeEngine engine;

    QUrl cwd = QUrl::fromLocalFile(QDir::currentPath() + QDir::separator());

    QCOMPARE(engine.baseUrl(), cwd);
    QCOMPARE(engine.rootContext()->resolvedUrl(QUrl("main.qml")), cwd.resolved(QUrl("main.qml")));

    QDir dir = QDir::current();
    dir.cdUp();
    QVERIFY(dir != QDir::current());
    QDir::setCurrent(dir.path());
    QVERIFY(QDir::current() == dir);

    QUrl cwd2 = QUrl::fromLocalFile(QDir::currentPath() + QDir::separator());
    QCOMPARE(engine.baseUrl(), cwd2);
    QCOMPARE(engine.rootContext()->resolvedUrl(QUrl("main.qml")), cwd2.resolved(QUrl("main.qml")));

    engine.setBaseUrl(cwd);
    QCOMPARE(engine.baseUrl(), cwd);
    QCOMPARE(engine.rootContext()->resolvedUrl(QUrl("main.qml")), cwd.resolved(QUrl("main.qml")));
}

void tst_qdeclarativeengine::contextForObject()
{
    QDeclarativeEngine *engine = new QDeclarativeEngine;

    // Test null-object
    QVERIFY(QDeclarativeEngine::contextForObject(0) == 0);

    // Test an object with no context
    QObject object;
    QVERIFY(QDeclarativeEngine::contextForObject(&object) == 0);

    // Test setting null-object
    QDeclarativeEngine::setContextForObject(0, engine->rootContext());

    // Test setting null-context
    QDeclarativeEngine::setContextForObject(&object, 0);

    // Test setting context
    QDeclarativeEngine::setContextForObject(&object, engine->rootContext());
    QVERIFY(QDeclarativeEngine::contextForObject(&object) == engine->rootContext());

    QDeclarativeContext context(engine->rootContext());

    // Try changing context
    QTest::ignoreMessage(QtWarningMsg, "QDeclarativeEngine::setContextForObject(): Object already has a QDeclarativeContext");
    QDeclarativeEngine::setContextForObject(&object, &context);
    QVERIFY(QDeclarativeEngine::contextForObject(&object) == engine->rootContext());

    // Delete context
    delete engine; engine = 0;
    QVERIFY(QDeclarativeEngine::contextForObject(&object) == 0);
}

void tst_qdeclarativeengine::offlineStoragePath()
{
    // Without these set, QDesktopServices::storageLocation returns
    // strings with extra "//" at the end. We set them to ignore this problem.
    qApp->setApplicationName("tst_qdeclarativeengine");
    qApp->setOrganizationName("Nokia");
    qApp->setOrganizationDomain("nokia.com");

    QDeclarativeEngine engine;

    QDir dir(QDesktopServices::storageLocation(QDesktopServices::DataLocation));
    dir.mkpath("QML");
    dir.cd("QML");
    dir.mkpath("OfflineStorage");
    dir.cd("OfflineStorage");

    QCOMPARE(QDir::fromNativeSeparators(engine.offlineStoragePath()), dir.path());

    engine.setOfflineStoragePath(QDir::homePath());
    QCOMPARE(engine.offlineStoragePath(), QDir::homePath());
}

void tst_qdeclarativeengine::clearComponentCache()
{
    QDeclarativeEngine engine;

    // Create original qml file
    {
        QFile file("temp.qml");
        QVERIFY(file.open(QIODevice::WriteOnly));
        file.write("import Qt 4.7\nQtObject {\nproperty int test: 10\n}\n");
        file.close();
    }

    // Test "test" property
    {
        QDeclarativeComponent component(&engine, "temp.qml");
        QObject *obj = component.create();
        QVERIFY(obj != 0);
        QCOMPARE(obj->property("test").toInt(), 10);
        delete obj;
    }

    // Modify qml file
    {
        QFile file("temp.qml");
        QVERIFY(file.open(QIODevice::WriteOnly));
        file.write("import Qt 4.7\nQtObject {\nproperty int test: 11\n}\n");
        file.close();
    }

    // Test cache hit
    {
        QDeclarativeComponent component(&engine, "temp.qml");
        QObject *obj = component.create();
        QVERIFY(obj != 0);
        QCOMPARE(obj->property("test").toInt(), 10);
        delete obj;
    }

    // Clear cache
    engine.clearComponentCache();

    // Test cache refresh
    {
        QDeclarativeComponent component(&engine, "temp.qml");
        QObject *obj = component.create();
        QVERIFY(obj != 0);
        QCOMPARE(obj->property("test").toInt(), 11);
        delete obj;
    }
}

QTEST_MAIN(tst_qdeclarativeengine)

#include "tst_qdeclarativeengine.moc"

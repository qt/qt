/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
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

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
#define SRCDIR "."
#endif

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
    void outputWarningsToStandardError();
    void objectOwnership();
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
    QVERIFY(engine->networkAccessManagerFactory() == &factory);
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
        file.write("import QtQuick 1.0\nQtObject {\nproperty int test: 10\n}\n");
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
        file.write("import QtQuick 1.0\nQtObject {\nproperty int test: 11\n}\n");
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

static QStringList warnings;
static void msgHandler(QtMsgType, const char *warning)
{
    warnings << QString::fromUtf8(warning);
}

void tst_qdeclarativeengine::outputWarningsToStandardError()
{
    QDeclarativeEngine engine;

    QCOMPARE(engine.outputWarningsToStandardError(), true);

    QDeclarativeComponent c(&engine);
    c.setData("import QtQuick 1.0; QtObject { property int a: undefined }", QUrl());

    QVERIFY(c.isReady() == true);

    warnings.clear();
    QtMsgHandler old = qInstallMsgHandler(msgHandler);

    QObject *o = c.create();

    qInstallMsgHandler(old);

    QVERIFY(o != 0);
    delete o;

    QCOMPARE(warnings.count(), 1);
    QCOMPARE(warnings.at(0), QLatin1String("<Unknown File>:1: Unable to assign [undefined] to int a"));
    warnings.clear();


    engine.setOutputWarningsToStandardError(false);
    QCOMPARE(engine.outputWarningsToStandardError(), false);


    old = qInstallMsgHandler(msgHandler);

    o = c.create();

    qInstallMsgHandler(old);

    QVERIFY(o != 0);
    delete o;

    QCOMPARE(warnings.count(), 0);
}

void tst_qdeclarativeengine::objectOwnership()
{
    {
    QCOMPARE(QDeclarativeEngine::objectOwnership(0), QDeclarativeEngine::CppOwnership);
    QDeclarativeEngine::setObjectOwnership(0, QDeclarativeEngine::JavaScriptOwnership);
    QCOMPARE(QDeclarativeEngine::objectOwnership(0), QDeclarativeEngine::CppOwnership);
    }

    {
    QObject o;
    QCOMPARE(QDeclarativeEngine::objectOwnership(&o), QDeclarativeEngine::CppOwnership);
    QDeclarativeEngine::setObjectOwnership(&o, QDeclarativeEngine::CppOwnership);
    QCOMPARE(QDeclarativeEngine::objectOwnership(&o), QDeclarativeEngine::CppOwnership);
    QDeclarativeEngine::setObjectOwnership(&o, QDeclarativeEngine::JavaScriptOwnership);
    QCOMPARE(QDeclarativeEngine::objectOwnership(&o), QDeclarativeEngine::JavaScriptOwnership);
    QDeclarativeEngine::setObjectOwnership(&o, QDeclarativeEngine::CppOwnership);
    QCOMPARE(QDeclarativeEngine::objectOwnership(&o), QDeclarativeEngine::CppOwnership);
    }

    {
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine);
    c.setData("import QtQuick 1.0; QtObject { property QtObject object: QtObject {} }", QUrl());

    QObject *o = c.create();
    QVERIFY(o != 0);

    QCOMPARE(QDeclarativeEngine::objectOwnership(o), QDeclarativeEngine::CppOwnership);

    QObject *o2 = qvariant_cast<QObject *>(o->property("object"));
    QCOMPARE(QDeclarativeEngine::objectOwnership(o2), QDeclarativeEngine::JavaScriptOwnership);

    delete o;
    }

}

QTEST_MAIN(tst_qdeclarativeengine)

#include "tst_qdeclarativeengine.moc"

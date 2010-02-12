/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <QtGui/QGraphicsWidget>
#include <QtGui/QGraphicsScene>

#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcomponent.h>
#include <private/qmlgraphicsloader_p.h>
#include "testhttpserver.h"

#define SERVER_PORT 14445

inline QUrl TEST_FILE(const QString &filename)
{
    return QUrl::fromLocalFile(QLatin1String(SRCDIR) + QLatin1String("/data/") + filename);
}

#define TRY_WAIT(expr) \
    do { \
        for (int ii = 0; ii < 6; ++ii) { \
            if ((expr)) break; \
            QTest::qWait(50); \
        } \
        QVERIFY((expr)); \
    } while (false)

class tst_QmlGraphicsLoader : public QObject

{
    Q_OBJECT
public:
    tst_QmlGraphicsLoader();

private slots:
    void url();
    void invalidUrl();
    void component();
    void clear();
    void urlToComponent();
    void componentToUrl();
    void sizeLoaderToItem();
    void sizeItemToLoader();
    void noResize();
    void sizeLoaderToGraphicsWidget();
    void sizeGraphicsWidgetToLoader();
    void noResizeGraphicsWidget();
    void networkRequestUrl();
    void failNetworkRequest();
//    void networkComponent();

private:
    QmlEngine engine;
};


tst_QmlGraphicsLoader::tst_QmlGraphicsLoader()
{
}

void tst_QmlGraphicsLoader::url()
{
    QmlComponent component(&engine);
    component.setData(QByteArray("import Qt 4.6\nLoader { source: \"Rect120x60.qml\" }"), TEST_FILE(""));
    QmlGraphicsLoader *loader = qobject_cast<QmlGraphicsLoader*>(component.create());
    QVERIFY(loader != 0);
    QVERIFY(loader->item());
    QVERIFY(loader->source() == QUrl::fromLocalFile(SRCDIR "/data/Rect120x60.qml"));
    QCOMPARE(loader->progress(), 1.0);
    QCOMPARE(loader->status(), QmlGraphicsLoader::Ready);
    QCOMPARE(static_cast<QGraphicsItem*>(loader)->children().count(), 1);

    delete loader;
}

void tst_QmlGraphicsLoader::component()
{
    QmlComponent component(&engine, TEST_FILE("/SetSourceComponent.qml"));
    QmlGraphicsItem *item = qobject_cast<QmlGraphicsItem*>(component.create());
    QVERIFY(item);

    QmlGraphicsLoader *loader = qobject_cast<QmlGraphicsLoader*>(item->QGraphicsObject::children().at(1)); 
    QVERIFY(loader);
    QVERIFY(loader->item());
    QCOMPARE(loader->progress(), 1.0);
    QCOMPARE(loader->status(), QmlGraphicsLoader::Ready);
    QCOMPARE(static_cast<QGraphicsItem*>(loader)->children().count(), 1);

    delete loader;
}

void tst_QmlGraphicsLoader::invalidUrl()
{
    QTest::ignoreMessage(QtWarningMsg, QString("(:-1: File error for URL " + QUrl::fromLocalFile(SRCDIR "/data/IDontExist.qml").toString() + ") ").toUtf8().constData());

    QmlComponent component(&engine);
    component.setData(QByteArray("import Qt 4.6\nLoader { source: \"IDontExist.qml\" }"), TEST_FILE(""));
    QmlGraphicsLoader *loader = qobject_cast<QmlGraphicsLoader*>(component.create());
    QVERIFY(loader != 0);
    QVERIFY(loader->item() == 0);
    QCOMPARE(loader->progress(), 1.0);
    QCOMPARE(loader->status(), QmlGraphicsLoader::Error);
    QCOMPARE(static_cast<QGraphicsItem*>(loader)->children().count(), 0);

    delete loader;
}

void tst_QmlGraphicsLoader::clear()
{
    {
        QmlComponent component(&engine);
        component.setData(QByteArray(
                    "import Qt 4.6\n"
                    " Loader { id: loader\n"
                    "  source: 'Rect120x60.qml'\n"
                    "  Timer { interval: 200; running: true; onTriggered: loader.source = '' }\n"
                    " }")
                , TEST_FILE(""));
        QmlGraphicsLoader *loader = qobject_cast<QmlGraphicsLoader*>(component.create());
        QVERIFY(loader != 0);
        QVERIFY(loader->item());
        QCOMPARE(loader->progress(), 1.0);
        QCOMPARE(static_cast<QGraphicsItem*>(loader)->children().count(), 1);

        QTest::qWait(500);

        QVERIFY(loader->item() == 0);
        QCOMPARE(loader->progress(), 0.0);
        QCOMPARE(loader->status(), QmlGraphicsLoader::Null);
        QCOMPARE(static_cast<QGraphicsItem*>(loader)->children().count(), 0);

        delete loader;
    }
    {
        QmlComponent component(&engine, TEST_FILE("/SetSourceComponent.qml"));
        QmlGraphicsItem *item = qobject_cast<QmlGraphicsItem*>(component.create());
        QVERIFY(item);

        QmlGraphicsLoader *loader = qobject_cast<QmlGraphicsLoader*>(item->QGraphicsObject::children().at(1)); 
        QVERIFY(loader);
        QVERIFY(loader->item());
        QCOMPARE(loader->progress(), 1.0);
        QCOMPARE(static_cast<QGraphicsItem*>(loader)->children().count(), 1);

        loader->setSourceComponent(0);

        QVERIFY(loader->item() == 0);
        QCOMPARE(loader->progress(), 0.0);
        QCOMPARE(loader->status(), QmlGraphicsLoader::Null);
        QCOMPARE(static_cast<QGraphicsItem*>(loader)->children().count(), 0);

        delete loader;
    }
}

void tst_QmlGraphicsLoader::urlToComponent()
{
    QmlComponent component(&engine);
    component.setData(QByteArray("import Qt 4.6\n"
                "Loader {\n"
                " id: loader\n"
                " Component { id: myComp; Rectangle { width: 10; height: 10 } }\n"
                " source: \"Rect120x60.qml\"\n"
                " Timer { interval: 100; running: true; onTriggered: loader.sourceComponent = myComp }\n"
                "}" )
            , TEST_FILE(""));
    QmlGraphicsLoader *loader = qobject_cast<QmlGraphicsLoader*>(component.create());
    QTest::qWait(500);
    QVERIFY(loader != 0);
    QVERIFY(loader->item());
    QCOMPARE(loader->progress(), 1.0);
    QCOMPARE(static_cast<QGraphicsItem*>(loader)->children().count(), 1);
    QCOMPARE(loader->width(), 10.0);
    QCOMPARE(loader->height(), 10.0);

    delete loader;
}

void tst_QmlGraphicsLoader::componentToUrl()
{
    QmlComponent component(&engine, TEST_FILE("/SetSourceComponent.qml"));
    QmlGraphicsItem *item = qobject_cast<QmlGraphicsItem*>(component.create());
    QVERIFY(item);

    QmlGraphicsLoader *loader = qobject_cast<QmlGraphicsLoader*>(item->QGraphicsObject::children().at(1)); 
    QVERIFY(loader);
    QVERIFY(loader->item());
    QCOMPARE(loader->progress(), 1.0);
    QCOMPARE(static_cast<QGraphicsItem*>(loader)->children().count(), 1);

    loader->setSource(TEST_FILE("/Rect120x60.qml"));
    QVERIFY(loader->item());
    QCOMPARE(loader->progress(), 1.0);
    QCOMPARE(static_cast<QGraphicsItem*>(loader)->children().count(), 1);
    QCOMPARE(loader->width(), 120.0);
    QCOMPARE(loader->height(), 60.0);

    delete loader;
}

void tst_QmlGraphicsLoader::sizeLoaderToItem()
{
    QmlComponent component(&engine, TEST_FILE("/SizeToItem.qml"));
    QmlGraphicsLoader *loader = qobject_cast<QmlGraphicsLoader*>(component.create());
    QVERIFY(loader != 0);
    QVERIFY(loader->resizeMode() == QmlGraphicsLoader::SizeLoaderToItem);
    QCOMPARE(loader->width(), 120.0);
    QCOMPARE(loader->height(), 60.0);

    // Check resize
    QmlGraphicsItem *rect = qobject_cast<QmlGraphicsItem*>(loader->item());
    QVERIFY(rect);
    rect->setWidth(150);
    rect->setHeight(45);
    QCOMPARE(loader->width(), 150.0);
    QCOMPARE(loader->height(), 45.0);

    // Switch mode
    loader->setResizeMode(QmlGraphicsLoader::SizeItemToLoader);
    loader->setWidth(180);
    loader->setHeight(30);
    QCOMPARE(rect->width(), 180.0);
    QCOMPARE(rect->height(), 30.0);
}

void tst_QmlGraphicsLoader::sizeItemToLoader()
{
    QmlComponent component(&engine, TEST_FILE("/SizeToLoader.qml"));
    QmlGraphicsLoader *loader = qobject_cast<QmlGraphicsLoader*>(component.create());
    QVERIFY(loader != 0);
    QVERIFY(loader->resizeMode() == QmlGraphicsLoader::SizeItemToLoader);
    QCOMPARE(loader->width(), 200.0);
    QCOMPARE(loader->height(), 80.0);

    QmlGraphicsItem *rect = qobject_cast<QmlGraphicsItem*>(loader->item());
    QVERIFY(rect);
    QCOMPARE(rect->width(), 200.0);
    QCOMPARE(rect->height(), 80.0);

    // Check resize
    loader->setWidth(180);
    loader->setHeight(30);
    QCOMPARE(rect->width(), 180.0);
    QCOMPARE(rect->height(), 30.0);

    // Switch mode
    loader->setResizeMode(QmlGraphicsLoader::SizeLoaderToItem);
    rect->setWidth(160);
    rect->setHeight(45);
    QCOMPARE(loader->width(), 160.0);
    QCOMPARE(loader->height(), 45.0);
}

void tst_QmlGraphicsLoader::noResize()
{
    QmlComponent component(&engine, TEST_FILE("/NoResize.qml"));
    QmlGraphicsLoader *loader = qobject_cast<QmlGraphicsLoader*>(component.create());
    QVERIFY(loader != 0);
    QCOMPARE(loader->width(), 200.0);
    QCOMPARE(loader->height(), 80.0);

    QmlGraphicsItem *rect = qobject_cast<QmlGraphicsItem*>(loader->item());
    QVERIFY(rect);
    QCOMPARE(rect->width(), 120.0);
    QCOMPARE(rect->height(), 60.0);
}

void tst_QmlGraphicsLoader::sizeLoaderToGraphicsWidget()
{
    QmlComponent component(&engine, TEST_FILE("/SizeLoaderToGraphicsWidget.qml"));
    QmlGraphicsLoader *loader = qobject_cast<QmlGraphicsLoader*>(component.create());
    QGraphicsScene scene;
    scene.addItem(loader);

    QVERIFY(loader != 0);
    QVERIFY(loader->resizeMode() == QmlGraphicsLoader::SizeLoaderToItem);
    QCOMPARE(loader->width(), 250.0);
    QCOMPARE(loader->height(), 250.0);

    // Check resize
    QGraphicsWidget *widget = qobject_cast<QGraphicsWidget*>(loader->item());
    QVERIFY(widget);
    widget->resize(QSizeF(150,45));
    QCOMPARE(loader->width(), 150.0);
    QCOMPARE(loader->height(), 45.0);

    // Switch mode
    loader->setResizeMode(QmlGraphicsLoader::SizeItemToLoader);
    loader->setWidth(180);
    loader->setHeight(30);
    QCOMPARE(widget->size().width(), 180.0);
    QCOMPARE(widget->size().height(), 30.0);
}

void tst_QmlGraphicsLoader::sizeGraphicsWidgetToLoader()
{
    QmlComponent component(&engine, TEST_FILE("/SizeGraphicsWidgetToLoader.qml"));
    QmlGraphicsLoader *loader = qobject_cast<QmlGraphicsLoader*>(component.create());
    QGraphicsScene scene;
    scene.addItem(loader);

    QVERIFY(loader != 0);
    QVERIFY(loader->resizeMode() == QmlGraphicsLoader::SizeItemToLoader);
    QCOMPARE(loader->width(), 200.0);
    QCOMPARE(loader->height(), 80.0);

    QGraphicsWidget *widget = qobject_cast<QGraphicsWidget*>(loader->item());
    QVERIFY(widget);
    QCOMPARE(widget->size().width(), 200.0);
    QCOMPARE(widget->size().height(), 80.0);

    // Check resize
    loader->setWidth(180);
    loader->setHeight(30);
    QCOMPARE(widget->size().width(), 180.0);
    QCOMPARE(widget->size().height(), 30.0);

    // Switch mode
    loader->setResizeMode(QmlGraphicsLoader::SizeLoaderToItem);
    widget->resize(QSizeF(160,45));
    QCOMPARE(loader->width(), 160.0);
    QCOMPARE(loader->height(), 45.0);
}

void tst_QmlGraphicsLoader::noResizeGraphicsWidget()
{
    QmlComponent component(&engine, TEST_FILE("/NoResizeGraphicsWidget.qml"));
    QmlGraphicsLoader *loader = qobject_cast<QmlGraphicsLoader*>(component.create());
    QGraphicsScene scene;
    scene.addItem(loader);

    QVERIFY(loader != 0);
    QCOMPARE(loader->width(), 200.0);
    QCOMPARE(loader->height(), 80.0);

    QGraphicsWidget *widget = qobject_cast<QGraphicsWidget*>(loader->item());
    QVERIFY(widget);
    QCOMPARE(widget->size().width(), 250.0);
    QCOMPARE(widget->size().height(), 250.0);
}

void tst_QmlGraphicsLoader::networkRequestUrl()
{
    TestHTTPServer server(SERVER_PORT);
    QVERIFY(server.isValid());
    server.serveDirectory("data");

    QmlComponent component(&engine);
    component.setData(QByteArray("import Qt 4.6\nLoader { source: \"http://127.0.0.1:14445/Rect120x60.qml\" }"), TEST_FILE(""));
    QmlGraphicsLoader *loader = qobject_cast<QmlGraphicsLoader*>(component.create());
    QVERIFY(loader != 0);

    TRY_WAIT(loader->status() == QmlGraphicsLoader::Ready);

    QVERIFY(loader->item());
    QCOMPARE(loader->progress(), 1.0);
    QCOMPARE(static_cast<QGraphicsItem*>(loader)->children().count(), 1);

    delete loader;
}

/* XXX Component waits until all dependencies are loaded.  Is this actually possible?
void tst_QmlGraphicsLoader::networkComponent()
{
    TestHTTPServer server(SERVER_PORT);
    QVERIFY(server.isValid());
    server.serveDirectory("slowdata", TestHTTPServer::Delay);

    QmlComponent component(&engine);
    component.setData(QByteArray(
                "import Qt 4.6\n"
                "import \"http://127.0.0.1:14445/\" as NW\n"
                "Item {\n"
                " Component { id: comp; NW.SlowRect {} }\n"
                " Loader { sourceComponent: comp } }")
            , TEST_FILE(""));

    QmlGraphicsItem *item = qobject_cast<QmlGraphicsItem*>(component.create());
    QVERIFY(item);

    QmlGraphicsLoader *loader = qobject_cast<QmlGraphicsLoader*>(item->QGraphicsObject::children().at(1)); 
    QVERIFY(loader);
    TRY_WAIT(loader->status() == QmlGraphicsLoader::Ready);

    QVERIFY(loader->item());
    QCOMPARE(loader->progress(), 1.0);
    QCOMPARE(loader->status(), QmlGraphicsLoader::Ready);
    QCOMPARE(static_cast<QGraphicsItem*>(loader)->children().count(), 1);

    delete loader;
}
*/

void tst_QmlGraphicsLoader::failNetworkRequest()
{
    TestHTTPServer server(SERVER_PORT);
    QVERIFY(server.isValid());
    server.serveDirectory("data");

    QTest::ignoreMessage(QtWarningMsg, "(:-1: Network error for URL http://127.0.0.1:14445/IDontExist.qml) ");

    QmlComponent component(&engine);
    component.setData(QByteArray("import Qt 4.6\nLoader { source: \"http://127.0.0.1:14445/IDontExist.qml\" }"), TEST_FILE(""));
    QmlGraphicsLoader *loader = qobject_cast<QmlGraphicsLoader*>(component.create());
    QVERIFY(loader != 0);

    TRY_WAIT(loader->status() == QmlGraphicsLoader::Error);

    QVERIFY(loader->item() == 0);
    QCOMPARE(loader->progress(), 0.0);
    QCOMPARE(static_cast<QGraphicsItem*>(loader)->children().count(), 0);

    delete loader;
}

QTEST_MAIN(tst_QmlGraphicsLoader)

#include "tst_qmlgraphicsloader.moc"

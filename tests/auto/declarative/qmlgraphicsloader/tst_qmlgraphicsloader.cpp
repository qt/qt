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
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcomponent.h>
#include <private/qmlgraphicsloader_p.h>

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

private:
    QmlEngine engine;
};

/*
inline QUrl TEST_FILE(const QString &filename)
{
    QFileInfo fileInfo(__FILE__);
    return QUrl::fromLocalFile(fileInfo.absoluteDir().filePath(filename));
}

inline QUrl TEST_FILE(const char *filename)
{
    return TEST_FILE(QLatin1String(filename));
}
*/

tst_QmlGraphicsLoader::tst_QmlGraphicsLoader()
{
}

void tst_QmlGraphicsLoader::url()
{
    QmlComponent component(&engine, QByteArray("import Qt 4.6\nLoader { source: \"Rect120x60.qml\" }"), QUrl("file://" SRCDIR "/"));
    QmlGraphicsLoader *loader = qobject_cast<QmlGraphicsLoader*>(component.create());
    QVERIFY(loader != 0);
    QVERIFY(loader->item());
    QVERIFY(loader->source() == QUrl("file://" SRCDIR "/Rect120x60.qml"));
    QCOMPARE(loader->progress(), 1.0);
    QCOMPARE(loader->status(), QmlGraphicsLoader::Ready);
    QCOMPARE(static_cast<QGraphicsItem*>(loader)->children().count(), 1);

    delete loader;
}

void tst_QmlGraphicsLoader::component()
{
    QmlComponent component(&engine, QUrl("file://" SRCDIR "/SetSourceComponent.qml"));
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
    QTest::ignoreMessage(QtWarningMsg, "(:-1: File error for URL file://" SRCDIR "/IDontExist.qml) ");

    QmlComponent component(&engine, QByteArray("import Qt 4.6\nLoader { source: \"IDontExist.qml\" }"), QUrl("file://" SRCDIR "/"));
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
        QmlComponent component(&engine, QByteArray(
                    "import Qt 4.6\n"
                    " Loader { id: loader\n"
                    "  source: 'Rect120x60.qml'\n"
                    "  Timer { interval: 200; running: true; onTriggered: loader.source = '' }\n"
                    " }")
                , QUrl("file://" SRCDIR "/"));
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
        QmlComponent component(&engine, QUrl("file://" SRCDIR "/SetSourceComponent.qml"));
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
    QmlComponent component(&engine, QByteArray("import Qt 4.6\n"
                "Loader {\n"
                " id: loader\n"
                " Component { id: myComp; Rectangle { width: 10; height: 10 } }\n"
                " source: \"Rect120x60.qml\"\n"
                " Timer { interval: 100; running: true; onTriggered: loader.sourceComponent = myComp }\n"
                "}" )
            , QUrl("file://" SRCDIR "/"));
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
    QmlComponent component(&engine, QUrl("file://" SRCDIR "/SetSourceComponent.qml"));
    QmlGraphicsItem *item = qobject_cast<QmlGraphicsItem*>(component.create());
    QVERIFY(item);

    QmlGraphicsLoader *loader = qobject_cast<QmlGraphicsLoader*>(item->QGraphicsObject::children().at(1)); 
    QVERIFY(loader);
    QVERIFY(loader->item());
    QCOMPARE(loader->progress(), 1.0);
    QCOMPARE(static_cast<QGraphicsItem*>(loader)->children().count(), 1);

    loader->setSource(QUrl("file://" SRCDIR "/Rect120x60.qml"));
    QVERIFY(loader->item());
    QCOMPARE(loader->progress(), 1.0);
    QCOMPARE(static_cast<QGraphicsItem*>(loader)->children().count(), 1);
    QCOMPARE(loader->width(), 120.0);
    QCOMPARE(loader->height(), 60.0);

    delete loader;
}

void tst_QmlGraphicsLoader::sizeLoaderToItem()
{
    QmlComponent component(&engine, QUrl("file://" SRCDIR "/SizeToItem.qml"));
    QmlGraphicsLoader *loader = qobject_cast<QmlGraphicsLoader*>(component.create());
    QVERIFY(loader != 0);
    QVERIFY(loader->resizeMode() == QmlGraphicsLoader::SizeLoaderToItem);
    QCOMPARE(loader->width(), 120.0);
    QCOMPARE(loader->height(), 60.0);

    // Check resize
    QmlGraphicsItem *rect = loader->item();
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
    QmlComponent component(&engine, QUrl("file://" SRCDIR "/SizeToLoader.qml"));
    QmlGraphicsLoader *loader = qobject_cast<QmlGraphicsLoader*>(component.create());
    QVERIFY(loader != 0);
    QVERIFY(loader->resizeMode() == QmlGraphicsLoader::SizeItemToLoader);
    QCOMPARE(loader->width(), 200.0);
    QCOMPARE(loader->height(), 80.0);

    QmlGraphicsItem *rect = loader->item();
    QVERIFY(rect);
    QCOMPARE(rect->width(), 200.0);
    QCOMPARE(rect->height(), 80.0);

    // Check resize
    loader->setWidth(180);
    loader->setHeight(30);
    QCOMPARE(rect->width(), 180.0);
    QCOMPARE(rect->height(), 30.0);
}

void tst_QmlGraphicsLoader::noResize()
{
    QmlComponent component(&engine, QUrl("file://" SRCDIR "/NoResize.qml"));
    QmlGraphicsLoader *loader = qobject_cast<QmlGraphicsLoader*>(component.create());
    QVERIFY(loader != 0);
    QCOMPARE(loader->width(), 200.0);
    QCOMPARE(loader->height(), 80.0);

    QmlGraphicsItem *rect = loader->item();
    QVERIFY(rect);
    QCOMPARE(rect->width(), 120.0);
    QCOMPARE(rect->height(), 60.0);
}

QTEST_MAIN(tst_QmlGraphicsLoader)

#include "tst_qmlgraphicsloader.moc"

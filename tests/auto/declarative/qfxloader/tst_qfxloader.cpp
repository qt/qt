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
#include <QtDeclarative/qfxloader.h>

class tst_qfxloader : public QObject

{
    Q_OBJECT
public:
    tst_qfxloader();

private slots:
    void url();
    void component();
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

tst_qfxloader::tst_qfxloader()
{
}

void tst_qfxloader::url()
{
    QmlComponent component(&engine, QByteArray("import Qt 4.6\nLoader { source: \"Rect120x60.qml\" }"), QUrl("file://" SRCDIR "/"));
    QFxLoader *loader = qobject_cast<QFxLoader*>(component.create());
    QVERIFY(loader != 0);
    QVERIFY(loader->item());
    QCOMPARE(loader->progress(), 1.0);
    QCOMPARE(static_cast<QGraphicsItem*>(loader)->children().count(), 1);
}

void tst_qfxloader::component()
{
    QmlComponent component(&engine, QUrl("file://" SRCDIR "/SetSourceComponent.qml"));
    QFxItem *item = qobject_cast<QFxItem*>(component.create());
    QVERIFY(item);

    QFxLoader *loader = qobject_cast<QFxLoader*>(item->QGraphicsObject::children().at(1)); 
    QVERIFY(loader);
    QVERIFY(loader->item());
    QCOMPARE(loader->progress(), 1.0);
    QCOMPARE(static_cast<QGraphicsItem*>(loader)->children().count(), 1);
}

void tst_qfxloader::sizeLoaderToItem()
{
    QmlComponent component(&engine, QUrl("file://" SRCDIR "/SizeToItem.qml"));
    QFxLoader *loader = qobject_cast<QFxLoader*>(component.create());
    QVERIFY(loader != 0);
    QCOMPARE(loader->width(), 120.0);
    QCOMPARE(loader->height(), 60.0);
}

void tst_qfxloader::sizeItemToLoader()
{
    QmlComponent component(&engine, QUrl("file://" SRCDIR "/SizeToLoader.qml"));
    QFxLoader *loader = qobject_cast<QFxLoader*>(component.create());
    QVERIFY(loader != 0);
    QCOMPARE(loader->width(), 200.0);
    QCOMPARE(loader->height(), 80.0);

    QFxItem *rect = loader->item();
    QVERIFY(rect);
    QCOMPARE(rect->width(), 200.0);
    QCOMPARE(rect->height(), 80.0);
}

void tst_qfxloader::noResize()
{
    QmlComponent component(&engine, QUrl("file://" SRCDIR "/NoResize.qml"));
    QFxLoader *loader = qobject_cast<QFxLoader*>(component.create());
    QVERIFY(loader != 0);
    QCOMPARE(loader->width(), 200.0);
    QCOMPARE(loader->height(), 80.0);

    QFxItem *rect = loader->item();
    QVERIFY(rect);
    QCOMPARE(rect->width(), 120.0);
    QCOMPARE(rect->height(), 60.0);
}

QTEST_MAIN(tst_qfxloader)

#include "tst_qfxloader.moc"

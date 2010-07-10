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
#include <QtTest/QtTest>
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativeimageprovider.h>
#include <private/qdeclarativeimage_p.h>
#include <QImageReader>

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
#define SRCDIR "."
#endif

// QDeclarativeImageProvider::request() is run in an idle thread where possible
// Be generous in our timeout.
#define TRY_WAIT(expr) \
    do { \
        for (int ii = 0; ii < 10; ++ii) { \
            if ((expr)) break; \
            QTest::qWait(100); \
        } \
        QVERIFY((expr)); \
    } while (false)

Q_DECLARE_METATYPE(QDeclarativeImageProvider*);


class tst_qdeclarativeimageprovider : public QObject
{
    Q_OBJECT
public:
    tst_qdeclarativeimageprovider()
    {
    }

private slots:
    void requestImage_sync_data();
    void requestImage_sync();
    void requestImage_async_data();
    void requestImage_async();

    void requestPixmap_sync_data();
    void requestPixmap_sync();
    void requestPixmap_async();

    void removeProvider_data();
    void removeProvider();

private:
    QString newImageFileName() const;
    void fillRequestTestsData(const QString &id);
    void runTest(bool async, QDeclarativeImageProvider *provider);
};


class TestQImageProvider : public QDeclarativeImageProvider
{
public:
    TestQImageProvider()
        : QDeclarativeImageProvider(Image)
    {
    }

    QImage requestImage(const QString &id, QSize *size, const QSize& requestedSize)
    {
        if (id == QLatin1String("no-such-file.png"))
            return QImage();

        int width = 100; 
        int height = 100;
        QImage image(width, height, QImage::Format_RGB32);
        if (size) 
            *size = QSize(width, height);
        if (requestedSize.isValid())
            image = image.scaled(requestedSize);
        return image;
    }
};
Q_DECLARE_METATYPE(TestQImageProvider*);


class TestQPixmapProvider : public QDeclarativeImageProvider
{
public:
    TestQPixmapProvider()
        : QDeclarativeImageProvider(Pixmap)
    {
    }

    QPixmap requestPixmap(const QString &id, QSize *size, const QSize& requestedSize)
    {
        if (id == QLatin1String("no-such-file.png"))
            return QPixmap();

        int width = 100; 
        int height = 100;
        QPixmap image(width, height);
        if (size) 
            *size = QSize(width, height);
        if (requestedSize.isValid())
            image = image.scaled(requestedSize);
        return image;
    }
};
Q_DECLARE_METATYPE(TestQPixmapProvider*);


QString tst_qdeclarativeimageprovider::newImageFileName() const
{
    // need to generate new filenames each time or else images are loaded
    // from cache and we won't get loading status changes when testing 
    // async loading
    static int count = 0;
    return QString("image://test/image-%1.png").arg(count++);
}

void tst_qdeclarativeimageprovider::fillRequestTestsData(const QString &id)
{
    QTest::addColumn<QString>("source");
    QTest::addColumn<QString>("properties");
    QTest::addColumn<QSize>("size");
    QTest::addColumn<QString>("error");

    QTest::newRow(QTest::toString(id + " exists")) << newImageFileName() << "" << QSize(100,100) << "";
    QTest::newRow(QTest::toString(id + " scaled")) << newImageFileName() << "sourceSize: \"80x30\"" << QSize(80,30) << "";

    QTest::newRow(QTest::toString(id + " missing")) << "image://test/no-such-file.png" << "" << QSize()
        << "file::2:1: QML Image: Failed to get image from provider: image://test/no-such-file.png";
    QTest::newRow(QTest::toString(id + " unknown provider")) << "image://bogus/exists.png" << "" << QSize()
        << "file::2:1: QML Image: Failed to get image from provider: image://bogus/exists.png";
}

void tst_qdeclarativeimageprovider::runTest(bool async, QDeclarativeImageProvider *provider)
{
    QFETCH(QString, source);
    QFETCH(QString, properties);
    QFETCH(QSize, size);
    QFETCH(QString, error);

    if (!error.isEmpty())
        QTest::ignoreMessage(QtWarningMsg, error.toUtf8());

    QDeclarativeEngine engine;

    engine.addImageProvider("test", provider);
    QVERIFY(engine.imageProvider("test") != 0);

    QString componentStr = "import Qt 4.7\nImage { source: \"" + source + "\"; " 
            + (async ? "asynchronous: true; " : "")
            + properties + " }";
    QDeclarativeComponent component(&engine);
    component.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
    QDeclarativeImage *obj = qobject_cast<QDeclarativeImage*>(component.create());
    QVERIFY(obj != 0);

    if (async) 
        TRY_WAIT(obj->status() == QDeclarativeImage::Loading);

    QCOMPARE(obj->source(), QUrl(source));

    if (error.isEmpty()) {
        if (async)
            TRY_WAIT(obj->status() == QDeclarativeImage::Ready);
        else
            QVERIFY(obj->status() == QDeclarativeImage::Ready);
        QCOMPARE(obj->width(), 100.0);
        QCOMPARE(obj->height(), 100.0);
        QCOMPARE(obj->pixmap().width(), size.width());
        QCOMPARE(obj->pixmap().height(), size.height());
        QCOMPARE(obj->fillMode(), QDeclarativeImage::Stretch);
        QCOMPARE(obj->progress(), 1.0);
    } else {
        if (async)
            TRY_WAIT(obj->status() == QDeclarativeImage::Error);
        else
            QVERIFY(obj->status() == QDeclarativeImage::Error);
    }

    delete obj;
}

void tst_qdeclarativeimageprovider::requestImage_sync_data()
{
    fillRequestTestsData("qimage|sync");
}

void tst_qdeclarativeimageprovider::requestImage_sync()
{
    runTest(false, new TestQImageProvider);
}

void tst_qdeclarativeimageprovider::requestImage_async_data()
{
    fillRequestTestsData("qimage|async");
}

void tst_qdeclarativeimageprovider::requestImage_async()
{
    runTest(true, new TestQImageProvider);
}

void tst_qdeclarativeimageprovider::requestPixmap_sync_data()
{
    fillRequestTestsData("qpixmap");
}

void tst_qdeclarativeimageprovider::requestPixmap_sync()
{
    runTest(false, new TestQPixmapProvider);
}

void tst_qdeclarativeimageprovider::requestPixmap_async()
{
    QDeclarativeEngine engine;
    QDeclarativeImageProvider *provider = new TestQPixmapProvider;

    engine.addImageProvider("test", provider);
    QVERIFY(engine.imageProvider("test") != 0);

    // pixmaps are loaded synchronously regardless of 'asynchronous' value
    QString componentStr = "import Qt 4.7\nImage { asynchronous: true; source: \"image://test/pixmap-async-test.png\" }";
    QDeclarativeComponent component(&engine);
    component.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
    QDeclarativeImage *obj = qobject_cast<QDeclarativeImage*>(component.create());
    QVERIFY(obj != 0);

    delete obj;
}

void tst_qdeclarativeimageprovider::removeProvider_data()
{
    QTest::addColumn<QDeclarativeImageProvider*>("provider");

    QTest::newRow("qimage") << static_cast<QDeclarativeImageProvider*>(new TestQImageProvider);
    QTest::newRow("qpixmap") << static_cast<QDeclarativeImageProvider*>(new TestQPixmapProvider);
}

void tst_qdeclarativeimageprovider::removeProvider()
{
    QFETCH(QDeclarativeImageProvider*, provider);

    QDeclarativeEngine engine;

    engine.addImageProvider("test", provider);
    QVERIFY(engine.imageProvider("test") != 0);

    // add provider, confirm it works
    QString componentStr = "import Qt 4.7\nImage { source: \"" + newImageFileName() + "\" }";
    QDeclarativeComponent component(&engine);
    component.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
    QDeclarativeImage *obj = qobject_cast<QDeclarativeImage*>(component.create());
    QVERIFY(obj != 0);

    QCOMPARE(obj->status(), QDeclarativeImage::Ready);

    // remove the provider and confirm
    QString fileName = newImageFileName();
    QString error("file::2:1: QML Image: Failed to get image from provider: " + fileName);
    QTest::ignoreMessage(QtWarningMsg, error.toUtf8());

    engine.removeImageProvider("test");

    obj->setSource(QUrl(fileName));
    QCOMPARE(obj->status(), QDeclarativeImage::Error);

    delete obj;
}


QTEST_MAIN(tst_qdeclarativeimageprovider)

#include "tst_qdeclarativeimageprovider.moc"

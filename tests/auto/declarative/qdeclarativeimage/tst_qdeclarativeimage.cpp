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
#include <QTextDocument>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDir>

#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativecomponent.h>
#include <QtDeclarative/qdeclarativeview.h>
#include <private/qdeclarativeimage_p.h>
#include <private/qdeclarativeimagebase_p.h>
#include <private/qdeclarativeloader_p.h>
#include <QtDeclarative/qdeclarativecontext.h>
#include <QtDeclarative/qdeclarativeexpression.h>

#include "../shared/testhttpserver.h"

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
#define SRCDIR "."
#endif

#define SERVER_PORT 14451
#define SERVER_ADDR "http://127.0.0.1:14451"

#define TRY_WAIT(expr) \
    do { \
        for (int ii = 0; ii < 60; ++ii) { \
            if ((expr)) break; \
            QTest::qWait(50); \
        } \
        QVERIFY((expr)); \
    } while (false)


class tst_qdeclarativeimage : public QObject

{
    Q_OBJECT
public:
    tst_qdeclarativeimage();

private slots:
    void noSource();
    void imageSource();
    void imageSource_data();
    void clearSource();
    void resized();
    void preserveAspectRatio();
    void smooth();
    void svg();
    void big();
    void tiling_QTBUG_6716();

private:
    template<typename T>
    T *findItem(QGraphicsObject *parent, const QString &id, int index=-1);

    QDeclarativeEngine engine;
};

tst_qdeclarativeimage::tst_qdeclarativeimage()
{
}

void tst_qdeclarativeimage::noSource()
{
    QString componentStr = "import Qt 4.7\nImage { source: \"\" }";
    QDeclarativeComponent component(&engine);
    component.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
    QDeclarativeImage *obj = qobject_cast<QDeclarativeImage*>(component.create());
    QVERIFY(obj != 0);
    QCOMPARE(obj->source(), QUrl());
    QVERIFY(obj->status() == QDeclarativeImage::Null);
    QCOMPARE(obj->width(), 0.);
    QCOMPARE(obj->height(), 0.);
    QCOMPARE(obj->fillMode(), QDeclarativeImage::Stretch);
    QCOMPARE(obj->progress(), 0.0);

    delete obj;
}

void tst_qdeclarativeimage::imageSource_data()
{
    QTest::addColumn<QString>("source");
    QTest::addColumn<qreal>("width");
    QTest::addColumn<qreal>("height");
    QTest::addColumn<bool>("remote");
    QTest::addColumn<bool>("async");
    QTest::addColumn<QString>("error");

    QTest::newRow("local") << QUrl::fromLocalFile(SRCDIR "/data/colors.png").toString() << 120.0 << 120.0 << false << false << "";
    QTest::newRow("local async") << QUrl::fromLocalFile(SRCDIR "/data/colors1.png").toString() << 120.0 << 120.0 << false << true << "";
    QTest::newRow("local not found") << QUrl::fromLocalFile(SRCDIR "/data/no-such-file.png").toString() << 0.0 << 0.0 << false
        << false << "file::2:1: QML Image: Cannot open: " + QUrl::fromLocalFile(SRCDIR "/data/no-such-file.png").toString();
    QTest::newRow("local async not found") << QUrl::fromLocalFile(SRCDIR "/data/no-such-file-1.png").toString() << 0.0 << 0.0 << false
        << true << "file::2:1: QML Image: Cannot open: " + QUrl::fromLocalFile(SRCDIR "/data/no-such-file-1.png").toString();
    QTest::newRow("remote") << SERVER_ADDR "/colors.png" << 120.0 << 120.0 << true << false << "";
    QTest::newRow("remote svg") << SERVER_ADDR "/heart.svg" << 550.0 << 500.0 << true << false << "";
    QTest::newRow("remote not found") << SERVER_ADDR "/no-such-file.png" << 0.0 << 0.0 << true
        << false << "file::2:1: QML Image: Error downloading " SERVER_ADDR "/no-such-file.png - server replied: Not found";

}

void tst_qdeclarativeimage::imageSource()
{
    QFETCH(QString, source);
    QFETCH(qreal, width);
    QFETCH(qreal, height);
    QFETCH(bool, remote);
    QFETCH(bool, async);
    QFETCH(QString, error);

    TestHTTPServer server(SERVER_PORT);
    if (remote) {
        QVERIFY(server.isValid());
        server.serveDirectory(SRCDIR "/data");
    }

    if (!error.isEmpty())
        QTest::ignoreMessage(QtWarningMsg, error.toUtf8());

    QString componentStr = "import Qt 4.7\nImage { source: \"" + source + "\"; asynchronous: "
        + (async ? QLatin1String("true") : QLatin1String("false")) + " }";
    QDeclarativeComponent component(&engine);
    component.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
    QDeclarativeImage *obj = qobject_cast<QDeclarativeImage*>(component.create());
    QVERIFY(obj != 0);

    if (async)
        QVERIFY(obj->asynchronous() == true);

    if (remote || async)
        TRY_WAIT(obj->status() == QDeclarativeImage::Loading);

    QCOMPARE(obj->source(), remote ? source : QUrl(source));

    if (error.isEmpty()) {
        TRY_WAIT(obj->status() == QDeclarativeImage::Ready);
        QCOMPARE(obj->width(), width);
        QCOMPARE(obj->height(), height);
        QCOMPARE(obj->fillMode(), QDeclarativeImage::Stretch);
        QCOMPARE(obj->progress(), 1.0);
    } else {
        TRY_WAIT(obj->status() == QDeclarativeImage::Error);
    }

    delete obj;
}

void tst_qdeclarativeimage::clearSource()
{
    QString componentStr = "import Qt 4.7\nImage { source: srcImage }";
    QDeclarativeContext *ctxt = engine.rootContext();
    ctxt->setContextProperty("srcImage", QUrl::fromLocalFile(SRCDIR "/data/colors.png"));
    QDeclarativeComponent component(&engine);
    component.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
    QDeclarativeImage *obj = qobject_cast<QDeclarativeImage*>(component.create());
    QVERIFY(obj != 0);
    QVERIFY(obj->status() == QDeclarativeImage::Ready);
    QCOMPARE(obj->width(), 120.);
    QCOMPARE(obj->height(), 120.);
    QCOMPARE(obj->progress(), 1.0);

    ctxt->setContextProperty("srcImage", "");
    QVERIFY(obj->source().isEmpty());
    QVERIFY(obj->status() == QDeclarativeImage::Null);
    QCOMPARE(obj->width(), 0.);
    QCOMPARE(obj->height(), 0.);
    QCOMPARE(obj->progress(), 0.0);
}

void tst_qdeclarativeimage::resized()
{
    QString componentStr = "import Qt 4.7\nImage { source: \"" SRCDIR "/data/colors.png\"; width: 300; height: 300 }";
    QDeclarativeComponent component(&engine);
    component.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
    QDeclarativeImage *obj = qobject_cast<QDeclarativeImage*>(component.create());
    QVERIFY(obj != 0);
    QCOMPARE(obj->width(), 300.);
    QCOMPARE(obj->height(), 300.);
    QCOMPARE(obj->fillMode(), QDeclarativeImage::Stretch);
    delete obj;
}


void tst_qdeclarativeimage::preserveAspectRatio()
{
    QDeclarativeView *canvas = new QDeclarativeView(0);
    canvas->show();

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/aspectratio.qml"));
    QDeclarativeImage *image = qobject_cast<QDeclarativeImage*>(canvas->rootObject());
    QVERIFY(image != 0);
    image->setWidth(80.0);
    QCOMPARE(image->width(), 80.);
    QCOMPARE(image->height(), 80.);

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/aspectratio.qml"));
    image = qobject_cast<QDeclarativeImage*>(canvas->rootObject());
    image->setHeight(60.0);
    QVERIFY(image != 0);
    QCOMPARE(image->height(), 60.);
    QCOMPARE(image->width(), 60.);
    delete canvas;
}

void tst_qdeclarativeimage::smooth()
{
    QString componentStr = "import Qt 4.7\nImage { source: \"" SRCDIR "/data/colors.png\"; smooth: true; width: 300; height: 300 }";
    QDeclarativeComponent component(&engine);
    component.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
    QDeclarativeImage *obj = qobject_cast<QDeclarativeImage*>(component.create());
    QVERIFY(obj != 0);
    QCOMPARE(obj->width(), 300.);
    QCOMPARE(obj->height(), 300.);
    QCOMPARE(obj->smooth(), true);
    QCOMPARE(obj->fillMode(), QDeclarativeImage::Stretch);

    delete obj;
}

void tst_qdeclarativeimage::svg()
{
    QString src = QUrl::fromLocalFile(SRCDIR "/data/heart.svg").toString();
    QString componentStr = "import Qt 4.7\nImage { source: \"" + src + "\"; sourceSize.width: 300; sourceSize.height: 300 }";
    QDeclarativeComponent component(&engine);
    component.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
    QDeclarativeImage *obj = qobject_cast<QDeclarativeImage*>(component.create());
    QVERIFY(obj != 0);
    QCOMPARE(obj->pixmap().width(), 300);
    QCOMPARE(obj->pixmap().height(), 300);
    QCOMPARE(obj->width(), 550.0);
    QCOMPARE(obj->height(), 500.0);
#if defined(Q_OS_MAC)
    QCOMPARE(obj->pixmap(), QPixmap(SRCDIR "/data/heart-mac.png"));
#elif defined(Q_OS_WIN32)
    QCOMPARE(obj->pixmap(), QPixmap(SRCDIR "/data/heart-win32.png"));
#else
    QCOMPARE(obj->pixmap(), QPixmap(SRCDIR "/data/heart.png"));
#endif

    obj->setSourceSize(QSize(200,200));

    QCOMPARE(obj->pixmap().width(), 200);
    QCOMPARE(obj->pixmap().height(), 200);
    QCOMPARE(obj->width(), 550.0);
    QCOMPARE(obj->height(), 500.0);
#if defined(Q_OS_MAC)
    QCOMPARE(obj->pixmap(), QPixmap(SRCDIR "/data/heart200-mac.png"));
#elif defined(Q_OS_WIN32)
    QCOMPARE(obj->pixmap(), QPixmap(SRCDIR "/data/heart200-win32.png"));
#else
    QCOMPARE(obj->pixmap(), QPixmap(SRCDIR "/data/heart200.png"));
#endif
    delete obj;
}

void tst_qdeclarativeimage::big()
{
    // If the JPEG loader does not implement scaling efficiently, it would
    // have to build a 400 MB image. That would be a bug in the JPEG loader.

    QString src = QUrl::fromLocalFile(SRCDIR "/data/big.jpeg").toString();
    QString componentStr = "import Qt 4.7\nImage { source: \"" + src + "\"; sourceSize.width: 256; sourceSize.height: 256 }";

    QDeclarativeComponent component(&engine);
    component.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
    QDeclarativeImage *obj = qobject_cast<QDeclarativeImage*>(component.create());
    QVERIFY(obj != 0);
    QCOMPARE(obj->pixmap().width(), 256);
    QCOMPARE(obj->pixmap().height(), 256);
    QCOMPARE(obj->width(), 10240.0);
    QCOMPARE(obj->height(), 10240.0);
    QCOMPARE(obj->pixmap(), QPixmap(SRCDIR "/data/big256.png"));

    delete obj;
}

void tst_qdeclarativeimage::tiling_QTBUG_6716()
{
    QDeclarativeView *canvas = new QDeclarativeView(0);
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/tiling.qml"));
    canvas->show();
    qApp->processEvents();

    QDeclarativeImage *vTiling = findItem<QDeclarativeImage>(canvas->rootObject(), "vTiling");
    QDeclarativeImage *hTiling = findItem<QDeclarativeImage>(canvas->rootObject(), "hTiling");

    QVERIFY(vTiling != 0);
    QVERIFY(hTiling != 0);

    {
        QPixmap pm(vTiling->width(), vTiling->height());
        QPainter p(&pm);
        vTiling->paint(&p, 0, 0);

        QImage img = pm.toImage();
        for (int x = 0; x < vTiling->width(); ++x) {
            for (int y = 0; y < vTiling->height(); ++y) {
                QVERIFY(img.pixel(x, y) == qRgb(0, 255, 0));
            }
        }
    }

    {
        QPixmap pm(hTiling->width(), hTiling->height());
        QPainter p(&pm);
        hTiling->paint(&p, 0, 0);

        QImage img = pm.toImage();
        for (int x = 0; x < hTiling->width(); ++x) {
            for (int y = 0; y < hTiling->height(); ++y) {
                QVERIFY(img.pixel(x, y) == qRgb(0, 255, 0));
            }
        }
    }
}

/*
   Find an item with the specified objectName.  If index is supplied then the
   item must also evaluate the {index} expression equal to index
*/
template<typename T>
T *tst_qdeclarativeimage::findItem(QGraphicsObject *parent, const QString &objectName, int index)
{
    const QMetaObject &mo = T::staticMetaObject;
    //qDebug() << parent->childItems().count() << "children";
    for (int i = 0; i < parent->childItems().count(); ++i) {
        QDeclarativeItem *item = qobject_cast<QDeclarativeItem*>(parent->childItems().at(i));
        if(!item)
            continue;
        //qDebug() << "try" << item;
        if (mo.cast(item) && (objectName.isEmpty() || item->objectName() == objectName)) {
            if (index != -1) {
                QDeclarativeExpression e(qmlContext(item), item, "index");
                if (e.evaluate().toInt() == index)
                    return static_cast<T*>(item);
            } else {
                return static_cast<T*>(item);
            }
        }
        item = findItem<T>(item, objectName, index);
        if (item)
            return static_cast<T*>(item);
    }

    return 0;
}

QTEST_MAIN(tst_qdeclarativeimage)

#include "tst_qdeclarativeimage.moc"

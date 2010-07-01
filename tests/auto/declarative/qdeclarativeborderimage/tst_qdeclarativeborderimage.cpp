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
#include <private/qdeclarativeborderimage_p.h>
#include <private/qdeclarativeimagebase_p.h>
#include <private/qdeclarativescalegrid_p_p.h>
#include <private/qdeclarativeloader_p.h>
#include <QtDeclarative/qdeclarativecontext.h>

#include "../shared/testhttpserver.h"

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
#define SRCDIR "."
#endif

#define SERVER_PORT 14446
#define SERVER_ADDR "http://127.0.0.1:14446"

#define TRY_WAIT(expr) \
    do { \
        for (int ii = 0; ii < 60; ++ii) { \
            if ((expr)) break; \
            QTest::qWait(50); \
        } \
        QVERIFY((expr)); \
    } while (false)


class tst_qdeclarativeborderimage : public QObject

{
    Q_OBJECT
public:
    tst_qdeclarativeborderimage();

private slots:
    void noSource();
    void imageSource();
    void imageSource_data();
    void clearSource();
    void resized();
    void smooth();
    void tileModes();
    void sciSource();
    void sciSource_data();
    void invalidSciFile();
    void pendingRemoteRequest();
    void pendingRemoteRequest_data();

private:
    QDeclarativeEngine engine;
};

tst_qdeclarativeborderimage::tst_qdeclarativeborderimage()
{
}

void tst_qdeclarativeborderimage::noSource()
{
    QString componentStr = "import Qt 4.7\nBorderImage { source: \"\" }";
    QDeclarativeComponent component(&engine);
    component.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
    QDeclarativeBorderImage *obj = qobject_cast<QDeclarativeBorderImage*>(component.create());
    QVERIFY(obj != 0);
    QCOMPARE(obj->source(), QUrl());
    QCOMPARE(obj->width(), 0.);
    QCOMPARE(obj->height(), 0.);
    QCOMPARE(obj->horizontalTileMode(), QDeclarativeBorderImage::Stretch);
    QCOMPARE(obj->verticalTileMode(), QDeclarativeBorderImage::Stretch);

    delete obj;
}

void tst_qdeclarativeborderimage::imageSource_data()
{
    QTest::addColumn<QString>("source");
    QTest::addColumn<bool>("remote");
    QTest::addColumn<QString>("error");

    QTest::newRow("local") << QUrl::fromLocalFile(SRCDIR "/data/colors.png").toString() << false << "";
    QTest::newRow("local not found") << QUrl::fromLocalFile(SRCDIR "/data/no-such-file.png").toString() << false
        << "file::2:1: QML BorderImage: Cannot open: " + QUrl::fromLocalFile(SRCDIR "/data/no-such-file.png").toString();
    QTest::newRow("remote") << SERVER_ADDR "/colors.png" << true << "";
    QTest::newRow("remote not found") << SERVER_ADDR "/no-such-file.png" << true
        << "file::2:1: QML BorderImage: Error downloading " SERVER_ADDR "/no-such-file.png - server replied: Not found";
}

void tst_qdeclarativeborderimage::imageSource()
{
    QFETCH(QString, source);
    QFETCH(bool, remote);
    QFETCH(QString, error);

    TestHTTPServer *server = 0;
    if (remote) {
        server = new TestHTTPServer(SERVER_PORT);
        QVERIFY(server->isValid());
        server->serveDirectory(SRCDIR "/data");
    }

    if (!error.isEmpty())
        QTest::ignoreMessage(QtWarningMsg, error.toUtf8());

    QString componentStr = "import Qt 4.7\nBorderImage { source: \"" + source + "\" }";
    QDeclarativeComponent component(&engine);
    component.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
    QDeclarativeBorderImage *obj = qobject_cast<QDeclarativeBorderImage*>(component.create());
    QVERIFY(obj != 0);

    if (remote)
        TRY_WAIT(obj->status() == QDeclarativeBorderImage::Loading);

    QCOMPARE(obj->source(), remote ? source : QUrl(source));

    if (error.isEmpty()) {
        TRY_WAIT(obj->status() == QDeclarativeBorderImage::Ready);
        QCOMPARE(obj->width(), 120.);
        QCOMPARE(obj->height(), 120.);
        QCOMPARE(obj->horizontalTileMode(), QDeclarativeBorderImage::Stretch);
        QCOMPARE(obj->verticalTileMode(), QDeclarativeBorderImage::Stretch);
    } else {
        TRY_WAIT(obj->status() == QDeclarativeBorderImage::Error);
    }

    delete obj;
    delete server;
}

void tst_qdeclarativeborderimage::clearSource()
{
    QString componentStr = "import Qt 4.7\nBorderImage { source: srcImage }";
    QDeclarativeContext *ctxt = engine.rootContext();
    ctxt->setContextProperty("srcImage", QUrl::fromLocalFile(SRCDIR "/data/colors.png"));
    QDeclarativeComponent component(&engine);
    component.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
    QDeclarativeBorderImage *obj = qobject_cast<QDeclarativeBorderImage*>(component.create());
    QVERIFY(obj != 0);
    QVERIFY(obj->status() == QDeclarativeBorderImage::Ready);
    QCOMPARE(obj->width(), 120.);
    QCOMPARE(obj->height(), 120.);

    ctxt->setContextProperty("srcImage", "");
    QVERIFY(obj->source().isEmpty());
    QVERIFY(obj->status() == QDeclarativeBorderImage::Null);
    QCOMPARE(obj->width(), 0.);
    QCOMPARE(obj->height(), 0.);
}

void tst_qdeclarativeborderimage::resized()
{
    QString componentStr = "import Qt 4.7\nBorderImage { source: \"" + QUrl::fromLocalFile(SRCDIR "/data/colors.png").toString() + "\"; width: 300; height: 300 }";
    QDeclarativeComponent component(&engine);
    component.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
    QDeclarativeBorderImage *obj = qobject_cast<QDeclarativeBorderImage*>(component.create());
    QVERIFY(obj != 0);
    QCOMPARE(obj->width(), 300.);
    QCOMPARE(obj->height(), 300.);
    QCOMPARE(obj->horizontalTileMode(), QDeclarativeBorderImage::Stretch);
    QCOMPARE(obj->verticalTileMode(), QDeclarativeBorderImage::Stretch);

    delete obj;
}

void tst_qdeclarativeborderimage::smooth()
{
    QString componentStr = "import Qt 4.7\nBorderImage { source: \"" SRCDIR "/data/colors.png\"; smooth: true; width: 300; height: 300 }";
    QDeclarativeComponent component(&engine);
    component.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
    QDeclarativeBorderImage *obj = qobject_cast<QDeclarativeBorderImage*>(component.create());
    QVERIFY(obj != 0);
    QCOMPARE(obj->width(), 300.);
    QCOMPARE(obj->height(), 300.);
    QCOMPARE(obj->smooth(), true);
    QCOMPARE(obj->horizontalTileMode(), QDeclarativeBorderImage::Stretch);
    QCOMPARE(obj->verticalTileMode(), QDeclarativeBorderImage::Stretch);

    delete obj;
}

void tst_qdeclarativeborderimage::tileModes()
{
    {
        QString componentStr = "import Qt 4.7\nBorderImage { source: \"" SRCDIR "/data/colors.png\"; width: 100; height: 300; horizontalTileMode: BorderImage.Repeat; verticalTileMode: BorderImage.Repeat }";
        QDeclarativeComponent component(&engine);
        component.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
        QDeclarativeBorderImage *obj = qobject_cast<QDeclarativeBorderImage*>(component.create());
        QVERIFY(obj != 0);
        QCOMPARE(obj->width(), 100.);
        QCOMPARE(obj->height(), 300.);
        QCOMPARE(obj->horizontalTileMode(), QDeclarativeBorderImage::Repeat);
        QCOMPARE(obj->verticalTileMode(), QDeclarativeBorderImage::Repeat);

        delete obj;
    }
    {
        QString componentStr = "import Qt 4.7\nBorderImage { source: \"" SRCDIR "/data/colors.png\"; width: 300; height: 150; horizontalTileMode: BorderImage.Round; verticalTileMode: BorderImage.Round }";
        QDeclarativeComponent component(&engine);
        component.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
        QDeclarativeBorderImage *obj = qobject_cast<QDeclarativeBorderImage*>(component.create());
        QVERIFY(obj != 0);
        QCOMPARE(obj->width(), 300.);
        QCOMPARE(obj->height(), 150.);
        QCOMPARE(obj->horizontalTileMode(), QDeclarativeBorderImage::Round);
        QCOMPARE(obj->verticalTileMode(), QDeclarativeBorderImage::Round);

        delete obj;
    }
}

void tst_qdeclarativeborderimage::sciSource()
{
    QFETCH(QString, source);
    QFETCH(bool, valid);

    bool remote = source.startsWith("http");
    TestHTTPServer *server = 0;
    if (remote) {
        server = new TestHTTPServer(SERVER_PORT);
        QVERIFY(server->isValid());
        server->serveDirectory(SRCDIR "/data");
    }

    QString componentStr = "import Qt 4.7\nBorderImage { source: \"" + source + "\"; width: 300; height: 300 }";
    QDeclarativeComponent component(&engine);
    component.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
    QDeclarativeBorderImage *obj = qobject_cast<QDeclarativeBorderImage*>(component.create());
    QVERIFY(obj != 0);

    if (remote)
        TRY_WAIT(obj->status() == QDeclarativeBorderImage::Loading);

    QCOMPARE(obj->source(), remote ? source : QUrl(source));
    QCOMPARE(obj->width(), 300.);
    QCOMPARE(obj->height(), 300.);

    if (valid) {
        TRY_WAIT(obj->status() == QDeclarativeBorderImage::Ready);
        QCOMPARE(obj->border()->left(), 10);
        QCOMPARE(obj->border()->top(), 20);
        QCOMPARE(obj->border()->right(), 30);
        QCOMPARE(obj->border()->bottom(), 40);
        QCOMPARE(obj->horizontalTileMode(), QDeclarativeBorderImage::Round);
        QCOMPARE(obj->verticalTileMode(), QDeclarativeBorderImage::Repeat);
    } else {
        TRY_WAIT(obj->status() == QDeclarativeBorderImage::Error);
    }

    delete obj;
    delete server;
}

void tst_qdeclarativeborderimage::sciSource_data()
{
    QTest::addColumn<QString>("source");
    QTest::addColumn<bool>("valid");

    QTest::newRow("local") << QUrl::fromLocalFile(SRCDIR "/data/colors-round.sci").toString() << true;
    QTest::newRow("local not found") << QUrl::fromLocalFile(SRCDIR "/data/no-such-file.sci").toString() << false;
    QTest::newRow("remote") << SERVER_ADDR "/colors-round.sci" << true;
    QTest::newRow("remote not found") << SERVER_ADDR "/no-such-file.sci" << false;
}

void tst_qdeclarativeborderimage::invalidSciFile()
{
    QTest::ignoreMessage(QtWarningMsg, "QDeclarativeGridScaledImage: Invalid tile rule specified. Using Stretch."); // for "Roun"
    QTest::ignoreMessage(QtWarningMsg, "QDeclarativeGridScaledImage: Invalid tile rule specified. Using Stretch."); // for "Repea"

    QString componentStr = "import Qt 4.7\nBorderImage { source: \"" + QUrl::fromLocalFile(SRCDIR "/data/invalid.sci").toString() +"\"; width: 300; height: 300 }";
    QDeclarativeComponent component(&engine);
    component.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
    QDeclarativeBorderImage *obj = qobject_cast<QDeclarativeBorderImage*>(component.create());
    QVERIFY(obj != 0);
    QCOMPARE(obj->width(), 300.);
    QCOMPARE(obj->height(), 300.);
    QCOMPARE(obj->status(), QDeclarativeImageBase::Error);
    QCOMPARE(obj->horizontalTileMode(), QDeclarativeBorderImage::Stretch);
    QCOMPARE(obj->verticalTileMode(), QDeclarativeBorderImage::Stretch);

    delete obj;
}

void tst_qdeclarativeborderimage::pendingRemoteRequest()
{
    QFETCH(QString, source);

    QString componentStr = "import Qt 4.7\nBorderImage { source: \"" + source + "\" }";
    QDeclarativeComponent component(&engine);
    component.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
    QDeclarativeBorderImage *obj = qobject_cast<QDeclarativeBorderImage*>(component.create());
    QVERIFY(obj != 0);
    QCOMPARE(obj->status(), QDeclarativeBorderImage::Loading);

    // verify no crash
    // This will cause a delayed "QThread: Destroyed while thread is still running" warning
    delete obj;
    QTest::qWait(50);
}

void tst_qdeclarativeborderimage::pendingRemoteRequest_data()
{
    QTest::addColumn<QString>("source");

    QTest::newRow("png file") << "http://localhost/none.png";
    QTest::newRow("sci file") << "http://localhost/none.sci";
}

QTEST_MAIN(tst_qdeclarativeborderimage)

#include "tst_qdeclarativeborderimage.moc"

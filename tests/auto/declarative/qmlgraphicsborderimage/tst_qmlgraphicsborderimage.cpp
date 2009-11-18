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
#include <QTextDocument>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDir>

#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcomponent.h>
#include <private/qmlgraphicsborderimage_p.h>
#include <private/qmlgraphicsimagebase_p.h>
#include <private/qmlgraphicsscalegrid_p_p.h>
#include <private/qmlgraphicsloader_p.h>

#include "../shared/testhttpserver.h"


#define SERVER_PORT 14445
#define SERVER_ADDR "http://127.0.0.1:14445"

#define TRY_WAIT(expr) \
    do { \
        for (int ii = 0; ii < 6; ++ii) { \
            if ((expr)) break; \
            QTest::qWait(50); \
        } \
        QVERIFY((expr)); \
    } while (false)


class tst_qmlgraphicsborderimage : public QObject

{
    Q_OBJECT
public:
    tst_qmlgraphicsborderimage();

private slots:
    void noSource();
    void imageSource();
    void imageSource_data();
    void resized();
    void smooth();
    void tileModes();
    void sciSource();
    void sciSource_data();
    void invalidSciFile();

private:
    QmlEngine engine;
};

tst_qmlgraphicsborderimage::tst_qmlgraphicsborderimage()
{
}

void tst_qmlgraphicsborderimage::noSource()
{
    QString componentStr = "import Qt 4.6\nBorderImage { source: \"\" }";
    QmlComponent component(&engine, componentStr.toLatin1(), QUrl("file://"));
    QmlGraphicsBorderImage *obj = qobject_cast<QmlGraphicsBorderImage*>(component.create());
    QVERIFY(obj != 0);
    QCOMPARE(obj->source(), QUrl());
    QCOMPARE(obj->width(), 0.);
    QCOMPARE(obj->height(), 0.);
    QCOMPARE(obj->horizontalTileMode(), QmlGraphicsBorderImage::Stretch);
    QCOMPARE(obj->verticalTileMode(), QmlGraphicsBorderImage::Stretch);

    delete obj;
}

void tst_qmlgraphicsborderimage::imageSource()
{
    QFETCH(QString, source);
    QFETCH(bool, remote);
    QFETCH(bool, valid);

    TestHTTPServer server(SERVER_PORT);
    if (remote) {
        QVERIFY(server.isValid());
        server.serveDirectory(SRCDIR "/data");
    }

    QString componentStr = "import Qt 4.6\nBorderImage { source: \"" + source + "\" }";
    QmlComponent component(&engine, componentStr.toLatin1(), QUrl("file://"));
    QmlGraphicsBorderImage *obj = qobject_cast<QmlGraphicsBorderImage*>(component.create());
    QVERIFY(obj != 0);
    
    if (remote)
        TRY_WAIT(obj->status() == QmlGraphicsBorderImage::Loading);

    QCOMPARE(obj->source(), remote ? source : QUrl::fromLocalFile(source));

    if (valid) {
        TRY_WAIT(obj->status() == QmlGraphicsBorderImage::Ready);
        QCOMPARE(obj->width(), 120.);
        QCOMPARE(obj->height(), 120.);
        QCOMPARE(obj->horizontalTileMode(), QmlGraphicsBorderImage::Stretch);
        QCOMPARE(obj->verticalTileMode(), QmlGraphicsBorderImage::Stretch);
    } else {
        TRY_WAIT(obj->status() == QmlGraphicsBorderImage::Error);
    }

    delete obj;
}

void tst_qmlgraphicsborderimage::imageSource_data()
{
    QTest::addColumn<QString>("source");
    QTest::addColumn<bool>("remote");
    QTest::addColumn<bool>("valid");

    QTest::newRow("local") << SRCDIR "/data/colors.png" << false << true;
    QTest::newRow("local not found") << SRCDIR "/data/no-such-file.png" << false << false;
    QTest::newRow("remote") << SERVER_ADDR "/colors.png" << true << true;
    QTest::newRow("remote not found") << SERVER_ADDR "/no-such-file.png" << true << false;
}

void tst_qmlgraphicsborderimage::resized()
{
    QString componentStr = "import Qt 4.6\nBorderImage { source: \"" SRCDIR "/data/colors.png\"; width: 300; height: 300 }";
    QmlComponent component(&engine, componentStr.toLatin1(), QUrl("file://"));
    QmlGraphicsBorderImage *obj = qobject_cast<QmlGraphicsBorderImage*>(component.create());
    QVERIFY(obj != 0);
    QCOMPARE(obj->width(), 300.);
    QCOMPARE(obj->height(), 300.);
    QCOMPARE(obj->horizontalTileMode(), QmlGraphicsBorderImage::Stretch);
    QCOMPARE(obj->verticalTileMode(), QmlGraphicsBorderImage::Stretch);

    delete obj;
}

void tst_qmlgraphicsborderimage::smooth()
{
    QString componentStr = "import Qt 4.6\nBorderImage { source: \"" SRCDIR "/data/colors.png\"; smooth: true; width: 300; height: 300 }";
    QmlComponent component(&engine, componentStr.toLatin1(), QUrl("file://"));
    QmlGraphicsBorderImage *obj = qobject_cast<QmlGraphicsBorderImage*>(component.create());
    QVERIFY(obj != 0);
    QCOMPARE(obj->width(), 300.);
    QCOMPARE(obj->height(), 300.);
    QCOMPARE(obj->smooth(), true);
    QCOMPARE(obj->horizontalTileMode(), QmlGraphicsBorderImage::Stretch);
    QCOMPARE(obj->verticalTileMode(), QmlGraphicsBorderImage::Stretch);

    delete obj;
}

void tst_qmlgraphicsborderimage::tileModes()
{
    {
        QString componentStr = "import Qt 4.6\nBorderImage { source: \"" SRCDIR "/data/colors.png\"; width: 100; height: 300; horizontalTileMode: BorderImage.Repeat; verticalTileMode: BorderImage.Repeat }";
        QmlComponent component(&engine, componentStr.toLatin1(), QUrl("file://"));
        QmlGraphicsBorderImage *obj = qobject_cast<QmlGraphicsBorderImage*>(component.create());
        QVERIFY(obj != 0);
        QCOMPARE(obj->width(), 100.);
        QCOMPARE(obj->height(), 300.);
        QCOMPARE(obj->horizontalTileMode(), QmlGraphicsBorderImage::Repeat);
        QCOMPARE(obj->verticalTileMode(), QmlGraphicsBorderImage::Repeat);

        delete obj;
    }
    {
        QString componentStr = "import Qt 4.6\nBorderImage { source: \"" SRCDIR "/data/colors.png\"; width: 300; height: 150; horizontalTileMode: BorderImage.Round; verticalTileMode: BorderImage.Round }";
        QmlComponent component(&engine, componentStr.toLatin1(), QUrl("file://"));
        QmlGraphicsBorderImage *obj = qobject_cast<QmlGraphicsBorderImage*>(component.create());
        QVERIFY(obj != 0);
        QCOMPARE(obj->width(), 300.);
        QCOMPARE(obj->height(), 150.);
        QCOMPARE(obj->horizontalTileMode(), QmlGraphicsBorderImage::Round);
        QCOMPARE(obj->verticalTileMode(), QmlGraphicsBorderImage::Round);

        delete obj;
    }
}

void tst_qmlgraphicsborderimage::sciSource()
{
    QFETCH(QString, source);
    QFETCH(bool, remote);
    QFETCH(bool, valid);

    TestHTTPServer server(SERVER_PORT);
    if (remote) {
        QVERIFY(server.isValid());
        server.serveDirectory(SRCDIR "/data");
    }

    QString componentStr = "import Qt 4.6\nBorderImage { source: \"" + source + "\"; width: 300; height: 300 }";
    QmlComponent component(&engine, componentStr.toLatin1(), QUrl("file://"));
    QmlGraphicsBorderImage *obj = qobject_cast<QmlGraphicsBorderImage*>(component.create());
    QVERIFY(obj != 0);
    
    if (remote)
        TRY_WAIT(obj->status() == QmlGraphicsBorderImage::Loading);
    
    QCOMPARE(obj->source(), remote ? source : QUrl::fromLocalFile(source));
    QCOMPARE(obj->width(), 300.);
    QCOMPARE(obj->height(), 300.);
    
    if (valid) {
        TRY_WAIT(obj->status() == QmlGraphicsBorderImage::Ready);
        QCOMPARE(obj->border()->left(), 10);
        QCOMPARE(obj->border()->top(), 20);
        QCOMPARE(obj->border()->right(), 30);
        QCOMPARE(obj->border()->bottom(), 40);
        QCOMPARE(obj->horizontalTileMode(), QmlGraphicsBorderImage::Round);
        QCOMPARE(obj->verticalTileMode(), QmlGraphicsBorderImage::Repeat);
    } else {
        TRY_WAIT(obj->status() == QmlGraphicsBorderImage::Error);
    }

    delete obj;
}

void tst_qmlgraphicsborderimage::sciSource_data()
{
    QTest::addColumn<QString>("source");
    QTest::addColumn<bool>("remote");
    QTest::addColumn<bool>("valid");

    QTest::newRow("local") << SRCDIR "/data/colors-round.sci" << false << true;
    QTest::newRow("local not found") << SRCDIR "/data/no-such-file.sci" << false << false;
    QTest::newRow("remote") << SERVER_ADDR "/colors-round.sci" << true << true;
    QTest::newRow("remote not found") << SERVER_ADDR "/no-such-file.sci" << true << false;
}

void tst_qmlgraphicsborderimage::invalidSciFile()
{
    QString componentStr = "import Qt 4.6\nBorderImage { source: \"" SRCDIR "/data/invalid.sci\"; width: 300; height: 300 }";
    QmlComponent component(&engine, componentStr.toLatin1(), QUrl("file://"));
    QmlGraphicsBorderImage *obj = qobject_cast<QmlGraphicsBorderImage*>(component.create());
    QVERIFY(obj != 0);
    QCOMPARE(obj->width(), 300.);
    QCOMPARE(obj->height(), 300.);
    QCOMPARE(obj->status(), QmlGraphicsImageBase::Error);
    QCOMPARE(obj->horizontalTileMode(), QmlGraphicsBorderImage::Stretch);
    QCOMPARE(obj->verticalTileMode(), QmlGraphicsBorderImage::Stretch);

    delete obj;
}



QTEST_MAIN(tst_qmlgraphicsborderimage)

#include "tst_qmlgraphicsborderimage.moc"

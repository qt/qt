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

#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcomponent.h>
#include <private/qmlgraphicsimage_p.h>
#include <private/qmlgraphicsimagebase_p.h>
#include <private/qmlgraphicsloader_p.h>
#include <QtDeclarative/qmlcontext.h>

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


class tst_qmlgraphicsimage : public QObject

{
    Q_OBJECT
public:
    tst_qmlgraphicsimage();

private slots:
    void noSource();
    void imageSource();
    void imageSource_data();
    void clearSource();
    void resized();
    void smooth();
    void pixmap();

private:
    QmlEngine engine;
};

tst_qmlgraphicsimage::tst_qmlgraphicsimage()
{
}

void tst_qmlgraphicsimage::noSource()
{
    QString componentStr = "import Qt 4.6\nImage { source: \"\" }";
    QmlComponent component(&engine);
    component.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
    QmlGraphicsImage *obj = qobject_cast<QmlGraphicsImage*>(component.create());
    QVERIFY(obj != 0);
    QCOMPARE(obj->source(), QUrl());
    QVERIFY(obj->status() == QmlGraphicsImage::Null);
    QCOMPARE(obj->width(), 0.);
    QCOMPARE(obj->height(), 0.);
    QCOMPARE(obj->fillMode(), QmlGraphicsImage::Stretch);
    QCOMPARE(obj->progress(), 0.0);

    delete obj;
}

void tst_qmlgraphicsimage::imageSource_data()
{
    QTest::addColumn<QString>("source");
    QTest::addColumn<bool>("remote");
    QTest::addColumn<QString>("error");

    QTest::newRow("local") << QUrl::fromLocalFile(SRCDIR "/data/colors.png").toString() << false << "";
    QTest::newRow("local not found") << QUrl::fromLocalFile(SRCDIR "/data/no-such-file.png").toString() << false
        << "Cannot open  QUrl( \"" + QUrl::fromLocalFile(SRCDIR "/data/no-such-file.png").toString() + "\" )  ";
    QTest::newRow("remote") << SERVER_ADDR "/colors.png" << true << "";
    QTest::newRow("remote not found") << SERVER_ADDR "/no-such-file.png" << true
        << "\"Error downloading " SERVER_ADDR "/no-such-file.png - server replied: Not found\" ";
}

void tst_qmlgraphicsimage::imageSource()
{
    QFETCH(QString, source);
    QFETCH(bool, remote);
    QFETCH(QString, error);

    TestHTTPServer server(SERVER_PORT);
    if (remote) {
        QVERIFY(server.isValid());
        server.serveDirectory(SRCDIR "/data");
    }

    if (!error.isEmpty())
        QTest::ignoreMessage(QtWarningMsg, error.toUtf8());

    QString componentStr = "import Qt 4.6\nImage { source: \"" + source + "\" }";
    QmlComponent component(&engine);
    component.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
    QmlGraphicsImage *obj = qobject_cast<QmlGraphicsImage*>(component.create());
    QVERIFY(obj != 0);
    
    if (remote)
        TRY_WAIT(obj->status() == QmlGraphicsImage::Loading);

    QCOMPARE(obj->source(), remote ? source : QUrl(source));

    if (error.isEmpty()) {
        TRY_WAIT(obj->status() == QmlGraphicsImage::Ready);
        QCOMPARE(obj->width(), 120.);
        QCOMPARE(obj->height(), 120.);
        QCOMPARE(obj->fillMode(), QmlGraphicsImage::Stretch);
        QCOMPARE(obj->progress(), 1.0);
    } else {
        TRY_WAIT(obj->status() == QmlGraphicsImage::Error);
    }

    delete obj;
}

void tst_qmlgraphicsimage::clearSource()
{
    QString componentStr = "import Qt 4.6\nImage { source: srcImage }";
    QmlContext *ctxt = engine.rootContext();
    ctxt->setContextProperty("srcImage", QUrl::fromLocalFile(SRCDIR "/data/colors.png"));
    QmlComponent component(&engine);
    component.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
    QmlGraphicsImage *obj = qobject_cast<QmlGraphicsImage*>(component.create());
    QVERIFY(obj != 0);
    QVERIFY(obj->status() == QmlGraphicsImage::Ready);
    QCOMPARE(obj->width(), 120.);
    QCOMPARE(obj->height(), 120.);
    QCOMPARE(obj->progress(), 1.0);

    ctxt->setContextProperty("srcImage", "");
    QVERIFY(obj->source().isEmpty());
    QVERIFY(obj->status() == QmlGraphicsImage::Null);
    QCOMPARE(obj->width(), 0.);
    QCOMPARE(obj->height(), 0.);
    QCOMPARE(obj->progress(), 0.0);
}

void tst_qmlgraphicsimage::resized()
{
    QString componentStr = "import Qt 4.6\nImage { source: \"" SRCDIR "/data/colors.png\"; width: 300; height: 300 }";
    QmlComponent component(&engine);
    component.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
    QmlGraphicsImage *obj = qobject_cast<QmlGraphicsImage*>(component.create());
    QVERIFY(obj != 0);
    QCOMPARE(obj->width(), 300.);
    QCOMPARE(obj->height(), 300.);
    QCOMPARE(obj->fillMode(), QmlGraphicsImage::Stretch);

    delete obj;
}

void tst_qmlgraphicsimage::smooth()
{
    QString componentStr = "import Qt 4.6\nImage { source: \"" SRCDIR "/data/colors.png\"; smooth: true; width: 300; height: 300 }";
    QmlComponent component(&engine);
    component.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
    QmlGraphicsImage *obj = qobject_cast<QmlGraphicsImage*>(component.create());
    QVERIFY(obj != 0);
    QCOMPARE(obj->width(), 300.);
    QCOMPARE(obj->height(), 300.);
    QCOMPARE(obj->smooth(), true);
    QCOMPARE(obj->fillMode(), QmlGraphicsImage::Stretch);

    delete obj;
}

void tst_qmlgraphicsimage::pixmap()
{
    QString componentStr = "import Qt 4.6\nImage { pixmap: testPixmap }";

    QPixmap pixmap;
    QmlContext *ctxt = engine.rootContext();
    ctxt->setContextProperty("testPixmap", pixmap);

    QmlComponent component(&engine);
    component.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));

    QmlGraphicsImage *obj = qobject_cast<QmlGraphicsImage*>(component.create());
    QVERIFY(obj != 0);
    QCOMPARE(obj->source(), QUrl());
    QVERIFY(obj->status() == QmlGraphicsImage::Null);
    QCOMPARE(obj->width(), 0.);
    QCOMPARE(obj->height(), 0.);
    QCOMPARE(obj->fillMode(), QmlGraphicsImage::Stretch);
    QCOMPARE(obj->progress(), 0.0);
    QVERIFY(obj->pixmap().isNull());

    pixmap = QPixmap(SRCDIR "/data/colors.png");
    ctxt->setContextProperty("testPixmap", pixmap);
    QCOMPARE(obj->width(), 120.);
    QCOMPARE(obj->height(), 120.);
    QVERIFY(obj->status() == QmlGraphicsImage::Ready);

    delete obj;
}

QTEST_MAIN(tst_qmlgraphicsimage)

#include "tst_qmlgraphicsimage.moc"

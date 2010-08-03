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
#include <QtTest/QtTest>
#include <private/qdeclarativepixmapcache_p.h>
#include <QtDeclarative/qdeclarativeengine.h>
#include <QNetworkReply>
#include "testhttpserver.h"
#include "../../../shared/util.h"

// These don't let normal people run tests!
//#include "../network-settings.h"

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
#define SRCDIR "."
#endif

class tst_qdeclarativepixmapcache : public QObject
{
    Q_OBJECT
public:
    tst_qdeclarativepixmapcache() :
        thisfile(QUrl::fromLocalFile(__FILE__)),
        server(14452)
    {
        server.serveDirectory(SRCDIR "/data/http");
    }

private slots:
    void single();
    void single_data();
    void parallel();
    void parallel_data();
    void massive();

private:
    QDeclarativeEngine engine;
    QUrl thisfile;
    TestHTTPServer server;
};


static int slotters=0;

class Slotter : public QObject
{
    Q_OBJECT
public:
    Slotter()
    {
        gotslot = false;
        slotters++;
    }
    bool gotslot;

public slots:
    void got()
    {
        gotslot = true;
        --slotters;
        if (slotters==0)
            QTestEventLoop::instance().exitLoop();
    }
};

#ifndef QT_NO_LOCALFILE_OPTIMIZED_QML
static const bool localfile_optimized = true;
#else
static const bool localfile_optimized = false;
#endif

void tst_qdeclarativepixmapcache::single_data()
{
    // Note, since QDeclarativePixmapCache is shared, tests affect each other!
    // so use different files fore all test functions.

    QTest::addColumn<QUrl>("target");
    QTest::addColumn<bool>("incache");
    QTest::addColumn<bool>("exists");
    QTest::addColumn<bool>("neterror");

    // File URLs are optimized
    QTest::newRow("local") << thisfile.resolved(QUrl("data/exists.png")) << localfile_optimized << true << false;
    QTest::newRow("local") << thisfile.resolved(QUrl("data/notexists.png")) << localfile_optimized << false << false;
    QTest::newRow("remote") << QUrl("http://127.0.0.1:14452/exists.png") << false << true << false;
    QTest::newRow("remote") << QUrl("http://127.0.0.1:14452/notexists.png") << false << false << true;
}

void tst_qdeclarativepixmapcache::single()
{
    QFETCH(QUrl, target);
    QFETCH(bool, incache);
    QFETCH(bool, exists);
    QFETCH(bool, neterror);

    QString expectedError;
    if (neterror) {
        expectedError = "Error downloading " + target.toString() + " - server replied: Not found";
    } else if (!exists) {
        expectedError = "Cannot open: " + target.toString();
    }

    QDeclarativePixmap pixmap;
    QVERIFY(pixmap.width() <= 0); // Check Qt assumption

    pixmap.load(&engine, target);

    if (incache) {
        QCOMPARE(pixmap.error(), expectedError);
        if (exists) {
            QVERIFY(pixmap.status() == QDeclarativePixmap::Ready);
            QVERIFY(pixmap.width() > 0);
        } else {
            QVERIFY(pixmap.status() == QDeclarativePixmap::Error);
            QVERIFY(pixmap.width() <= 0);
        }
    } else {
        QVERIFY(pixmap.width() <= 0);

        Slotter getter;
        pixmap.connectFinished(&getter, SLOT(got()));
        QTestEventLoop::instance().enterLoop(10);
        QVERIFY(!QTestEventLoop::instance().timeout());
        QVERIFY(getter.gotslot);
        if (exists) {
            QVERIFY(pixmap.status() == QDeclarativePixmap::Ready);
            QVERIFY(pixmap.width() > 0);
        } else {
            QVERIFY(pixmap.status() == QDeclarativePixmap::Error);
            QVERIFY(pixmap.width() <= 0);
        }
        QCOMPARE(pixmap.error(), expectedError);
    }
}

void tst_qdeclarativepixmapcache::parallel_data()
{
    // Note, since QDeclarativePixmapCache is shared, tests affect each other!
    // so use different files fore all test functions.

    QTest::addColumn<QUrl>("target1");
    QTest::addColumn<QUrl>("target2");
    QTest::addColumn<int>("incache");
    QTest::addColumn<int>("cancel"); // which one to cancel

    QTest::newRow("local")
            << thisfile.resolved(QUrl("data/exists1.png"))
            << thisfile.resolved(QUrl("data/exists2.png"))
            << (localfile_optimized ? 2 : 0)
            << -1;

    QTest::newRow("remote")
            << QUrl("http://127.0.0.1:14452/exists2.png")
            << QUrl("http://127.0.0.1:14452/exists3.png")
            << 0
            << -1;

    QTest::newRow("remoteagain")
            << QUrl("http://127.0.0.1:14452/exists2.png")
            << QUrl("http://127.0.0.1:14452/exists3.png")
            << 2
            << -1;

    QTest::newRow("remotecopy")
            << QUrl("http://127.0.0.1:14452/exists4.png")
            << QUrl("http://127.0.0.1:14452/exists4.png")
            << 0
            << -1;

    QTest::newRow("remotecopycancel")
            << QUrl("http://127.0.0.1:14452/exists5.png")
            << QUrl("http://127.0.0.1:14452/exists5.png")
            << 0
            << 0;
}

void tst_qdeclarativepixmapcache::parallel()
{
    QFETCH(QUrl, target1);
    QFETCH(QUrl, target2);
    QFETCH(int, incache);
    QFETCH(int, cancel);

    QList<QUrl> targets;
    targets << target1 << target2;

    QList<QDeclarativePixmap *> pixmaps;
    QList<bool> pending;
    QList<Slotter*> getters;

    for (int i=0; i<targets.count(); ++i) {
        QUrl target = targets.at(i);
        QDeclarativePixmap *pixmap = new QDeclarativePixmap;

        pixmap->load(&engine, target);

        QVERIFY(pixmap->status() != QDeclarativePixmap::Error);
        pixmaps.append(pixmap);
        if (pixmap->isReady()) {
            QVERIFY(pixmap->width() >  0);
            getters.append(0);
            pending.append(false);
        } else {
            QVERIFY(pixmap->width() <= 0);
            getters.append(new Slotter);
            pixmap->connectFinished(getters[i], SLOT(got()));
            pending.append(true);
        }
    }

    QCOMPARE(incache+slotters, targets.count());

    if (cancel >= 0) {
        pixmaps.at(cancel)->clear(getters[cancel]);
        slotters--;
    }

    if (slotters) {
        QTestEventLoop::instance().enterLoop(10);
        QVERIFY(!QTestEventLoop::instance().timeout());
    }

    for (int i=0; i<targets.count(); ++i) {
        QDeclarativePixmap *pixmap = pixmaps[i];

        if (i == cancel) {
            QVERIFY(!getters[i]->gotslot);
        } else {
            if (pending[i]) 
                QVERIFY(getters[i]->gotslot);

            QVERIFY(pixmap->isReady());
            QVERIFY(pixmap->width() > 0);
            delete getters[i];
        }
    }

    qDeleteAll(pixmaps);
}

void tst_qdeclarativepixmapcache::massive()
{
    QUrl url = thisfile.resolved(QUrl("data/massive.png"));

    // Confirm that massive images remain in the cache while they are
    // in use by the application.
    {
    qint64 cachekey = 0;
    QDeclarativePixmap p(0, url);
    QVERIFY(p.isReady());
    QVERIFY(p.pixmap().size() == QSize(10000, 1000));
    cachekey = p.pixmap().cacheKey();

    QDeclarativePixmap p2(0, url);
    QVERIFY(p2.isReady());
    QVERIFY(p2.pixmap().size() == QSize(10000, 1000));

    QVERIFY(p2.pixmap().cacheKey() == cachekey);
    }

    // Confirm that massive images are removed from the cache when
    // they become unused
    {
    qint64 cachekey = 0;
    {
        QDeclarativePixmap p(0, url);
        QVERIFY(p.isReady());
        QVERIFY(p.pixmap().size() == QSize(10000, 1000));
        cachekey = p.pixmap().cacheKey();
    }

    QDeclarativePixmap p2(0, url);
    QVERIFY(p2.isReady());
    QVERIFY(p2.pixmap().size() == QSize(10000, 1000));

    QVERIFY(p2.pixmap().cacheKey() != cachekey);
    }
}

QTEST_MAIN(tst_qdeclarativepixmapcache)

#include "tst_qdeclarativepixmapcache.moc"

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
#include <private/qmlpixmapcache_p.h>
#include <QtDeclarative/qmlengine.h>
#include <QNetworkReply>

// These don't let normal people run tests!
//#include "../network-settings.h"

class tst_qmlpixmapcache : public QObject
{
    Q_OBJECT
public:
    tst_qmlpixmapcache() :
        thisfile(QUrl::fromLocalFile(__FILE__))
    {
    }

private slots:
    void single();
    void single_data();
    void parallel();
    void parallel_data();

private:
    QmlEngine engine;
    QUrl thisfile;
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

void tst_qmlpixmapcache::single_data()
{
    // Note, since QmlPixmapCache is shared, tests affect each other!
    // so use different files fore all test functions.

    QTest::addColumn<QUrl>("target");
    QTest::addColumn<bool>("incache");
    QTest::addColumn<bool>("exists");
    QTest::addColumn<bool>("neterror");

    // File URLs are optimized
    QTest::newRow("local") << thisfile.resolved(QUrl("data/exists.png")) << localfile_optimized << true << false;
    QTest::newRow("local") << thisfile.resolved(QUrl("data/notexists.png")) << localfile_optimized << false << false;
    QTest::newRow("remote") << QUrl("http://qt.nokia.com/logo.png") << false << true << false;
    QTest::newRow("remote") << QUrl("http://qt.nokia.com/thereisnologo.png") << false << false << true;
}

void tst_qmlpixmapcache::single()
{
    QFETCH(QUrl, target);
    QFETCH(bool, incache);
    QFETCH(bool, exists);
    QFETCH(bool, neterror);

    if (neterror) {
        QString expected = "\"Error downloading " + target.toString() + " - server replied: Not Found\" ";
        QTest::ignoreMessage(QtWarningMsg, expected.toLatin1());
    } else if (!exists) {
        QString expected = "Cannot open  QUrl( \"" + target.toString() + "\" )  ";
        QTest::ignoreMessage(QtWarningMsg, expected.toLatin1());
    }

    QPixmap pixmap;
    QVERIFY(pixmap.width() <= 0); // Check Qt assumption
    QmlPixmapReply::Status status = QmlPixmapCache::get(target, &pixmap);

    if (incache) {
        if (exists) {
            QVERIFY(status == QmlPixmapReply::Ready);
            QVERIFY(pixmap.width() > 0);
        } else {
            QVERIFY(status == QmlPixmapReply::Error);
            QVERIFY(pixmap.width() <= 0);
        }
    } else {
        QmlPixmapReply *reply = QmlPixmapCache::request(&engine, target);
        QVERIFY(reply);
        QVERIFY(pixmap.width() <= 0);

        Slotter getter;
        connect(reply, SIGNAL(finished()), &getter, SLOT(got()));
        QTestEventLoop::instance().enterLoop(10);
        QVERIFY(!QTestEventLoop::instance().timeout());
        QVERIFY(getter.gotslot);
        if (exists) {
            QVERIFY(QmlPixmapCache::get(target, &pixmap) == QmlPixmapReply::Ready);
            QVERIFY(pixmap.width() > 0);
        } else {
            QVERIFY(QmlPixmapCache::get(target, &pixmap) == QmlPixmapReply::Error);
            QVERIFY(pixmap.width() <= 0);
        }
    }

    QCOMPARE(QmlPixmapCache::pendingRequests(), 0);
}

void tst_qmlpixmapcache::parallel_data()
{
    // Note, since QmlPixmapCache is shared, tests affect each other!
    // so use different files fore all test functions.

    QTest::addColumn<QUrl>("target1");
    QTest::addColumn<QUrl>("target2");
    QTest::addColumn<int>("incache");
    QTest::addColumn<int>("cancel"); // which one to cancel
    QTest::addColumn<int>("requests");

    QTest::newRow("local")
            << thisfile.resolved(QUrl("data/exists1.png"))
            << thisfile.resolved(QUrl("data/exists2.png"))
            << (localfile_optimized ? 2 : 0)
            << -1
            << (localfile_optimized ? 0 : 2)
            ;

    QTest::newRow("remote")
            << QUrl("http://qt.nokia.com/images/template/checkbox-on.png")
            << QUrl("http://qt.nokia.com/images/products/qt-logo/image_tile")
            << 0
            << -1
            << 2
            ;

    QTest::newRow("remoteagain")
            << QUrl("http://qt.nokia.com/images/template/checkbox-on.png")
            << QUrl("http://qt.nokia.com/images/products/qt-logo/image_tile")
            << 2
            << -1
            << 0
            ;

    QTest::newRow("remotecopy")
            << QUrl("http://qt.nokia.com/images/template/checkbox-off.png")
            << QUrl("http://qt.nokia.com/images/template/checkbox-off.png")
            << 0
            << -1
            << 1
            ;

    QTest::newRow("remotecopycancel")
            << QUrl("http://qt.nokia.com/rounded_block_bg.png")
            << QUrl("http://qt.nokia.com/rounded_block_bg.png")
            << 0
            << 0
            << 1
            ;
}

void tst_qmlpixmapcache::parallel()
{
    QFETCH(QUrl, target1);
    QFETCH(QUrl, target2);
    QFETCH(int, incache);
    QFETCH(int, cancel);
    QFETCH(int, requests);

    QList<QUrl> targets;
    targets << target1 << target2;

    QList<QmlPixmapReply*> replies;
    QList<Slotter*> getters;
    for (int i=0; i<targets.count(); ++i) {
        QUrl target = targets.at(i);
        QPixmap pixmap;
        QmlPixmapReply::Status status = QmlPixmapCache::get(target, &pixmap);
        QmlPixmapReply *reply = 0;
        if (status != QmlPixmapReply::Error && status != QmlPixmapReply::Ready)
            reply = QmlPixmapCache::request(&engine, target);
        replies.append(reply);
        if (!reply) {
            QVERIFY(pixmap.width() > 0);
            getters.append(0);
        } else {
            QVERIFY(pixmap.width() <= 0);
            getters.append(new Slotter);
            connect(reply, SIGNAL(finished()), getters[i], SLOT(got()));
        }
    }

    QCOMPARE(incache+slotters, targets.count());
    QCOMPARE(QmlPixmapCache::pendingRequests(), requests);

    if (cancel >= 0) {
        QmlPixmapCache::cancel(targets.at(cancel), getters[cancel]);
        slotters--;
    }

    if (slotters) {
        QTestEventLoop::instance().enterLoop(10);
        QVERIFY(!QTestEventLoop::instance().timeout());
    }

    for (int i=0; i<targets.count(); ++i) {
        QmlPixmapReply *reply = replies[i];
        if (reply) {
            if (i == cancel) {
                QVERIFY(!getters[i]->gotslot);
            } else {
                QVERIFY(getters[i]->gotslot);
                QPixmap pixmap;
                QVERIFY(QmlPixmapCache::get(targets[i], &pixmap) == QmlPixmapReply::Ready);
                QVERIFY(pixmap.width() > 0);
            }
            delete getters[i];
        }
    }

    QCOMPARE(QmlPixmapCache::pendingRequests(), 0);
}

QTEST_MAIN(tst_qmlpixmapcache)

#include "tst_qmlpixmapcache.moc"

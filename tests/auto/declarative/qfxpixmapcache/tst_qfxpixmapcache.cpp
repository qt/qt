#include <qtest.h>
#include <QtTest/QtTest>
#include <QtDeclarative/qfxpixmapcache.h>
#include <QtDeclarative/qmlengine.h>
#include <QNetworkReply>

// These don't let normal people run tests!
//#include "../network-settings.h"

class tst_qfxpixmapcache : public QObject
{
    Q_OBJECT
public:
    tst_qfxpixmapcache() :
        thisfile("file://" __FILE__)
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

void tst_qfxpixmapcache::single_data()
{
    // Note, since QFxPixmapCache is shared, tests affect each other!
    // so use different files fore all test functions.

    QTest::addColumn<QUrl>("target");
    QTest::addColumn<bool>("incache");
    QTest::addColumn<bool>("exists");

    // File URLs are optimized
    QTest::newRow("local") << thisfile.resolved(QUrl("data/exists.png")) << localfile_optimized << true;
    QTest::newRow("local") << thisfile.resolved(QUrl("data/notexists.png")) << localfile_optimized << false;
    QTest::newRow("remote") << QUrl("http://qt.nokia.com/logo.png") << false << true;
    QTest::newRow("remote") << QUrl("http://qt.nokia.com/thereisnologo.png") << false << false;
}

void tst_qfxpixmapcache::single()
{
    QFETCH(QUrl, target);
    QFETCH(bool, incache);
    QFETCH(bool, exists);

    QPixmap pixmap;
    QVERIFY(pixmap.width() <= 0); // Check Qt assumption
    QNetworkReply *reply= QFxPixmapCache::get(&engine, target, &pixmap);

    if (incache) {
        QVERIFY(!reply);
        if (exists)
            QVERIFY(pixmap.width() > 0);
        else
            QVERIFY(pixmap.width() <= 0);
    } else {
        QVERIFY(reply);
        QVERIFY(pixmap.width() <= 0);

        Slotter getter;
        connect(reply, SIGNAL(finished()), &getter, SLOT(got()));
        QTestEventLoop::instance().enterLoop(10);
        QVERIFY(!QTestEventLoop::instance().timeout());
        QVERIFY(getter.gotslot);
        if (exists) {
            QVERIFY(QFxPixmapCache::find(target, &pixmap));
            QVERIFY(pixmap.width() > 0);
        } else {
            QVERIFY(!QFxPixmapCache::find(target, &pixmap));
            QVERIFY(pixmap.width() <= 0);
        }
    }

    QCOMPARE(QFxPixmapCache::pendingRequests(), 0);
}

void tst_qfxpixmapcache::parallel_data()
{
    // Note, since QFxPixmapCache is shared, tests affect each other!
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

void tst_qfxpixmapcache::parallel()
{
    QFETCH(QUrl, target1);
    QFETCH(QUrl, target2);
    QFETCH(int, incache);
    QFETCH(int, cancel);
    QFETCH(int, requests);

    QList<QUrl> targets;
    targets << target1 << target2;

    QList<QNetworkReply*> replies;
    QList<Slotter*> getters;
    for (int i=0; i<targets.count(); ++i) {
        QUrl target = targets.at(i);
        QPixmap pixmap;
        QNetworkReply *reply = QFxPixmapCache::get(&engine, target, &pixmap);
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
    QCOMPARE(QFxPixmapCache::pendingRequests(), requests);

    if (cancel >= 0) {
        QFxPixmapCache::cancelGet(targets.at(cancel), getters[cancel]);
        slotters--;
    }

    if (slotters) {
        QTestEventLoop::instance().enterLoop(10);
        QVERIFY(!QTestEventLoop::instance().timeout());
    }

    for (int i=0; i<targets.count(); ++i) {
        QNetworkReply *reply = replies[i];
        if (reply) {
            if (i == cancel) {
                QVERIFY(!getters[i]->gotslot);
            } else {
                QVERIFY(getters[i]->gotslot);
                QPixmap pixmap;
                QVERIFY(QFxPixmapCache::find(targets[i], &pixmap));
                QVERIFY(pixmap.width() > 0);
            }
            delete getters[i];
        }
    }

    QCOMPARE(QFxPixmapCache::pendingRequests(), 0);
}

QTEST_MAIN(tst_qfxpixmapcache)

#include "tst_qfxpixmapcache.moc"

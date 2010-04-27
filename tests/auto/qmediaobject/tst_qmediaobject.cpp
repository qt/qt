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

#include <QtTest/QtTest>

#include <QtCore/qtimer.h>

#include <QtMediaServices/qmediaobject.h>
#include <QtMediaServices/qmediaservice.h>
#include <QtMediaServices/qmetadatacontrol.h>


class tst_QMediaObject : public QObject
{
    Q_OBJECT

private slots:
    void propertyWatch();
    void notifySignals_data();
    void notifySignals();
    void notifyInterval_data();
    void notifyInterval();

    void nullMetaDataControl();
    void isMetaDataAvailable();
    void isWritable();
    void metaDataChanged();
    void metaData_data();
    void metaData();
    void setMetaData_data();
    void setMetaData();
    void extendedMetaData_data() { metaData_data(); }
    void extendedMetaData();
    void setExtendedMetaData_data() { extendedMetaData_data(); }
    void setExtendedMetaData();


private:
    void setupNotifyTests();
};

class QtTestMetaDataProvider : public QMetaDataControl
{
    Q_OBJECT
public:
    QtTestMetaDataProvider(QObject *parent = 0)
        : QMetaDataControl(parent)
        , m_available(false)
        , m_writable(false)
    {
    }

    bool isMetaDataAvailable() const { return m_available; }
    void setMetaDataAvailable(bool available) {
        if (m_available != available)
            emit metaDataAvailableChanged(m_available = available);
    }
    QList<QtMediaServices::MetaData> availableMetaData() const { return m_data.keys(); }

    bool isWritable() const { return m_writable; }
    void setWritable(bool writable) { emit writableChanged(m_writable = writable); }

    QVariant metaData(QtMediaServices::MetaData key) const { return m_data.value(key); }
    void setMetaData(QtMediaServices::MetaData key, const QVariant &value) {
        m_data.insert(key, value); }

    QVariant extendedMetaData(const QString &key) const { return m_extendedData.value(key); }
    void setExtendedMetaData(const QString &key, const QVariant &value) {
        m_extendedData.insert(key, value); }

    QStringList availableExtendedMetaData() const { return m_extendedData.keys(); }

    using QMetaDataControl::metaDataChanged;

    void populateMetaData()
    {
        m_available = true;
    }

    bool m_available;
    bool m_writable;
    QMap<QtMediaServices::MetaData, QVariant> m_data;
    QMap<QString, QVariant> m_extendedData;
};

class QtTestMetaDataService : public QMediaService
{
    Q_OBJECT
public:
    QtTestMetaDataService(QObject *parent = 0):QMediaService(parent), hasMetaData(true)
    {
    }

    QMediaControl *control(const char *iid) const
    {
        if (hasMetaData && qstrcmp(iid, QMetaDataControl_iid) == 0)
            return const_cast<QtTestMetaDataProvider *>(&metaData);
        else
            return 0;
    }

    QtTestMetaDataProvider metaData;
    bool hasMetaData;
};


class QtTestMediaObject : public QMediaObject
{
    Q_OBJECT
    Q_PROPERTY(int a READ a WRITE setA NOTIFY aChanged)
    Q_PROPERTY(int b READ b WRITE setB NOTIFY bChanged)
    Q_PROPERTY(int c READ c WRITE setC NOTIFY cChanged)
    Q_PROPERTY(int d READ d WRITE setD)
public:
    QtTestMediaObject(QMediaService *service = 0): QMediaObject(0, service), m_a(0), m_b(0), m_c(0), m_d(0) {}

    using QMediaObject::addPropertyWatch;
    using QMediaObject::removePropertyWatch;

    int a() const { return m_a; }
    void setA(int a) { m_a = a; }

    int b() const { return m_b; }
    void setB(int b) { m_b = b; }

    int c() const { return m_c; }
    void setC(int c) { m_c = c; }

    int d() const { return m_d; }
    void setD(int d) { m_d = d; }

Q_SIGNALS:
    void aChanged(int a);
    void bChanged(int b);
    void cChanged(int c);

private:
    int m_a;
    int m_b;
    int m_c;
    int m_d;
};

void tst_QMediaObject::propertyWatch()
{
    QtTestMediaObject object;
    object.setNotifyInterval(0);

    QEventLoop loop;
    connect(&object, SIGNAL(aChanged(int)), &QTestEventLoop::instance(), SLOT(exitLoop()));
    connect(&object, SIGNAL(bChanged(int)), &QTestEventLoop::instance(), SLOT(exitLoop()));
    connect(&object, SIGNAL(cChanged(int)), &QTestEventLoop::instance(), SLOT(exitLoop()));

    QSignalSpy aSpy(&object, SIGNAL(aChanged(int)));
    QSignalSpy bSpy(&object, SIGNAL(bChanged(int)));
    QSignalSpy cSpy(&object, SIGNAL(cChanged(int)));

    QTestEventLoop::instance().enterLoop(1);

    QCOMPARE(aSpy.count(), 0);
    QCOMPARE(bSpy.count(), 0);
    QCOMPARE(cSpy.count(), 0);

    int aCount = 0;
    int bCount = 0;
    int cCount = 0;

    object.addPropertyWatch("a");

    QTestEventLoop::instance().enterLoop(1);

    QVERIFY(aSpy.count() > aCount);
    QCOMPARE(bSpy.count(), 0);
    QCOMPARE(cSpy.count(), 0);
    QCOMPARE(aSpy.last().value(0).toInt(), 0);

    aCount = aSpy.count();

    object.setA(54);
    object.setB(342);
    object.setC(233);

    QTestEventLoop::instance().enterLoop(1);

    QVERIFY(aSpy.count() > aCount);
    QCOMPARE(bSpy.count(), 0);
    QCOMPARE(cSpy.count(), 0);
    QCOMPARE(aSpy.last().value(0).toInt(), 54);

    aCount = aSpy.count();

    object.addPropertyWatch("b");
    object.addPropertyWatch("d");
    object.removePropertyWatch("e");
    object.setA(43);
    object.setB(235);
    object.setC(90);

    QTestEventLoop::instance().enterLoop(1);

    QVERIFY(aSpy.count() > aCount);
    QVERIFY(bSpy.count() > bCount);
    QCOMPARE(cSpy.count(), 0);
    QCOMPARE(aSpy.last().value(0).toInt(), 43);
    QCOMPARE(bSpy.last().value(0).toInt(), 235);

    aCount = aSpy.count();
    bCount = bSpy.count();

    object.removePropertyWatch("a");
    object.addPropertyWatch("c");
    object.addPropertyWatch("e");

    QTestEventLoop::instance().enterLoop(1);

    QCOMPARE(aSpy.count(), aCount);
    QVERIFY(bSpy.count() > bCount);
    QVERIFY(cSpy.count() > cCount);
    QCOMPARE(bSpy.last().value(0).toInt(), 235);
    QCOMPARE(cSpy.last().value(0).toInt(), 90);

    bCount = bSpy.count();
    cCount = cSpy.count();

    object.setA(435);
    object.setC(9845);

    QTestEventLoop::instance().enterLoop(1);

    QCOMPARE(aSpy.count(), aCount);
    QVERIFY(bSpy.count() > bCount);
    QVERIFY(cSpy.count() > cCount);
    QCOMPARE(bSpy.last().value(0).toInt(), 235);
    QCOMPARE(cSpy.last().value(0).toInt(), 9845);

    bCount = bSpy.count();
    cCount = cSpy.count();

    object.setA(8432);
    object.setB(324);
    object.setC(443);
    object.removePropertyWatch("c");
    object.removePropertyWatch("d");

    QTestEventLoop::instance().enterLoop(1);

    QCOMPARE(aSpy.count(), aCount);
    QVERIFY(bSpy.count() > bCount);
    QCOMPARE(cSpy.count(), cCount);
    QCOMPARE(bSpy.last().value(0).toInt(), 324);
    QCOMPARE(cSpy.last().value(0).toInt(), 9845);

    bCount = bSpy.count();

    object.removePropertyWatch("b");

    QTestEventLoop::instance().enterLoop(1);

    QCOMPARE(aSpy.count(), aCount);
    QCOMPARE(bSpy.count(), bCount);
    QCOMPARE(cSpy.count(), cCount);
}

void tst_QMediaObject::setupNotifyTests()
{
    QTest::addColumn<int>("interval");
    QTest::addColumn<int>("count");

    QTest::newRow("single 750ms")
            << 750
            << 1;
    QTest::newRow("single 600ms")
            << 600
            << 1;
    QTest::newRow("x3 300ms")
            << 300
            << 3;
    QTest::newRow("x5 180ms")
            << 180
            << 5;
}

void tst_QMediaObject::notifySignals_data()
{
    setupNotifyTests();
}

void tst_QMediaObject::notifySignals()
{
    QFETCH(int, interval);
    QFETCH(int, count);

    QtTestMediaObject object;
    object.setNotifyInterval(interval);
    object.addPropertyWatch("a");

    QSignalSpy spy(&object, SIGNAL(aChanged(int)));

    QTestEventLoop::instance().enterLoop(1);

    QCOMPARE(spy.count(), count);
}

void tst_QMediaObject::notifyInterval_data()
{
    setupNotifyTests();
}

void tst_QMediaObject::notifyInterval()
{
    QFETCH(int, interval);

    QtTestMediaObject object;
    QSignalSpy spy(&object, SIGNAL(notifyIntervalChanged(int)));

    object.setNotifyInterval(interval);
    QCOMPARE(object.notifyInterval(), interval);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.last().value(0).toInt(), interval);

    object.setNotifyInterval(interval);
    QCOMPARE(object.notifyInterval(), interval);
    QCOMPARE(spy.count(), 1);
}

void tst_QMediaObject::nullMetaDataControl()
{
    const QString titleKey(QLatin1String("Title"));
    const QString title(QLatin1String("Host of Seraphim"));

    QtTestMetaDataService service;
    service.hasMetaData = false;

    QtTestMediaObject object(&service);

    QSignalSpy spy(&object, SIGNAL(metaDataChanged()));

    QCOMPARE(object.isMetaDataAvailable(), false);
    QCOMPARE(object.isMetaDataWritable(), false);

    object.setMetaData(QtMediaServices::Title, title);
    object.setExtendedMetaData(titleKey, title);

    QCOMPARE(object.metaData(QtMediaServices::Title).toString(), QString());
    QCOMPARE(object.extendedMetaData(titleKey).toString(), QString());
    QCOMPARE(object.availableMetaData(), QList<QtMediaServices::MetaData>());
    QCOMPARE(object.availableExtendedMetaData(), QStringList());
    QCOMPARE(spy.count(), 0);
}

void tst_QMediaObject::isMetaDataAvailable()
{
    QtTestMetaDataService service;
    service.metaData.setMetaDataAvailable(false);

    QtTestMediaObject object(&service);
    QCOMPARE(object.isMetaDataAvailable(), false);

    QSignalSpy spy(&object, SIGNAL(metaDataAvailableChanged(bool)));
    service.metaData.setMetaDataAvailable(true);

    QCOMPARE(object.isMetaDataAvailable(), true);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toBool(), true);

    service.metaData.setMetaDataAvailable(false);

    QCOMPARE(object.isMetaDataAvailable(), false);
    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy.at(1).at(0).toBool(), false);
}

void tst_QMediaObject::isWritable()
{
    QtTestMetaDataService service;
    service.metaData.setWritable(false);

    QtTestMediaObject object(&service);

    QSignalSpy spy(&object, SIGNAL(metaDataWritableChanged(bool)));

    QCOMPARE(object.isMetaDataWritable(), false);

    service.metaData.setWritable(true);

    QCOMPARE(object.isMetaDataWritable(), true);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toBool(), true);

    service.metaData.setWritable(false);

    QCOMPARE(object.isMetaDataWritable(), false);
    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy.at(1).at(0).toBool(), false);
}

void tst_QMediaObject::metaDataChanged()
{
    QtTestMetaDataService service;
    QtTestMediaObject object(&service);

    QSignalSpy spy(&object, SIGNAL(metaDataChanged()));

    service.metaData.metaDataChanged();
    QCOMPARE(spy.count(), 1);

    service.metaData.metaDataChanged();
    QCOMPARE(spy.count(), 2);
}

void tst_QMediaObject::metaData_data()
{
    QTest::addColumn<QString>("artist");
    QTest::addColumn<QString>("title");
    QTest::addColumn<QString>("genre");

    QTest::newRow("")
            << QString::fromLatin1("Dead Can Dance")
            << QString::fromLatin1("Host of Seraphim")
            << QString::fromLatin1("Awesome");
}

void tst_QMediaObject::metaData()
{
    QFETCH(QString, artist);
    QFETCH(QString, title);
    QFETCH(QString, genre);

    QtTestMetaDataService service;
    service.metaData.populateMetaData();

    QtTestMediaObject object(&service);
    QVERIFY(object.availableMetaData().isEmpty());

    service.metaData.m_data.insert(QtMediaServices::AlbumArtist, artist);
    service.metaData.m_data.insert(QtMediaServices::Title, title);
    service.metaData.m_data.insert(QtMediaServices::Genre, genre);

    QCOMPARE(object.metaData(QtMediaServices::AlbumArtist).toString(), artist);
    QCOMPARE(object.metaData(QtMediaServices::Title).toString(), title);

    QList<QtMediaServices::MetaData> metaDataKeys = object.availableMetaData();
    QCOMPARE(metaDataKeys.size(), 3);
    QVERIFY(metaDataKeys.contains(QtMediaServices::AlbumArtist));
    QVERIFY(metaDataKeys.contains(QtMediaServices::Title));
    QVERIFY(metaDataKeys.contains(QtMediaServices::Genre));
}

void tst_QMediaObject::setMetaData_data()
{
    QTest::addColumn<QString>("title");

    QTest::newRow("")
            << QString::fromLatin1("In the Kingdom of the Blind the One eyed are Kings");
}

void tst_QMediaObject::setMetaData()
{
    QFETCH(QString, title);

    QtTestMetaDataService service;
    service.metaData.populateMetaData();

    QtTestMediaObject object(&service);

    object.setMetaData(QtMediaServices::Title, title);
    QCOMPARE(object.metaData(QtMediaServices::Title).toString(), title);
    QCOMPARE(service.metaData.m_data.value(QtMediaServices::Title).toString(), title);
}

void tst_QMediaObject::extendedMetaData()
{
    QFETCH(QString, artist);
    QFETCH(QString, title);
    QFETCH(QString, genre);

    QtTestMetaDataService service;
    QtTestMediaObject object(&service);
    QVERIFY(object.availableExtendedMetaData().isEmpty());

    service.metaData.m_extendedData.insert(QLatin1String("Artist"), artist);
    service.metaData.m_extendedData.insert(QLatin1String("Title"), title);
    service.metaData.m_extendedData.insert(QLatin1String("Genre"), genre);

    QCOMPARE(object.extendedMetaData(QLatin1String("Artist")).toString(), artist);
    QCOMPARE(object.extendedMetaData(QLatin1String("Title")).toString(), title);

    QStringList extendedKeys = object.availableExtendedMetaData();
    QCOMPARE(extendedKeys.size(), 3);
    QVERIFY(extendedKeys.contains(QLatin1String("Artist")));
    QVERIFY(extendedKeys.contains(QLatin1String("Title")));
    QVERIFY(extendedKeys.contains(QLatin1String("Genre")));
}

void tst_QMediaObject::setExtendedMetaData()
{
    QtTestMetaDataService service;
    service.metaData.populateMetaData();

    QtTestMediaObject object(&service);

    QString title(QLatin1String("In the Kingdom of the Blind the One eyed are Kings"));

    object.setExtendedMetaData(QLatin1String("Title"), title);
    QCOMPARE(object.extendedMetaData(QLatin1String("Title")).toString(), title);
    QCOMPARE(service.metaData.m_extendedData.value(QLatin1String("Title")).toString(), title);
}

QTEST_MAIN(tst_QMediaObject)

#include "tst_qmediaobject.moc"

/*
  libconninet - Internet Connectivity support library

  Copyright (C) 2009-2010 Nokia Corporation. All rights reserved.

  Contact: Jukka Rissanen <jukka.rissanen@nokia.com>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public License
  version 2.1 as published by the Free Software Foundation.

  This library is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
  02110-1301 USA
*/


#include <QtTest/QtTest>
#include <QDebug>

#include <iapmonitor.h>
#include <iapconf.h>

static const char *iap_id = "test_monitor_1";

class TestIAPMonitor : public Maemo::IAPMonitor
{
public:
    QString addedIap;
    QString removedIap;
    
protected:
    virtual void iapAdded(const QString &id)
    {
        addedIap = id;
    }

    virtual void iapRemoved(const QString &id)
    {
        removedIap = id;
    }
};

class Ut_IAPMonitor : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void init();
    void cleanup();
    void initTestCase();
    void cleanupTestCase();

    void check();

private:
    TestIAPMonitor *mon;
    Maemo::IAPConf *iap;
};

void Ut_IAPMonitor::init()
{
    mon = new TestIAPMonitor;
}

void Ut_IAPMonitor::cleanup()
{
    delete mon;
    mon = 0;
}

void Ut_IAPMonitor::initTestCase()
{
} 

void Ut_IAPMonitor::cleanupTestCase()
{
}

void Ut_IAPMonitor::check()
{
    QVERIFY(mon->addedIap.isEmpty());

    iap = new Maemo::IAPConf(iap_id);
    iap->set("ipv4_type", "AUTO",
	    "wlan_wepkey1", "connt",
	    "wlan_wepdefkey", 1,
	    "wlan_ssid", QByteArray(iap_id));
    
    QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
    QVERIFY(mon->addedIap == iap_id);
    mon->addedIap.clear();

    QVERIFY(mon->removedIap.isEmpty());

    // Unset only one value and verify that IAP is not removed
    iap->set("ipv4_type", QVariant());
    QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
    QVERIFY(mon->removedIap.isEmpty());

    // Clear the whole IAP and check that it is removed
    iap->clear();
    delete iap;

    QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);

    QVERIFY(mon->removedIap == iap_id);
}

QTEST_MAIN(Ut_IAPMonitor)

#include "ut_iapmonitor.moc"

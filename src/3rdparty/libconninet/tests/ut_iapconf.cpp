/*
  libconninet - Internet Connectivity support library

  Copyright (C) 2009 Nokia Corporation. All rights reserved.

  Contact: Aapo Makela <aapo.makela@nokia.com>

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

#include <iapconf.h>

class Ut_IAPConf : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void init();
    void cleanup();
    void initTestCase();
    void cleanupTestCase();

    void setupIAP();
    void setupIAPContainingDot();
    void unsetIAPValueIsNotValid();
    void verifyAllIAPs();
    void allForEmptyConfReturnsEmptyList();
    void settingInvalidValueUnsetsKey();

private:
    Maemo::IAPConf *mSubject;
};

void Ut_IAPConf::init()
{
    mSubject = new Maemo::IAPConf("test_iap");
}

void Ut_IAPConf::cleanup()
{
    // Clear made settings
    mSubject->clear();

    delete mSubject;
    mSubject = 0;
}

void Ut_IAPConf::initTestCase()
{
} 

void Ut_IAPConf::cleanupTestCase()
{
}

void Ut_IAPConf::setupIAP()
{
    // Set bunch of values
    mSubject->set("ipv4_type", "AUTO",
                  "wlan_wepkey1", "connt",
                  "wlan_wepdefkey", 1,
                  "wlan_ssid", QByteArray("CONNTEST-1"));

    // Set one value
    mSubject->setValue("type", "WLAN_INFRA");

    // Check all values that they were set correctly
    QVERIFY(mSubject->value("ipv4_type").toString() == "AUTO");
    QVERIFY(mSubject->value("wlan_wepkey1").toString() == "connt");
    QVERIFY(mSubject->value("wlan_wepdefkey").toInt() == 1);
    QVERIFY(mSubject->value("wlan_ssid").toByteArray() == QByteArray("CONNTEST-1"));
    QVERIFY(mSubject->value("type").toString() == "WLAN_INFRA");
}

void Ut_IAPConf::setupIAPContainingDot()
{
    delete mSubject;
    mSubject = new Maemo::IAPConf("test.iap");

    // Set and check one value
    mSubject->setValue("type", "DUMMY");
    QVERIFY(mSubject->value("type").toString() == "DUMMY");
}

void Ut_IAPConf::unsetIAPValueIsNotValid()
{
    QVariant invalidValue = mSubject->value("this_value_does_not_exist");
    QVERIFY(invalidValue.isValid() == false);
}

void Ut_IAPConf::verifyAllIAPs()
{
    int count = 0, extras = 0;
    QRegExp regexp("iap[1-3]");
    Maemo::IAPConf iap1("iap1");
    Maemo::IAPConf iap2("iap2");
    Maemo::IAPConf iap3("iap3");

    iap1.clear();
    iap2.clear();
    iap3.clear();

    iap1.setValue("name", "iap1");
    iap2.setValue("name", "iap2");
    iap3.setValue("name", "iap3");

    count = extras = 0;
    QList<QString> iaps;
    Maemo::IAPConf::getAll(iaps, true);
    foreach (QString iap_path, iaps) {
        QString iap_id = iap_path.section('/', 5);  /* This is the IAP id */
	if (!iap_id.contains(regexp)) {
	    extras++;
	    continue;
	}
	Maemo::IAPConf iap(iap_id);
	QString name = iap.value("name").toString();
	QVERIFY(name == iap_id);
	count++;
    }
    QCOMPARE(count, iaps.size()-extras);

    iap1.clear();
    iap2.clear();
    iap3.clear();

    iap1.setValue("name", "iap1");
    iap2.setValue("name", "iap2");
    iap3.setValue("name", "iap3");

    count = extras = 0;
    Maemo::IAPConf::getAll(iaps);
    foreach (QString iap_id, iaps) {
        if (!iap_id.contains(regexp)) {
	    extras++;
	    continue;
	}
	Maemo::IAPConf iap(iap_id);
	QString name = iap.value("name").toString();
	QVERIFY(name == iap_id);
	count++;
    }
    QCOMPARE(count, iaps.size()-extras);
}

void Ut_IAPConf::allForEmptyConfReturnsEmptyList()
{
    // Clear everything in configuration
    mSubject->clearAll();

    // Get all for a list and check that it is empty
    QStringList iaps;
    mSubject->getAll(iaps);
    QVERIFY(iaps.isEmpty());
}

void Ut_IAPConf::settingInvalidValueUnsetsKey()
{
    // Setup some IAP
    setupIAP();

    // Set invalid value to unset "wlan_wepdefkey" key and verify that the
    // value is unset
    mSubject->setValue("wlan_wepdefkey", QVariant());
    QVERIFY(!mSubject->value("wlan_wepdefkey").isValid());
}

QTEST_MAIN(Ut_IAPConf)

#include "ut_iapconf.moc"

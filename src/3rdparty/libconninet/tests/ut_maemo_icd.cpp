/*
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 * Contact: Jukka Rissanen <jukka.rissanen@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */


// !!!!
// !!!! NOTE: THESE TEST DO NOT REALLY WORK YET BECAUSE OF MISSING
// !!!!       FUNCTIONALITY IN ICD2 STUB. YOU HAVE BEEN WARNED.
// !!!!


#include <QtTest/QtTest>
#include <QCoreApplication>
#include <QEventLoop>
#include <QDebug>
#include <icd/dbus_api.h>

#include "maemo_icd.h"

class Ut_MaemoIcd : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void init();
    void cleanup();
    void initTestCase();
    void cleanupTestCase();

    void scan_req();
    void scan_cancel_req();
    void connect_req_default();
    void state_req_all();
    void state_req();
    void statistics_req_all();
    void statistics_req();
    void addrinfo_req_all();
    void addrinfo_req();

private:
    void disconnect_req_default(); // this is currently not run
    void connect_req_specific();   // this is currently not run
    QProcess *icd_stub;
    bool connect_iap(Maemo::IcdConnectResult &connect_result,
		     QString &result,
		     QString &error,
		     QString iap=QString());
};


QString create_error_str(Maemo::Icd &icd)
{
    return icd.error();
}


void Ut_MaemoIcd::init()
{
    icd_stub = new QProcess(this);
    icd_stub->setStandardOutputFile("/tmp/ut_maemo_icd.log");
    icd_stub->start("/usr/bin/icd2_stub.py");
    QTest::qWait(1000);

    // Set the statistics
    QProcess dbus_send;
    dbus_send.start("dbus-send --type=method_call --system "
		    "--dest=com.nokia.icd2 /com/nokia/icd2 "
		    "com.nokia.icd2.testing.set_statistics "
		    "uint32:1024 uint32:256");
    dbus_send.waitForFinished();

}

void Ut_MaemoIcd::cleanup()
{
    icd_stub->terminate();
    icd_stub->waitForFinished();
}

void Ut_MaemoIcd::initTestCase()
{
} 

void Ut_MaemoIcd::cleanupTestCase()
{
}

void Ut_MaemoIcd::scan_req()
{    
    QList<Maemo::IcdScanResult> scanned;
    QStringList scannedNetworkTypes;
    QStringList networkTypesToScan;
    QString error;
    Maemo::Icd icd(ICD_SHORT_SCAN_TIMEOUT);

    scannedNetworkTypes = icd.scan(ICD_SCAN_REQUEST_ACTIVE,
				   networkTypesToScan,
				   scanned,
				   error);
    QVERIFY(error.isEmpty());
    QCOMPARE(scanned.size(), 3);
    QVERIFY(scannedNetworkTypes[0] == "WLAN_INFRA");
    QVERIFY(scannedNetworkTypes[1] == "DUN_GSM_PS");
}

void Ut_MaemoIcd::scan_cancel_req()
{
    Maemo::Icd icd;
    icd.scanCancel();
    // Not much to verify here
}

bool Ut_MaemoIcd::connect_iap(Maemo::IcdConnectResult &connect_result,
			      QString &result,
			      QString &error,
			      QString iap)
{
    icd_connection_flags flags = ICD_CONNECTION_FLAG_USER_EVENT;
    bool st;
    Maemo::Icd icd(ICD_SHORT_CONNECT_TIMEOUT);

    if (iap.isEmpty()) {
      qDebug() << "connecting to default IAP";
      st = icd.connect(flags, connect_result);
    } else {
      qDebug() << "connecting to" << iap;
      st = icd.connect(flags, iap, result);
    }

    error = create_error_str(icd);
    return st;
}

void Ut_MaemoIcd::connect_req_default()
{
    Maemo::IcdConnectResult connect_result;
    QString result, error;
    bool st;
    st = connect_iap(connect_result, result, error);
    QVERIFY2(st, error.toAscii().data());
    result = connect_result.connect.network_id.data();
    qDebug() << result;
}


void Ut_MaemoIcd::disconnect_req_default()
{
    icd_connection_flags flags = ICD_CONNECTION_FLAG_USER_EVENT;
    bool st;
    Maemo::Icd icd(ICD_SHORT_CONNECT_TIMEOUT);
    icd.disconnect(flags);
}


void Ut_MaemoIcd::connect_req_specific()
{
    Maemo::IcdConnectResult connect_result;
    QString result;
    QString error;
    bool st;
    st = connect_iap(connect_result, result, error, "wpa2gx2.osso.net");
    QVERIFY2(st, error.toAscii().data());
    qDebug() << result;
}


void Ut_MaemoIcd::state_req_all()
{
    QList<Maemo::IcdStateResult> state_results;
    Maemo::Icd icd;
    int sig;
    sig = icd.state(state_results);
    QVERIFY2(sig==1, create_error_str(icd).toAscii().data());
}


void Ut_MaemoIcd::state_req()
{
    Maemo::IcdStateResult state_result;
    Maemo::Icd icd;
    int sig;
    QString service_type, service_id;
    QString network_type("WLAN_INFRA");
    QByteArray network_id("wpa2gx2.osso.net");
    sig = icd.state(service_type, 0, service_id,
		    network_type, (uint)0x17a1, network_id,
		    state_result);
    QVERIFY2(sig==1, create_error_str(icd).toAscii().data());
}


void Ut_MaemoIcd::statistics_req_all()
{
    QList<Maemo::IcdStatisticsResult> stats_results;
    Maemo::Icd icd;
    int sig;
    QString err;
    sig = icd.statistics(stats_results);
    err = create_error_str(icd);
    if (!err.isEmpty())
      QVERIFY2(sig==1, err.toAscii().data());
    else
      QCOMPARE(sig, 1);

    for(int i=0; i<sig; i++) {
        QVERIFY(stats_results[i].bytes_received == 1024);
        QVERIFY(stats_results[i].bytes_sent == 256);
    }
}


// Can be uncommented when needed function is enabled in Icd class
void Ut_MaemoIcd::statistics_req()
{
    Maemo::IcdStatisticsResult stats_result;
    Maemo::Icd icd;
    int sig;
    QString service_type, service_id;
    QString network_type("WLAN_INFRA");
    QByteArray network_id("wpa2gx2.osso.net");
    sig = icd.statistics(service_type, 0, service_id,
			 network_type, (uint)0x17a1, network_id,
			 stats_result);
    QVERIFY2(sig==1, create_error_str(icd).toAscii().data());
    QVERIFY(stats_result.bytes_received == 1024);
    QVERIFY(stats_result.bytes_sent == 256);
}


void Ut_MaemoIcd::addrinfo_req_all()
{
    QList<Maemo::IcdAddressInfoResult> addr_results;
    Maemo::Icd icd;
    int sig;
    sig = icd.addrinfo(addr_results);
    QVERIFY2(sig==1, create_error_str(icd).toAscii().data());
}


void Ut_MaemoIcd::addrinfo_req()
{
    Maemo::IcdAddressInfoResult addr_result;
    Maemo::Icd icd;
    int sig;
    QString service_type, service_id;
    QString network_type("WLAN_INFRA");
    QByteArray network_id("wpa2gx2.osso.net");
    sig = icd.addrinfo(service_type, 0, service_id,
			 network_type, (uint)0x17a1, network_id,
			 addr_result);
    QVERIFY2(sig==1, create_error_str(icd).toAscii().data());
}


QTEST_MAIN(Ut_MaemoIcd)

#include "ut_maemo_icd.moc"

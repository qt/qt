/* * This file is part of conn-dui-settings-inet *
 *
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 * Contact: Aapo Makela <aapo.makela@nokia.com>
 *
 * This software, including documentation, is protected by copyright
 * controlled by Nokia Corporation. All rights are reserved. Copying,
 * including reproducing, storing, adapting or translating, any or all of
 * this material requires the prior written consent of Nokia Corporation.
 * This material also contains confidential information which may not be
 * disclosed to others without the prior written consent of Nokia.
 */

#include <QtTest/QtTest>
#include <QCoreApplication>
#include <QEventLoop>
#include <QDebug>
#include <icd/dbus_api.h>

#include <dbusdispatcher.h>

class Ut_DBusDispatcher : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void init();
    void cleanup();
    void initTestCase();
    void cleanupTestCase();

    void simpleSignalReceived(const QString& interface, 
                              const QString& signal,
                              const QList<QVariant>& args);
    void simpleCallReply(const QString& method, 
                         const QList<QVariant>& args,
                         const QString& error);
    void simpleCall();

    void complexCallReply(const QString& method, 
                          const QList<QVariant>& args,
                          const QString& error);
    void complexCall();

private:
    Maemo::DBusDispatcher *mSubject;
    QProcess *icd_stub;

    QString mMethod;
    QString mInterface;
    QString mSignal;
    QList<QVariant> mArgs;
};

void Ut_DBusDispatcher::init()
{
    mSubject = new Maemo::DBusDispatcher("com.nokia.icd2", "/com/nokia/icd2",
					 "com.nokia.icd2");

    // Start icd2 stub
    icd_stub = new QProcess(this);
    icd_stub->start("/usr/bin/icd2_stub.py");
    QTest::qWait(1000);
}

void Ut_DBusDispatcher::cleanup()
{
    // Terminate icd2 stub
    icd_stub->terminate();
    icd_stub->waitForFinished();

    delete mSubject;
    mSubject = 0;
}

void Ut_DBusDispatcher::initTestCase()
{
} 

void Ut_DBusDispatcher::cleanupTestCase()
{
}

void Ut_DBusDispatcher::simpleSignalReceived(const QString& interface, 
                                             const QString& signal,
                                             const QList<QVariant>& args)
{
    // Signal handler, which simply records what has been signalled
    mInterface = interface;
    mSignal = signal;
    mArgs = args;
}

void Ut_DBusDispatcher::simpleCallReply(const QString& method, 
                                        const QList<QVariant>& args,
                                        const QString& error)
{
    mMethod = method;

    // Check that method matches and at least WLAN_INFRA is returned
    QVERIFY(error.isEmpty());
    QVERIFY(args[0].toStringList().contains("WLAN_INFRA"));
}

void Ut_DBusDispatcher::simpleCall()
{
    uint flags = 0;
    QList<QVariant> reply;
    int idx = 0;
    QTimer timer;

    // Connect signals
    connect(mSubject, SIGNAL(signalReceived(const QString&, 
                                            const QString&,
                                            const QList<QVariant>&)),
            this, SLOT(simpleSignalReceived(const QString&, 
                                      const QString&,
                                      const QList<QVariant>&)));
    connect(mSubject, SIGNAL(callReply(const QString&,
                                       const QList<QVariant>&,
                                       const QString&)),
            this, SLOT(simpleCallReply(const QString&,
                                 const QList<QVariant>&,
                                 const QString&)));

    // Request scan and verify the call starts succesfully
    QVERIFY(mSubject->callAsynchronous("scan_req", flags));

    // Wait 1st scan signal for 10 secs
    timer.setSingleShot(true);
    timer.start(10000);
    while (timer.isActive() && mInterface.isEmpty()) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
        idx++;
    }
    timer.stop();

    // Sanity checks for the scan result
    QVERIFY(mInterface == "com.nokia.icd2");    // interface is icd2
    QVERIFY(mMethod == "scan_req");             // method is scan_req
    QVERIFY(mSignal == "scan_result_sig");      // signal is scan result
    QVERIFY(mArgs[0] == QVariant(0) ||
	    mArgs[0] == QVariant(4));           // First argument is status
                                                // (0 == NEW, 4 == COMPLETED)
    //QVERIFY(mArgs.contains(QVariant("WLAN_INFRA"))); // WLAN scan result
}

void Ut_DBusDispatcher::complexCallReply(const QString& method, 
                                         const QList<QVariant>& args,
                                         const QString& error)
{
    mMethod = method;

    // Check that method has not return arguments and error is not set
    QVERIFY(error.isEmpty());
    QVERIFY(args.isEmpty());
}

void Ut_DBusDispatcher::complexCall()
{    
    uint flags = ICD_CONNECTION_FLAG_UI_EVENT;
    QList<QVariant> reply;
    QVariantList networks;
    QVariantList network1;

    network1 << "" << (uint)0 << "" << "WLAN_INFRA" << (uint)0x05000011 << QByteArray("osso@46@net");
    networks << QVariant(network1);

    // Connect signal
    connect(mSubject, SIGNAL(callReply(const QString&,
                                       const QList<QVariant>&,
                                       const QString&)),
            this, SLOT(complexCallReply(const QString&,
                                        const QList<QVariant>&,
                                        const QString&)));

    // Request connect and verify the call starts succesfully
    QVERIFY(mSubject->callAsynchronous("connect_req", flags, networks));

    QTest::qWait(1000);

    // Sanity checks for the scan result
    QVERIFY(mInterface == "com.nokia.icd2");    // interface is icd2
    QVERIFY(mMethod == "connect_req");          // method connect_req
}

QTEST_MAIN(Ut_DBusDispatcher)

#include "ut_dbusdispatcher.moc"

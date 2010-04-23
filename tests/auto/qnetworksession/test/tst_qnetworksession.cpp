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
#include <QLocalServer>
#include <QLocalSocket>
#include <QTimer>
#include "../../qbearertestcommon.h"

#include <QtNetwork/qnetworkconfigmanager.h>
#include <QtNetwork/qnetworksession.h>

#if defined(Q_WS_MAEMO_6) || defined(Q_WS_MAEMO_5)
#include <stdio.h>
#include <iapconf.h>
#endif

QT_USE_NAMESPACE

// Can be used to configure tests that require manual attention (such as roaming)
//#define QNETWORKSESSION_MANUAL_TESTS 1

Q_DECLARE_METATYPE(QNetworkConfiguration)
Q_DECLARE_METATYPE(QNetworkConfiguration::Type);
Q_DECLARE_METATYPE(QNetworkSession::State);
Q_DECLARE_METATYPE(QNetworkSession::SessionError);

class tst_QNetworkSession : public QObject
{
    Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();

private slots:

    void robustnessBombing();

    void outOfProcessSession();
    void invalidSession();

    void repeatedOpenClose_data();
    void repeatedOpenClose();
    
    void roamingErrorCodes();
    
    void sessionStop_data();
    void sessionStop();

    void sessionProperties_data();
    void sessionProperties();

    void userChoiceSession_data();
    void userChoiceSession();

    void sessionOpenCloseStop_data();
    void sessionOpenCloseStop();

    void sessionAutoClose_data();
    void sessionAutoClose();

private:
    QNetworkConfigurationManager manager;

    int inProcessSessionManagementCount;

#if defined(Q_WS_MAEMO_6) || defined(Q_WS_MAEMO_5)
    Maemo::IAPConf *iapconf;
    Maemo::IAPConf *iapconf2;
    Maemo::IAPConf *gprsiap;
#define MAX_IAPS 10
    Maemo::IAPConf *iaps[MAX_IAPS];
    QProcess *icd_stub;
#endif
};

// Helper functions
bool openSession(QNetworkSession *session);
bool closeSession(QNetworkSession *session, bool lastSessionOnConfiguration = true);
void updateConfigurations();
QNetworkConfiguration suitableConfiguration(QString bearerType, QNetworkConfiguration::Type configType);

void tst_QNetworkSession::initTestCase()
{
    qRegisterMetaType<QNetworkSession::State>("QNetworkSession::State");
    qRegisterMetaType<QNetworkSession::SessionError>("QNetworkSession::SessionError");
    qRegisterMetaType<QNetworkConfiguration>("QNetworkConfiguration");
    qRegisterMetaType<QNetworkConfiguration::Type>("QNetworkConfiguration::Type");

#if defined(Q_WS_MAEMO_6) || defined(Q_WS_MAEMO_5)
    iapconf = new Maemo::IAPConf("007");
    iapconf->setValue("ipv4_type", "AUTO");
    iapconf->setValue("wlan_wepkey1", "connt");
    iapconf->setValue("wlan_wepdefkey", 1);
    iapconf->setValue("wlan_ssid", QByteArray("JamesBond"));
    iapconf->setValue("name", "James Bond");
    iapconf->setValue("type", "WLAN_INFRA");

    gprsiap = new Maemo::IAPConf("This-is-GPRS-IAP");
    gprsiap->setValue("ask_password", false);
    gprsiap->setValue("gprs_accesspointname", "internet");
    gprsiap->setValue("gprs_password", "");
    gprsiap->setValue("gprs_username", "");
    gprsiap->setValue("ipv4_autodns", true);
    gprsiap->setValue("ipv4_type", "AUTO");
    gprsiap->setValue("sim_imsi", "244070123456789");
    gprsiap->setValue("name", "MI6");
    gprsiap->setValue("type", "GPRS");

    iapconf2 = new Maemo::IAPConf("osso.net");
    iapconf2->setValue("ipv4_type", "AUTO");
    iapconf2->setValue("wlan_wepkey1", "osso.net");
    iapconf2->setValue("wlan_wepdefkey", 1);
    iapconf2->setValue("wlan_ssid", QByteArray("osso.net"));
    iapconf2->setValue("name", "osso.net");
    iapconf2->setValue("type", "WLAN_INFRA");
    iapconf2->setValue("wlan_security", "WEP");

    /* Create large number of IAPs in the gconf and see what happens */
    fflush(stdout);
    printf("Creating %d IAPS: ", MAX_IAPS);
    for (int i=0; i<MAX_IAPS; i++) {
	QString num = QString().sprintf("%d", i);
	QString iap = "iap-" + num;
	iaps[i] = new Maemo::IAPConf(iap);
	iaps[i]->setValue("name", QString("test-iap-")+num);
	iaps[i]->setValue("type", "WLAN_INFRA");
	iaps[i]->setValue("wlan_ssid", QString(QString("test-ssid-")+num).toAscii());
	iaps[i]->setValue("wlan_security", "WPA_PSK");
	iaps[i]->setValue("EAP_wpa_preshared_passphrase", QString("test-passphrase-")+num);
	printf(".");
	fflush(stdout);
    }
    printf("\n");
    fflush(stdout);

    icd_stub = new QProcess(this);
    icd_stub->start("/usr/bin/icd2_stub.py");
    QTest::qWait(1000);

    // Add a known network to scan list that icd2 stub returns
    QProcess dbus_send;
    // 007 network
    dbus_send.start("dbus-send --type=method_call --system "
		    "--dest=com.nokia.icd2 /com/nokia/icd2 "
		    "com.nokia.icd2.testing.add_available_network "
		    "string:'' uint32:0 string:'' "
		    "string:WLAN_INFRA uint32:5000011 array:byte:48,48,55");
    dbus_send.waitForFinished();

    // osso.net network
    dbus_send.start("dbus-send --type=method_call --system "
		    "--dest=com.nokia.icd2 /com/nokia/icd2 "
		    "com.nokia.icd2.testing.add_available_network "
		    "string:'' uint32:0 string:'' "
		    "string:WLAN_INFRA uint32:83886097 array:byte:111,115,115,111,46,110,101,116");
    dbus_send.waitForFinished();
#endif

    inProcessSessionManagementCount = -1;

    QSignalSpy spy(&manager, SIGNAL(updateCompleted()));
    manager.updateConfigurations();
    QTRY_VERIFY(spy.count() == 1);
}

void tst_QNetworkSession::cleanupTestCase()
{
    if (!(manager.capabilities() & QNetworkConfigurationManager::SystemSessionSupport) &&
        (manager.capabilities() & QNetworkConfigurationManager::CanStartAndStopInterfaces) &&
        inProcessSessionManagementCount == 0) {
        qWarning("No usable configurations found to complete all possible tests in "
                 "inProcessSessionManagement()");
    }

#if defined(Q_WS_MAEMO_6) || defined(Q_WS_MAEMO_5)
    iapconf->clear();
    delete iapconf;
    iapconf2->clear();
    delete iapconf2;
    gprsiap->clear();
    delete gprsiap;

    printf("Deleting %d IAPS : ", MAX_IAPS);
    for (int i=0; i<MAX_IAPS; i++) {
	iaps[i]->clear();
	delete iaps[i];
	printf(".");
	fflush(stdout);
    }
    printf("\n");
    qDebug() << "Deleted" << MAX_IAPS << "IAPs";

    icd_stub->terminate();
    icd_stub->waitForFinished();
#endif
}

// Robustness test for calling interfaces in nonsense order / with nonsense parameters
void tst_QNetworkSession::robustnessBombing() 
{
    QNetworkConfigurationManager mgr;
    QNetworkSession testSession(mgr.defaultConfiguration());
    // Should not reset even session is not opened
    testSession.migrate();
    testSession.accept();
    testSession.ignore();
    testSession.reject();
    quint64 temp;
    temp = testSession.bytesWritten();
    temp = testSession.bytesReceived();
    temp = testSession.activeTime();
}


void tst_QNetworkSession::invalidSession()
{
    // 1. Verify that session created with invalid configuration remains in invalid state
    QNetworkSession session(QNetworkConfiguration(), 0);
    QVERIFY(!session.isOpen());
    QVERIFY(session.state() == QNetworkSession::Invalid);
    QVERIFY(session.error() == QNetworkSession::InvalidConfigurationError);
    
    // 2. Verify that opening session with invalid configuration both 1) emits invalidconfigurationerror and 2) sets session's state as invalid.
    QSignalSpy errorSpy(&session, SIGNAL(error(QNetworkSession::SessionError)));
    session.open();
    session.waitForOpened(1000); // Should bail out right away
    QVERIFY(errorSpy.count() == 1); 
    QNetworkSession::SessionError error =
           qvariant_cast<QNetworkSession::SessionError> (errorSpy.first().at(0));
    QVERIFY(error == QNetworkSession::InvalidConfigurationError);
    QVERIFY(session.error() == QNetworkSession::InvalidConfigurationError);
    QVERIFY(session.state() == QNetworkSession::Invalid);

#ifdef QNETWORKSESSION_MANUAL_TESTS
    QNetworkConfiguration definedConfig = suitableConfiguration("WLAN",QNetworkConfiguration::InternetAccessPoint);
    if (definedConfig.isValid()) {
        // 3. Verify that opening a session with defined configuration emits error and enters notavailable-state
        // TODO these timer waits should be changed to waiting appropriate signals, now these wait excessively
        qDebug() << "Shutdown WLAN IAP (waiting 60 seconds): " << definedConfig.name();
        QTest::qWait(60000);
        // Shutting down WLAN should bring back to defined -state.
        QVERIFY((definedConfig.state() & QNetworkConfiguration::Defined) == QNetworkConfiguration::Defined);
        QNetworkSession definedSession(definedConfig);
        QSignalSpy errorSpy(&definedSession, SIGNAL(error(QNetworkSession::SessionError)));
        QNetworkSession::SessionError sessionError;

        definedSession.open();

        QVERIFY(definedConfig.isValid()); // Session remains valid
        QVERIFY(definedSession.state() == QNetworkSession::NotAvailable); // State is not available because WLAN is not in coverage
        QVERIFY(!errorSpy.isEmpty()); // Session tells with error about invalidated configuration
        sessionError = qvariant_cast<QNetworkSession::SessionError> (errorSpy.first().at(0));
        qDebug() << "Error code is: " << sessionError;
        QVERIFY(sessionError == QNetworkSession::InvalidConfigurationError);
        
        qDebug() << "Turn the WLAN IAP back on (waiting 60 seconds): " << definedConfig.name();
        QTest::qWait(60000);
        updateConfigurations();
        
        QVERIFY(definedConfig.state() == QNetworkConfiguration::Discovered);
    }
        
    QNetworkConfiguration invalidatedConfig = suitableConfiguration("WLAN",QNetworkConfiguration::InternetAccessPoint);
    if (invalidatedConfig.isValid()) {
        // 4. Verify that invalidating a session after its successfully configured works
        QNetworkSession invalidatedSession(invalidatedConfig);
        QSignalSpy errorSpy(&invalidatedSession, SIGNAL(error(QNetworkSession::SessionError)));
        QNetworkSession::SessionError sessionError;
        
        qDebug() << "Delete the WLAN IAP from phone now (waiting 60 seconds): " << invalidatedConfig.name();
        QTest::qWait(60000);
        
        invalidatedSession.open();
        QVERIFY(!invalidatedConfig.isValid());
        QVERIFY(invalidatedSession.state() == QNetworkSession::Invalid);
        QVERIFY(!errorSpy.isEmpty());
        
        sessionError = qvariant_cast<QNetworkSession::SessionError> (errorSpy.first().at(0));
        QVERIFY(sessionError == QNetworkSession::InvalidConfigurationError);
        qDebug() << "Add the WLAN IAP back (waiting 60 seconds): " << invalidatedConfig.name();
        QTest::qWait(60000);
    }
#endif
}

void tst_QNetworkSession::sessionProperties_data()
{
    QTest::addColumn<QNetworkConfiguration>("configuration");

    QTest::newRow("invalid configuration") << QNetworkConfiguration();

    foreach (const QNetworkConfiguration &config, manager.allConfigurations()) {
        const QString name = config.name().isEmpty() ? QString("<Hidden>") : config.name();
        QTest::newRow(name.toLocal8Bit().constData()) << config;
    }
}

void tst_QNetworkSession::sessionProperties()
{
    QFETCH(QNetworkConfiguration, configuration);

    QNetworkSession session(configuration);

    QVERIFY(session.configuration() == configuration);

    QStringList validBearerNames = QStringList() << QLatin1String("Unknown")
                                                 << QLatin1String("Ethernet")
                                                 << QLatin1String("WLAN")
                                                 << QLatin1String("2G")
                                                 << QLatin1String("CDMA2000")
                                                 << QLatin1String("WCDMA")
                                                 << QLatin1String("HSPA")
                                                 << QLatin1String("Bluetooth")
                                                 << QLatin1String("WiMAX");

    if (!configuration.isValid()) {
        QVERIFY(configuration.bearerName().isEmpty());
    } else {
        qDebug() << "Type:" << configuration.type()
                 << "Bearer:" << configuration.bearerName();

        switch (configuration.type())
        {
            case QNetworkConfiguration::ServiceNetwork:
            case QNetworkConfiguration::UserChoice:
            default:
                QVERIFY(configuration.bearerName().isEmpty());
                break;
            case QNetworkConfiguration::InternetAccessPoint:
                QVERIFY(validBearerNames.contains(configuration.bearerName()));
                break;
        }
    }

    // QNetworkSession::interface() should return an invalid interface unless
    // session is in the connected state.
    qDebug() << "Session state:" << session.state();
#ifndef QT_NO_NETWORKINTERFACE
    qDebug() << "Session iface:" << session.interface().isValid() << session.interface().name();
#if !(defined(Q_OS_SYMBIAN) && defined(__WINS__))
    // On Symbian emulator, the support for data bearers is limited
    QCOMPARE(session.state() == QNetworkSession::Connected, session.interface().isValid());
#endif
#endif

    if (!configuration.isValid()) {
        QVERIFY(configuration.state() == QNetworkConfiguration::Undefined &&
                session.state() == QNetworkSession::Invalid);
    } else {
        switch (configuration.state()) {
        case QNetworkConfiguration::Undefined:
            QVERIFY(session.state() == QNetworkSession::NotAvailable);
            break;
        case QNetworkConfiguration::Defined:
            QVERIFY(session.state() == QNetworkSession::NotAvailable);
            break;
        case QNetworkConfiguration::Discovered:
            QVERIFY(session.state() == QNetworkSession::Connecting ||
                    session.state() == QNetworkSession::Disconnected);
            break;
        case QNetworkConfiguration::Active:
            QVERIFY(session.state() == QNetworkSession::Connected ||
                    session.state() == QNetworkSession::Closing ||
                    session.state() == QNetworkSession::Roaming);
            break;
        default:
            QFAIL("Invalid configuration state");
        };
    }
}

void tst_QNetworkSession::repeatedOpenClose_data() {
    QTest::addColumn<QString>("bearerType");
    QTest::addColumn<QNetworkConfiguration::Type>("configurationType");
    QTest::addColumn<int>("repeatTimes");

    QTest::newRow("WLAN_IAP") << "WLAN" << QNetworkConfiguration::InternetAccessPoint << 3;
    // QTest::newRow("Cellular_IAP") << "cellular" << QNetworkConfiguration::InternetAccessPoint << 3;
    // QTest::newRow("SNAP") << "bearer_type_not_relevant_with_SNAPs" << QNetworkConfiguration::ServiceNetwork << 3;
}

// Tests repeated-open close.
void tst_QNetworkSession::repeatedOpenClose() {
    QFETCH(QString, bearerType);
    QFETCH(QNetworkConfiguration::Type, configurationType);
    QFETCH(int, repeatTimes);

    // First check that opening once succeeds and determine if repeatable testing is doable
    QNetworkConfiguration config = suitableConfiguration(bearerType, configurationType);
    if (!config.isValid()) {
        QSKIP("No suitable configurations, skipping this round of repeated open-close test.", SkipSingle);
    }
    qDebug() << "Using following configuratio to repeatedly open and close: " << config.name();
    QNetworkSession permanentSession(config);
    if (!openSession(&permanentSession) || 
        !closeSession(&permanentSession)) {
        QSKIP("Unable to open/close session, skipping this round of repeated open-close test.", SkipSingle); 
    }
    for (int i = repeatTimes; i > 0; i--) { 
       QVERIFY(openSession(&permanentSession));
       QVERIFY(closeSession(&permanentSession));
    }
}

void tst_QNetworkSession::roamingErrorCodes() {
#ifndef Q_OS_SYMBIAN
    QSKIP("Roaming supported on Symbian.", SkipAll);
#else 
    QNetworkConfiguration wlanIapConfig = suitableConfiguration("WLAN", QNetworkConfiguration::InternetAccessPoint);
    if (!wlanIapConfig.isValid()) {
        QSKIP("No WLAN IAP accessible, skipping test.", SkipAll);
    }
    // Check that opening and closing two sessions on same config work gracefully:
    QNetworkSession iapSession(wlanIapConfig);
    QVERIFY(openSession(&iapSession));
    QNetworkSession adminIapSession(wlanIapConfig);
    QVERIFY(openSession(&adminIapSession));
    QVERIFY(closeSession(&iapSession, false)); // false == not a last session based on the configuration
    QVERIFY(closeSession(&adminIapSession));
    
    // Open configurations again, force close bearer and check that errors are emitted correctly
    // on the other session
    QVERIFY(openSession(&iapSession));
    QVERIFY(openSession(&adminIapSession));
    QSignalSpy errorSpy(&iapSession, SIGNAL(error(QNetworkSession::SessionError)));   
    adminIapSession.stop(); // requires NetworkControl capabilities
    QTRY_VERIFY(!errorSpy.isEmpty()); // wait for error signals
    QNetworkSession::SessionError error = qvariant_cast<QNetworkSession::SessionError>(errorSpy.first().at(0));
    QVERIFY(error == QNetworkSession::SessionAbortedError);
    QVERIFY(iapSession.state() == QNetworkSession::Disconnected);
    QVERIFY(adminIapSession.state() == QNetworkSession::Disconnected);
#endif // Q_OS_SYMBIAN
   
#ifdef QNETWORKSESSION_MANUAL_TESTS
     // Check for roaming error.
     // Case requires that you have controllable WLAN in Internet SNAP (only).
    QNetworkConfiguration snapConfig = suitableConfiguration("bearer_not_relevant_with_snaps", QNetworkConfiguration::ServiceNetwork);
    if (!snapConfig.isValid()) {
        QSKIP("No SNAP accessible, skipping test.", SkipAll);
    }
    QNetworkSession snapSession(snapConfig);
    QVERIFY(openSession(&snapSession));
    QSignalSpy errorSpySnap(&snapSession, SIGNAL(error(QNetworkSession::SessionError)));
    qDebug("Disconnect the WLAN now");
    QTRY_VERIFY(!errorSpySnap.isEmpty()); // wait for error signals
    QVERIFY(errorSpySnap.count() == 1);
    error = qvariant_cast<QNetworkSession::SessionError>(errorSpySnap.first().at(0));
    qDebug() << "Error received when turning off wlan on SNAP: " << error;
    QVERIFY(error == QNetworkSession::RoamingError);
    
    qDebug("Connect the WLAN now");
    QTest::qWait(60000); // Wait for WLAN to get up
    QNetworkConfiguration wlanIapConfig2 = suitableConfiguration("WLAN", QNetworkConfiguration::InternetAccessPoint);
    QNetworkSession iapSession2(wlanIapConfig2);
    QVERIFY(openSession(&iapSession2));
    QSignalSpy errorSpy2(&iapSession2, SIGNAL(error(QNetworkSession::SessionError)));
    qDebug("Disconnect the WLAN now");
    QTRY_VERIFY(!errorSpy2.isEmpty()); // wait for error signals
    QVERIFY(errorSpy2.count() == 1);
    error = qvariant_cast<QNetworkSession::SessionError>(errorSpy2.first().at(0));
    QVERIFY(error == QNetworkSession::SessionAbortedError);
    QVERIFY(iapSession2.state() == QNetworkSession::Disconnected);
#endif
}


void tst_QNetworkSession::sessionStop_data() {
    QTest::addColumn<QString>("bearerType");
    QTest::addColumn<QNetworkConfiguration::Type>("configurationType");

    QTest::newRow("SNAP") << "bearer_type_not_relevant_with_SNAPs" << QNetworkConfiguration::ServiceNetwork;
    QTest::newRow("WLAN_IAP") << "WLAN" << QNetworkConfiguration::InternetAccessPoint;
    QTest::newRow("Cellular_IAP") << "cellular" << QNetworkConfiguration::InternetAccessPoint;
}

void tst_QNetworkSession::sessionStop() 
{
#ifndef Q_OS_SYMBIAN
    QSKIP("Testcase contains mainly Symbian specific checks, because it is only platform to really support interface (IAP-level) Stop.", SkipAll);
#endif 
    QFETCH(QString, bearerType);
    QFETCH(QNetworkConfiguration::Type, configurationType);
    
    int configWaitdelayInMs = 2000;
    
    QNetworkConfiguration config = suitableConfiguration(bearerType, configurationType);
    if (!config.isValid()) {
        QSKIP("No suitable configurations, skipping this round of session stop test.", SkipSingle);
    }
    qDebug() << "Using following configuration to open and stop a session: " << config.name();

    QNetworkSession openedSession(config);
    QNetworkSession closedSession(config);
    QNetworkSession innocentSession(config);
    QNetworkConfigurationManager mgr;
    
    QSignalSpy closedSessionOpenedSpy(&closedSession, SIGNAL(opened()));
    QSignalSpy closedSessionClosedSpy(&closedSession, SIGNAL(closed()));
    QSignalSpy closedSessionStateChangedSpy(&closedSession, SIGNAL(stateChanged(QNetworkSession::State)));
    QSignalSpy closedErrorSpy(&closedSession, SIGNAL(error(QNetworkSession::SessionError)));
    
    QSignalSpy innocentSessionClosedSpy(&innocentSession, SIGNAL(closed()));
    QSignalSpy innocentSessionStateChangedSpy(&innocentSession, SIGNAL(stateChanged(QNetworkSession::State)));
    QSignalSpy innocentErrorSpy(&innocentSession, SIGNAL(error(QNetworkSession::SessionError)));
    QNetworkSession::SessionError sessionError;
    
    // 1. Verify that stopping an opened session works (the simplest usecase).
    qDebug("----------1. Verify that stopping an opened session works (the simplest usecase)");
    QSignalSpy configChangeSpy(&mgr, SIGNAL(configurationChanged(QNetworkConfiguration)));
    QVERIFY(openSession(&openedSession));
    qDebug("Waiting for %d ms to get all configurationChange signals from platform.", configWaitdelayInMs);
    // Clear signals caused by opening
    closedSessionOpenedSpy.clear();
    closedSessionClosedSpy.clear();
    closedSessionStateChangedSpy.clear();
    closedErrorSpy.clear();
    openedSession.stop();

    QVERIFY(openedSession.state() == QNetworkSession::Disconnected);
    QTest::qWait(configWaitdelayInMs); // Wait to get all relevant configurationChange() signals
    QVERIFY(config.state() != QNetworkConfiguration::Active);
    
    // 2. Verify that stopping a session based on non-connected configuration does nothing
    qDebug("----------2. Verify that stopping a session based on non-connected configuration does nothing");
    QNetworkSession::State closedSessionOriginalState = closedSession.state();
    // Clear all possible signals
    configChangeSpy.clear();
    closedSessionOpenedSpy.clear();
    closedSessionClosedSpy.clear();
    closedSessionStateChangedSpy.clear();
    closedErrorSpy.clear();
    
    closedSession.stop();
    qDebug("Waiting for %d ms to get all configurationChange signals from platform.", configWaitdelayInMs);
    QTest::qWait(configWaitdelayInMs); // Wait to get all relevant configurationChange() signals
    
    QVERIFY(closedSessionOpenedSpy.isEmpty());
    QVERIFY(closedSessionClosedSpy.isEmpty());
    QVERIFY(closedSessionStateChangedSpy.isEmpty());
    QVERIFY(closedErrorSpy.isEmpty());
    QVERIFY(closedSession.state() == closedSessionOriginalState); // State remains
    
    // 3. Check that stopping a opened session affects also other opened session based on the same configuration.
    if (config.type() == QNetworkConfiguration::InternetAccessPoint) {
        qDebug("----------3. Check that stopping a opened session affects also other opened session based on the same configuration.");
        QVERIFY(openSession(&openedSession));
        QVERIFY(openSession(&innocentSession));

        configChangeSpy.clear();
        innocentSessionClosedSpy.clear();
        innocentSessionStateChangedSpy.clear();
        innocentErrorSpy.clear();
      
        openedSession.stop();    
        qDebug("Waiting for %d ms to get all configurationChange signals from platform.", configWaitdelayInMs);
        QTest::qWait(configWaitdelayInMs); // Wait to get all relevant configurationChange() signals

        QVERIFY(!innocentSessionClosedSpy.isEmpty());
        QVERIFY(!innocentSessionStateChangedSpy.isEmpty());
        QVERIFY(!innocentErrorSpy.isEmpty());
        QVERIFY(innocentSession.state() == QNetworkSession::Disconnected);
        QVERIFY(openedSession.state() == QNetworkSession::Disconnected);
        sessionError = qvariant_cast<QNetworkSession::SessionError>(innocentErrorSpy.first().at(0));
        QVERIFY(sessionError == QNetworkSession::SessionAbortedError);

        innocentSessionClosedSpy.clear();
        innocentSessionStateChangedSpy.clear();
        innocentErrorSpy.clear();
    } else {
        qDebug("----------3. Skip for SNAP configuration.");
    }
    // 4. Check that stopping a non-opened session stops the other session based on the 
    // same configuration if configuration is IAP. Stopping closed SNAP session has no impact on other opened SNAP session.
    if (config.type() == QNetworkConfiguration::ServiceNetwork) {
        qDebug("----------4. Skip for SNAP configuration.");
    } else if (config.type() == QNetworkConfiguration::InternetAccessPoint) {
        qDebug("----------4. Check that stopping a non-opened session stops the other session based on the same configuration");
        QVERIFY(openSession(&innocentSession));
        qDebug("Waiting for %d ms after open to make sure all platform indications are propagated", configWaitdelayInMs);
        QTest::qWait(configWaitdelayInMs);
        closedSession.stop();
        qDebug("Waiting for %d ms to get all configurationChange signals from platform..", configWaitdelayInMs);
        QTest::qWait(configWaitdelayInMs); // Wait to get all relevant configurationChange() signals
    
        QVERIFY(!innocentSessionClosedSpy.isEmpty());
        QVERIFY(!innocentSessionStateChangedSpy.isEmpty());
        QVERIFY(!innocentErrorSpy.isEmpty());
        QVERIFY(innocentSession.state() == QNetworkSession::Disconnected);
        QVERIFY(closedSession.state() == QNetworkSession::Disconnected);
        sessionError = qvariant_cast<QNetworkSession::SessionError>(innocentErrorSpy.first().at(0));
        QVERIFY(sessionError == QNetworkSession::SessionAbortedError);
        QVERIFY(config.state() == QNetworkConfiguration::Discovered);
    }
    
    // 5. Sanity check that stopping invalid session does not crash
    qDebug("----------5. Sanity check that stopping invalid session does not crash");
    QNetworkSession invalidSession(QNetworkConfiguration(), 0);
    QVERIFY(invalidSession.state() == QNetworkSession::Invalid);
    invalidSession.stop();
    QVERIFY(invalidSession.state() == QNetworkSession::Invalid);
}

void tst_QNetworkSession::userChoiceSession_data()
{
    QTest::addColumn<QNetworkConfiguration>("configuration");

    QNetworkConfiguration config = manager.defaultConfiguration();
    if (config.type() == QNetworkConfiguration::UserChoice)
        QTest::newRow("UserChoice") << config;
    else
        QSKIP("Default configuration is not a UserChoice configuration.", SkipAll);
}

void tst_QNetworkSession::userChoiceSession()
{
    QFETCH(QNetworkConfiguration, configuration);

    QVERIFY(configuration.type() == QNetworkConfiguration::UserChoice);

    QNetworkSession session(configuration);

    // Check that configuration was really set
    QVERIFY(session.configuration() == configuration);

    QVERIFY(!session.isOpen());

    // Check that session is not active
    QVERIFY(session.sessionProperty("ActiveConfiguration").toString().isEmpty());

    // The remaining tests require the session to be not NotAvailable.
    if (session.state() == QNetworkSession::NotAvailable)
        QSKIP("Network is not available.", SkipSingle);

    QSignalSpy sessionOpenedSpy(&session, SIGNAL(opened()));
    QSignalSpy sessionClosedSpy(&session, SIGNAL(closed()));
    QSignalSpy stateChangedSpy(&session, SIGNAL(stateChanged(QNetworkSession::State)));
    QSignalSpy errorSpy(&session, SIGNAL(error(QNetworkSession::SessionError)));

    // Test opening the session.
    {
        bool expectStateChange = session.state() != QNetworkSession::Connected;

        session.open();

        session.waitForOpened();

        if (session.isOpen())
            QVERIFY(!sessionOpenedSpy.isEmpty() || !errorSpy.isEmpty());
        if (!errorSpy.isEmpty()) {
            QNetworkSession::SessionError error =
                qvariant_cast<QNetworkSession::SessionError>(errorSpy.first().at(0));
            if (error == QNetworkSession::OperationNotSupportedError) {
                // The session needed to bring up the interface,
                // but the operation is not supported.
                QSKIP("Configuration does not support open().", SkipSingle);
            } else if (error == QNetworkSession::InvalidConfigurationError) {
                // The session needed to bring up the interface, but it is not possible for the
                // specified configuration.
                if ((session.configuration().state() & QNetworkConfiguration::Discovered) ==
                    QNetworkConfiguration::Discovered) {
                    QFAIL("Failed to open session for Discovered configuration.");
                } else {
                    QSKIP("Cannot test session for non-Discovered configuration.", SkipSingle);
                }
            } else if (error == QNetworkSession::UnknownSessionError) {
                    QSKIP("Unknown session error.", SkipSingle);
            } else {
                QFAIL("Error opening session.");
            }
        } else if (!sessionOpenedSpy.isEmpty()) {
            QCOMPARE(sessionOpenedSpy.count(), 1);
            QVERIFY(sessionClosedSpy.isEmpty());
            QVERIFY(errorSpy.isEmpty());

            if (expectStateChange)
                QTRY_VERIFY(!stateChangedSpy.isEmpty());

            QVERIFY(session.state() == QNetworkSession::Connected);
#ifndef QT_NO_NETWORKINTERFACE
#if !(defined(Q_OS_SYMBIAN) && defined(__WINS__))
            // On Symbian emulator, the support for data bearers is limited
            QVERIFY(session.interface().isValid());
#endif
#endif

            const QString userChoiceIdentifier =
                session.sessionProperty("UserChoiceConfiguration").toString();

            QVERIFY(!userChoiceIdentifier.isEmpty());
            QVERIFY(userChoiceIdentifier != configuration.identifier());

            QNetworkConfiguration userChoiceConfiguration =
                manager.configurationFromIdentifier(userChoiceIdentifier);

            QVERIFY(userChoiceConfiguration.isValid());
            QVERIFY(userChoiceConfiguration.type() != QNetworkConfiguration::UserChoice);

            const QString testIdentifier("abc");
            //resetting UserChoiceConfiguration is ignored (read only property)
            session.setSessionProperty("UserChoiceConfiguration", testIdentifier);
            QVERIFY(session.sessionProperty("UserChoiceConfiguration").toString() != testIdentifier);

            const QString activeIdentifier =
                session.sessionProperty("ActiveConfiguration").toString();

            QVERIFY(!activeIdentifier.isEmpty());
            QVERIFY(activeIdentifier != configuration.identifier());

            QNetworkConfiguration activeConfiguration =
                manager.configurationFromIdentifier(activeIdentifier);

            QVERIFY(activeConfiguration.isValid());
            QVERIFY(activeConfiguration.type() == QNetworkConfiguration::InternetAccessPoint);
            
            //resetting ActiveConfiguration is ignored (read only property)
            session.setSessionProperty("ActiveConfiguration", testIdentifier);
            QVERIFY(session.sessionProperty("ActiveConfiguration").toString() != testIdentifier);

            if (userChoiceConfiguration.type() == QNetworkConfiguration::InternetAccessPoint) {
                QVERIFY(userChoiceConfiguration == activeConfiguration);
            } else {
                QVERIFY(userChoiceConfiguration.type() == QNetworkConfiguration::ServiceNetwork);
                QVERIFY(userChoiceConfiguration.children().contains(activeConfiguration));
            }
        } else {
            QFAIL("Timeout waiting for session to open.");
        }
    }
}

void tst_QNetworkSession::sessionOpenCloseStop_data()
{
    QTest::addColumn<QNetworkConfiguration>("configuration");
    QTest::addColumn<bool>("forceSessionStop");

    foreach (const QNetworkConfiguration &config, manager.allConfigurations()) {
        const QString name = config.name().isEmpty() ? QString("<Hidden>") : config.name();
        QTest::newRow((name + QLatin1String(" close")).toLocal8Bit().constData())
            << config << false;
        QTest::newRow((name + QLatin1String(" stop")).toLocal8Bit().constData())
            << config << true;
    }

    inProcessSessionManagementCount = 0;
}

void tst_QNetworkSession::sessionOpenCloseStop()
{
    QFETCH(QNetworkConfiguration, configuration);
    QFETCH(bool, forceSessionStop);

    QNetworkSession session(configuration);

    // Test initial state of the session.
    {
        QVERIFY(session.configuration() == configuration);
        QVERIFY(!session.isOpen());
        // session may be invalid if configuration is removed between when
        // sessionOpenCloseStop_data() is called and here.
        QVERIFY((configuration.isValid() && (session.state() != QNetworkSession::Invalid)) ||
                (!configuration.isValid() && (session.state() == QNetworkSession::Invalid)));
        QVERIFY(session.error() == QNetworkSession::UnknownSessionError);
    }

    // The remaining tests require the session to be not NotAvailable.
    if (session.state() == QNetworkSession::NotAvailable)
        QSKIP("Network is not available.", SkipSingle);

    QSignalSpy sessionOpenedSpy(&session, SIGNAL(opened()));
    QSignalSpy sessionClosedSpy(&session, SIGNAL(closed()));
    QSignalSpy stateChangedSpy(&session, SIGNAL(stateChanged(QNetworkSession::State)));
    QSignalSpy errorSpy(&session, SIGNAL(error(QNetworkSession::SessionError)));

    // Test opening the session.
    {
        QNetworkSession::State previousState = session.state();
        bool expectStateChange = previousState != QNetworkSession::Connected;

        session.open();

        session.waitForOpened();

        if (session.isOpen())
            QVERIFY(!sessionOpenedSpy.isEmpty() || !errorSpy.isEmpty());
        if (!errorSpy.isEmpty()) {
            QNetworkSession::SessionError error =
                qvariant_cast<QNetworkSession::SessionError>(errorSpy.first().at(0));

            QVERIFY(session.state() == previousState);

            if (error == QNetworkSession::OperationNotSupportedError) {
                // The session needed to bring up the interface,
                // but the operation is not supported.
                QSKIP("Configuration does not support open().", SkipSingle);
            } else if (error == QNetworkSession::InvalidConfigurationError) {
                // The session needed to bring up the interface, but it is not possible for the
                // specified configuration.
                if ((session.configuration().state() & QNetworkConfiguration::Discovered) ==
                    QNetworkConfiguration::Discovered) {
                    QFAIL("Failed to open session for Discovered configuration.");
                } else {
                    QSKIP("Cannot test session for non-Discovered configuration.", SkipSingle);
                }
            } else if (error == QNetworkSession::UnknownSessionError) {
                    QSKIP("Unknown Session error.", SkipSingle);
            } else {
                QFAIL("Error opening session.");
            }
        } else if (!sessionOpenedSpy.isEmpty()) {
            QCOMPARE(sessionOpenedSpy.count(), 1);
            QVERIFY(sessionClosedSpy.isEmpty());
            QVERIFY(errorSpy.isEmpty());

            if (expectStateChange) {
                QTRY_VERIFY(stateChangedSpy.count() >= 2);

                QNetworkSession::State state =
                    qvariant_cast<QNetworkSession::State>(stateChangedSpy.at(0).at(0));
                QVERIFY(state == QNetworkSession::Connecting);

                state = qvariant_cast<QNetworkSession::State>(stateChangedSpy.at(1).at(0));
                QVERIFY(state == QNetworkSession::Connected);
            }

            QVERIFY(session.state() == QNetworkSession::Connected);
#ifndef QT_NO_NETWORKINTERFACE
#if !(defined(Q_OS_SYMBIAN) && defined(__WINS__))
            // On Symbian emulator, the support for data bearers is limited
            QVERIFY(session.interface().isValid());
#endif
#endif
        } else {
            QFAIL("Timeout waiting for session to open.");
        }
    }

    sessionOpenedSpy.clear();
    sessionClosedSpy.clear();
    stateChangedSpy.clear();
    errorSpy.clear();

    QNetworkSession session2(configuration);

    QSignalSpy sessionOpenedSpy2(&session2, SIGNAL(opened()));
    QSignalSpy sessionClosedSpy2(&session2, SIGNAL(closed()));
    QSignalSpy stateChangedSpy2(&session2, SIGNAL(stateChanged(QNetworkSession::State)));
    QSignalSpy errorSpy2(&session2, SIGNAL(error(QNetworkSession::SessionError)));

    // Test opening a second session.
    {
        QVERIFY(session2.configuration() == configuration);
        QVERIFY(!session2.isOpen());
        QVERIFY(session2.state() == QNetworkSession::Connected);
        QVERIFY(session.error() == QNetworkSession::UnknownSessionError);

        session2.open();

        QTRY_VERIFY(!sessionOpenedSpy2.isEmpty() || !errorSpy2.isEmpty());

        QVERIFY(session.isOpen());
        QVERIFY(session2.isOpen());
        QVERIFY(session.state() == QNetworkSession::Connected);
        QVERIFY(session2.state() == QNetworkSession::Connected);
#ifndef QT_NO_NETWORKINTERFACE
#if !(defined(Q_OS_SYMBIAN) && defined(__WINS__))
        // On Symbian emulator, the support for data bearers is limited
        QVERIFY(session.interface().isValid());
#endif
        QCOMPARE(session.interface().hardwareAddress(), session2.interface().hardwareAddress());
        QCOMPARE(session.interface().index(), session2.interface().index());
#endif
    }

    sessionOpenedSpy2.clear();

    if (forceSessionStop) {
        // Test forcing the second session to stop the interface.
        QNetworkSession::State previousState = session.state();
#ifdef Q_CC_NOKIAX86
        // For S60 emulator builds: RConnection::Stop does not work on all Emulators
        bool expectStateChange = false;
#else
        bool expectStateChange = previousState != QNetworkSession::Disconnected;
#endif

        session2.stop();

        QTRY_VERIFY(!sessionClosedSpy2.isEmpty() || !errorSpy2.isEmpty());

        QVERIFY(!session2.isOpen());

        if (!errorSpy2.isEmpty()) {
	    QVERIFY(!errorSpy.isEmpty());

            // check for SessionAbortedError
            QNetworkSession::SessionError error =
                qvariant_cast<QNetworkSession::SessionError>(errorSpy.first().at(0));
            QNetworkSession::SessionError error2 =
                qvariant_cast<QNetworkSession::SessionError>(errorSpy2.first().at(0));

            QVERIFY(error == QNetworkSession::SessionAbortedError);
            QVERIFY(error2 == QNetworkSession::SessionAbortedError);

            QCOMPARE(errorSpy.count(), 1);
            QCOMPARE(errorSpy2.count(), 1);

            errorSpy.clear();
            errorSpy2.clear();
        }

        QVERIFY(errorSpy.isEmpty());
        QVERIFY(errorSpy2.isEmpty());

        if (expectStateChange)
            QTRY_VERIFY(stateChangedSpy2.count() >= 2 || !errorSpy2.isEmpty());

        if (!errorSpy2.isEmpty()) {
            QVERIFY(session2.state() == previousState);
            QVERIFY(session.state() == previousState);

            QNetworkSession::SessionError error =
                qvariant_cast<QNetworkSession::SessionError>(errorSpy2.first().at(0));
            if (error == QNetworkSession::OperationNotSupportedError) {
                // The session needed to bring down the interface,
                // but the operation is not supported.
                QSKIP("Configuration does not support stop().", SkipSingle);
            } else if (error == QNetworkSession::InvalidConfigurationError) {
                // The session needed to bring down the interface, but it is not possible for the
                // specified configuration.
                if ((session.configuration().state() & QNetworkConfiguration::Discovered) ==
                    QNetworkConfiguration::Discovered) {
                    QFAIL("Failed to stop session for Discovered configuration.");
                } else {
                    QSKIP("Cannot test session for non-Discovered configuration.", SkipSingle);
                }
            } else {
                QFAIL("Error stopping session.");
            }
        } else if (!sessionClosedSpy2.isEmpty()) {
            if (expectStateChange) {
                if (configuration.type() == QNetworkConfiguration::ServiceNetwork) {
                    bool roamedSuccessfully = false;

                    QCOMPARE(stateChangedSpy2.count(), 4);

                    QNetworkSession::State state =
                        qvariant_cast<QNetworkSession::State>(stateChangedSpy2.at(0).at(0));
                    QVERIFY(state == QNetworkSession::Connecting);

                    state = qvariant_cast<QNetworkSession::State>(stateChangedSpy2.at(1).at(0));
                    QVERIFY(state == QNetworkSession::Connected);

                    state = qvariant_cast<QNetworkSession::State>(stateChangedSpy2.at(2).at(0));
                    QVERIFY(state == QNetworkSession::Closing);

                    state = qvariant_cast<QNetworkSession::State>(stateChangedSpy2.at(3).at(0));
                    QVERIFY(state == QNetworkSession::Disconnected);
                    
                    QTRY_VERIFY(stateChangedSpy.count() > 0);
                    state = qvariant_cast<QNetworkSession::State>(stateChangedSpy.at(0).at(0));
                    if (state == QNetworkSession::Roaming) {
                        QTRY_VERIFY(!errorSpy.isEmpty() || stateChangedSpy.count() > 1);
                        if (stateChangedSpy.count() > 1 &&
                            qvariant_cast<QNetworkSession::State>(stateChangedSpy.at(1).at(0)) ==
                            QNetworkSession::Connected) {
                            roamedSuccessfully = true;
                        }
                    }

                    if (roamedSuccessfully) {
                        QString configId = session.sessionProperty("ActiveConfiguration").toString();
                        QNetworkConfiguration config = manager.configurationFromIdentifier(configId); 
                        QNetworkSession session3(config);
                        QSignalSpy errorSpy3(&session3, SIGNAL(error(QNetworkSession::SessionError)));
                        QSignalSpy sessionOpenedSpy3(&session3, SIGNAL(opened()));
                        
                        session3.open();
                        session3.waitForOpened();
                        
                        if (session.isOpen())
                            QVERIFY(!sessionOpenedSpy3.isEmpty() || !errorSpy3.isEmpty());
                        
                        session.stop();

                        QTRY_VERIFY(session.state() == QNetworkSession::Disconnected);
                        QTRY_VERIFY(session3.state() == QNetworkSession::Disconnected);
                    }
#ifndef Q_CC_NOKIAX86
                    if (!roamedSuccessfully)
                        QVERIFY(!errorSpy.isEmpty());
#endif
                } else {
                    QCOMPARE(stateChangedSpy2.count(), 2);

                    QNetworkSession::State state =
                        qvariant_cast<QNetworkSession::State>(stateChangedSpy2.at(0).at(0));
                    QVERIFY(state == QNetworkSession::Closing);

                    state = qvariant_cast<QNetworkSession::State>(stateChangedSpy2.at(1).at(0));
                    QVERIFY(state == QNetworkSession::Disconnected);
                }

                QTRY_VERIFY(!sessionClosedSpy.isEmpty());

                QTRY_VERIFY(session.state() == QNetworkSession::Disconnected);
                QTRY_VERIFY(session2.state() == QNetworkSession::Disconnected);
            }

            QVERIFY(errorSpy2.isEmpty());

            ++inProcessSessionManagementCount;
        } else {
            QFAIL("Timeout waiting for session to stop.");
        }

#ifndef Q_CC_NOKIAX86
        QVERIFY(!sessionClosedSpy.isEmpty());
#endif
        QVERIFY(!sessionClosedSpy2.isEmpty());

#ifndef Q_CC_NOKIAX86
        QVERIFY(!session.isOpen());
#endif
        QVERIFY(!session2.isOpen());
    } else {
        // Test closing the second session.
        {
            int stateChangedCountBeforeClose = stateChangedSpy2.count();
            session2.close();

            QTRY_VERIFY(!sessionClosedSpy2.isEmpty());
#ifndef Q_CC_NOKIAX86        
            QVERIFY(stateChangedSpy2.count() == stateChangedCountBeforeClose);
#endif        

            QVERIFY(sessionClosedSpy.isEmpty());

            QVERIFY(session.isOpen());
            QVERIFY(!session2.isOpen());
            QVERIFY(session.state() == QNetworkSession::Connected);
            QVERIFY(session2.state() == QNetworkSession::Connected);
#ifndef QT_NO_NETWORKINTERFACE
#if !(defined(Q_OS_SYMBIAN) && defined(__WINS__))
            // On Symbian emulator, the support for data bearers is limited
            QVERIFY(session.interface().isValid());
#endif
            QCOMPARE(session.interface().hardwareAddress(), session2.interface().hardwareAddress());
            QCOMPARE(session.interface().index(), session2.interface().index());
#endif
        }

        sessionClosedSpy2.clear();

        // Test closing the first session.
        {
#ifdef Q_CC_NOKIAX86
            // For S60 emulator builds: RConnection::Close does not actually
            //                          close network connection on all Emulators
            bool expectStateChange = false;
#else
            bool expectStateChange = session.state() != QNetworkSession::Disconnected &&
                                     manager.capabilities() & QNetworkConfigurationManager::SystemSessionSupport;
#endif

            session.close();

            QTRY_VERIFY(!sessionClosedSpy.isEmpty() || !errorSpy.isEmpty());

            QVERIFY(!session.isOpen());

            if (expectStateChange)
                QTRY_VERIFY(!stateChangedSpy.isEmpty() || !errorSpy.isEmpty());

            if (!errorSpy.isEmpty()) {
                QNetworkSession::SessionError error =
                    qvariant_cast<QNetworkSession::SessionError>(errorSpy.first().at(0));
                if (error == QNetworkSession::OperationNotSupportedError) {
                    // The session needed to bring down the interface,
                    // but the operation is not supported.
                    QSKIP("Configuration does not support close().", SkipSingle);
                } else if (error == QNetworkSession::InvalidConfigurationError) {
                    // The session needed to bring down the interface, but it is not possible for the
                    // specified configuration.
                    if ((session.configuration().state() & QNetworkConfiguration::Discovered) ==
                        QNetworkConfiguration::Discovered) {
                        QFAIL("Failed to close session for Discovered configuration.");
                    } else {
                        QSKIP("Cannot test session for non-Discovered configuration.", SkipSingle);
                    }
                } else {
                    QFAIL("Error closing session.");
                }
            } else if (!sessionClosedSpy.isEmpty()) {
                QVERIFY(sessionOpenedSpy.isEmpty());
                QCOMPARE(sessionClosedSpy.count(), 1);
                if (expectStateChange)
                    QVERIFY(!stateChangedSpy.isEmpty());
                QVERIFY(errorSpy.isEmpty());

                if (expectStateChange)
                    QTRY_VERIFY(session.state() == QNetworkSession::Disconnected);

                ++inProcessSessionManagementCount;
            } else {
                QFAIL("Timeout waiting for session to close.");
            }
        }
    }
}

QDebug operator<<(QDebug debug, const QList<QNetworkConfiguration> &list)
{
    debug.nospace() << "( ";
    foreach (const QNetworkConfiguration &config, list)
        debug.nospace() << config.identifier() << ", ";
    debug.nospace() << ")\n";
    return debug;
}

// Note: outOfProcessSession requires that at least one configuration is
// at Discovered -state.
void tst_QNetworkSession::outOfProcessSession()
{
    qDebug() << "START";

#if defined(Q_OS_SYMBIAN) && defined(__WINS__)
    QSKIP("Symbian emulator does not support two [QR]PRocesses linking a dll (QtBearer.dll) with global writeable static data.", SkipAll);
#endif
    QNetworkConfigurationManager manager;
    // Create a QNetworkConfigurationManager to detect configuration changes made in Lackey. This
    // is actually the essence of this testcase - to check that platform mediates/reflects changes
    // regardless of process boundaries. The interprocess communication is more like a way to get
    // this test-case act correctly and timely.
    QList<QNetworkConfiguration> before = manager.allConfigurations(QNetworkConfiguration::Active);
    QSignalSpy spy(&manager, SIGNAL(configurationChanged(QNetworkConfiguration)));

    // Cannot read/write to processes on WinCE or Symbian.
    // Easiest alternative is to use sockets for IPC.
    QLocalServer oopServer;
    // First remove possible earlier listening address which would cause listen to fail 
    // (e.g. previously abruptly ended unit test might cause this)
    QLocalServer::removeServer("tst_qnetworksession");
    oopServer.listen("tst_qnetworksession");

    qDebug() << "starting lackey";
    QProcess lackey;
    lackey.start("lackey/lackey");
    qDebug() << lackey.error() << lackey.errorString();
    QVERIFY(lackey.waitForStarted());

    qDebug() << "waiting for connection";
    QVERIFY(oopServer.waitForNewConnection(-1));
    QLocalSocket *oopSocket = oopServer.nextPendingConnection();
    qDebug() << "got connection";
    do {
        QByteArray output;

        if (oopSocket->waitForReadyRead())
            output = oopSocket->readLine().trimmed();

        if (output.startsWith("Started session ")) {
            QString identifier = QString::fromLocal8Bit(output.mid(20).constData());
            QNetworkConfiguration changed;

            do {
                QTRY_VERIFY(!spy.isEmpty());
                changed = qvariant_cast<QNetworkConfiguration>(spy.takeFirst().at(0));
            } while (changed.identifier() != identifier);

            QVERIFY((changed.state() & QNetworkConfiguration::Active) ==
                    QNetworkConfiguration::Active);

            QVERIFY(!before.contains(changed));

            QList<QNetworkConfiguration> after =
                manager.allConfigurations(QNetworkConfiguration::Active);

            QVERIFY(after.contains(changed));

            spy.clear();

            oopSocket->write("stop\n");
            oopSocket->waitForBytesWritten();

            do {
                QTRY_VERIFY(!spy.isEmpty());

                changed = qvariant_cast<QNetworkConfiguration>(spy.takeFirst().at(0));
            } while (changed.identifier() != identifier);

            QVERIFY((changed.state() & QNetworkConfiguration::Active) !=
                    QNetworkConfiguration::Active);

            QList<QNetworkConfiguration> afterStop =
                    manager.allConfigurations(QNetworkConfiguration::Active);

            QVERIFY(!afterStop.contains(changed));

            oopSocket->disconnectFromServer();
            oopSocket->waitForDisconnected(-1);

            lackey.waitForFinished();
        }
    // This is effected by QTBUG-4903, process will always report as running
    //} while (lackey.state() == QProcess::Running);

    // Workaround: the socket in the lackey will disconnect on exit
    } while (oopSocket->state() == QLocalSocket::ConnectedState);

    switch (lackey.exitCode()) {
    case 0:
        qDebug("Lackey returned exit success (0)");
        break;
    case 1:
        QSKIP("No discovered configurations found.", SkipAll);
    case 2:
        QSKIP("Lackey could not start session.", SkipAll);
    default:
        QSKIP("Lackey failed", SkipAll);
    }
    qDebug("STOP");
}

// A convinience / helper function for testcases. Return the first matching configuration.
// Ignores configurations in other than 'discovered' -state. Returns invalid (QNetworkConfiguration())
// if none found.
QNetworkConfiguration suitableConfiguration(QString bearerType, QNetworkConfiguration::Type configType) {
    
    // Refresh configurations and derive configurations matching given parameters.
    QNetworkConfigurationManager mgr;
    QSignalSpy updateSpy(&mgr, SIGNAL(updateCompleted()));
    mgr.updateConfigurations();
    QTRY_NOOP(updateSpy.count() == 1);
    if (updateSpy.count() != 1) {
        qDebug("tst_QNetworkSession::suitableConfiguration() failure: unable to update configurations");
        return QNetworkConfiguration();
    }
    QList<QNetworkConfiguration> discoveredConfigs = mgr.allConfigurations(QNetworkConfiguration::Discovered);
    foreach(QNetworkConfiguration config, discoveredConfigs) {
        if ((config.state() & QNetworkConfiguration::Active) == QNetworkConfiguration::Active) {
            // qDebug() << "Dumping config because is active: " << config.name();
            discoveredConfigs.removeOne(config);
        } else if (config.type() != configType) {
            // qDebug() << "Dumping config because type (IAP/SNAP) mismatches: " << config.name();
            discoveredConfigs.removeOne(config);
        } else if ((config.type() == QNetworkConfiguration::InternetAccessPoint) &&
                    bearerType == "cellular") { // 'cellular' bearertype is for convinience
            if (config.bearerName() != "2G" &&
                config.bearerName() != "CDMA2000" &&
                config.bearerName() != "WCDMA" &&
                config.bearerName() != "HSPA") {
                // qDebug() << "Dumping config because bearer mismatches (cellular): " << config.name();
                discoveredConfigs.removeOne(config);
            }
        } else if ((config.type() == QNetworkConfiguration::InternetAccessPoint) && 
                    bearerType != config.bearerName()) {
            // qDebug() << "Dumping config because bearer mismatches (WLAN): " << config.name();
            discoveredConfigs.removeOne(config);
        }
    }
    if (discoveredConfigs.isEmpty()) {
        qDebug("tst_QNetworkSession::suitableConfiguration() failure: no suitable configurations present.");
        return QNetworkConfiguration();
    } else {
        return discoveredConfigs.first();
    }
}

// A convinience-function: updates configurations and waits that they are updated.
void updateConfigurations() 
{
    QNetworkConfigurationManager mgr;
    QSignalSpy updateSpy(&mgr, SIGNAL(updateCompleted()));
    mgr.updateConfigurations();
    QTRY_NOOP(updateSpy.count() == 1);
}

// A convinience function for test-cases: opens the given configuration and return
// true if it was done gracefully.
bool openSession(QNetworkSession *session) {
    QNetworkConfigurationManager mgr;
    QSignalSpy openedSpy(session, SIGNAL(opened()));
    QSignalSpy stateChangeSpy(session, SIGNAL(stateChanged(QNetworkSession::State)));
    QSignalSpy errorSpy(session, SIGNAL(error(QNetworkSession::SessionError)));
    QSignalSpy configChangeSpy(&mgr, SIGNAL(configurationChanged(QNetworkConfiguration)));
    // Store some initial statuses, because expected signals differ if the config is already
    // active by some other session
    QNetworkConfiguration::StateFlags configInitState = session->configuration().state();
    QNetworkSession::State sessionInitState = session->state();

    if (session->isOpen() ||
        !session->sessionProperty("ActiveConfiguration").toString().isEmpty()) {
        qDebug("tst_QNetworkSession::openSession() failure: session was already open / active.");
        return false;
    } else {
        session->open();
        session->waitForOpened(120000); // Bringing interfaces up and down may take time at platform
    }
    // Check that connection opening went by the book. Add checks here if more strictness needed.
    if (!session->isOpen()) {
        qDebug("tst_QNetworkSession::openSession() failure: QNetworkSession::open() failed.");
        return false;
    }
    if (openedSpy.count() != 1) {
        qDebug("tst_QNetworkSession::openSession() failure: QNetworkSession::opened() - signal not received.");
        return false;
    }
    if (!errorSpy.isEmpty()) {
        qDebug("tst_QNetworkSession::openSession() failure: QNetworkSession::error() - signal was detected.");
        return false;
    }
    if (sessionInitState != QNetworkSession::Connected && 
        stateChangeSpy.isEmpty()) {
        qDebug("tst_QNetworkSession::openSession() failure: QNetworkSession::stateChanged() - signals not detected.");
        return false;
    }
    if (configInitState != QNetworkConfiguration::Active && 
        configChangeSpy.isEmpty()) {
        qDebug("tst_QNetworkSession::openSession() failure: QNetworkConfigurationManager::configurationChanged() - signals not detected.");
        return false;
    }
    if (session->configuration().state() != QNetworkConfiguration::Active) {
        qDebug("tst_QNetworkSession::openSession() failure: session's configuration is not in 'Active' -state.");
        return false;
    }
    return true;
}

// Helper function for closing opened session. Performs checks that 
// session is closed gradefully (e.g. signals). Function does not delete
// the session. The lastSessionOnConfiguration (true by default) is used to
// tell if there are more sessions open, basing on same configration. This 
// impacts the checks made.
bool closeSession(QNetworkSession *session, bool lastSessionOnConfiguration) {
    if (!session) {
        qDebug("tst_QNetworkSession::closeSession() failure: NULL session given");
        return false;
    }
    if (session->state() != QNetworkSession::Connected || 
        !session->isOpen()) {
        qDebug("tst_QNetworkSession::closeSession() failure: session is not opened.");
        return false;
    }    
    QNetworkConfigurationManager mgr;
    QSignalSpy sessionClosedSpy(session, SIGNAL(closed()));
    QSignalSpy sessionStateChangedSpy(session, SIGNAL(stateChanged(QNetworkSession::State)));
    QSignalSpy sessionErrorSpy(session, SIGNAL(error(QNetworkSession::SessionError)));
    QSignalSpy configChangeSpy(&mgr, SIGNAL(configurationChanged(QNetworkConfiguration)));
    
    session->close();
    
    if (!sessionErrorSpy.isEmpty()) {
        qDebug("tst_QNetworkSession::closeSession() failure: QNetworkSession::error() received.");
        return false;
    }
    if (sessionClosedSpy.count() != 1) {
        qDebug("tst_QNetworkSession::closeSession() failure: QNetworkSession::closed() signal not received.");
        return false;
    }
    if (lastSessionOnConfiguration && 
        sessionStateChangedSpy.isEmpty()) {
        qDebug("tst_QNetworkSession::closeSession() failure: QNetworkSession::stateChanged() signals not received.");
        return false;
    }
    if (lastSessionOnConfiguration &&
        session->state() != QNetworkSession::Disconnected) {
        qDebug("tst_QNetworkSession::closeSession() failure: QNetworkSession is not in Disconnected -state");
        return false;
    }
    QTRY_NOOP(!configChangeSpy.isEmpty());
    if (lastSessionOnConfiguration &&
        configChangeSpy.isEmpty()) {
        qDebug("tst_QNetworkSession::closeSession() failure: QNetworkConfigurationManager::configurationChanged() - signal not detected.");
        return false;
    }
    if (lastSessionOnConfiguration &&
        session->configuration().state() == QNetworkConfiguration::Active) {
         qDebug("tst_QNetworkSession::closeSession() failure: session's configuration is still in active state.");
         return false;
    }
    return true;
}

void tst_QNetworkSession::sessionAutoClose_data()
{
    QTest::addColumn<QNetworkConfiguration>("configuration");

    bool testData = false;
    foreach (const QNetworkConfiguration &config,
             manager.allConfigurations(QNetworkConfiguration::Discovered)) {
        QNetworkSession session(config);
        if (!session.sessionProperty(QLatin1String("AutoCloseSessionTimeout")).isValid())
            continue;

        testData = true;

        const QString name = config.name().isEmpty() ? QString("<Hidden>") : config.name();
        QTest::newRow(name.toLocal8Bit().constData()) << config;
    }

    if (!testData)
        QSKIP("No applicable configurations to test", SkipAll);
}

void tst_QNetworkSession::sessionAutoClose()
{
    QFETCH(QNetworkConfiguration, configuration);

    QNetworkSession session(configuration);

    QVERIFY(session.configuration() == configuration);

    QVariant autoCloseSession = session.sessionProperty(QLatin1String("AutoCloseSessionTimeout"));

    QVERIFY(autoCloseSession.isValid());

    // property defaults to false
    QCOMPARE(autoCloseSession.toInt(), -1);

    QSignalSpy closeSpy(&session, SIGNAL(closed()));

    session.open();
    session.waitForOpened();

    if (!session.isOpen())
        QSKIP("Session not open", SkipSingle);

    // set session to auto close at next polling interval.
    session.setSessionProperty(QLatin1String("AutoCloseSessionTimeout"), 0);

    QTRY_VERIFY(!closeSpy.isEmpty());

    QCOMPARE(session.state(), QNetworkSession::Connected);

    QVERIFY(!session.isOpen());

    QVERIFY(session.configuration() == configuration);

    autoCloseSession = session.sessionProperty(QLatin1String("AutoCloseSessionTimeout"));

    QVERIFY(autoCloseSession.isValid());

    QCOMPARE(autoCloseSession.toInt(), -1);
}

QTEST_MAIN(tst_QNetworkSession)

#include "tst_qnetworksession.moc"

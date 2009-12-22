/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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

#include <qmobilityglobal.h>
#include "qnetworksession_s60_p.h"
#include "qnetworkconfiguration_s60_p.h"
#include "qnetworkconfigmanager_s60_p.h"
#include <es_enum.h>
#include <es_sock.h>
#include <in_sock.h>
#include <stdapis/sys/socket.h>
#include <stdapis/net/if.h>

QTM_BEGIN_NAMESPACE

QNetworkSessionPrivate::QNetworkSessionPrivate()
    : CActive(CActive::EPriorityStandard), state(QNetworkSession::Invalid),
      isActive(false), ipConnectionNotifier(0), iError(QNetworkSession::UnknownSessionError),
      iALREnabled(0)
{
    CActiveScheduler::Add(this);
    
    // Try to load "Open C" dll dynamically and
    // try to attach to setdefaultif function dynamically.
    if (iOpenCLibrary.Load(_L("libc")) == KErrNone) {
        iDynamicSetdefaultif = (TOpenCSetdefaultifFunction)iOpenCLibrary.Lookup(564);
    }

    TRAP_IGNORE(iConnectionMonitor.ConnectL());
}

QNetworkSessionPrivate::~QNetworkSessionPrivate()
{
    isActive = false;

    // Cancel Connection Progress Notifications first.
    // Note: ConnectionNotifier must be destroyed before Canceling RConnection::Start()
    //       => deleting ipConnectionNotifier results RConnection::CancelProgressNotification()
    delete ipConnectionNotifier;
    ipConnectionNotifier = NULL;

    // Cancel possible RConnection::Start()
    Cancel();
    
#ifdef SNAP_FUNCTIONALITY_AVAILABLE
    if (iMobility) {
        delete iMobility;
        iMobility = NULL;
    }
#endif

    iConnection.Close();
    iSocketServ.Close();
    if (iDynamicSetdefaultif) {
        iDynamicSetdefaultif(0);
    }

    iConnectionMonitor.CancelNotifications();
    iConnectionMonitor.Close();

    iOpenCLibrary.Close();
}

void QNetworkSessionPrivate::syncStateWithInterface()
{
    if (!publicConfig.d) {
        return;
    }

    // Start monitoring changes in IAP states
    TRAP_IGNORE(iConnectionMonitor.NotifyEventL(*this));

    // Check open connections to see if there is already
    // an open connection to selected IAP or SNAP
    TUint count;
    TRequestStatus status;
    iConnectionMonitor.GetConnectionCount(count, status);
    User::WaitForRequest(status);
    if (status.Int() != KErrNone) {
        return;
    }

    TUint numSubConnections;
    TUint connectionId;
    for (TUint i = 1; i <= count; i++) {
        TInt ret = iConnectionMonitor.GetConnectionInfo(i, connectionId, numSubConnections);
        if (ret == KErrNone) {
            TUint apId;
            iConnectionMonitor.GetUintAttribute(connectionId, 0, KIAPId, apId, status);
            User::WaitForRequest(status);
            if (status.Int() == KErrNone) {
                TInt connectionStatus;
                iConnectionMonitor.GetIntAttribute(connectionId, 0, KConnectionStatus, connectionStatus, status);
                User::WaitForRequest(status);
                if (connectionStatus == KLinkLayerOpen) {
                    if (state != QNetworkSession::Closing) {
                        if (newState(QNetworkSession::Connected, apId)) {
                            return;
                        }
                    }
                }
            }
        }
    }

    if (state != QNetworkSession::Connected) {
        // There were no open connections to used IAP or SNAP
        if ((publicConfig.d.data()->state & QNetworkConfiguration::Discovered) ==
            QNetworkConfiguration::Discovered) {
            newState(QNetworkSession::Disconnected);
        } else {
            newState(QNetworkSession::NotAvailable);
        }
    }
}

QNetworkInterface QNetworkSessionPrivate::interface(TUint iapId) const
{
    QString interfaceName;

    TSoInetInterfaceInfo ifinfo;
    TPckg<TSoInetInterfaceInfo> ifinfopkg(ifinfo);
    TSoInetIfQuery ifquery;
    TPckg<TSoInetIfQuery> ifquerypkg(ifquery);
 
    // Open dummy socket for interface queries
    RSocket socket;
    TInt retVal = socket.Open(iSocketServ, _L("udp"));
    if (retVal != KErrNone) {
        return QNetworkInterface();
    }
 
    // Start enumerating interfaces
    socket.SetOpt(KSoInetEnumInterfaces, KSolInetIfCtrl);
    while(socket.GetOpt(KSoInetNextInterface, KSolInetIfCtrl, ifinfopkg) == KErrNone) {
        ifquery.iName = ifinfo.iName;
        TInt err = socket.GetOpt(KSoInetIfQueryByName, KSolInetIfQuery, ifquerypkg);
        if(err == KErrNone && ifquery.iZone[1] == iapId) { // IAP ID is index 1 of iZone
            if(ifinfo.iAddress.Address() > 0) {
                interfaceName = QString::fromUtf16(ifinfo.iName.Ptr(),ifinfo.iName.Length());
                break;
            }
        }
    }
 
    socket.Close();
 
    if (interfaceName.isEmpty()) {
        return QNetworkInterface();
    }
 
    return QNetworkInterface::interfaceFromName(interfaceName);
}

QNetworkInterface QNetworkSessionPrivate::currentInterface() const
{
    if (!publicConfig.isValid() || state != QNetworkSession::Connected) {
        return QNetworkInterface();
    }
    
    return activeInterface;
}

QVariant QNetworkSessionPrivate::sessionProperty(const QString& /*key*/) const
{
    return QVariant();
}

void QNetworkSessionPrivate::setSessionProperty(const QString& /*key*/, const QVariant& /*value*/)
{
}

QString QNetworkSessionPrivate::errorString() const
{
    switch (iError) {
    case QNetworkSession::UnknownSessionError:
        return tr("Unknown session error.");
    case QNetworkSession::SessionAbortedError:
        return tr("The session was aborted by the user or system.");
    case QNetworkSession::OperationNotSupportedError:
        return tr("The requested operation is not supported by the system.");
    case QNetworkSession::InvalidConfigurationError:
        return tr("The specified configuration cannot be used.");
    case QNetworkSession::RoamingError:
        return tr("Roaming was aborted or is not possible.");
    }
 
    return QString();
}

QNetworkSession::SessionError QNetworkSessionPrivate::error() const
{
    return iError;
}

void QNetworkSessionPrivate::open()
{
    if (isActive || !publicConfig.d || (state == QNetworkSession::Connecting)) {
        return;
    }

    // Cancel notifications from RConnectionMonitor
    // => RConnection::ProgressNotification will be used for IAP/SNAP monitoring
    iConnectionMonitor.CancelNotifications();
    
    TInt error = iSocketServ.Connect();
    if (error != KErrNone) {
        // Could not open RSocketServ
        newState(QNetworkSession::Invalid);
        iError = QNetworkSession::UnknownSessionError;
        emit q->error(iError);
        syncStateWithInterface();    
        return;
    }
    
    error = iConnection.Open(iSocketServ);
    if (error != KErrNone) {
        // Could not open RConnection
        iSocketServ.Close();
        newState(QNetworkSession::Invalid);
        iError = QNetworkSession::UnknownSessionError;
        emit q->error(iError);
        syncStateWithInterface();    
        return;
    }
    
    // Use RConnection::ProgressNotification for IAP/SNAP monitoring
    // (<=> ConnectionProgressNotifier uses RConnection::ProgressNotification)
    if (!ipConnectionNotifier) {
        ipConnectionNotifier = new ConnectionProgressNotifier(*this,iConnection);
    }
    if (ipConnectionNotifier) {
        ipConnectionNotifier->StartNotifications();
    }
    
    if (publicConfig.type() == QNetworkConfiguration::InternetAccessPoint) {
        // Search through existing connections.
        // If there is already connection which matches to given IAP
        // try to attach to existing connection.
        TBool connected(EFalse);
        TConnectionInfoBuf connInfo;
        TUint count;
        if (iConnection.EnumerateConnections(count) == KErrNone) {
            for (TUint i=1; i<=count; i++) {
                // Note: GetConnectionInfo expects 1-based index.
                if (iConnection.GetConnectionInfo(i, connInfo) == KErrNone) {
                    if (connInfo().iIapId == publicConfig.d.data()->numericId) {
                        if (iConnection.Attach(connInfo, RConnection::EAttachTypeNormal) == KErrNone) {
                            activeConfig = publicConfig;
                            activeInterface = interface(activeConfig.d.data()->numericId);
                            connected = ETrue;
                            startTime = QDateTime::currentDateTime();
                            if (iDynamicSetdefaultif) {
                                // Use name of the IAP to set default IAP
                                QByteArray nameAsByteArray = publicConfig.name().toUtf8();
                                ifreq ifr;
                                strcpy(ifr.ifr_name, nameAsByteArray.constData());

                                error = iDynamicSetdefaultif(&ifr);
                            }
                            isActive = true;
                            // Make sure that state will be Connected
                            newState(QNetworkSession::Connected);
                            emit quitPendingWaitsForOpened();
                            break;
                        }
                    }
                }
            }
        }
        if (!connected) {
            TCommDbConnPref pref;
            pref.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
            pref.SetIapId(publicConfig.d.data()->numericId);
            iConnection.Start(pref, iStatus);
            if (!IsActive()) {
                SetActive();
            }
            newState(QNetworkSession::Connecting);
        }
    } else if (publicConfig.type() == QNetworkConfiguration::ServiceNetwork) {
        TConnSnapPref snapPref(publicConfig.d.data()->numericId);
        iConnection.Start(snapPref, iStatus);
        if (!IsActive()) {
            SetActive();
        }
        newState(QNetworkSession::Connecting);
    } else if (publicConfig.type() == QNetworkConfiguration::UserChoice) {
        iKnownConfigsBeforeConnectionStart = ((QNetworkConfigurationManagerPrivate*)publicConfig.d.data()->manager)->accessPointConfigurations.keys();
        iConnection.Start(iStatus);
        if (!IsActive()) {
            SetActive();
        }
        newState(QNetworkSession::Connecting);
    }
 
    if (error != KErrNone) {
        isActive = false;
        iError = QNetworkSession::UnknownSessionError;
        emit q->error(iError);
        if (ipConnectionNotifier) {
            ipConnectionNotifier->StopNotifications();
        }
        syncStateWithInterface();    
    }
}

TUint QNetworkSessionPrivate::iapClientCount(TUint aIAPId) const
{
    TRequestStatus status;
    TUint connectionCount;
    iConnectionMonitor.GetConnectionCount(connectionCount, status);
    User::WaitForRequest(status);
    if (status.Int() == KErrNone) {
        for (TUint i = 1; i <= connectionCount; i++) {
            TUint connectionId;
            TUint subConnectionCount;
            iConnectionMonitor.GetConnectionInfo(i, connectionId, subConnectionCount);
            TUint apId;
            iConnectionMonitor.GetUintAttribute(connectionId, subConnectionCount, KIAPId, apId, status);
            User::WaitForRequest(status);
            if (apId == aIAPId) {
                TConnMonClientEnumBuf buf;
                iConnectionMonitor.GetPckgAttribute(connectionId, 0, KClientInfo, buf, status);
                User::WaitForRequest(status);
                if (status.Int() == KErrNone) {
                    return buf().iCount;
                }
            }
        }
    }
    return 0;
}

void QNetworkSessionPrivate::close(bool allowSignals)
{
    if (!isActive) {
        return;
    }

    TUint activeIap = activeConfig.d.data()->numericId;
    isActive = false;
    activeConfig = QNetworkConfiguration();
    serviceConfig = QNetworkConfiguration();
    
    Cancel();
#ifdef SNAP_FUNCTIONALITY_AVAILABLE
    if (iMobility) {
        delete iMobility;
        iMobility = NULL;
    }
#endif

    if (ipConnectionNotifier) {
        ipConnectionNotifier->StopNotifications();
    }
    
    iConnection.Close();
    iSocketServ.Close();
    if (iDynamicSetdefaultif) {
        iDynamicSetdefaultif(0);
    }

#ifdef Q_CC_NOKIAX86
    if ((allowSignals && iapClientCount(activeIap) <= 0) ||
#else
    if ((allowSignals && iapClientCount(activeIap) <= 1) ||
#endif
        (publicConfig.type() == QNetworkConfiguration::UserChoice)) {
        newState(QNetworkSession::Closing);
    }
    
    syncStateWithInterface();
    if (allowSignals) {
        if (publicConfig.type() == QNetworkConfiguration::UserChoice) {
            newState(QNetworkSession::Disconnected);
        }
        emit q->closed();
    }
}

void QNetworkSessionPrivate::stop()
{
    if (!isActive) {
        return;
    }
    isActive = false;
    newState(QNetworkSession::Closing);
    iConnection.Stop(RConnection::EStopAuthoritative);
    isActive = true;
    close(false);
    emit q->closed();
}

void QNetworkSessionPrivate::migrate()
{
#ifdef SNAP_FUNCTIONALITY_AVAILABLE
    iMobility->MigrateToPreferredCarrier();
#endif
}

void QNetworkSessionPrivate::ignore()
{
#ifdef SNAP_FUNCTIONALITY_AVAILABLE
    iMobility->IgnorePreferredCarrier();
    if (!iALRUpgradingConnection) {
        newState(QNetworkSession::Disconnected);
    } else {
        newState(QNetworkSession::Connected,iOldRoamingIap);
    }
#endif
}

void QNetworkSessionPrivate::accept()
{
#ifdef SNAP_FUNCTIONALITY_AVAILABLE
    iMobility->NewCarrierAccepted();
    if (iDynamicSetdefaultif) {
        // Use name of the IAP to set default IAP
        QByteArray nameAsByteArray = activeConfig.name().toUtf8();
        ifreq ifr;
        strcpy(ifr.ifr_name, nameAsByteArray.constData());

        iDynamicSetdefaultif(&ifr);
    }
    newState(QNetworkSession::Connected, iNewRoamingIap);
#endif
}

void QNetworkSessionPrivate::reject()
{
#ifdef SNAP_FUNCTIONALITY_AVAILABLE
    iMobility->NewCarrierRejected();
    if (!iALRUpgradingConnection) {
        newState(QNetworkSession::Disconnected);
    } else {
        newState(QNetworkSession::Connected, iOldRoamingIap);
    }
#endif
}

#ifdef SNAP_FUNCTIONALITY_AVAILABLE
void QNetworkSessionPrivate::PreferredCarrierAvailable(TAccessPointInfo aOldAPInfo,
                                                       TAccessPointInfo aNewAPInfo,
                                                       TBool aIsUpgrade,
                                                       TBool aIsSeamless)
{
    iOldRoamingIap = aOldAPInfo.AccessPoint();
    iNewRoamingIap = aNewAPInfo.AccessPoint();
    newState(QNetworkSession::Roaming);
    if (iALREnabled > 0) {
        iALRUpgradingConnection = aIsUpgrade;
        QList<QNetworkConfiguration> configs = publicConfig.children();
        for (int i=0; i < configs.count(); i++) {
            if (configs[i].d.data()->numericId == aNewAPInfo.AccessPoint()) {
                emit q->preferredConfigurationChanged(configs[i],aIsSeamless);
            }
        }
    } else {
        migrate();
    }
}

void QNetworkSessionPrivate::NewCarrierActive(TAccessPointInfo /*aNewAPInfo*/, TBool /*aIsSeamless*/)
{
    if (iALREnabled > 0) {
        emit q->newConfigurationActivated();
    } else {
        accept();
    }
}

void QNetworkSessionPrivate::Error(TInt /*aError*/)
{
    if (isActive) {
        isActive = false;
        activeConfig = QNetworkConfiguration();
        serviceConfig = QNetworkConfiguration();
        iError = QNetworkSession::RoamingError;
        emit q->error(iError);
        Cancel();
        if (ipConnectionNotifier) {
            ipConnectionNotifier->StopNotifications();
        }
        syncStateWithInterface();
        // In some cases IAP is still in Connected state when
        // syncStateWithInterface(); is called
        // => Following call makes sure that Session state
        //    changes immediately to Disconnected.
        newState(QNetworkSession::Disconnected);
        emit q->closed();
    }
}
#endif

void QNetworkSessionPrivate::setALREnabled(bool enabled)
{
    if (enabled) {
        iALREnabled++;
    } else {
        iALREnabled--;
    }
}

QNetworkConfiguration QNetworkSessionPrivate::bestConfigFromSNAP(const QNetworkConfiguration& snapConfig) const
{
    QNetworkConfiguration config;
    QList<QNetworkConfiguration> subConfigurations = snapConfig.children();
    for (int i = 0; i < subConfigurations.count(); i++ ) {
        if (subConfigurations[i].state() == QNetworkConfiguration::Active) {
            config = subConfigurations[i];
            break;
        } else if (!config.isValid() && subConfigurations[i].state() == QNetworkConfiguration::Discovered) {
            config = subConfigurations[i];
        }
    }
    if (!config.isValid() && subConfigurations.count() > 0) {
        config = subConfigurations[0];
    }
    return config;
}

QString QNetworkSessionPrivate::bearerName() const
{
    QNetworkConfiguration config;
    if (publicConfig.type() == QNetworkConfiguration::InternetAccessPoint) {
        config = publicConfig;
    } else if (publicConfig.type() == QNetworkConfiguration::ServiceNetwork) {
        if (activeConfig.isValid()) {
            config = activeConfig;
        } else {
            config = bestConfigFromSNAP(publicConfig);
        }
    } else if (publicConfig.type() == QNetworkConfiguration::UserChoice) {
        if (activeConfig.isValid()) {
            config = activeConfig;
        }
    }

    if (!config.isValid()) {
        return QString();
    }

    switch (config.d.data()->bearer) {
        case QNetworkConfigurationPrivate::BearerEthernet:  return QString("Ethernet");
        case QNetworkConfigurationPrivate::BearerWLAN:      return QString("WLAN");
        case QNetworkConfigurationPrivate::Bearer2G:        return QString("2G");
        case QNetworkConfigurationPrivate::BearerCDMA2000:  return QString("CDMA2000");
        case QNetworkConfigurationPrivate::BearerWCDMA:     return QString("WCDMA");
        case QNetworkConfigurationPrivate::BearerHSPA:      return QString("HSPA");
        case QNetworkConfigurationPrivate::BearerBluetooth: return QString("Bluetooth");
        case QNetworkConfigurationPrivate::BearerWiMAX:     return QString("WiMAX");
        default: return QString();
    }
}

quint64 QNetworkSessionPrivate::bytesWritten() const
{
    return transferredData(KUplinkData);
}

quint64 QNetworkSessionPrivate::bytesReceived() const
{
    return transferredData(KDownlinkData);
}

quint64 QNetworkSessionPrivate::transferredData(TUint dataType) const
{
    if (!publicConfig.isValid()) {
        return 0;
    }
    
    QNetworkConfiguration config;
    if (publicConfig.type() == QNetworkConfiguration::UserChoice) {
        if (serviceConfig.isValid()) {
            config = serviceConfig;
        } else {
            if (activeConfig.isValid()) {
                config = activeConfig;
            }
        }
    } else {
        config = publicConfig;
    }

    if (!config.isValid()) {
        return 0;
    }
    
    TUint count;
    TRequestStatus status;
    iConnectionMonitor.GetConnectionCount(count, status);
    User::WaitForRequest(status);
    if (status.Int() != KErrNone) {
        return 0;
    }
    
    TUint transferredData = 0;
    TUint numSubConnections;
    TUint connectionId;
    bool configFound;
    for (TUint i = 1; i <= count; i++) {
        TInt ret = iConnectionMonitor.GetConnectionInfo(i, connectionId, numSubConnections);
        if (ret == KErrNone) {
            TUint apId;
            iConnectionMonitor.GetUintAttribute(connectionId, 0, KIAPId, apId, status);
            User::WaitForRequest(status);
            if (status.Int() == KErrNone) {
                configFound = false;
                if (config.type() == QNetworkConfiguration::ServiceNetwork) {
                    QList<QNetworkConfiguration> configs = config.children();
                    for (int i=0; i < configs.count(); i++) {
                        if (configs[i].d.data()->numericId == apId) {
                            configFound = true;
                            break;
                        }
                    }
                } else if (config.d.data()->numericId == apId) {
                    configFound = true;
                }
                if (configFound) {
                    TUint tData;
                    iConnectionMonitor.GetUintAttribute(connectionId, 0, dataType, tData, status );
                    User::WaitForRequest(status);
                    if (status.Int() == KErrNone) {
                    transferredData += tData;
                    }
                }
            }
        }
    }
    
    return transferredData;
}

quint64 QNetworkSessionPrivate::activeTime() const
{
    if (!isActive || startTime.isNull()) {
        return 0;
    }
    return startTime.secsTo(QDateTime::currentDateTime());
}

QNetworkConfiguration QNetworkSessionPrivate::activeConfiguration(TUint32 iapId) const
{
    if (iapId == 0) {
        _LIT(KSetting, "IAP\\Id");
        iConnection.GetIntSetting(KSetting, iapId);
    }
 
#ifdef SNAP_FUNCTIONALITY_AVAILABLE
    if (publicConfig.type() == QNetworkConfiguration::ServiceNetwork) {
        // Try to search IAP from the used SNAP using IAP Id
        QList<QNetworkConfiguration> children = publicConfig.children();
        for (int i=0; i < children.count(); i++) {
            if (children[i].d.data()->numericId == iapId) {
                return children[i];
            }
        }

        // Given IAP Id was not found from the used SNAP
        // => Try to search matching IAP using mappingName
        //    mappingName contains:
        //      1. "Access point name" for "Packet data" Bearer
        //      2. "WLAN network name" (= SSID) for "Wireless LAN" Bearer
        //      3. "Dial-up number" for "Data call Bearer" or "High Speed (GSM)" Bearer
        //    <=> Note: It's possible that in this case reported IAP is
        //              clone of the one of the IAPs of the used SNAP
        //              => If mappingName matches, clone has been found
        QNetworkConfiguration pt;
        pt.d = ((QNetworkConfigurationManagerPrivate*)publicConfig.d.data()->manager)->accessPointConfigurations.value(QString::number(qHash(iapId)));
        if (pt.d) {
            for (int i=0; i < children.count(); i++) {
                if (children[i].d.data()->mappingName == pt.d.data()->mappingName) {
                    return children[i];
                }
            }
        } else {
            // Given IAP Id was not found from known IAPs array
            return QNetworkConfiguration();
        }

        // Matching IAP was not found from used SNAP
        // => IAP from another SNAP is returned
        //    (Note: Returned IAP matches to given IAP Id)
        return pt;
    }
#endif
    
    if (publicConfig.type() == QNetworkConfiguration::UserChoice) {
        if (publicConfig.d.data()->manager) {
            QNetworkConfiguration pt;
            // Try to found User Selected IAP from known IAPs (accessPointConfigurations)
            pt.d = ((QNetworkConfigurationManagerPrivate*)publicConfig.d.data()->manager)->accessPointConfigurations.value(QString::number(qHash(iapId)));
            if (pt.d) {
                return pt;
            } else {
                // Check if new (WLAN) IAP was created in IAP/SNAP dialog
                // 1. Sync internal configurations array to commsdb first
                ((QNetworkConfigurationManagerPrivate*)publicConfig.d.data()->manager)->updateConfigurations();
                // 2. Check if new configuration was created during connection creation
                QList<QString> knownConfigs = ((QNetworkConfigurationManagerPrivate*)publicConfig.d.data()->manager)->accessPointConfigurations.keys();
                if (knownConfigs.count() > iKnownConfigsBeforeConnectionStart.count()) {
                    // Configuration count increased => new configuration was created
                    // => Search new, created configuration
                    QString newIapId;
                    for (int i=0; i < iKnownConfigsBeforeConnectionStart.count(); i++) {
                        if (knownConfigs[i] != iKnownConfigsBeforeConnectionStart[i]) {
                            newIapId = knownConfigs[i];
                            break;
                        }
                    }
                    if (newIapId.isEmpty()) {
                        newIapId = knownConfigs[knownConfigs.count()-1];
                    }
                    pt.d = ((QNetworkConfigurationManagerPrivate*)publicConfig.d.data()->manager)->accessPointConfigurations.value(newIapId);
                    if (pt.d) {
                        return pt;
                    }
                }
            }
        }
        return QNetworkConfiguration();
    }

    return publicConfig;
}

void QNetworkSessionPrivate::RunL()
{
    TInt statusCode = iStatus.Int();

    switch (statusCode) {
        case KErrNone: // Connection created succesfully
            {
            TInt error = KErrNone;
            QNetworkConfiguration newActiveConfig = activeConfiguration();
            if (!newActiveConfig.isValid()) {
                error = KErrGeneral;
            } else if (iDynamicSetdefaultif) {
                // Use name of the IAP to set default IAP
                QByteArray nameAsByteArray = newActiveConfig.name().toUtf8();
                ifreq ifr;
                strcpy(ifr.ifr_name, nameAsByteArray.constData());

                error = iDynamicSetdefaultif(&ifr);
            }
            
            if (error != KErrNone) {
                isActive = false;
                iError = QNetworkSession::UnknownSessionError;
                emit q->error(iError);
                Cancel();
                if (ipConnectionNotifier) {
                    ipConnectionNotifier->StopNotifications();
                }
                syncStateWithInterface();
                return;
            }
 
#ifdef SNAP_FUNCTIONALITY_AVAILABLE
            if (publicConfig.type() == QNetworkConfiguration::ServiceNetwork) {
                // Activate ALR monitoring
                iMobility = CActiveCommsMobilityApiExt::NewL(iConnection, *this);
            }
#endif
            isActive = true;
            activeConfig = newActiveConfig;
            activeInterface = interface(activeConfig.d.data()->numericId);
            if (publicConfig.type() == QNetworkConfiguration::UserChoice) {
                QNetworkConfiguration pt;
                pt.d = activeConfig.d.data()->serviceNetworkPtr;
                serviceConfig = pt;
            }
            
            startTime = QDateTime::currentDateTime();

            newState(QNetworkSession::Connected);
            emit quitPendingWaitsForOpened();
            }
            break;
        case KErrNotFound: // Connection failed
            isActive = false;
            activeConfig = QNetworkConfiguration();
            serviceConfig = QNetworkConfiguration();
            iError = QNetworkSession::InvalidConfigurationError;
            emit q->error(iError);
            Cancel();
            if (ipConnectionNotifier) {
                ipConnectionNotifier->StopNotifications();
            }
            syncStateWithInterface();
            break;
        case KErrCancel: // Connection attempt cancelled
        case KErrAlreadyExists: // Connection already exists
        default:
            isActive = false;
            activeConfig = QNetworkConfiguration();
            serviceConfig = QNetworkConfiguration();
            iError = QNetworkSession::UnknownSessionError;
            emit q->error(iError);
            Cancel();
            if (ipConnectionNotifier) {
                ipConnectionNotifier->StopNotifications();
            }
            syncStateWithInterface();
            break;
    }
}

void QNetworkSessionPrivate::DoCancel()
{
    iConnection.Close();
}

bool QNetworkSessionPrivate::newState(QNetworkSession::State newState, TUint accessPointId)
{
    // Make sure that activeConfig is always updated when SNAP is signaled to be
    // connected.
    if (isActive && publicConfig.type() == QNetworkConfiguration::ServiceNetwork &&
        newState == QNetworkSession::Connected) {
        activeConfig = activeConfiguration(accessPointId);
        activeInterface = interface(activeConfig.d.data()->numericId);
    }

    // Make sure that same state is not signaled twice in a row.
    if (state == newState) {
        return true;
    }

    // Make sure that Connecting state does not overwrite Roaming state
    if (state == QNetworkSession::Roaming && newState == QNetworkSession::Connecting) {
        return false;
    }

    bool emitSessionClosed = false;
    if (isActive && state == QNetworkSession::Connected && newState == QNetworkSession::Disconnected) {
        // Active & Connected state should change directly to Disconnected state
        // only when something forces connection to close (eg. when another
        // application or session stops connection or when network drops
        // unexpectedly).
        isActive = false;
        activeConfig = QNetworkConfiguration();
        serviceConfig = QNetworkConfiguration();
        iError = QNetworkSession::SessionAbortedError;
        emit q->error(iError);
        Cancel();
        if (ipConnectionNotifier) {
            ipConnectionNotifier->StopNotifications();
        }
        // Start monitoring changes in IAP states
        TRAP_IGNORE(iConnectionMonitor.NotifyEventL(*this));
        emitSessionClosed = true; // Emit SessionClosed after state change has been reported
    }

    bool retVal = false;
    if (accessPointId == 0) {
        state = newState;
        emit q->stateChanged(state);
        retVal = true;
    } else {
        if (publicConfig.type() == QNetworkConfiguration::InternetAccessPoint) {
            if (publicConfig.d.data()->numericId == accessPointId) {
                state = newState;
                emit q->stateChanged(state);
                retVal = true;
            }
        } else if (publicConfig.type() == QNetworkConfiguration::UserChoice && isActive) {
            if (activeConfig.d.data()->numericId == accessPointId) {
                state = newState;
                emit q->stateChanged(state);
                retVal = true;
            }
        } else if (publicConfig.type() == QNetworkConfiguration::ServiceNetwork) {
            QList<QNetworkConfiguration> subConfigurations = publicConfig.children();
            for (int i = 0; i < subConfigurations.count(); i++) {
                if (subConfigurations[i].d.data()->numericId == accessPointId) {
                    if (newState == QNetworkSession::Connected) {
                        // Make sure that when AccessPoint is reported to be Connected
                        // also state of the related configuration changes to Active.
                        subConfigurations[i].d.data()->state = QNetworkConfiguration::Active;
    
                        state = newState;
                        emit q->stateChanged(state);
                        retVal = true;
                    } else {
                        if (newState == QNetworkSession::Disconnected) {
                            // Make sure that when AccessPoint is reported to be disconnected
                            // also state of the related configuration changes from Active to Defined.
                            subConfigurations[i].d.data()->state = QNetworkConfiguration::Defined;
                        }
                        QNetworkConfiguration config = bestConfigFromSNAP(publicConfig);
                        if ((config.state() == QNetworkConfiguration::Defined) ||
                            (config.state() == QNetworkConfiguration::Discovered)) {
                            state = newState;
                            emit q->stateChanged(state);
                            retVal = true;
                        }
                    }
                }
            }
        }
    }
    
    if (emitSessionClosed) {
        emit q->closed();
    }

    return retVal;
}

void QNetworkSessionPrivate::handleSymbianConnectionStatusChange(TInt aConnectionStatus,
                                                                 TInt aError,
                                                                 TUint accessPointId)
{
    switch (aConnectionStatus)
        {
        // Connection unitialised
        case KConnectionUninitialised:
            break;

        // Starting connetion selection
        case KStartingSelection:
            break;

        // Selection finished
        case KFinishedSelection:
            if (aError == KErrNone)
                {
                // The user successfully selected an IAP to be used
                break;
                }
            else
                {
                // The user pressed e.g. "Cancel" and did not select an IAP
                newState(QNetworkSession::Disconnected,accessPointId);
                }
            break;

        // Connection failure
        case KConnectionFailure:
            newState(QNetworkSession::NotAvailable);
            break;

        // Prepearing connection (e.g. dialing)
        case KPsdStartingConfiguration:
        case KPsdFinishedConfiguration:
        case KCsdFinishedDialling:
        case KCsdScanningScript:
        case KCsdGettingLoginInfo:
        case KCsdGotLoginInfo:
            break;

        // Creating connection (e.g. GPRS activation)
        case KCsdStartingConnect:
        case KCsdFinishedConnect:
            newState(QNetworkSession::Connecting,accessPointId);
            break;

        // Starting log in
        case KCsdStartingLogIn:
            break;

        // Finished login
        case KCsdFinishedLogIn:
            break;

        // Connection open
        case KConnectionOpen:
            break;

        case KLinkLayerOpen:
            newState(QNetworkSession::Connected,accessPointId);
            break;

        // Connection blocked or suspended
        case KDataTransferTemporarilyBlocked:
            break;

        // Hangup or GRPS deactivation
        case KConnectionStartingClose:
            newState(QNetworkSession::Closing,accessPointId);
            break;

        // Connection closed
        case KConnectionClosed:
            break;

        case KLinkLayerClosed:
            newState(QNetworkSession::Disconnected,accessPointId);
            break;

        // Unhandled state
        default:
            break;
        }
}

void QNetworkSessionPrivate::EventL(const CConnMonEventBase& aEvent)
{
    switch (aEvent.EventType())
    {
        case EConnMonConnectionStatusChange:
            {
            CConnMonConnectionStatusChange* realEvent;
            realEvent = (CConnMonConnectionStatusChange*) &aEvent;

            TUint connectionId = realEvent->ConnectionId();
            TInt connectionStatus = realEvent->ConnectionStatus();

            // Try to Find IAP Id using connection Id
            TUint apId = 0;
            if (publicConfig.type() == QNetworkConfiguration::ServiceNetwork) {
                QList<QNetworkConfiguration> subConfigurations = publicConfig.children();
                for (int i = 0; i < subConfigurations.count(); i++ ) {
                    if (subConfigurations[i].d.data()->connectionId == connectionId) {
                        apId = subConfigurations[i].d.data()->numericId;
                        break;
                    }
                }
            } else if (publicConfig.type() == QNetworkConfiguration::InternetAccessPoint) {
                if (publicConfig.d.data()->connectionId == connectionId) {
                    apId = publicConfig.d.data()->numericId;
                }
            }

            if (apId > 0) {
                handleSymbianConnectionStatusChange(connectionStatus, KErrNone, apId);
            }
            }
            break;

        case EConnMonCreateConnection:
            {
            CConnMonCreateConnection* realEvent;
            realEvent = (CConnMonCreateConnection*) &aEvent;
            TUint apId;
            TUint connectionId = realEvent->ConnectionId();
            TRequestStatus status;
            iConnectionMonitor.GetUintAttribute(connectionId, 0, KIAPId, apId, status);
            User::WaitForRequest(status);
            if (status.Int() == KErrNone) {
                // Store connection id to related AccessPoint Configuration
                if (publicConfig.type() == QNetworkConfiguration::ServiceNetwork) {
                    QList<QNetworkConfiguration> subConfigurations = publicConfig.children();
                    for (int i = 0; i < subConfigurations.count(); i++ ) {
                        if (subConfigurations[i].d.data()->numericId == apId) {
                            subConfigurations[i].d.data()->connectionId = connectionId;
                            break;
                        }
                    }
                } else if (publicConfig.type() == QNetworkConfiguration::InternetAccessPoint) {
                    if (publicConfig.d.data()->numericId == apId) {
                        publicConfig.d.data()->connectionId = connectionId;
                    }
                }
            }
            }
            break;

        case EConnMonDeleteConnection:
            {
            CConnMonDeleteConnection* realEvent;
            realEvent = (CConnMonDeleteConnection*) &aEvent;
            TUint connectionId = realEvent->ConnectionId();
            // Remove connection id from related AccessPoint Configuration
            if (publicConfig.type() == QNetworkConfiguration::ServiceNetwork) {
                QList<QNetworkConfiguration> subConfigurations = publicConfig.children();
                for (int i = 0; i < subConfigurations.count(); i++ ) {
                    if (subConfigurations[i].d.data()->connectionId == connectionId) {
                        subConfigurations[i].d.data()->connectionId = 0;
                        break;
                    }
                }
            } else if (publicConfig.type() == QNetworkConfiguration::InternetAccessPoint) {
                if (publicConfig.d.data()->connectionId == connectionId) {
                    publicConfig.d.data()->connectionId = 0;
                }
            }
            }
            break;

        default:
            // For unrecognized events
            break;
    }
}

ConnectionProgressNotifier::ConnectionProgressNotifier(QNetworkSessionPrivate& owner, RConnection& connection)
    : CActive(CActive::EPriorityStandard), iOwner(owner), iConnection(connection)
{
    CActiveScheduler::Add(this);
}

ConnectionProgressNotifier::~ConnectionProgressNotifier()
{
    Cancel();
}

void ConnectionProgressNotifier::StartNotifications()
{
    if (!IsActive()) {
        SetActive();
    }
    iConnection.ProgressNotification(iProgress, iStatus);
}

void ConnectionProgressNotifier::StopNotifications()
{
    Cancel();
}

void ConnectionProgressNotifier::DoCancel()
{
    iConnection.CancelProgressNotification();
}

void ConnectionProgressNotifier::RunL()
{
    if (iStatus == KErrNone) {
        iOwner.handleSymbianConnectionStatusChange(iProgress().iStage, iProgress().iError);
    
        SetActive();
        iConnection.ProgressNotification(iProgress, iStatus);
    }
}

#include "moc_qnetworksession_s60_p.cpp"

QTM_END_NAMESPACE

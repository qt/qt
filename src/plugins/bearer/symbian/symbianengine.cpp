/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
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

#include "symbianengine.h"
#include "qnetworksession_impl.h"

#include <commdb.h>
#include <cdbcols.h>
#include <d32dbms.h>
#include <nifvar.h>
#include <QEventLoop>
#include <QTimer>
#include <QTime>  // For randgen seeding
#include <QtCore> // For randgen seeding


#ifdef QT_BEARERMGMT_SYMBIAN_DEBUG
#include <QDebug>
#endif

#ifdef SNAP_FUNCTIONALITY_AVAILABLE
    #include <cmdestination.h>
    #include <cmconnectionmethod.h>
    #include <cmconnectionmethoddef.h>
    #include <cmpluginwlandef.h>
    #include <cmpluginpacketdatadef.h>
    #include <cmplugindialcommondefs.h>
#else
    #include <apaccesspointitem.h>
    #include <apdatahandler.h>
    #include <aputils.h> 
#endif

#ifndef QT_NO_BEARERMANAGEMENT

QT_BEGIN_NAMESPACE

#ifdef SNAP_FUNCTIONALITY_AVAILABLE
    static const int KValueThatWillBeAddedToSNAPId = 1000;
#endif
static const int KUserChoiceIAPId = 0;

SymbianNetworkConfigurationPrivate::SymbianNetworkConfigurationPrivate()
:   bearer(BearerUnknown), numericId(0), connectionId(0)
{
}

SymbianNetworkConfigurationPrivate::~SymbianNetworkConfigurationPrivate()
{
}

QString SymbianNetworkConfigurationPrivate::bearerName() const
{
    QMutexLocker locker(&mutex);

    switch (bearer) {
    case BearerEthernet:
        return QLatin1String("Ethernet");
    case BearerWLAN:
        return QLatin1String("WLAN");
    case Bearer2G:
        return QLatin1String("2G");
    case BearerCDMA2000:
        return QLatin1String("CDMA2000");
    case BearerWCDMA:
        return QLatin1String("WCDMA");
    case BearerHSPA:
        return QLatin1String("HSPA");
    case BearerBluetooth:
        return QLatin1String("Bluetooth");
    case BearerWiMAX:
        return QLatin1String("WiMAX");
    default:
        return QString();
    }
}

SymbianEngine::SymbianEngine(QObject *parent)
:   QBearerEngine(parent), CActive(CActive::EPriorityIdle), iFirstUpdate(true), iInitOk(true),
    iIgnoringUpdates(false), iTimeToWait(0), iIgnoreEventLoop(0)
{
    CActiveScheduler::Add(this);

    // Seed the randomgenerator
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()) + QCoreApplication::applicationPid());
    iIgnoreEventLoop = new QEventLoop(this);

    TRAPD(error, ipCommsDB = CCommsDatabase::NewL(EDatabaseTypeIAP));
    if (error != KErrNone) {
        iInitOk = false;
        return;
    }

    TRAP_IGNORE(iConnectionMonitor.ConnectL());
    TRAP_IGNORE(iConnectionMonitor.NotifyEventL(*this));

#ifdef SNAP_FUNCTIONALITY_AVAILABLE    
    TRAP(error, iCmManager.OpenL());
    if (error != KErrNone) {
        iInitOk = false;
        return;
    }
#endif
}

void SymbianEngine::initialize()
{
    QMutexLocker locker(&mutex);

    SymbianNetworkConfigurationPrivate *cpPriv = new SymbianNetworkConfigurationPrivate;
    cpPriv->name = "UserChoice";
    cpPriv->bearer = SymbianNetworkConfigurationPrivate::BearerUnknown;
    cpPriv->state = QNetworkConfiguration::Discovered;
    cpPriv->isValid = true;
    cpPriv->id = QString::number(qHash(KUserChoiceIAPId));
    cpPriv->numericId = KUserChoiceIAPId;
    cpPriv->connectionId = 0;
    cpPriv->type = QNetworkConfiguration::UserChoice;
    cpPriv->purpose = QNetworkConfiguration::UnknownPurpose;
    cpPriv->roamingSupported = false;

    QNetworkConfigurationPrivatePointer ptr(cpPriv);
    userChoiceConfigurations.insert(ptr->id, ptr);

    updateConfigurations();
    updateStatesToSnaps();
    updateAvailableAccessPoints(); // On first time updates synchronously (without WLAN scans)
    // Start monitoring IAP and/or SNAP changes in Symbian CommsDB
    startCommsDatabaseNotifications();
    iFirstUpdate = false;
}

SymbianEngine::~SymbianEngine()
{
    Cancel();

    iConnectionMonitor.CancelNotifications();
    iConnectionMonitor.Close();
    
#ifdef SNAP_FUNCTIONALITY_AVAILABLE    
    iCmManager.Close();
#endif
    
    delete ipAccessPointsAvailabilityScanner;

    // CCommsDatabase destructor uses cleanup stack. Since QNetworkConfigurationManager
    // is a global static, but the time we are here, E32Main() has been exited already and
    // the thread's default cleanup stack has been deleted. Without this line, a
    // 'E32USER-CBase 69' -panic will occur.
    CTrapCleanup* cleanup = CTrapCleanup::New();
    delete ipCommsDB;
    delete cleanup;
}

bool SymbianEngine::hasIdentifier(const QString &id)
{
    QMutexLocker locker(&mutex);

    return accessPointConfigurations.contains(id) ||
           snapConfigurations.contains(id) ||
           userChoiceConfigurations.contains(id);
}

QNetworkConfigurationManager::Capabilities SymbianEngine::capabilities() const
{
    QNetworkConfigurationManager::Capabilities capFlags;

    capFlags = QNetworkConfigurationManager::CanStartAndStopInterfaces |
               QNetworkConfigurationManager::DirectConnectionRouting |
               QNetworkConfigurationManager::SystemSessionSupport |
               QNetworkConfigurationManager::DataStatistics |
               QNetworkConfigurationManager::NetworkSessionRequired;

#ifdef SNAP_FUNCTIONALITY_AVAILABLE
    capFlags |= QNetworkConfigurationManager::ApplicationLevelRoaming |
                QNetworkConfigurationManager::ForcedRoaming;
#endif

    return capFlags;
}

QNetworkSessionPrivate *SymbianEngine::createSessionBackend()
{
    return new QNetworkSessionPrivateImpl(this);
}

void SymbianEngine::requestUpdate()
{
    QMutexLocker locker(&mutex);

    if (!iInitOk || iUpdateGoingOn) {
        return;
    }
    iUpdateGoingOn = true;

    stopCommsDatabaseNotifications();
    updateConfigurations(); // Synchronous call
    updateAvailableAccessPoints(); // Asynchronous call
}

void SymbianEngine::updateConfigurations()
{
    QMutexLocker locker(&mutex);

    if (!iInitOk) {
        return;
    }

    TRAP_IGNORE(updateConfigurationsL());
}

void SymbianEngine::updateConfigurationsL()
{
    QMutexLocker locker(&mutex);

    QList<QString> knownConfigs = accessPointConfigurations.keys();
    QList<QString> knownSnapConfigs = snapConfigurations.keys();

#ifdef SNAP_FUNCTIONALITY_AVAILABLE    
    // S60 version is >= Series60 3rd Edition Feature Pack 2
    TInt error = KErrNone;
    
    // Loop through all IAPs
    RArray<TUint32> connectionMethods; // IAPs
    CleanupClosePushL(connectionMethods);
    iCmManager.ConnectionMethodL(connectionMethods);
    for(int i = 0; i < connectionMethods.Count(); i++) {
        RCmConnectionMethod connectionMethod = iCmManager.ConnectionMethodL(connectionMethods[i]);
        CleanupClosePushL(connectionMethod);
        TUint32 iapId = connectionMethod.GetIntAttributeL(CMManager::ECmIapId);
        QString ident = QString::number(qHash(iapId));
        if (accessPointConfigurations.contains(ident)) {
            knownConfigs.removeOne(ident);
        } else {
            SymbianNetworkConfigurationPrivate* cpPriv = NULL;
            TRAP(error, cpPriv = configFromConnectionMethodL(connectionMethod));
            if (error == KErrNone) {
                QNetworkConfigurationPrivatePointer ptr(cpPriv);
                accessPointConfigurations.insert(ptr->id, ptr);

                locker.unlock();
                emit configurationAdded(ptr);
                locker.relock();
            }
        }
        CleanupStack::PopAndDestroy(&connectionMethod);
    }
    CleanupStack::PopAndDestroy(&connectionMethods);
    
    // Loop through all SNAPs
    RArray<TUint32> destinations;
    CleanupClosePushL(destinations);
    iCmManager.AllDestinationsL(destinations);
    for(int i = 0; i < destinations.Count(); i++) {
        RCmDestination destination;
        destination = iCmManager.DestinationL(destinations[i]);
        CleanupClosePushL(destination);
        QString ident = QString::number(qHash(destination.Id()+KValueThatWillBeAddedToSNAPId)); //TODO: Check if it's ok to add 1000 SNAP Id to prevent SNAP ids overlapping IAP ids
        if (snapConfigurations.contains(ident)) {
            knownSnapConfigs.removeOne(ident);
        } else {
            SymbianNetworkConfigurationPrivate *cpPriv = new SymbianNetworkConfigurationPrivate;
            CleanupStack::PushL(cpPriv);
    
            HBufC *pName = destination.NameLC();
            cpPriv->name = QString::fromUtf16(pName->Ptr(),pName->Length());
            CleanupStack::PopAndDestroy(pName);
            pName = NULL;
    
            cpPriv->isValid = true;
            cpPriv->id = ident;
            cpPriv->numericId = destination.Id();
            cpPriv->connectionId = 0;
            cpPriv->state = QNetworkConfiguration::Defined;
            cpPriv->type = QNetworkConfiguration::ServiceNetwork;
            cpPriv->purpose = QNetworkConfiguration::UnknownPurpose;
            cpPriv->roamingSupported = false;

            QNetworkConfigurationPrivatePointer ptr(cpPriv);
            snapConfigurations.insert(ident, ptr);

            locker.unlock();
            emit configurationAdded(ptr);
            locker.relock();
            
            CleanupStack::Pop(cpPriv);
        }
        QNetworkConfigurationPrivatePointer privSNAP = snapConfigurations.value(ident);
            
        for (int j=0; j < destination.ConnectionMethodCount(); j++) {
            RCmConnectionMethod connectionMethod = destination.ConnectionMethodL(j);
            CleanupClosePushL(connectionMethod);
            
            TUint32 iapId = connectionMethod.GetIntAttributeL(CMManager::ECmIapId);
            QString iface = QString::number(qHash(iapId));
            // Check that IAP can be found from accessPointConfigurations list
            QNetworkConfigurationPrivatePointer priv = accessPointConfigurations.value(iface);
            if (!priv) {
                SymbianNetworkConfigurationPrivate *cpPriv = NULL;
                TRAP(error, cpPriv = configFromConnectionMethodL(connectionMethod));
                if (error == KErrNone) {
                    QNetworkConfigurationPrivatePointer ptr(cpPriv);
                    toSymbianConfig(ptr)->serviceNetworkPtr = privSNAP;
                    accessPointConfigurations.insert(ptr->id, ptr);

                    locker.unlock();
                    emit configurationAdded(ptr);
                    locker.relock();

                    privSNAP->serviceNetworkMembers.append(ptr);
                }
            } else {
                knownConfigs.removeOne(iface);
                // Check that IAP can be found from related SNAP's configuration list
                bool iapFound = false;
                for (int i = 0; i < privSNAP->serviceNetworkMembers.count(); i++) {
                    if (toSymbianConfig(privSNAP->serviceNetworkMembers[i])->numericId == iapId) {
                        iapFound = true;
                        break;
                    }
                }
                if (!iapFound) {
                    toSymbianConfig(priv)->serviceNetworkPtr = privSNAP;
                    privSNAP->serviceNetworkMembers.append(priv);
                }
            }
            
            CleanupStack::PopAndDestroy(&connectionMethod);
        }
        
        if (privSNAP->serviceNetworkMembers.count() > 1) {
            // Roaming is supported only if SNAP contains more than one IAP
            privSNAP->roamingSupported = true;
        }
        
        CleanupStack::PopAndDestroy(&destination);
    }
    CleanupStack::PopAndDestroy(&destinations);
    
#else
    // S60 version is < Series60 3rd Edition Feature Pack 2
    CCommsDbTableView* pDbTView = ipCommsDB->OpenTableLC(TPtrC(IAP));

    // Loop through all IAPs
    TUint32 apId = 0;
    TInt retVal = pDbTView->GotoFirstRecord();
    while (retVal == KErrNone) {
        pDbTView->ReadUintL(TPtrC(COMMDB_ID), apId);
        QString ident = QString::number(qHash(apId));
        if (accessPointConfigurations.contains(ident)) {
            knownConfigs.removeOne(ident);
        } else {
            SymbianNetworkConfigurationPrivate *cpPriv = new SymbianNetworkConfigurationPrivate;
            if (readNetworkConfigurationValuesFromCommsDb(apId, cpPriv)) {
                QNetworkConfigurationPrivatePointer ptr(cpPriv);
                accessPointConfigurations.insert(ident, ptr);

                locker.unlock();
                emit configurationAdded(ptr);
                locker.relock();
            } else {
                delete cpPriv;
            }
        }
        retVal = pDbTView->GotoNextRecord();
    }
    CleanupStack::PopAndDestroy(pDbTView);
#endif
    updateActiveAccessPoints();
    
    foreach (const QString &oldIface, knownConfigs) {
        //remove non existing IAP
        QNetworkConfigurationPrivatePointer ptr = accessPointConfigurations.take(oldIface);

        locker.unlock();
        emit configurationRemoved(ptr);
        locker.relock();

        // Remove non existing IAP from SNAPs
        foreach (const QString &iface, snapConfigurations.keys()) {
            QNetworkConfigurationPrivatePointer ptr2 = snapConfigurations.value(iface);
            // => Check if one of the IAPs of the SNAP is active
            for (int i = 0; i < ptr2->serviceNetworkMembers.count(); ++i) {
                if (toSymbianConfig(ptr2->serviceNetworkMembers[i])->numericId ==
                    toSymbianConfig(ptr)->numericId) {
                    ptr2->serviceNetworkMembers.removeAt(i);
                    break;
                }
            }
        }    
    }

    foreach (const QString &oldIface, knownSnapConfigs) {
        //remove non existing SNAPs
        QNetworkConfigurationPrivatePointer ptr = snapConfigurations.take(oldIface);

        locker.unlock();
        emit configurationRemoved(ptr);
        locker.relock();
    }
}

#ifdef SNAP_FUNCTIONALITY_AVAILABLE
SymbianNetworkConfigurationPrivate *SymbianEngine::configFromConnectionMethodL(
        RCmConnectionMethod& connectionMethod)
{
    QMutexLocker locker(&mutex);

    SymbianNetworkConfigurationPrivate *cpPriv = new SymbianNetworkConfigurationPrivate;
    CleanupStack::PushL(cpPriv);
    
    TUint32 iapId = connectionMethod.GetIntAttributeL(CMManager::ECmIapId);
    QString ident = QString::number(qHash(iapId));
    
    HBufC *pName = connectionMethod.GetStringAttributeL(CMManager::ECmName);
    CleanupStack::PushL(pName);
    cpPriv->name = QString::fromUtf16(pName->Ptr(),pName->Length());
    CleanupStack::PopAndDestroy(pName);
    pName = NULL;
    
    TUint32 bearerId = connectionMethod.GetIntAttributeL(CMManager::ECmCommsDBBearerType);
    switch (bearerId) {
    case KCommDbBearerCSD:
        cpPriv->bearer = SymbianNetworkConfigurationPrivate::Bearer2G;
        break;
    case KCommDbBearerWcdma:
        cpPriv->bearer = SymbianNetworkConfigurationPrivate::BearerWCDMA;
        break;
    case KCommDbBearerLAN:
        cpPriv->bearer = SymbianNetworkConfigurationPrivate::BearerEthernet;
        break;
    case KCommDbBearerVirtual:
        cpPriv->bearer = SymbianNetworkConfigurationPrivate::BearerUnknown;
        break;
    case KCommDbBearerPAN:
        cpPriv->bearer = SymbianNetworkConfigurationPrivate::BearerUnknown;
        break;
    case KCommDbBearerWLAN:
        cpPriv->bearer = SymbianNetworkConfigurationPrivate::BearerWLAN;
        break;
    default:
        cpPriv->bearer = SymbianNetworkConfigurationPrivate::BearerUnknown;
        break;
    }
    
    TInt error = KErrNone;
    TUint32 bearerType = connectionMethod.GetIntAttributeL(CMManager::ECmBearerType);
    switch (bearerType) {
    case KUidPacketDataBearerType:
        // "Packet data" Bearer => Mapping is done using "Access point name"
        TRAP(error, pName = connectionMethod.GetStringAttributeL(CMManager::EPacketDataAPName));
        break;
    case KUidWlanBearerType:
        // "Wireless LAN" Bearer => Mapping is done using "WLAN network name" = SSID
        TRAP(error, pName = connectionMethod.GetStringAttributeL(CMManager::EWlanSSID));
        break;
    }
    if (!pName) {
        // "Data call" Bearer or "High Speed (GSM)" Bearer => Mapping is done using "Dial-up number"
        TRAP(error, pName = connectionMethod.GetStringAttributeL(CMManager::EDialDefaultTelNum));
    }

    if (error == KErrNone && pName) {
        CleanupStack::PushL(pName);
        cpPriv->mappingName = QString::fromUtf16(pName->Ptr(),pName->Length());
        CleanupStack::PopAndDestroy(pName);
        pName = NULL;
    }
 
    cpPriv->state = QNetworkConfiguration::Defined;
    TBool isConnected = connectionMethod.GetBoolAttributeL(CMManager::ECmConnected);
    if (isConnected) {
        cpPriv->state = QNetworkConfiguration::Active;
    }
    
    cpPriv->isValid = true;
    cpPriv->id = ident;
    cpPriv->numericId = iapId;
    cpPriv->connectionId = 0;
    cpPriv->type = QNetworkConfiguration::InternetAccessPoint;
    cpPriv->purpose = QNetworkConfiguration::UnknownPurpose;
    cpPriv->roamingSupported = false;
    
    CleanupStack::Pop(cpPriv);
    return cpPriv;
}
#else
bool SymbianEngine::readNetworkConfigurationValuesFromCommsDb(
        TUint32 aApId, SymbianNetworkConfigurationPrivate *apNetworkConfiguration)
{
    QMutexLocker locker(&mutex);

    TRAPD(error, readNetworkConfigurationValuesFromCommsDbL(aApId,apNetworkConfiguration));
    if (error != KErrNone) {
        return false;        
    }
    return true;
}

void SymbianEngine::readNetworkConfigurationValuesFromCommsDbL(
        TUint32 aApId, SymbianNetworkConfigurationPrivate *apNetworkConfiguration)
{
    QMutexLocker locker(&mutex);

    CApDataHandler* pDataHandler = CApDataHandler::NewLC(*ipCommsDB); 
    CApAccessPointItem* pAPItem = CApAccessPointItem::NewLC(); 
    TBuf<KCommsDbSvrMaxColumnNameLength> name;
    
    CApUtils* pApUtils = CApUtils::NewLC(*ipCommsDB);
    TUint32 apId = pApUtils->WapIdFromIapIdL(aApId);
    
    pDataHandler->AccessPointDataL(apId,*pAPItem);
    pAPItem->ReadTextL(EApIapName, name);
    if (name.Compare(_L("Easy WLAN")) == 0) {
        // "Easy WLAN" won't be accepted to the Configurations list
        User::Leave(KErrNotFound);
    }
    
    QString ident = QString::number(qHash(aApId));
    
    apNetworkConfiguration->name = QString::fromUtf16(name.Ptr(),name.Length());
    apNetworkConfiguration->isValid = true;
    apNetworkConfiguration->id = ident;
    apNetworkConfiguration->numericId = aApId;
    apNetworkConfiguration->connectionId = 0;
    apNetworkConfiguration->state = (QNetworkConfiguration::Defined);
    apNetworkConfiguration->type = QNetworkConfiguration::InternetAccessPoint;
    apNetworkConfiguration->purpose = QNetworkConfiguration::UnknownPurpose;
    apNetworkConfiguration->roamingSupported = false;
    switch (pAPItem->BearerTypeL()) {
    case EApBearerTypeCSD:      
        apNetworkConfiguration->bearer = SymbianNetworkConfigurationPrivate::Bearer2G;
        break;
    case EApBearerTypeGPRS:
        apNetworkConfiguration->bearer = SymbianNetworkConfigurationPrivate::Bearer2G;
        break;
    case EApBearerTypeHSCSD:
        apNetworkConfiguration->bearer = SymbianNetworkConfigurationPrivate::BearerHSPA;
        break;
    case EApBearerTypeCDMA:
        apNetworkConfiguration->bearer = SymbianNetworkConfigurationPrivate::BearerCDMA2000;
        break;
    case EApBearerTypeWLAN:
        apNetworkConfiguration->bearer = SymbianNetworkConfigurationPrivate::BearerWLAN;
        break;
    case EApBearerTypeLAN:
        apNetworkConfiguration->bearer = SymbianNetworkConfigurationPrivate::BearerEthernet;
        break;
    case EApBearerTypeLANModem:
        apNetworkConfiguration->bearer = SymbianNetworkConfigurationPrivate::BearerEthernet;
        break;
    default:
        apNetworkConfiguration->bearer = SymbianNetworkConfigurationPrivate::BearerUnknown;
        break;
    }
    
    CleanupStack::PopAndDestroy(pApUtils);
    CleanupStack::PopAndDestroy(pAPItem);
    CleanupStack::PopAndDestroy(pDataHandler);
}
#endif

QNetworkConfigurationPrivatePointer SymbianEngine::defaultConfiguration()
{
    QMutexLocker locker(&mutex);

    QNetworkConfigurationPrivatePointer ptr;

    if (iInitOk) {
        stopCommsDatabaseNotifications();
        TRAP_IGNORE(ptr = defaultConfigurationL());
        startCommsDatabaseNotifications();
    }

    return ptr;
}

QStringList SymbianEngine::accessPointConfigurationIdentifiers()
{
    QMutexLocker locker(&mutex);

    return accessPointConfigurations.keys();
}

QNetworkConfigurationPrivatePointer SymbianEngine::defaultConfigurationL()
{
    QMutexLocker locker(&mutex);

    QNetworkConfigurationPrivatePointer ptr;

#ifdef SNAP_FUNCTIONALITY_AVAILABLE
    // Check Default Connection (SNAP or IAP)
    TCmDefConnValue defaultConnectionValue;
    iCmManager.ReadDefConnL(defaultConnectionValue);
    if (defaultConnectionValue.iType == ECmDefConnDestination) {
        QString iface = QString::number(qHash(defaultConnectionValue.iId+KValueThatWillBeAddedToSNAPId));
        ptr = snapConfigurations.value(iface);
    } else if (defaultConnectionValue.iType == ECmDefConnConnectionMethod) {
        QString iface = QString::number(qHash(defaultConnectionValue.iId));
        ptr = accessPointConfigurations.value(iface);
    }
#endif
    
    if (!ptr || !ptr->isValid) {
        QString iface = QString::number(qHash(KUserChoiceIAPId));
        ptr = userChoiceConfigurations.value(iface);
    }
    
    return ptr;
}

void SymbianEngine::updateActiveAccessPoints()
{
    QMutexLocker locker(&mutex);

    bool online = false;
    QList<QString> inactiveConfigs = accessPointConfigurations.keys();

    TRequestStatus status;
    TUint connectionCount;
    iConnectionMonitor.GetConnectionCount(connectionCount, status);
    User::WaitForRequest(status);
    
    // Go through all connections and set state of related IAPs to Active.
    // Status needs to be checked carefully, because ConnMon lists also e.g.
    // WLAN connections that are being currently tried --> we don't want to
    // state these as active.
    TUint connectionId;
    TUint subConnectionCount;
    TUint apId;
    TInt connectionStatus;
    if (status.Int() == KErrNone) {
        for (TUint i = 1; i <= connectionCount; i++) {
            iConnectionMonitor.GetConnectionInfo(i, connectionId, subConnectionCount);
            iConnectionMonitor.GetUintAttribute(connectionId, subConnectionCount, KIAPId, apId, status);
            User::WaitForRequest(status);
            QString ident = QString::number(qHash(apId));
            QNetworkConfigurationPrivatePointer ptr = accessPointConfigurations.value(ident);
            if (ptr) {
                iConnectionMonitor.GetIntAttribute(connectionId, subConnectionCount, KConnectionStatus, connectionStatus, status);
                User::WaitForRequest(status);          
                if (connectionStatus == KLinkLayerOpen) {
                    online = true;
                    inactiveConfigs.removeOne(ident);

                    QMutexLocker configLocker(&ptr->mutex);
                    toSymbianConfig(ptr)->connectionId = connectionId;

                    // Configuration is Active
                    changeConfigurationStateTo(ptr, QNetworkConfiguration::Active);
                }
            }
        }
    }

    // Make sure that state of rest of the IAPs won't be Active
    foreach (const QString &iface, inactiveConfigs) {
        QNetworkConfigurationPrivatePointer ptr = accessPointConfigurations.value(iface);
        if (ptr) {
            // Configuration is either Defined or Discovered
            changeConfigurationStateAtMaxTo(ptr, QNetworkConfiguration::Discovered);
        }
    }

    if (iOnline != online) {
        iOnline = online;
        locker.unlock();
        emit this->onlineStateChanged(iOnline);
        locker.relock();
    }
}

void SymbianEngine::updateAvailableAccessPoints()
{
    QMutexLocker locker(&mutex);

    if (!ipAccessPointsAvailabilityScanner) {
        ipAccessPointsAvailabilityScanner = new AccessPointsAvailabilityScanner(*this, iConnectionMonitor);
    }
    if (ipAccessPointsAvailabilityScanner) {
        // Scanning may take a while because WLAN scanning will be done (if device supports WLAN).
        ipAccessPointsAvailabilityScanner->StartScanning();
    }
}

void SymbianEngine::accessPointScanningReady(TBool scanSuccessful, TConnMonIapInfo iapInfo)
{
    QMutexLocker locker(&mutex);

    iUpdateGoingOn = false;
    if (scanSuccessful) {
        QList<QString> unavailableConfigs = accessPointConfigurations.keys();
        
        // Set state of returned IAPs to Discovered
        // if state is not already Active
        for(TUint i=0; i<iapInfo.iCount; i++) {
            QString ident = QString::number(qHash(iapInfo.iIap[i].iIapId));
            QNetworkConfigurationPrivatePointer ptr = accessPointConfigurations.value(ident);
            if (ptr) {
                unavailableConfigs.removeOne(ident);
                if (ptr->state < QNetworkConfiguration::Active) {
                    // Configuration is either Discovered or Active
                    changeConfigurationStateAtMinTo(ptr, QNetworkConfiguration::Discovered);
                }
            }
        }
        
        // Make sure that state of rest of the IAPs won't be Active
        foreach (const QString &iface, unavailableConfigs) {
            QNetworkConfigurationPrivatePointer ptr = accessPointConfigurations.value(iface);
            if (ptr) {
                // Configuration is Defined
                changeConfigurationStateAtMaxTo(ptr, QNetworkConfiguration::Discovered);
            }
        }
    }

    updateStatesToSnaps();
    
    if (!iFirstUpdate) {
        startCommsDatabaseNotifications();
        locker.unlock();
        emit updateCompleted();
        locker.relock();
    }
}

void SymbianEngine::updateStatesToSnaps()
{
    QMutexLocker locker(&mutex);

    // Go through SNAPs and set correct state to SNAPs
    foreach (const QString &iface, snapConfigurations.keys()) {
        bool discovered = false;
        bool active = false;
        QNetworkConfigurationPrivatePointer ptr = snapConfigurations.value(iface);
        // => Check if one of the IAPs of the SNAP is discovered or active
        //    => If one of IAPs is active, also SNAP is active
        //    => If one of IAPs is discovered but none of the IAPs is active, SNAP is discovered
        for (int i=0; i<ptr->serviceNetworkMembers.count(); i++) {
            if ((ptr->serviceNetworkMembers[i]->state & QNetworkConfiguration::Active)
                    == QNetworkConfiguration::Active) {
                active = true;
                break;
            } else if ((ptr->serviceNetworkMembers[i]->state & QNetworkConfiguration::Discovered)
                        == QNetworkConfiguration::Discovered) {
                discovered = true;
            }
        }
        if (active) {
            changeConfigurationStateTo(ptr, QNetworkConfiguration::Active);
        } else if (discovered) {
            changeConfigurationStateTo(ptr, QNetworkConfiguration::Discovered);
        } else {
            changeConfigurationStateTo(ptr, QNetworkConfiguration::Defined);
        }
    }    
}

bool SymbianEngine::changeConfigurationStateTo(QNetworkConfigurationPrivatePointer ptr,
                                               QNetworkConfiguration::StateFlags newState)
{
    QMutexLocker locker(&mutex);

    ptr->mutex.lock();
    if (newState != ptr->state) {
        ptr->state = newState;
        ptr->mutex.unlock();

        locker.unlock();
        emit configurationChanged(ptr);
        locker.relock();

        return true;
    } else {
        ptr->mutex.unlock();
    }
    return false;
}

/* changeConfigurationStateAtMinTo function does not overwrite possible better
 * state (e.g. Discovered state does not overwrite Active state) but
 * makes sure that state is at minimum given state.
*/
bool SymbianEngine::changeConfigurationStateAtMinTo(QNetworkConfigurationPrivatePointer ptr,
                                                    QNetworkConfiguration::StateFlags newState)
{
    QMutexLocker locker(&mutex);

    ptr->mutex.lock();
    if ((newState | ptr->state) != ptr->state) {
        ptr->state = (ptr->state | newState);
        ptr->mutex.unlock();

        locker.unlock();
        emit configurationChanged(ptr);
        locker.relock();

        return true;
    } else {
        ptr->mutex.unlock();
    }
    return false;
}

/* changeConfigurationStateAtMaxTo function overwrites possible better
 * state (e.g. Discovered state overwrites Active state) and
 * makes sure that state is at maximum given state (e.g. Discovered state
 * does not overwrite Defined state).
*/
bool SymbianEngine::changeConfigurationStateAtMaxTo(QNetworkConfigurationPrivatePointer ptr,
                                                    QNetworkConfiguration::StateFlags newState)
{
    QMutexLocker locker(&mutex);

    ptr->mutex.lock();
    if ((newState & ptr->state) != ptr->state) {
        ptr->state = (newState & ptr->state);
        ptr->mutex.unlock();

        locker.unlock();
        emit configurationChanged(ptr);
        locker.relock();

        return true;
    } else {
        ptr->mutex.unlock();
    }
    return false;
}

void SymbianEngine::startCommsDatabaseNotifications()
{
    QMutexLocker locker(&mutex);

    if (!iWaitingCommsDatabaseNotifications) {
        iWaitingCommsDatabaseNotifications = ETrue;
        if (!IsActive()) {
            SetActive();
            // Start waiting for new notification
            ipCommsDB->RequestNotification(iStatus);
        }
    }
}

void SymbianEngine::stopCommsDatabaseNotifications()
{
    QMutexLocker locker(&mutex);

    if (iWaitingCommsDatabaseNotifications) {
        iWaitingCommsDatabaseNotifications = EFalse;
        if (!IsActive()) {
            SetActive();
            // Make sure that notifier recorded events will not be returned
            // as soon as the client issues the next RequestNotification() request.
            ipCommsDB->RequestNotification(iStatus);
            ipCommsDB->CancelRequestNotification();
        } else {
            ipCommsDB->CancelRequestNotification();
        }
    }
}

void SymbianEngine::RunL()
{
    QMutexLocker locker(&mutex);

    if (iIgnoringUpdates) {
#ifdef QT_BEARERMGMT_SYMBIAN_DEBUG
        qDebug("QNCM CommsDB event handling postponed (postpone-timer running because IAPs/SNAPs were updated very recently).");
#endif
        return;
    }

    if (iStatus != KErrCancel) {
        RDbNotifier::TEvent event = STATIC_CAST(RDbNotifier::TEvent, iStatus.Int());

        switch (event) {
        case RDbNotifier::EUnlock:   /** All read locks have been removed.  */
        case RDbNotifier::ECommit:   /** A transaction has been committed.  */ 
        case RDbNotifier::ERollback: /** A transaction has been rolled back */
        case RDbNotifier::ERecover:  /** The database has been recovered    */
#ifdef QT_BEARERMGMT_SYMBIAN_DEBUG
            qDebug("QNCM CommsDB event (of type RDbNotifier::TEvent) received: %d", iStatus.Int());
#endif
            iIgnoringUpdates = true;
            // Other events than ECommit get lower priority. In practice with those events,
            // we delay_before_updating methods, whereas
            // with ECommit we _update_before_delaying the reaction to next event.
            // Few important notes: 1) listening to only ECommit does not seem to be adequate,
            // but updates will be missed. Hence other events are reacted upon too.
            // 2) RDbNotifier records the most significant event, and that will be returned once
            // we issue new RequestNotification, and hence updates will not be missed even
            // when we are 'not reacting to them' for few seconds.
            if (event == RDbNotifier::ECommit) {
                TRAPD(error, updateConfigurationsL());
                if (error == KErrNone) {
                    updateStatesToSnaps();
                }
                waitRandomTime();
            } else {
                waitRandomTime();
                TRAPD(error, updateConfigurationsL());
                if (error == KErrNone) {
                    updateStatesToSnaps();
                }   
            }
            iIgnoringUpdates = false; // Wait time done, allow updating again
            iWaitingCommsDatabaseNotifications = true;
            break;
        default:
            // Do nothing
            break;
        }
    }

    if (iWaitingCommsDatabaseNotifications) {
        if (!IsActive()) {
            SetActive();
            // Start waiting for new notification
            ipCommsDB->RequestNotification(iStatus);
        }
    }
}

void SymbianEngine::DoCancel()
{
    QMutexLocker locker(&mutex);

    ipCommsDB->CancelRequestNotification();
}

void SymbianEngine::EventL(const CConnMonEventBase& aEvent)
{
    QMutexLocker locker(&mutex);

    switch (aEvent.EventType()) {
    case EConnMonConnectionStatusChange:
        {
        CConnMonConnectionStatusChange* realEvent;
        realEvent = (CConnMonConnectionStatusChange*) &aEvent;
        TInt connectionStatus = realEvent->ConnectionStatus();
#ifdef QT_BEARERMGMT_SYMBIAN_DEBUG
        qDebug() << "QNCM Connection status : " << QString::number(connectionStatus) << " , connection monitor Id : " << realEvent->ConnectionId();
#endif
        if (connectionStatus == KConfigDaemonStartingRegistration) {
            TUint connectionId = realEvent->ConnectionId();
            TUint subConnectionCount = 0;
            TUint apId;            
            TRequestStatus status;
            iConnectionMonitor.GetUintAttribute(connectionId, subConnectionCount, KIAPId, apId, status);
            User::WaitForRequest(status);
            QString ident = QString::number(qHash(apId));
            QNetworkConfigurationPrivatePointer ptr = accessPointConfigurations.value(ident);
            if (ptr) {
                QMutexLocker configLocker(&ptr->mutex);
                toSymbianConfig(ptr)->connectionId = connectionId;
                emit this->configurationStateChanged(toSymbianConfig(ptr)->numericId, connectionId, QNetworkSession::Connecting);
            }
        } else if (connectionStatus == KLinkLayerOpen) {
            // Connection has been successfully opened
            TUint connectionId = realEvent->ConnectionId();
            TUint subConnectionCount = 0;
            TUint apId;            
            TRequestStatus status;
            iConnectionMonitor.GetUintAttribute(connectionId, subConnectionCount, KIAPId, apId, status);
            User::WaitForRequest(status);
            QString ident = QString::number(qHash(apId));
            QNetworkConfigurationPrivatePointer ptr = accessPointConfigurations.value(ident);
            if (ptr) {
                QMutexLocker configLocker(&ptr->mutex);
                toSymbianConfig(ptr)->connectionId = connectionId;
                // Configuration is Active
                if (changeConfigurationStateTo(ptr, QNetworkConfiguration::Active)) {
                    updateStatesToSnaps();
                }
                emit this->configurationStateChanged(toSymbianConfig(ptr)->numericId, connectionId, QNetworkSession::Connected);
                if (!iOnline) {
                    iOnline = true;
                    emit this->onlineStateChanged(iOnline);
                }
            }
        } else if (connectionStatus == KConfigDaemonStartingDeregistration) {
            TUint connectionId = realEvent->ConnectionId();
            QNetworkConfigurationPrivatePointer ptr = dataByConnectionId(connectionId);
            if (ptr) {
                QMutexLocker configLocker(&ptr->mutex);
                emit this->configurationStateChanged(toSymbianConfig(ptr)->numericId, connectionId, QNetworkSession::Closing);
            }
        } else if (connectionStatus == KLinkLayerClosed ||
                   connectionStatus == KConnectionClosed) {
            // Connection has been closed. Which of the above events is reported, depends on the Symbian
            // platform.
            TUint connectionId = realEvent->ConnectionId();
            QNetworkConfigurationPrivatePointer ptr = dataByConnectionId(connectionId);
            if (ptr) {
                // Configuration is either Defined or Discovered
                if (changeConfigurationStateAtMaxTo(ptr, QNetworkConfiguration::Discovered)) {
                    updateStatesToSnaps();
                }

                QMutexLocker configLocker(&ptr->mutex);
                emit this->configurationStateChanged(toSymbianConfig(ptr)->numericId, connectionId, QNetworkSession::Disconnected);
            }
            
            bool online = false;
            foreach (const QString &iface, accessPointConfigurations.keys()) {
                QNetworkConfigurationPrivatePointer ptr = accessPointConfigurations.value(iface);
                QMutexLocker configLocker(&ptr->mutex);
                if (ptr->state == QNetworkConfiguration::Active) {
                    online = true;
                    break;
                }
            }
            if (iOnline != online) {
                iOnline = online;
                emit this->onlineStateChanged(iOnline);
            }
        }
        }
        break;    

    case EConnMonIapAvailabilityChange:
        {
        CConnMonIapAvailabilityChange* realEvent;
        realEvent = (CConnMonIapAvailabilityChange*) &aEvent;
        TConnMonIapInfo iaps = realEvent->IapAvailability();
        QList<QString> unDiscoveredConfigs = accessPointConfigurations.keys();
        for ( TUint i = 0; i < iaps.Count(); i++ ) {
            QString ident = QString::number(qHash(iaps.iIap[i].iIapId));

            QNetworkConfigurationPrivatePointer ptr = accessPointConfigurations.value(ident);
            if (ptr) {
                // Configuration is either Discovered or Active 
                changeConfigurationStateAtMinTo(ptr, QNetworkConfiguration::Discovered);
                unDiscoveredConfigs.removeOne(ident);
            }
        }
        foreach (const QString &iface, unDiscoveredConfigs) {
            QNetworkConfigurationPrivatePointer ptr = accessPointConfigurations.value(iface);
            if (ptr) {
                // Configuration is Defined
                changeConfigurationStateAtMaxTo(ptr, QNetworkConfiguration::Defined);
            }
        }
        }
        break;

    case EConnMonCreateConnection:
        {
        // This event is caught to keep connection monitor IDs up-to-date.
        CConnMonCreateConnection* realEvent;
        realEvent = (CConnMonCreateConnection*) &aEvent;
        TUint subConnectionCount = 0;
        TUint apId;
        TUint connectionId = realEvent->ConnectionId();
        TRequestStatus status;
        iConnectionMonitor.GetUintAttribute(connectionId, subConnectionCount, KIAPId, apId, status);
        User::WaitForRequest(status);
        QString ident = QString::number(qHash(apId));
        QNetworkConfigurationPrivatePointer ptr = accessPointConfigurations.value(ident);
        if (ptr) {
            QMutexLocker configLocker(&ptr->mutex);
#ifdef QT_BEARERMGMT_SYMBIAN_DEBUG
            qDebug() << "QNCM updating connection monitor ID : from, to, whose: " << toSymbianConfig(ptr)->connectionId << connectionId << ptr->name;
#endif
            toSymbianConfig(ptr)->connectionId = connectionId;
        }
        }
        break;
    default:
        // For unrecognized events
        break;
    }
}

// Sessions may use this function to report configuration state changes,
// because on some Symbian platforms (especially Symbian^3) all state changes are not
// reported by the RConnectionMonitor, in particular in relation to stop() call,
// whereas they _are_ reported on RConnection progress notifier used by sessions --> centralize
// this data here so that other sessions may benefit from it too (not all sessions necessarily have
// RConnection progress notifiers available but they relay on having e.g. disconnected information from
// manager). Currently only 'Disconnected' state is of interest because it has proven to be troublesome.
void SymbianEngine::configurationStateChangeReport(TUint32 accessPointId, QNetworkSession::State newState)
{
#ifdef QT_BEARERMGMT_SYMBIAN_DEBUG
    qDebug() << "QNCM A session reported state change for IAP ID: " << accessPointId << " whose new state is: " << newState;
#endif
    switch (newState) {
    case QNetworkSession::Disconnected:
        {
            QString ident = QString::number(qHash(accessPointId));
            QNetworkConfigurationPrivatePointer ptr = accessPointConfigurations.value(ident);
            if (ptr) {
                // Configuration is either Defined or Discovered
                if (changeConfigurationStateAtMaxTo(ptr, QNetworkConfiguration::Discovered)) {
                    updateStatesToSnaps();
                }

                QMutexLocker configLocker(&ptr->mutex);
                emit this->configurationStateChanged(toSymbianConfig(ptr)->numericId,
                                                     toSymbianConfig(ptr)->connectionId,
                                                     QNetworkSession::Disconnected);
            }
        }
        break;
    default:
        break;
    }
}

// Waits for 2..6 seconds.
void SymbianEngine::waitRandomTime()
{
    iTimeToWait = (qAbs(qrand()) % 7) * 1000;
    if (iTimeToWait < 2000) {
        iTimeToWait = 2000;
    }
#ifdef QT_BEARERMGMT_SYMBIAN_DEBUG
    qDebug("QNCM waiting random time: %d ms", iTimeToWait);
#endif
    QTimer::singleShot(iTimeToWait, iIgnoreEventLoop, SLOT(quit()));
    iIgnoreEventLoop->exec();
}

QNetworkConfigurationPrivatePointer SymbianEngine::dataByConnectionId(TUint aConnectionId)
{
    QMutexLocker locker(&mutex);

    QNetworkConfiguration item;
    QHash<QString, QNetworkConfigurationPrivatePointer>::const_iterator i =
            accessPointConfigurations.constBegin();
    while (i != accessPointConfigurations.constEnd()) {
        QNetworkConfigurationPrivatePointer ptr = i.value();
        QMutexLocker configLocker(&ptr->mutex);
        if (toSymbianConfig(ptr)->connectionId == aConnectionId)
            return ptr;

        ++i;
    }

    return QNetworkConfigurationPrivatePointer();
}

AccessPointsAvailabilityScanner::AccessPointsAvailabilityScanner(SymbianEngine& owner,
                                                               RConnectionMonitor& connectionMonitor)
    : CActive(CActive::EPriorityStandard), iOwner(owner), iConnectionMonitor(connectionMonitor)
{
    CActiveScheduler::Add(this);  
}

AccessPointsAvailabilityScanner::~AccessPointsAvailabilityScanner()
{
    Cancel();
}

void AccessPointsAvailabilityScanner::DoCancel()
{
    iConnectionMonitor.CancelAsyncRequest(EConnMonGetPckgAttribute);
}

void AccessPointsAvailabilityScanner::StartScanning()
{
    if (iOwner.iFirstUpdate) {
        // On first update (the mgr is being instantiated) update only those bearers who
        // don't need time-consuming scans (WLAN).
        // Note: EBearerIdWCDMA covers also GPRS bearer
        iConnectionMonitor.GetPckgAttribute(EBearerIdWCDMA, 0, KIapAvailability, iIapBuf, iStatus);
        User::WaitForRequest(iStatus);
        if (iStatus.Int() == KErrNone) {
            iOwner.accessPointScanningReady(true,iIapBuf());
        }
    } else {
        iConnectionMonitor.GetPckgAttribute(EBearerIdAll, 0, KIapAvailability, iIapBuf, iStatus);
        if (!IsActive()) {
            SetActive();
        }
    }
}

void AccessPointsAvailabilityScanner::RunL()
{
    if (iStatus.Int() != KErrNone) {
        iIapBuf().iCount = 0;
        iOwner.accessPointScanningReady(false,iIapBuf());
    } else {
        iOwner.accessPointScanningReady(true,iIapBuf());
    }
}

QT_END_NAMESPACE

#endif // QT_NO_BEARERMANAGEMENT

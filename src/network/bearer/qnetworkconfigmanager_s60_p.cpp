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

#include "qnetworkconfigmanager_s60_p.h"

#include <commdb.h>
#include <cdbcols.h>
#include <d32dbms.h>

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

QTM_BEGIN_NAMESPACE

static const int KValueThatWillBeAddedToSNAPId = 1000;
static const int KUserChoiceIAPId = 0;

QNetworkConfigurationManagerPrivate::QNetworkConfigurationManagerPrivate()
    : QObject(0), CActive(CActive::EPriorityIdle), capFlags(0), iFirstUpdate(true), iInitOk(true)
{
    CActiveScheduler::Add(this);

    registerPlatformCapabilities();
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
    
    QNetworkConfigurationPrivate* cpPriv = new QNetworkConfigurationPrivate();
    cpPriv->name = "UserChoice";
    cpPriv->bearer = QNetworkConfigurationPrivate::BearerUnknown;
    cpPriv->state = QNetworkConfiguration::Discovered;
    cpPriv->isValid = true;
    cpPriv->id = QString::number(qHash(KUserChoiceIAPId));
    cpPriv->numericId = KUserChoiceIAPId;
    cpPriv->connectionId = 0;
    cpPriv->type = QNetworkConfiguration::UserChoice;
    cpPriv->purpose = QNetworkConfiguration::UnknownPurpose;
    cpPriv->roamingSupported = false;
    cpPriv->manager = this;
    QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> ptr(cpPriv);
    userChoiceConfigurations.insert(cpPriv->id, ptr);

    updateConfigurations();
    updateStatesToSnaps();
    
    // Start monitoring IAP and/or SNAP changes in Symbian CommsDB
    startCommsDatabaseNotifications();
}

QNetworkConfigurationManagerPrivate::~QNetworkConfigurationManagerPrivate() 
{
    Cancel();

    QList<QString> configIdents = snapConfigurations.keys();
    foreach(QString oldIface, configIdents) {
        QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv = snapConfigurations.take(oldIface);
        priv->isValid = false;
        priv->id.clear();
    }

    configIdents = accessPointConfigurations.keys();
    foreach(QString oldIface, configIdents) {
        QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv = accessPointConfigurations.take(oldIface);
        priv->isValid = false;
        priv->id.clear();
    }

    configIdents = userChoiceConfigurations.keys();
    foreach(QString oldIface, configIdents) {
        QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv = userChoiceConfigurations.take(oldIface);
        priv->isValid = false;
        priv->id.clear();
        priv->manager = 0;
    }

    iConnectionMonitor.CancelNotifications();
    iConnectionMonitor.Close();
    
#ifdef SNAP_FUNCTIONALITY_AVAILABLE    
    iCmManager.Close();
#endif
    
    delete ipAccessPointsAvailabilityScanner;
    delete ipCommsDB;
}


void QNetworkConfigurationManagerPrivate::registerPlatformCapabilities()
{
    capFlags |= QNetworkConfigurationManager::CanStartAndStopInterfaces;
    capFlags |= QNetworkConfigurationManager::DirectConnectionRouting;
    capFlags |= QNetworkConfigurationManager::SystemSessionSupport;
#ifdef SNAP_FUNCTIONALITY_AVAILABLE
    capFlags |= QNetworkConfigurationManager::ApplicationLevelRoaming;
    capFlags |= QNetworkConfigurationManager::ForcedRoaming;
#endif
    capFlags |= QNetworkConfigurationManager::DataStatistics;
}

void QNetworkConfigurationManagerPrivate::performAsyncConfigurationUpdate()
{
    if (!iInitOk || iUpdateGoingOn) {
        return;
    }
    iUpdateGoingOn = true;

    stopCommsDatabaseNotifications();
    updateConfigurations(); // Synchronous call
    updateAvailableAccessPoints(); // Asynchronous call
}

void QNetworkConfigurationManagerPrivate::updateConfigurations()
{
    if (!iInitOk) {
        return;
    }

    TRAP_IGNORE(updateConfigurationsL());
}

void QNetworkConfigurationManagerPrivate::updateConfigurationsL()
{
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
            QNetworkConfigurationPrivate* cpPriv = NULL;
            TRAP(error, cpPriv = configFromConnectionMethodL(connectionMethod));
            if (error == KErrNone) {
                QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> ptr(cpPriv);
                accessPointConfigurations.insert(cpPriv->id, ptr);
                if (!iFirstUpdate) {
                    QNetworkConfiguration item;
                    item.d = ptr;
                    emit configurationAdded(item);
                }
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
            QNetworkConfigurationPrivate* cpPriv = new QNetworkConfigurationPrivate();
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
            cpPriv->manager = this;

            QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> ptr(cpPriv);
            snapConfigurations.insert(ident, ptr);
            if (!iFirstUpdate) {
                QNetworkConfiguration item;
                item.d = ptr;
                emit configurationAdded(item);
            }
            
            CleanupStack::Pop(cpPriv);
        }
        QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> privSNAP = snapConfigurations.value(ident);
            
        for (int j=0; j < destination.ConnectionMethodCount(); j++) {
            RCmConnectionMethod connectionMethod = destination.ConnectionMethodL(j);
            CleanupClosePushL(connectionMethod);
            
            TUint32 iapId = connectionMethod.GetIntAttributeL(CMManager::ECmIapId);
            QString iface = QString::number(qHash(iapId));
            // Check that IAP can be found from accessPointConfigurations list
            QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv = accessPointConfigurations.value(iface);
            if (priv.data() == 0) {
                QNetworkConfigurationPrivate* cpPriv = NULL; 
                TRAP(error, cpPriv = configFromConnectionMethodL(connectionMethod));
                if (error == KErrNone) {
                    QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> ptr(cpPriv);
                    ptr.data()->serviceNetworkPtr = privSNAP;
                    accessPointConfigurations.insert(cpPriv->id, ptr);
                    if (!iFirstUpdate) {
                        QNetworkConfiguration item;
                        item.d = ptr;
                        emit configurationAdded(item);
                    }
                    privSNAP->serviceNetworkMembers.append(ptr);
                }
            } else {
                knownConfigs.removeOne(iface);
                // Check that IAP can be found from related SNAP's configuration list
                bool iapFound = false;
                for (int i=0; i<privSNAP->serviceNetworkMembers.count(); i++) {
                    if (privSNAP->serviceNetworkMembers[i]->numericId == iapId) {
                        iapFound = true;
                        break;
                    }
                }
                if (!iapFound) {
                    priv.data()->serviceNetworkPtr = privSNAP; 
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
            QNetworkConfigurationPrivate* cpPriv = new QNetworkConfigurationPrivate();
            if (readNetworkConfigurationValuesFromCommsDb(apId, cpPriv)) {
                QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> ptr(cpPriv);
                accessPointConfigurations.insert(ident, ptr);
                if (!iFirstUpdate) {
                    QNetworkConfiguration item;
                    item.d = ptr;
                    emit configurationAdded(item);
                }
            } else {
                delete cpPriv;
            }
        }
        retVal = pDbTView->GotoNextRecord();
    }
    CleanupStack::PopAndDestroy(pDbTView);
#endif
    updateActiveAccessPoints();
    
    foreach (QString oldIface, knownConfigs) {
        //remove non existing IAP
        QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv = accessPointConfigurations.take(oldIface);
        priv->isValid = false;
        if (!iFirstUpdate) {
            QNetworkConfiguration item;
            item.d = priv;
            emit configurationRemoved(item);
        }
        // Remove non existing IAP from SNAPs
        QList<QString> snapConfigIdents = snapConfigurations.keys();
        foreach (QString iface, snapConfigIdents) {
            QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv2 = snapConfigurations.value(iface);
            // => Check if one of the IAPs of the SNAP is active
            for (int i=0; i<priv2->serviceNetworkMembers.count(); i++) {
                if (priv2->serviceNetworkMembers[i]->numericId == priv->numericId) {
                    priv2->serviceNetworkMembers.removeAt(i);
                    break;
                }
            }
        }    
    }
    foreach (QString oldIface, knownSnapConfigs) {
        //remove non existing SNAPs
        QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv = snapConfigurations.take(oldIface);
        priv->isValid = false;
        if (!iFirstUpdate) {
            QNetworkConfiguration item;
            item.d = priv;
            emit configurationRemoved(item);
        }
    }

    iFirstUpdate = false;
}

#ifdef SNAP_FUNCTIONALITY_AVAILABLE
QNetworkConfigurationPrivate* QNetworkConfigurationManagerPrivate::configFromConnectionMethodL(
        RCmConnectionMethod& connectionMethod)
{
    QNetworkConfigurationPrivate* cpPriv = new QNetworkConfigurationPrivate();
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
        cpPriv->bearer = QNetworkConfigurationPrivate::Bearer2G;
        break;
    case KCommDbBearerWcdma:
        cpPriv->bearer = QNetworkConfigurationPrivate::BearerWCDMA;
        break;
    case KCommDbBearerLAN:
        cpPriv->bearer = QNetworkConfigurationPrivate::BearerEthernet;
        break;
    case KCommDbBearerVirtual:
        cpPriv->bearer = QNetworkConfigurationPrivate::BearerUnknown;
        break;
    case KCommDbBearerPAN:
        cpPriv->bearer = QNetworkConfigurationPrivate::BearerUnknown;
        break;
    case KCommDbBearerWLAN:
        cpPriv->bearer = QNetworkConfigurationPrivate::BearerWLAN;
        break;
    default:
        cpPriv->bearer = QNetworkConfigurationPrivate::BearerUnknown;
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
    cpPriv->manager = this;
    
    CleanupStack::Pop(cpPriv);
    return cpPriv;
}
#else
bool QNetworkConfigurationManagerPrivate::readNetworkConfigurationValuesFromCommsDb(
        TUint32 aApId, QNetworkConfigurationPrivate* apNetworkConfiguration)
{
    TRAPD(error, readNetworkConfigurationValuesFromCommsDbL(aApId,apNetworkConfiguration));
    if (error != KErrNone) {
        return false;        
    }
    return true;
}

void QNetworkConfigurationManagerPrivate::readNetworkConfigurationValuesFromCommsDbL(
        TUint32 aApId, QNetworkConfigurationPrivate* apNetworkConfiguration)
{
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
        apNetworkConfiguration->bearer = QNetworkConfigurationPrivate::Bearer2G;
        break;
    case EApBearerTypeGPRS:
        apNetworkConfiguration->bearer = QNetworkConfigurationPrivate::Bearer2G;
        break;
    case EApBearerTypeHSCSD:
        apNetworkConfiguration->bearer = QNetworkConfigurationPrivate::BearerHSPA;
        break;
    case EApBearerTypeCDMA:
        apNetworkConfiguration->bearer = QNetworkConfigurationPrivate::BearerCDMA2000;
        break;
    case EApBearerTypeWLAN:
        apNetworkConfiguration->bearer = QNetworkConfigurationPrivate::BearerWLAN;
        break;
    case EApBearerTypeLAN:
        apNetworkConfiguration->bearer = QNetworkConfigurationPrivate::BearerEthernet;
        break;
    case EApBearerTypeLANModem:
        apNetworkConfiguration->bearer = QNetworkConfigurationPrivate::BearerEthernet;
        break;
    default:
        apNetworkConfiguration->bearer = QNetworkConfigurationPrivate::BearerUnknown;
        break;
    }
    apNetworkConfiguration->manager = this;
    
    CleanupStack::PopAndDestroy(pApUtils);
    CleanupStack::PopAndDestroy(pAPItem);
    CleanupStack::PopAndDestroy(pDataHandler);
}
#endif

QNetworkConfiguration QNetworkConfigurationManagerPrivate::defaultConfiguration()
{
    QNetworkConfiguration config;

    if (iInitOk) {
        stopCommsDatabaseNotifications();
        TRAP_IGNORE(config = defaultConfigurationL());
        startCommsDatabaseNotifications();
    }

    return config;
}

QNetworkConfiguration QNetworkConfigurationManagerPrivate::defaultConfigurationL()
{
    QNetworkConfiguration item;

#ifdef SNAP_FUNCTIONALITY_AVAILABLE
    // Check Default Connection (SNAP or IAP)
    TCmDefConnValue defaultConnectionValue;
    iCmManager.ReadDefConnL(defaultConnectionValue);
    if (defaultConnectionValue.iType == ECmDefConnDestination) {
        QString iface = QString::number(qHash(defaultConnectionValue.iId+KValueThatWillBeAddedToSNAPId));
        QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv = snapConfigurations.value(iface);
        if (priv.data() != 0) {
            item.d = priv;
        }
    } else if (defaultConnectionValue.iType == ECmDefConnConnectionMethod) {
        QString iface = QString::number(qHash(defaultConnectionValue.iId));
        QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv = accessPointConfigurations.value(iface);
        if (priv.data() != 0) {
            item.d = priv;
        }
    } 
#endif
    
    if (!item.isValid()) {
        QString iface = QString::number(qHash(KUserChoiceIAPId));
        QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv = userChoiceConfigurations.value(iface);
        if (priv.data() != 0) {
            item.d = priv;
        }
    }
    
    return item;
}

void QNetworkConfigurationManagerPrivate::updateActiveAccessPoints()
{
    bool online = false;
    QList<QString> inactiveConfigs = accessPointConfigurations.keys();

    TRequestStatus status;
    TUint connectionCount;
    iConnectionMonitor.GetConnectionCount(connectionCount, status);
    User::WaitForRequest(status);
    
    // Go through all connections and set state of related IAPs to Active
    TUint connectionId;
    TUint subConnectionCount;
    TUint apId;
    if (status.Int() == KErrNone) {
        for (TUint i = 1; i <= connectionCount; i++) {
            iConnectionMonitor.GetConnectionInfo(i, connectionId, subConnectionCount);
            iConnectionMonitor.GetUintAttribute(connectionId, subConnectionCount, KIAPId, apId, status);
            User::WaitForRequest(status);
            QString ident = QString::number(qHash(apId));
            QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv = accessPointConfigurations.value(ident);
            if (priv.data()) {
                online = true;
                inactiveConfigs.removeOne(ident);
                priv.data()->connectionId = connectionId;
                // Configuration is Active
                changeConfigurationStateTo(priv, QNetworkConfiguration::Active);
            }
        }
    }

    // Make sure that state of rest of the IAPs won't be Active
    foreach (QString iface, inactiveConfigs) {
        QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv = accessPointConfigurations.value(iface);
        if (priv.data()) {
            // Configuration is either Defined or Discovered
            changeConfigurationStateAtMaxTo(priv, QNetworkConfiguration::Discovered);
        }
    }

    if (iOnline != online) {
        iOnline = online;
        emit this->onlineStateChanged(iOnline);
    }
}

void QNetworkConfigurationManagerPrivate::updateAvailableAccessPoints()
{
    if (!ipAccessPointsAvailabilityScanner) {
        ipAccessPointsAvailabilityScanner = new AccessPointsAvailabilityScanner(*this, iConnectionMonitor);
    }
    if (ipAccessPointsAvailabilityScanner) {
        // Scanning may take a while because WLAN scanning will be done (if device supports WLAN).
        ipAccessPointsAvailabilityScanner->StartScanning();
    }
}

void QNetworkConfigurationManagerPrivate::accessPointScanningReady(TBool scanSuccessful, TConnMonIapInfo iapInfo)
{
    iUpdateGoingOn = false;
    if (scanSuccessful) {
        QList<QString> unavailableConfigs = accessPointConfigurations.keys();
        
        // Set state of returned IAPs to Discovered
        // if state is not already Active
        for(TUint i=0; i<iapInfo.iCount; i++) {
            QString ident = QString::number(qHash(iapInfo.iIap[i].iIapId));
            QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv = accessPointConfigurations.value(ident);
            if (priv.data()) {
                unavailableConfigs.removeOne(ident);
                if (priv.data()->state < QNetworkConfiguration::Active) {
                    // Configuration is either Discovered or Active
                    changeConfigurationStateAtMinTo(priv, QNetworkConfiguration::Discovered);
                }
            }
        }
        
        // Make sure that state of rest of the IAPs won't be Discovered or Active
        foreach (QString iface, unavailableConfigs) {
            QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv = accessPointConfigurations.value(iface);
            if (priv.data()) {
                // Configuration is Defined
                changeConfigurationStateAtMaxTo(priv, QNetworkConfiguration::Defined);
            }
        }
    }

    updateStatesToSnaps();
    
    startCommsDatabaseNotifications();
    
    emit this->configurationUpdateComplete();
}

void QNetworkConfigurationManagerPrivate::updateStatesToSnaps()
{
    // Go through SNAPs and set correct state to SNAPs
    QList<QString> snapConfigIdents = snapConfigurations.keys();
    foreach (QString iface, snapConfigIdents) {
        bool discovered = false;
        bool active = false;
        QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv = snapConfigurations.value(iface);
        // => Check if one of the IAPs of the SNAP is discovered or active
        //    => If one of IAPs is active, also SNAP is active
        //    => If one of IAPs is discovered but none of the IAPs is active, SNAP is discovered
        for (int i=0; i<priv->serviceNetworkMembers.count(); i++) {
            QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv2 = priv->serviceNetworkMembers[i];
            if ((priv->serviceNetworkMembers[i]->state & QNetworkConfiguration::Active) 
                    == QNetworkConfiguration::Active) {
                active = true;
                break;
            } else if ((priv->serviceNetworkMembers[i]->state & QNetworkConfiguration::Discovered) 
                        == QNetworkConfiguration::Discovered) {
                discovered = true;
            }
        }
        if (active) {
            changeConfigurationStateTo(priv, QNetworkConfiguration::Active);
        } else if (discovered) {
            changeConfigurationStateTo(priv, QNetworkConfiguration::Discovered);
        } else {
            changeConfigurationStateTo(priv, QNetworkConfiguration::Defined);
        }
    }    
}

bool QNetworkConfigurationManagerPrivate::changeConfigurationStateTo(QExplicitlySharedDataPointer<QNetworkConfigurationPrivate>& sharedData,
                                                                     QNetworkConfiguration::StateFlags newState)
{
    if (newState != sharedData.data()->state) {
        sharedData.data()->state = newState;
        QNetworkConfiguration item;
        item.d = sharedData;
        if (!iFirstUpdate) {
            emit configurationChanged(item);
        }
        return true;
    }
    return false;
}

/* changeConfigurationStateAtMinTo function does not overwrite possible better
 * state (e.g. Discovered state does not overwrite Active state) but
 * makes sure that state is at minimum given state.
*/
bool QNetworkConfigurationManagerPrivate::changeConfigurationStateAtMinTo(QExplicitlySharedDataPointer<QNetworkConfigurationPrivate>& sharedData,
                                                                          QNetworkConfiguration::StateFlags newState)
{
    if ((newState | sharedData.data()->state) != sharedData.data()->state) {
        sharedData.data()->state = (sharedData.data()->state | newState);
        QNetworkConfiguration item;
        item.d = sharedData;
        if (!iFirstUpdate) {
            emit configurationChanged(item);
        }
        return true;
    }
    return false;
}

/* changeConfigurationStateAtMaxTo function overwrites possible better
 * state (e.g. Discovered state overwrites Active state) and
 * makes sure that state is at maximum given state (e.g. Discovered state
 * does not overwrite Defined state).
*/
bool QNetworkConfigurationManagerPrivate::changeConfigurationStateAtMaxTo(QExplicitlySharedDataPointer<QNetworkConfigurationPrivate>& sharedData,
                                                                          QNetworkConfiguration::StateFlags newState)
{
    if ((newState & sharedData.data()->state) != sharedData.data()->state) {
        sharedData.data()->state = (newState & sharedData.data()->state);
        QNetworkConfiguration item;
        item.d = sharedData;
        if (!iFirstUpdate) {
            emit configurationChanged(item);
        }
        return true;
    }
    return false;
}

void QNetworkConfigurationManagerPrivate::startCommsDatabaseNotifications()
{
    if (!iWaitingCommsDatabaseNotifications) {
        iWaitingCommsDatabaseNotifications = ETrue;
        if (!IsActive()) {
            SetActive();
            // Start waiting for new notification
            ipCommsDB->RequestNotification(iStatus);
        }
    }
}

void QNetworkConfigurationManagerPrivate::stopCommsDatabaseNotifications()
{
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

void QNetworkConfigurationManagerPrivate::RunL()
{
    if (iStatus != KErrCancel) {
        RDbNotifier::TEvent event = STATIC_CAST(RDbNotifier::TEvent, iStatus.Int());
        switch (event) {
        case RDbNotifier::EUnlock:   /** All read locks have been removed.  */
        case RDbNotifier::ECommit:   /** A transaction has been committed.  */ 
        case RDbNotifier::ERollback: /** A transaction has been rolled back */
        case RDbNotifier::ERecover:  /** The database has been recovered    */
            // Note that if further database events occur while a client is handling
            // a request completion, the notifier records the most significant database
            // event and this is signalled as soon as the client issues the next
            // RequestNotification() request.
            // => Stop recording notifications
            stopCommsDatabaseNotifications();
            TRAPD(error, updateConfigurationsL());
            if (error == KErrNone) {
                updateStatesToSnaps();
            }
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

void QNetworkConfigurationManagerPrivate::DoCancel()
{
    ipCommsDB->CancelRequestNotification();
}


void QNetworkConfigurationManagerPrivate::EventL(const CConnMonEventBase& aEvent)
{
    switch (aEvent.EventType()) {
    case EConnMonCreateConnection:
        {
        CConnMonCreateConnection* realEvent;
        realEvent = (CConnMonCreateConnection*) &aEvent;
        TUint subConnectionCount = 0;
        TUint apId;            
        TUint connectionId = realEvent->ConnectionId();
        TRequestStatus status;
        iConnectionMonitor.GetUintAttribute(connectionId, subConnectionCount, KIAPId, apId, status);
        User::WaitForRequest(status);
        QString ident = QString::number(qHash(apId));
        QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv = accessPointConfigurations.value(ident);
        if (priv.data()) {
            priv.data()->connectionId = connectionId;
            // Configuration is Active
            if (changeConfigurationStateTo(priv, QNetworkConfiguration::Active)) {
                updateStatesToSnaps();
            }
            if (!iOnline) {
                iOnline = true;
                emit this->onlineStateChanged(iOnline);
            }
        }
        }
        break;

    case EConnMonDeleteConnection:
        {
        CConnMonDeleteConnection* realEvent;
        realEvent = (CConnMonDeleteConnection*) &aEvent;
        TUint connectionId = realEvent->ConnectionId();
        QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv = dataByConnectionId(connectionId);
        if (priv.data()) {
            priv.data()->connectionId = 0;
            // Configuration is either Defined or Discovered
            if (changeConfigurationStateAtMaxTo(priv, QNetworkConfiguration::Discovered)) {
                updateStatesToSnaps();
            }
        }
        
        bool online = false;
        QList<QString> iapConfigs = accessPointConfigurations.keys();
        foreach (QString iface, iapConfigs) {
            QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv = accessPointConfigurations.value(iface);
            if (priv.data()->state == QNetworkConfiguration::Active) {
                online = true;
                break;
            }
        }
        if (iOnline != online) {
            iOnline = online;
            emit this->onlineStateChanged(iOnline);
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
            QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv = accessPointConfigurations.value(ident);
            if (priv.data()) {
                // Configuration is either Discovered or Active 
                changeConfigurationStateAtMinTo(priv, QNetworkConfiguration::Discovered);
                unDiscoveredConfigs.removeOne(ident);
            }
        }
        foreach (QString iface, unDiscoveredConfigs) {
            QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv = accessPointConfigurations.value(iface);
            if (priv.data()) {
                // Configuration is Defined
                changeConfigurationStateAtMaxTo(priv, QNetworkConfiguration::Defined);
            }
        }
        }
        break;

    default:
        // For unrecognized events
        break;
    }
}

QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> QNetworkConfigurationManagerPrivate::dataByConnectionId(TUint aConnectionId)
{
    QNetworkConfiguration item;
    
    QHash<QString, QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> >::const_iterator i =
            accessPointConfigurations.constBegin();
    while (i != accessPointConfigurations.constEnd()) {
        QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv = i.value();
        if (priv.data()->connectionId == aConnectionId) {
            return priv;
        }
        ++i;
    }

    return QExplicitlySharedDataPointer<QNetworkConfigurationPrivate>();
}

AccessPointsAvailabilityScanner::AccessPointsAvailabilityScanner(QNetworkConfigurationManagerPrivate& owner,
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
    iConnectionMonitor.GetPckgAttribute(EBearerIdAll, 0, KIapAvailability, iIapBuf, iStatus);
    if (!IsActive()) {
        SetActive();
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
#include "moc_qnetworkconfigmanager_s60_p.cpp"
QTM_END_NAMESPACE

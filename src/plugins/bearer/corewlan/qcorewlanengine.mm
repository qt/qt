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

#include "qcorewlanengine.h"
#include "../qnetworksession_impl.h"

#include <QtNetwork/private/qnetworkconfiguration_p.h>

#include <QtCore/qthread.h>
#include <QtCore/qmutex.h>
#include <QtCore/qcoreapplication.h>
#include <QtCore/qstringlist.h>

#include <QtCore/qdebug.h>

#include <QDir>
#include <CoreWLAN/CoreWLAN.h>
#include <CoreWLAN/CWInterface.h>
#include <CoreWLAN/CWNetwork.h>
#include <CoreWLAN/CWNetwork.h>
#include <CoreWLAN/CW8021XProfile.h>

#include <Foundation/NSEnumerator.h>
#include <Foundation/NSKeyValueObserving.h>
#include <Foundation/NSAutoreleasePool.h>
#include <Foundation/NSLock.h>

#include <SystemConfiguration/SCNetworkConfiguration.h>
#include <private/qt_cocoa_helpers_mac_p.h>
#include "private/qcore_mac_p.h"

#ifndef QT_NO_BEARERMANAGEMENT

QT_BEGIN_NAMESPACE

@interface QNSListener : NSObject
{
    NSNotificationCenter *center;
    CWInterface *currentInterface;
    QCoreWlanEngine *engine;
    NSLock *locker;
}
- (void)notificationHandler;//:(NSNotification *)notification;
- (void)remove;
- (void)setEngine:(QCoreWlanEngine *)coreEngine;
- (void)dealloc;

@property (assign) QCoreWlanEngine* engine;

@end

@implementation QNSListener
@synthesize engine;

- (id) init
{
    [locker lock];
    QMacCocoaAutoReleasePool pool;
    center = [NSNotificationCenter defaultCenter];
    currentInterface = [CWInterface interfaceWithName:nil];
//    [center addObserver:self selector:@selector(notificationHandler:) name:kCWLinkDidChangeNotification object:nil];
    [center addObserver:self selector:@selector(notificationHandler:) name:kCWPowerDidChangeNotification object:nil];
    [locker unlock];
    return self;
}

-(void)dealloc
{
    [super dealloc];
}

-(void)setEngine:(QCoreWlanEngine *)coreEngine
{
    [locker lock];
    if(!engine)
        engine = coreEngine;
    [locker unlock];
}

-(void)remove
{
    [locker lock];
    [center removeObserver:self];
    [locker unlock];
}

- (void)notificationHandler//:(NSNotification *)notification
{
    engine->requestUpdate();
}
@end

QNSListener *listener = 0;

void networkChangeCallback(SCDynamicStoreRef/* store*/, CFArrayRef changedKeys, void *info)
{
    for ( long i = 0; i < CFArrayGetCount(changedKeys); i++) {

        QString changed =  QCFString::toQString((CFStringRef)CFArrayGetValueAtIndex(changedKeys, i));
        if( changed.contains("/Network/Global/IPv4")) {
            QCoreWlanEngine* wlanEngine = static_cast<QCoreWlanEngine*>(info);
            wlanEngine->requestUpdate();
        }
    }
    return;
}

QCoreWlanEngine::QCoreWlanEngine(QObject *parent)
:   QBearerEngineImpl(parent)
{
    startNetworkChangeLoop();

    QMacCocoaAutoReleasePool pool;
    if([[CWInterface supportedInterfaces] count] > 0 && !listener) {
        listener = [[QNSListener alloc] init];
        listener.engine = this;
        hasWifi = true;
    } else {
        hasWifi = false;
    }
    QMetaObject::invokeMethod(this, "requestUpdate", Qt::QueuedConnection);
}

QCoreWlanEngine::~QCoreWlanEngine()
{
    while (!foundConfigurations.isEmpty())
        delete foundConfigurations.takeFirst();
    [listener remove];
    [listener release];
}

QString QCoreWlanEngine::getInterfaceFromId(const QString &id)
{
    QMutexLocker locker(&mutex);

    return configurationInterface.value(id);
}

bool QCoreWlanEngine::hasIdentifier(const QString &id)
{
    QMutexLocker locker(&mutex);

    return configurationInterface.contains(id);
}

void QCoreWlanEngine::connectToId(const QString &id)
{
    QMutexLocker locker(&mutex);
    QMacCocoaAutoReleasePool pool;
    QString interfaceString = getInterfaceFromId(id);

    CWInterface *wifiInterface =
        [CWInterface interfaceWithName: qt_mac_QStringToNSString(interfaceString)];

    if ([wifiInterface power]) {
        NSError *err = nil;
        NSMutableDictionary *params = [NSMutableDictionary dictionaryWithCapacity:0];

        QString wantedSsid;
        bool using8021X = false;

        QNetworkConfigurationPrivatePointer ptr = accessPointConfigurations.value(id);

        const QString idHash = QString::number(qHash(QLatin1String("corewlan:") + getNetworkNameFromSsid(ptr->name)));

        if (idHash != id) {
            NSArray *array = [CW8021XProfile allUser8021XProfiles];

            for (NSUInteger i = 0; i < [array count]; ++i) {
                const QString networkNameHashCheck = QString::number(qHash(QLatin1String("corewlan:") + qt_mac_NSStringToQString([[array objectAtIndex:i] userDefinedName])));

                const QString ssidHash = QString::number(qHash(QLatin1String("corewlan:") + qt_mac_NSStringToQString([[array objectAtIndex:i] ssid])));

                if (id == networkNameHashCheck || id == ssidHash) {
                    const QString thisName = getSsidFromNetworkName(id);
                    if (thisName.isEmpty())
                        wantedSsid = id;
                    else
                        wantedSsid = thisName;

                    [params setValue: [array objectAtIndex:i] forKey:kCWAssocKey8021XProfile];
                    using8021X = true;
                    break;
                }
            }
        }

        if (!using8021X) {
            QString wantedNetwork;
            QMapIterator<QString, QMap<QString,QString> > i(userProfiles);
            while (i.hasNext()) {
                i.next();
                wantedNetwork = i.key();
                const QString networkNameHash = QString::number(qHash(QLatin1String("corewlan:") + wantedNetwork));
                if (id == networkNameHash) {
                    wantedSsid =  getSsidFromNetworkName(wantedNetwork);
                    break;
                }
            }
        }
        NSDictionary *scanParameters = [NSDictionary dictionaryWithObjectsAndKeys:
                                        [NSNumber numberWithBool:YES], kCWScanKeyMerge,
                                        [NSNumber numberWithInteger:100], kCWScanKeyRestTime,
                                        qt_mac_QStringToNSString(wantedSsid), kCWScanKeySSID,
                                        nil];

        NSArray *scanArray = [NSArray arrayWithArray:[wifiInterface scanForNetworksWithParameters:scanParameters error:&err]];

        if(!err) {
            for(uint row=0; row < [scanArray count]; row++ ) {
                CWNetwork *apNetwork = [scanArray objectAtIndex:row];

                if(wantedSsid == qt_mac_NSStringToQString([apNetwork ssid])) {

                    if(!using8021X) {
                        SecKeychainAttribute attributes[3];

                        NSString *account = [apNetwork ssid];
                        NSString *keyKind = @"AirPort network password";
                        NSString *keyName = account;

                        attributes[0].tag = kSecAccountItemAttr;
                        attributes[0].data = (void *)[account UTF8String];
                        attributes[0].length = [account length];

                        attributes[1].tag = kSecDescriptionItemAttr;
                        attributes[1].data = (void *)[keyKind UTF8String];
                        attributes[1].length = [keyKind length];

                        attributes[2].tag = kSecLabelItemAttr;
                        attributes[2].data = (void *)[keyName UTF8String];
                        attributes[2].length = [keyName length];

                        SecKeychainAttributeList attributeList = {3,attributes};

                        SecKeychainSearchRef searchRef;
                        SecKeychainSearchCreateFromAttributes(NULL, kSecGenericPasswordItemClass, &attributeList, &searchRef);

                        NSString *password = @"";
                        SecKeychainItemRef searchItem;

                        if (SecKeychainSearchCopyNext(searchRef, &searchItem) == noErr) {
                            UInt32 realPasswordLength;
                            SecKeychainAttribute attributesW[8];
                            attributesW[0].tag = kSecAccountItemAttr;
                            SecKeychainAttributeList listW = {1,attributesW};
                            char *realPassword;
                            OSStatus status = SecKeychainItemCopyContent(searchItem, NULL, &listW, &realPasswordLength,(void **)&realPassword);

                            if (status == noErr) {
                                if (realPassword != NULL) {

                                    QByteArray pBuf;
                                    pBuf.resize(realPasswordLength);
                                    pBuf.prepend(realPassword);
                                    pBuf.insert(realPasswordLength,'\0');

                                    password = [NSString stringWithUTF8String:pBuf];
                                }
                                SecKeychainItemFreeContent(&listW, realPassword);
                            }

                            CFRelease(searchItem);
                        } else {
                            qDebug() << "SecKeychainSearchCopyNext error";
                        }
                        [params setValue: password forKey: kCWAssocKeyPassphrase];
                    } // end using8021X


                    bool result = [wifiInterface associateToNetwork: apNetwork parameters:[NSDictionary dictionaryWithDictionary:params] error:&err];

                    if(!err) {
                        if(!result) {
                            emit connectionError(id, ConnectError);
                        } else {
                            return;
                        }
                    } else {
                        qDebug() <<"associate ERROR"<<  qt_mac_NSStringToQString([err localizedDescription ]);
                    }
                }
            } //end scan network
        } else {
            qDebug() <<"scan ERROR"<<  qt_mac_NSStringToQString([err localizedDescription ]);
        }
        emit connectionError(id, InterfaceLookupError);
    }

    locker.unlock();
    emit connectionError(id, InterfaceLookupError);
}

void QCoreWlanEngine::disconnectFromId(const QString &id)
{
    QMutexLocker locker(&mutex);

    QString interfaceString = getInterfaceFromId(id);
    QMacCocoaAutoReleasePool pool;

    CWInterface *wifiInterface =
        [CWInterface interfaceWithName: qt_mac_QStringToNSString(interfaceString)];

    [wifiInterface disassociate];
    if ([[wifiInterface interfaceState]intValue] != kCWInterfaceStateInactive) {
        locker.unlock();
        emit connectionError(id, DisconnectionError);
        locker.relock();
    }
}

void QCoreWlanEngine::requestUpdate()
{
    getUserConfigurations();
    doRequestUpdate();
}

void QCoreWlanEngine::doRequestUpdate()
{
    QMutexLocker locker(&mutex);

    QMacCocoaAutoReleasePool pool;

    QStringList previous = accessPointConfigurations.keys();

    NSArray *wifiInterfaces = [CWInterface supportedInterfaces];
    for (uint row = 0; row < [wifiInterfaces count]; ++row) {
        foreach (const QString &interface,
                 scanForSsids(qt_mac_NSStringToQString([wifiInterfaces objectAtIndex:row]))) {
            previous.removeAll(interface);
        }
    }

    while (!previous.isEmpty()) {
        QNetworkConfigurationPrivatePointer ptr = accessPointConfigurations.take(previous.takeFirst());

        configurationInterface.remove(ptr->id);

        locker.unlock();
        emit configurationRemoved(ptr);
        locker.relock();
    }

    locker.unlock();
    emit updateCompleted();
}

QString QCoreWlanEngine::getSsidFromNetworkName(const QString &name)
{
    QMapIterator<QString, QMap<QString,QString> > i(userProfiles);
    while (i.hasNext()) {
        i.next();
        QMap<QString,QString> map = i.value();
        QMapIterator<QString, QString> ij(i.value());
         while (ij.hasNext()) {
             ij.next();
             const QString networkNameHash = QString::number(qHash(QLatin1String("corewlan:") + i.key()));
             if (name == i.key() || name == networkNameHash) {
                 return ij.key();
             }
        }
    }
    return QString();
}

QString QCoreWlanEngine::getNetworkNameFromSsid(const QString &ssid)
{
    QMapIterator<QString, QMap<QString,QString> > i(userProfiles);
    while (i.hasNext()) {
        i.next();
        QMap<QString,QString> map = i.value();
        QMapIterator<QString, QString> ij(i.value());
         while (ij.hasNext()) {
             ij.next();
             if(ij.key() == ssid) {
                 return i.key();
             }
         }
    }
    return QString();
}

QStringList QCoreWlanEngine::scanForSsids(const QString &interfaceName)
{
    QMutexLocker locker(&mutex);

    QStringList found;

    if(!hasWifi) {
        return found;
    }
    QMacCocoaAutoReleasePool pool;

    CWInterface *currentInterface = [CWInterface interfaceWithName:qt_mac_QStringToNSString(interfaceName)];
    QStringList addedConfigs;

    if([currentInterface power]) {
        NSError *err = nil;
        NSDictionary *parametersDict =  [NSDictionary dictionaryWithObjectsAndKeys:
                                   [NSNumber numberWithBool:YES], kCWScanKeyMerge,
                                   [NSNumber numberWithInt:kCWScanTypeFast], kCWScanKeyScanType, // get the networks in the scan cache
                                   [NSNumber numberWithInteger:100], kCWScanKeyRestTime, nil];
        NSArray* apArray = [currentInterface scanForNetworksWithParameters:parametersDict error:&err];
        CWNetwork *apNetwork;
        if (!err) {

            for(uint row=0; row < [apArray count]; row++ ) {
                apNetwork = [apArray objectAtIndex:row];

                const QString networkSsid = qt_mac_NSStringToQString([apNetwork ssid]);

                const QString id = QString::number(qHash(QLatin1String("corewlan:") + networkSsid));
                found.append(id);

                QNetworkConfiguration::StateFlags state = QNetworkConfiguration::Undefined;
                bool known = isKnownSsid(networkSsid);
                if( [currentInterface.interfaceState intValue] == kCWInterfaceStateRunning) {
                    if( networkSsid == qt_mac_NSStringToQString( [currentInterface ssid])) {
                        state = QNetworkConfiguration::Active;
                    }
                }
                if(state == QNetworkConfiguration::Undefined) {
                    if(known) {
                        state = QNetworkConfiguration::Discovered;
                    } else {
                        state = QNetworkConfiguration::Undefined;
                    }
                }
                QNetworkConfiguration::Purpose purpose = QNetworkConfiguration::UnknownPurpose;
                if([[apNetwork securityMode] intValue] == kCWSecurityModeOpen) {
                    purpose = QNetworkConfiguration::PublicPurpose;
                } else {
                    purpose = QNetworkConfiguration::PrivatePurpose;
                }

                found.append(foundNetwork(id, networkSsid, state, interfaceName, purpose));

            } //end row
        } //end error
    } // endwifi power

    // add known configurations that are not around.
    QMapIterator<QString, QMap<QString,QString> > i(userProfiles);
    while (i.hasNext()) {
        i.next();

        QString networkName = i.key();
        const QString id = QString::number(qHash(QLatin1String("corewlan:") + networkName));

        if(!found.contains(id)) {
            QString networkSsid = getSsidFromNetworkName(networkName);
            const QString ssidId = QString::number(qHash(QLatin1String("corewlan:") + networkSsid));
            QNetworkConfiguration::StateFlags state = QNetworkConfiguration::Undefined;
            QString interfaceName;
            QMapIterator<QString, QString> ij(i.value());
            while (ij.hasNext()) {
                ij.next();
                interfaceName = ij.value();
            }

            if( [currentInterface.interfaceState intValue] == kCWInterfaceStateRunning) {
                if( networkSsid == qt_mac_NSStringToQString([currentInterface ssid])) {
                    state = QNetworkConfiguration::Active;
                }
            }
            if(state == QNetworkConfiguration::Undefined) {
                if( userProfiles.contains(networkName)
                    && found.contains(ssidId)) {
                    state = QNetworkConfiguration::Discovered;
                }
            }

            if(state == QNetworkConfiguration::Undefined) {
                state = QNetworkConfiguration::Defined;
            }

            found.append(foundNetwork(id, networkName, state, interfaceName, QNetworkConfiguration::UnknownPurpose));
        }
    }
    return found;
}

QStringList QCoreWlanEngine::foundNetwork(const QString &id, const QString &name, const QNetworkConfiguration::StateFlags state, const QString &interfaceName, const QNetworkConfiguration::Purpose purpose)
{
    QStringList found;
    QMutexLocker locker(&mutex);
    if (accessPointConfigurations.contains(id)) {
        QNetworkConfigurationPrivatePointer ptr = accessPointConfigurations.value(id);

        bool changed = false;

        ptr->mutex.lock();

        if (!ptr->isValid) {
            ptr->isValid = true;
            changed = true;
        }

        if (ptr->name != name) {
            ptr->name = name;
            changed = true;
        }

        if (ptr->id != id) {
            ptr->id = id;
            changed = true;
        }

        if (ptr->state != state) {
            ptr->state = state;
            changed = true;
        }

        if (ptr->purpose != purpose) {
            ptr->purpose = purpose;
            changed = true;
        }
        ptr->mutex.unlock();

        if (changed) {
            locker.unlock();
            emit configurationChanged(ptr);
            locker.relock();
        }
        found.append(id);
    } else {
        QNetworkConfigurationPrivatePointer ptr(new QNetworkConfigurationPrivate);

        ptr->name = name;
        ptr->isValid = true;
        ptr->id = id;
        ptr->state = state;
        ptr->type = QNetworkConfiguration::InternetAccessPoint;
        ptr->bearer = QLatin1String("WLAN");
        ptr->purpose = purpose;

        accessPointConfigurations.insert(ptr->id, ptr);
        configurationInterface.insert(ptr->id, interfaceName);

        locker.unlock();
        emit configurationAdded(ptr);
        locker.relock();
       found.append(id);
    }
    return found;
}

bool QCoreWlanEngine::isWifiReady(const QString &wifiDeviceName)
{
    QMutexLocker locker(&mutex);

    if(hasWifi) {
        QMacCocoaAutoReleasePool pool;
        CWInterface *defaultInterface = [CWInterface interfaceWithName: qt_mac_QStringToNSString(wifiDeviceName)];
        if([defaultInterface power])
            return true;
    }
    return false;
}

bool QCoreWlanEngine::isKnownSsid(const QString &ssid)
{
    QMutexLocker locker(&mutex);

    QMapIterator<QString, QMap<QString,QString> > i(userProfiles);
    while (i.hasNext()) {
        i.next();
        QMap<QString,QString> map = i.value();
        if(map.keys().contains(ssid)) {
            return true;
        }
    }
    return false;
}

QNetworkSession::State QCoreWlanEngine::sessionStateForId(const QString &id)
{
    QMutexLocker locker(&mutex);

    QNetworkConfigurationPrivatePointer ptr = accessPointConfigurations.value(id);

    if (!ptr)
        return QNetworkSession::Invalid;

    if (!ptr->isValid) {
        return QNetworkSession::Invalid;
    } else if ((ptr->state & QNetworkConfiguration::Active) == QNetworkConfiguration::Active) {
        return QNetworkSession::Connected;
    } else if ((ptr->state & QNetworkConfiguration::Discovered) ==
                QNetworkConfiguration::Discovered) {
        return QNetworkSession::Disconnected;
    } else if ((ptr->state & QNetworkConfiguration::Defined) == QNetworkConfiguration::Defined) {
        return QNetworkSession::NotAvailable;
    } else if ((ptr->state & QNetworkConfiguration::Undefined) ==
                QNetworkConfiguration::Undefined) {
        return QNetworkSession::NotAvailable;
    }

    return QNetworkSession::Invalid;
}

QNetworkConfigurationManager::Capabilities QCoreWlanEngine::capabilities() const
{
    return QNetworkConfigurationManager::ForcedRoaming;
}

void QCoreWlanEngine::startNetworkChangeLoop()
{
    storeSession = NULL;

    SCDynamicStoreContext dynStoreContext = { 0, this/*(void *)storeSession*/, NULL, NULL, NULL };
    storeSession = SCDynamicStoreCreate(NULL,
                                 CFSTR("networkChangeCallback"),
                                 networkChangeCallback,
                                 &dynStoreContext);
    if (!storeSession ) {
        qWarning() << "could not open dynamic store: error:" << SCErrorString(SCError());
        return;
    }

    CFMutableArrayRef notificationKeys;
    notificationKeys = CFArrayCreateMutable(NULL, 0, &kCFTypeArrayCallBacks);
    CFMutableArrayRef patternsArray;
    patternsArray = CFArrayCreateMutable(NULL, 0, &kCFTypeArrayCallBacks);

    CFStringRef storeKey;
    storeKey = SCDynamicStoreKeyCreateNetworkGlobalEntity(NULL,
                                                     kSCDynamicStoreDomainState,
                                                     kSCEntNetIPv4);
    CFArrayAppendValue(notificationKeys, storeKey);
    CFRelease(storeKey);

    storeKey = SCDynamicStoreKeyCreateNetworkServiceEntity(NULL,
                                                      kSCDynamicStoreDomainState,
                                                      kSCCompAnyRegex,
                                                      kSCEntNetIPv4);
    CFArrayAppendValue(patternsArray, storeKey);
    CFRelease(storeKey);

    if (!SCDynamicStoreSetNotificationKeys(storeSession , notificationKeys, patternsArray)) {
        qWarning() << "register notification error:"<< SCErrorString(SCError());
        CFRelease(storeSession );
        CFRelease(notificationKeys);
        CFRelease(patternsArray);
        return;
    }
    CFRelease(notificationKeys);
    CFRelease(patternsArray);

    runloopSource = SCDynamicStoreCreateRunLoopSource(NULL, storeSession , 0);
    if (!runloopSource) {
        qWarning() << "runloop source error:"<< SCErrorString(SCError());
        CFRelease(storeSession );
        return;
    }

    CFRunLoopAddSource(CFRunLoopGetCurrent(), runloopSource, kCFRunLoopDefaultMode);
    return;
}

QNetworkSessionPrivate *QCoreWlanEngine::createSessionBackend()
{
    return new QNetworkSessionPrivateImpl;
}

QNetworkConfigurationPrivatePointer QCoreWlanEngine::defaultConfiguration()
{
    return QNetworkConfigurationPrivatePointer();
}

bool QCoreWlanEngine::requiresPolling() const
{
    return true;
}

void QCoreWlanEngine::getUserConfigurations()
{
    QMacCocoaAutoReleasePool pool;
    userProfiles.clear();

    NSArray *wifiInterfaces = [CWInterface supportedInterfaces];
    for(uint row=0; row < [wifiInterfaces count]; row++ ) {

        CWInterface *wifiInterface = [CWInterface interfaceWithName: [wifiInterfaces objectAtIndex:row]];
        NSString *nsInterfaceName = [wifiInterface name];
// add user configured system networks
        SCDynamicStoreRef dynRef = SCDynamicStoreCreate(kCFAllocatorSystemDefault, (CFStringRef)@"Qt corewlan", nil, nil);
        NSDictionary *airportPlist = (NSDictionary *)SCDynamicStoreCopyValue(dynRef, (CFStringRef)[[NSString stringWithFormat:@"Setup:/Network/Interface/%@/AirPort", nsInterfaceName] autorelease]);
        CFRelease(dynRef);

        NSDictionary *prefNetDict = [airportPlist objectForKey:@"PreferredNetworks"];

        NSArray *thisSsidarray = [prefNetDict valueForKey:@"SSID_STR"];
        for(NSString *ssidkey in thisSsidarray) {
            QString thisSsid = qt_mac_NSStringToQString(ssidkey);
            if(!userProfiles.contains(thisSsid)) {
                QMap <QString,QString> map;
                map.insert(thisSsid, qt_mac_NSStringToQString(nsInterfaceName));
                userProfiles.insert(thisSsid, map);
            }
        }
        CFRelease(airportPlist);

        // 802.1X user profiles
        QString userProfilePath = QDir::homePath() + "/Library/Preferences/com.apple.eap.profiles.plist";
        NSDictionary* eapDict = [[NSDictionary alloc] initWithContentsOfFile:qt_mac_QStringToNSString(userProfilePath)];
        NSString *profileStr= @"Profiles";
        NSString *nameStr = @"UserDefinedName";
        NSString *networkSsidStr = @"Wireless Network";
        for (id profileKey in eapDict) {
            if ([profileStr isEqualToString:profileKey]) {
                NSDictionary *itemDict = [eapDict objectForKey:profileKey];
                for (id itemKey in itemDict) {

                    NSInteger dictSize = [itemKey count];
                    id objects[dictSize];
                    id keys[dictSize];

                    [itemKey getObjects:objects andKeys:keys];
                    QString networkName;
                    QString ssid;
                    for(int i = 0; i < dictSize; i++) {
                        if([nameStr isEqualToString:keys[i]]) {
                            networkName = qt_mac_NSStringToQString(objects[i]);
                        }
                        if([networkSsidStr isEqualToString:keys[i]]) {
                            ssid = qt_mac_NSStringToQString(objects[i]);
                        }
                        if(!userProfiles.contains(networkName)
                            && !ssid.isEmpty()) {
                            QMap<QString,QString> map;
                            map.insert(ssid, qt_mac_NSStringToQString(nsInterfaceName));
                            userProfiles.insert(networkName, map);
                        }
                    }
                }
                [itemDict release];
            }
        }
        [eapDict release];
    }
}

QT_END_NAMESPACE

#endif // QT_NO_BEARERMANAGEMENT

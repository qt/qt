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

#if defined(MAC_SDK_10_6) //not much functionality without this
#include <CoreWLAN/CoreWLAN.h>
#include <CoreWLAN/CWInterface.h>
#include <CoreWLAN/CWNetwork.h>
#include <CoreWLAN/CWNetwork.h>
#endif

#include <Foundation/NSEnumerator.h>
#include <Foundation/NSKeyValueObserving.h>
#include <Foundation/NSAutoreleasePool.h>

#include <SystemConfiguration/SCNetworkConfiguration.h>
QMap <QString, QString> networkInterfaces;

QT_BEGIN_NAMESPACE

inline QString cfstringRefToQstring(CFStringRef cfStringRef) {
//    return QString([cfStringRef UTF8String]);
    QString retVal;
    CFIndex maxLength = 2 * CFStringGetLength(cfStringRef) + 1/*zero term*/; // max UTF8
    char *cstring = new char[maxLength];
    if (CFStringGetCString(CFStringRef(cfStringRef), cstring, maxLength, kCFStringEncodingUTF8)) {
        retVal = QString::fromUtf8(cstring);
    }
    delete[] cstring;
    return retVal;
}

inline CFStringRef qstringToCFStringRef(const QString &string)
{
    return CFStringCreateWithCharacters(0, reinterpret_cast<const UniChar *>(string.unicode()),
                                        string.length());
}

inline NSString *qstringToNSString(const QString &qstr)
{ return [reinterpret_cast<const NSString *>(qstringToCFStringRef(qstr)) autorelease]; }

inline QString nsstringToQString(const NSString *nsstr)
{ return cfstringRefToQstring(reinterpret_cast<const CFStringRef>(nsstr)); }

inline QStringList nsarrayToQStringList(void *nsarray)
{
    QStringList result;
    NSArray *array = static_cast<NSArray *>(nsarray);
    for (NSUInteger i=0; i<[array count]; ++i)
        result << nsstringToQString([array objectAtIndex:i]);
    return result;
}

static QString qGetInterfaceType(const QString &interfaceString)
{
    return networkInterfaces.value(interfaceString, QLatin1String("Unknown"));
}

void networkChangeCallback(SCDynamicStoreRef/* store*/, CFArrayRef changedKeys, void *info)
{
    for ( long i = 0; i < CFArrayGetCount(changedKeys); i++) {

        CFStringRef changed = (CFStringRef)CFArrayGetValueAtIndex(changedKeys, i);
        if( cfstringRefToQstring(changed).contains("/Network/Global/IPv4")) {
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
}

QCoreWlanEngine::~QCoreWlanEngine()
{
    while (!foundConfigurations.isEmpty())
        delete foundConfigurations.takeFirst();
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

    NSAutoreleasePool *autoreleasepool = [[NSAutoreleasePool alloc] init];
    QString interfaceString = getInterfaceFromId(id);

    if(networkInterfaces.value(interfaceString) == "WLAN") {
#if defined(MAC_SDK_10_6)
        CWInterface *wifiInterface = [CWInterface interfaceWithName: qstringToNSString(interfaceString)];
        CWConfiguration *userConfig = [ wifiInterface configuration];

        NSSet *remNets = [userConfig rememberedNetworks]; //CWWirelessProfile

        NSEnumerator *enumerator = [remNets objectEnumerator];
        CWWirelessProfile *wProfile;
        NSUInteger index=0;
        NSDictionary *parametersDict;
        NSArray* apArray;

        CW8021XProfile *user8021XProfile;
        NSError *err;
        NSMutableDictionary *params;

        while ((wProfile = [enumerator nextObject])) { //CWWirelessProfile

            if(id ==  QString::number(qHash(QLatin1String("corewlan:") + nsstringToQString([wProfile ssid])))) {
                user8021XProfile = nil;
                user8021XProfile = [ wProfile user8021XProfile];

                err = nil;
                params = [NSMutableDictionary dictionaryWithCapacity:0];

                if(user8021XProfile) {
                    [params setValue: user8021XProfile forKey:kCWAssocKey8021XProfile];
                } else {
                    [params setValue: [wProfile passphrase] forKey: kCWAssocKeyPassphrase];
                }

               parametersDict = nil;
               apArray = [NSMutableArray arrayWithArray:[wifiInterface scanForNetworksWithParameters:parametersDict error:&err]];

                if(!err) {

                    for(uint row=0; row < [apArray count]; row++ ) {
                        CWNetwork *apNetwork = [apArray objectAtIndex:row];
                        if([[apNetwork ssid] compare:[wProfile ssid]] == NSOrderedSame) {

                            bool result = [wifiInterface associateToNetwork: apNetwork parameters:[NSDictionary dictionaryWithDictionary:params] error:&err];

                            if(!result) {
                                emit connectionError(id, ConnectError);
                            } else {
                                [autoreleasepool release];
                                return;
                            }
                        }
                    }
                }
            }
            index++;
        }

        emit connectionError(id, InterfaceLookupError);
#endif
    } else {
        // not wifi
    }
    emit connectionError(id, OperationNotSupported);
        [autoreleasepool release];
}

void QCoreWlanEngine::disconnectFromId(const QString &id)
{
    QMutexLocker locker(&mutex);

    QString interfaceString = getInterfaceFromId(id);
    if(networkInterfaces.value(getInterfaceFromId(id)) == "WLAN") { //wifi only for now
#if defined(MAC_SDK_10_6)
        NSAutoreleasePool *autoreleasepool = [[NSAutoreleasePool alloc] init];
        CWInterface *wifiInterface = [CWInterface interfaceWithName:  qstringToNSString(interfaceString)];
        [wifiInterface disassociate];
        if([[wifiInterface interfaceState]intValue] != kCWInterfaceStateInactive) {
            emit connectionError(id, DisconnectionError);
        }
       [autoreleasepool release];
       return;
#endif
    } else {

    }
    emit connectionError(id, OperationNotSupported);
}

void QCoreWlanEngine::requestUpdate()
{
    QMutexLocker locker(&mutex);

    doRequestUpdate();
}

void QCoreWlanEngine::doRequestUpdate()
{
    QMutexLocker locker(&mutex);

    getAllScInterfaces();

    QStringList previous = accessPointConfigurations.keys();

    QMapIterator<QString, QString> i(networkInterfaces);
    while (i.hasNext()) {
        i.next();
        if (i.value() == QLatin1String("WLAN")) {
            QStringList added = scanForSsids(i.key());
            while (!added.isEmpty()) {
                previous.removeAll(added.takeFirst());
            }
        }

        QNetworkInterface interface = QNetworkInterface::interfaceFromName(i.key());

        if (!interface.isValid())
            continue;

        uint identifier;
        if (interface.index())
            identifier = qHash(QLatin1String("corewlan:") + QString::number(interface.index()));
        else
            identifier = qHash(QLatin1String("corewlan:") + interface.hardwareAddress());

        const QString id = QString::number(identifier);

        previous.removeAll(id);

        QString name = interface.humanReadableName();
        if (name.isEmpty())
            name = interface.name();

        QNetworkConfiguration::StateFlags state = QNetworkConfiguration::Undefined;

        if (interface.flags() && QNetworkInterface::IsRunning)
            state = QNetworkConfiguration::Defined;

        if (!interface.addressEntries().isEmpty())
            state = QNetworkConfiguration::Active;

        if (accessPointConfigurations.contains(id)) { //handle only scanned AP's
            QNetworkConfigurationPrivatePointer ptr = accessPointConfigurations.value(id);

            bool changed = false;

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

            if (changed)
                emit configurationChanged(ptr);
        }
    }

    while (!previous.isEmpty()) {
        QNetworkConfigurationPrivatePointer ptr = accessPointConfigurations.take(previous.takeFirst());

        configurationInterface.remove(ptr->id);
        emit configurationRemoved(ptr);
    }

    emit updateCompleted();
}

QStringList QCoreWlanEngine::scanForSsids(const QString &interfaceName)
{
    QMutexLocker locker(&mutex);

    QStringList found;

#if defined(MAC_SDK_10_6)
    NSAutoreleasePool *autoreleasepool = [[NSAutoreleasePool alloc] init];

    CWInterface *currentInterface = [CWInterface interfaceWithName:qstringToNSString(interfaceName)];
    if([currentInterface power]) {
        NSError *err = nil;
        NSDictionary *parametersDict = nil;
        NSArray* apArray = [currentInterface scanForNetworksWithParameters:parametersDict error:&err];

        CWNetwork *apNetwork;
        if (!err) {
            for(uint row=0; row < [apArray count]; row++ ) {
                NSAutoreleasePool *looppool = [[NSAutoreleasePool alloc] init];

                apNetwork = [apArray objectAtIndex:row];

                const QString networkSsid = nsstringToQString([apNetwork ssid]);

                const QString id = QString::number(qHash(QLatin1String("corewlan:") + networkSsid));
                found.append(id);

                QNetworkConfiguration::StateFlags state = QNetworkConfiguration::Undefined;

                if ([currentInterface.interfaceState intValue] == kCWInterfaceStateRunning) {
                    if (networkSsid == nsstringToQString([currentInterface ssid]))
                        state = QNetworkConfiguration::Active;
                } else {
                    if (isKnownSsid(interfaceName, networkSsid))
                        state = QNetworkConfiguration::Discovered;
                    else
                        state = QNetworkConfiguration::Defined;
                }

                if (accessPointConfigurations.contains(id)) {
                    QNetworkConfigurationPrivatePointer ptr = accessPointConfigurations.value(id);

                    bool changed = false;

                    if (!ptr->isValid) {
                        ptr->isValid = true;
                        changed = true;
                    }

                    if (ptr->name != networkSsid) {
                        ptr->name = networkSsid;
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

                    if (changed)
                        emit configurationChanged(ptr);
                } else {
                    QNetworkConfigurationPrivatePointer ptr(new QNetworkConfigurationPrivate);

                    ptr->name = networkSsid;
                    ptr->isValid = true;
                    ptr->id = id;
                    ptr->state = state;
                    ptr->type = QNetworkConfiguration::InternetAccessPoint;
                    ptr->bearer = QLatin1String("WLAN");

                    accessPointConfigurations.insert(id, ptr);
                    configurationInterface.insert(id, interfaceName);

                    emit configurationAdded(ptr);
                }
                [looppool release];
            }
        }
    }
    [autoreleasepool drain];
#else
    Q_UNUSED(interfaceName);
#endif
    return found;
}

bool QCoreWlanEngine::isWifiReady(const QString &wifiDeviceName)
{
    QMutexLocker locker(&mutex);

#if defined(MAC_SDK_10_6)
    CWInterface *defaultInterface = [CWInterface interfaceWithName: qstringToNSString(wifiDeviceName)];
    if([defaultInterface power])
        return true;
#else
    Q_UNUSED(wifiDeviceName);
#endif
    return false;
}

bool QCoreWlanEngine::isKnownSsid(const QString &interfaceName, const QString &ssid)
{
    QMutexLocker locker(&mutex);

#if defined(MAC_SDK_10_6)
    CWInterface *wifiInterface = [CWInterface interfaceWithName: qstringToNSString(interfaceName)];
    CWConfiguration *userConfig = [wifiInterface configuration];
    NSSet *remNets = [userConfig rememberedNetworks];
    for (CWWirelessProfile *wProfile in remNets) {
        if(ssid == nsstringToQString([wProfile ssid]))
            return true;
    }
#else
    Q_UNUSED(interfaceName);
    Q_UNUSED(ssid);
#endif
    return false;
}

bool QCoreWlanEngine::getAllScInterfaces()
{
    QMutexLocker locker(&mutex);

    networkInterfaces.clear();
    NSAutoreleasePool *autoreleasepool = [[NSAutoreleasePool alloc] init];

    CFArrayRef interfaces = SCNetworkInterfaceCopyAll();
    if (interfaces != NULL) {
        CFIndex interfaceCount;
        CFIndex interfaceIndex;
        interfaceCount = CFArrayGetCount(interfaces);
        for (interfaceIndex = 0; interfaceIndex < interfaceCount; interfaceIndex++) {
            NSAutoreleasePool *looppool = [[NSAutoreleasePool alloc] init];

            CFStringRef bsdName;
            CFTypeRef thisInterface = CFArrayGetValueAtIndex(interfaces, interfaceIndex);
            bsdName = SCNetworkInterfaceGetBSDName((SCNetworkInterfaceRef)thisInterface);
            QString interfaceName = cfstringRefToQstring(bsdName);
            QString typeStr;
            CFStringRef type = SCNetworkInterfaceGetInterfaceType((SCNetworkInterfaceRef)thisInterface);
            if ( CFEqual(type, kSCNetworkInterfaceTypeIEEE80211)) {
                typeStr = "WLAN";
            }
            if(!networkInterfaces.contains(interfaceName) && !typeStr.isEmpty()) {
                networkInterfaces.insert(interfaceName,typeStr);
            }
            [looppool release];
        }
    }
    CFRelease(interfaces);

    [autoreleasepool drain];
    return true;
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

QT_END_NAMESPACE

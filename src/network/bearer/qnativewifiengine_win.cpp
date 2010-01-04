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

#include "qnativewifiengine_win_p.h"
#include "qnetworkconfiguration_p.h"

#include <QtCore/qmutex.h>
#include <QtCore/private/qmutexpool_p.h>
#include <QtCore/qlibrary.h>
#include <QtCore/qstringlist.h>

#include <QtCore/qdebug.h>

#include <wtypes.h>
#undef interface

QTM_BEGIN_NAMESPACE

Q_GLOBAL_STATIC(QNativeWifiEngine, nativeWifiEngine)

#define WLAN_MAX_NAME_LENGTH 256
#define WLAN_MAX_PHY_TYPE_NUMBER 8
#define WLAN_NOTIFICATION_SOURCE_ALL 0x0000ffff
#define WLAN_AVAILABLE_NETWORK_CONNECTED 1
#define WLAN_AVAILABLE_NETWORK_HAS_PROFILE 2
#define DOT11_SSID_MAX_LENGTH 32

struct WLAN_NOTIFICATION_DATA {
    DWORD NotificationSource;
    DWORD NotificationCode;
    GUID InterfaceGuid;
    DWORD dwDataSize;
    PVOID pData;
};

enum WLAN_INTERFACE_STATE {
    wlan_interface_state_not_ready = 0,
    wlan_interface_state_connected,
    wlan_interface_state_ad_hoc_network_formed,
    wlan_interface_state_disconnecting,
    wlan_interface_state_disconnected,
    wlan_interface_state_associating,
    wlan_interface_state_discovering,
    wlan_interface_state_authenticating
};

struct WLAN_INTERFACE_INFO {
    GUID InterfaceGuid;
    WCHAR strInterfaceDescription[WLAN_MAX_NAME_LENGTH];
    WLAN_INTERFACE_STATE isState;
};

struct WLAN_INTERFACE_INFO_LIST {
    DWORD dwNumberOfItems;
    DWORD dwIndex;
    WLAN_INTERFACE_INFO InterfaceInfo[1];
};

struct DOT11_SSID {
    ULONG uSSIDLength;
    UCHAR ucSSID[DOT11_SSID_MAX_LENGTH];
};

struct NDIS_OBJECT_HEADER {
    UCHAR Type;
    UCHAR Revision;
    USHORT Size;
};

typedef UCHAR DOT11_MAC_ADDRESS[6];
struct DOT11_BSSID_LIST {
    NDIS_OBJECT_HEADER Header;
    ULONG uNumberOfEntries;
    ULONG uTotalNumOfEntries;
    DOT11_MAC_ADDRESS BSSIDs[1];
};

enum DOT11_BSS_TYPE {
    dot11_BSS_type_infrastructure = 1,
    dot11_BSS_type_independent = 2,
    dot11_BSS_type_any = 3
};

enum DOT11_PHY_TYPE {
    dot11_phy_type_unknown = 0,
    dot11_phy_type_any = dot11_phy_type_unknown,
    dot11_phy_type_fhss = 1,
    dot11_phy_type_dsss = 2,
    dot11_phy_type_irbaseband = 3,
    dot11_phy_type_ofdm = 4,
    dot11_phy_type_hrdsss = 5,
    dot11_phy_type_erp = 6,
    dot11_phy_type_ht = 7,
    dot11_phy_type_IHV_start = 0x80000000,
    dot11_phy_type_IHV_end = 0xffffffff
};

enum DOT11_AUTH_ALGORITHM {
    DOT11_AUTH_ALGO_80211_OPEN = 1,
    DOT11_AUTH_ALGO_80211_SHARED_KEY = 2,
    DOT11_AUTH_ALGO_WPA = 3,
    DOT11_AUTH_ALGO_WPA_PSK = 4,
    DOT11_AUTH_ALGO_WPA_NONE = 5,
    DOT11_AUTH_ALGO_RSNA = 6,
    DOT11_AUTH_ALGO_RSNA_PSK = 7,
    DOT11_AUTH_ALGO_IHV_START = 0x80000000,
    DOT11_AUTH_ALGO_IHV_END = 0xffffffff
};

enum DOT11_CIPHER_ALGORITHM {
    DOT11_CIPHER_ALGO_NONE = 0x00,
    DOT11_CIPHER_ALGO_WEP40 = 0x01,
    DOT11_CIPHER_ALGO_TKIP = 0x02,
    DOT11_CIPHER_ALGO_CCMP = 0x04,
    DOT11_CIPHER_ALGO_WEP104 = 0x05,
    DOT11_CIPHER_ALGO_WPA_USE_GROUP = 0x100,
    DOT11_CIPHER_ALGO_RSN_USE_GROUP = 0x100,
    DOT11_CIPHER_ALGO_WEP = 0x101,
    DOT11_CIPHER_ALGO_IHV_START = 0x80000000,
    DOT11_CIPHER_ALGO_IHV_END = 0xffffffff
};

struct WLAN_AVAILABLE_NETWORK {
    WCHAR strProfileName[WLAN_MAX_NAME_LENGTH];
    DOT11_SSID dot11Ssid;
    DOT11_BSS_TYPE dot11BssType;
    ULONG uNumberOfBssids;
    BOOL bNetworkConnectable;
    DWORD wlanNotConnectableReason;
    ULONG uNumberOfPhyTypes;
    DOT11_PHY_TYPE dot11PhyTypes[WLAN_MAX_PHY_TYPE_NUMBER];
    BOOL bMorePhyTypes;
    ULONG wlanSignalQuality;
    BOOL bSecurityEnabled;
    DOT11_AUTH_ALGORITHM dot11DefaultAuthAlgorithm;
    DOT11_CIPHER_ALGORITHM dot11DefaultCipherAlgorithm;
    DWORD dwFlags;
    DWORD dwReserved;
};

struct WLAN_AVAILABLE_NETWORK_LIST {
    DWORD dwNumberOfItems;
    DWORD dwIndex;
    WLAN_AVAILABLE_NETWORK Network[1];
};

enum WLAN_INTF_OPCODE {
    wlan_intf_opcode_autoconf_start = 0x000000000,
    wlan_intf_opcode_autoconf_enabled,
    wlan_intf_opcode_background_scan_enabled,
    wlan_intf_opcode_media_streaming_mode,
    wlan_intf_opcode_radio_state,
    wlan_intf_opcode_bss_type,
    wlan_intf_opcode_interface_state,
    wlan_intf_opcode_current_connection,
    wlan_intf_opcode_channel_number,
    wlan_intf_opcode_supported_infrastructure_auth_cipher_pairs,
    wlan_intf_opcode_supported_adhoc_auth_cipher_pairs,
    wlan_intf_opcode_supported_country_or_region_string_list,
    wlan_intf_opcode_current_operation_mode,
    wlan_intf_opcode_supported_safe_mode,
    wlan_intf_opcode_certified_safe_mode,
    wlan_intf_opcode_autoconf_end = 0x0fffffff,
    wlan_intf_opcode_msm_start = 0x10000100,
    wlan_intf_opcode_statistics,
    wlan_intf_opcode_rssi,
    wlan_intf_opcode_msm_end = 0x1fffffff,
    wlan_intf_opcode_security_start = 0x20010000,
    wlan_intf_opcode_security_end = 0x2fffffff,
    wlan_intf_opcode_ihv_start = 0x30000000,
    wlan_intf_opcode_ihv_end = 0x3fffffff
};

enum WLAN_OPCODE_VALUE_TYPE {
    wlan_opcode_value_type_query_only = 0,
    wlan_opcode_value_type_set_by_group_policy,
    wlan_opcode_value_type_set_by_user,
    wlan_opcode_value_type_invalid
};

enum WLAN_CONNECTION_MODE {
    wlan_connection_mode_profile = 0,
    wlan_connection_mode_temporary_profile,
    wlan_connection_mode_discovery_secure,
    wlan_connection_mode_discovery_unsecure,
    wlan_connection_mode_auto,
    wlan_connection_mode_invalid
};

struct WLAN_CONNECTION_PARAMETERS {
    WLAN_CONNECTION_MODE wlanConnectionMode;
    LPCWSTR strProfile;
    DOT11_SSID *pDot11Ssid;
    DOT11_BSSID_LIST *pDesiredBssidList;
    DOT11_BSS_TYPE dot11BssType;
    DWORD dwFlags;
};

struct WLAN_RAW_DATA {
    DWORD dwDataSize;
    BYTE DataBlob[1];
};

enum WLAN_NOTIFICATION_ACM {
    wlan_notification_acm_start = 0,
    wlan_notification_acm_autoconf_enabled,
    wlan_notification_acm_autoconf_disabled,
    wlan_notification_acm_background_scan_enabled,
    wlan_notification_acm_background_scan_disabled,
    wlan_notification_acm_bss_type_change,
    wlan_notification_acm_power_setting_change,
    wlan_notification_acm_scan_complete,
    wlan_notification_acm_scan_fail,
    wlan_notification_acm_connection_start,
    wlan_notification_acm_connection_complete,
    wlan_notification_acm_connection_attempt_fail,
    wlan_notification_acm_filter_list_change,
    wlan_notification_acm_interface_arrival,
    wlan_notification_acm_interface_removal,
    wlan_notification_acm_profile_change,
    wlan_notification_acm_profile_name_change,
    wlan_notification_acm_profiles_exhausted,
    wlan_notification_acm_network_not_available,
    wlan_notification_acm_network_available,
    wlan_notification_acm_disconnecting,
    wlan_notification_acm_disconnected,
    wlan_notification_acm_adhoc_network_state_change,
    wlan_notification_acm_end
};

struct WLAN_ASSOCIATION_ATTRIBUTES {
    DOT11_SSID dot11Ssid;
    DOT11_BSS_TYPE dot11BssType;
    DOT11_MAC_ADDRESS dot11Bssid;
    DOT11_PHY_TYPE dot11PhyType;
    ULONG uDot11PhyIndex;
    ULONG wlanSignalQuality;
    ULONG ulRxRate;
    ULONG ulTxRate;
};

struct WLAN_SECURITY_ATTRIBUTES {
    BOOL bSecurityEnabled;
    BOOL bOneXEnabled;
    DOT11_AUTH_ALGORITHM dot11AuthAlgorithm;
    DOT11_CIPHER_ALGORITHM dot11CipherAlgorithm;
};

struct WLAN_CONNECTION_ATTRIBUTES {
    WLAN_INTERFACE_STATE isState;
    WLAN_CONNECTION_MODE wlanConnectionMode;
    WCHAR strProfileName[WLAN_MAX_NAME_LENGTH];
    WLAN_ASSOCIATION_ATTRIBUTES wlanAssociationAttributes;
    WLAN_SECURITY_ATTRIBUTES wlanSecurityAttributes;
};

typedef void (WINAPI *WLAN_NOTIFICATION_CALLBACK)(WLAN_NOTIFICATION_DATA *, PVOID);

typedef DWORD (WINAPI *WlanOpenHandleProto)
    (DWORD dwClientVersion, PVOID pReserved, PDWORD pdwNegotiatedVersion, PHANDLE phClientHandle);
typedef DWORD (WINAPI *WlanRegisterNotificationProto)
    (HANDLE hClientHandle, DWORD dwNotifSource, BOOL bIgnoreDuplicate,
     WLAN_NOTIFICATION_CALLBACK funcCallback, PVOID pCallbackContext,
     PVOID pReserved, PDWORD pdwPrevNotifSource);
typedef DWORD (WINAPI *WlanEnumInterfacesProto)
    (HANDLE hClientHandle, PVOID pReserved, WLAN_INTERFACE_INFO_LIST **ppInterfaceList);
typedef DWORD (WINAPI *WlanGetAvailableNetworkListProto)
    (HANDLE hClientHandle, const GUID* pInterfaceGuid, DWORD dwFlags, PVOID pReserved,
     WLAN_AVAILABLE_NETWORK_LIST **ppAvailableNetworkList);
typedef DWORD (WINAPI *WlanQueryInterfaceProto)
    (HANDLE hClientHandle, const GUID *pInterfaceGuid, WLAN_INTF_OPCODE OpCode, PVOID pReserved,
     PDWORD pdwDataSize, PVOID *ppData, WLAN_OPCODE_VALUE_TYPE *pWlanOpcodeValueType);
typedef DWORD (WINAPI *WlanConnectProto)
    (HANDLE hClientHandle, const GUID *pInterfaceGuid,
     const WLAN_CONNECTION_PARAMETERS *pConnectionParameters, PVOID pReserved);
typedef DWORD (WINAPI *WlanDisconnectProto)
    (HANDLE hClientHandle, const GUID *pInterfaceGuid, PVOID pReserved);
typedef DWORD (WINAPI *WlanScanProto)
    (HANDLE hClientHandle, const GUID *pInterfaceGuid, const DOT11_SSID *pDot11Ssid,
     const WLAN_RAW_DATA *pIeData, PVOID pReserved);
typedef VOID (WINAPI *WlanFreeMemoryProto)(PVOID pMemory);
typedef DWORD (WINAPI *WlanCloseHandleProto)(HANDLE hClientHandle, PVOID pReserved);

static WlanOpenHandleProto local_WlanOpenHandle = 0;
static WlanRegisterNotificationProto local_WlanRegisterNotification = 0;
static WlanEnumInterfacesProto local_WlanEnumInterfaces = 0;
static WlanGetAvailableNetworkListProto local_WlanGetAvailableNetworkList = 0;
static WlanQueryInterfaceProto local_WlanQueryInterface = 0;
static WlanConnectProto local_WlanConnect = 0;
static WlanDisconnectProto local_WlanDisconnect = 0;
static WlanScanProto local_WlanScan = 0;
static WlanFreeMemoryProto local_WlanFreeMemory = 0;
static WlanCloseHandleProto local_WlanCloseHandle = 0;

static void resolveLibrary()
{
    static volatile bool triedResolve = false;

    if (!triedResolve) {
#ifndef QT_NO_THREAD
        QMutexLocker locker(QMutexPool::globalInstanceGet(&local_WlanOpenHandle));
#endif

        if (!triedResolve) {
            local_WlanOpenHandle = (WlanOpenHandleProto)
                QLibrary::resolve(QLatin1String("wlanapi.dll"), "WlanOpenHandle");
            local_WlanRegisterNotification = (WlanRegisterNotificationProto)
                QLibrary::resolve(QLatin1String("wlanapi.dll"), "WlanRegisterNotification");
            local_WlanEnumInterfaces = (WlanEnumInterfacesProto)
                QLibrary::resolve(QLatin1String("wlanapi.dll"), "WlanEnumInterfaces");
            local_WlanGetAvailableNetworkList = (WlanGetAvailableNetworkListProto)
                QLibrary::resolve(QLatin1String("wlanapi.dll"), "WlanGetAvailableNetworkList");
            local_WlanQueryInterface = (WlanQueryInterfaceProto)
                QLibrary::resolve(QLatin1String("wlanapi.dll"), "WlanQueryInterface");
            local_WlanConnect = (WlanConnectProto)
                QLibrary::resolve(QLatin1String("wlanapi.dll"), "WlanConnect");
            local_WlanDisconnect = (WlanDisconnectProto)
                QLibrary::resolve(QLatin1String("wlanapi.dll"), "WlanDisconnect");
            local_WlanScan = (WlanScanProto)
                QLibrary::resolve(QLatin1String("wlanapi.dll"), "WlanScan");
            local_WlanFreeMemory = (WlanFreeMemoryProto)
                QLibrary::resolve(QLatin1String("wlanapi.dll"), "WlanFreeMemory");
            local_WlanCloseHandle = (WlanCloseHandleProto)
                QLibrary::resolve(QLatin1String("wlanapi.dll"), "WlanCloseHandle");

            triedResolve = true;
        }
    }
}

void qNotificationCallback(WLAN_NOTIFICATION_DATA *data, QNativeWifiEngine *d)
{
    Q_UNUSED(d);

    switch (data->NotificationCode) {
    case wlan_notification_acm_connection_complete:
    case wlan_notification_acm_disconnected:
        d->emitConfigurationsChanged();
        break;
    default:
        qDebug() << "wlan unknown notification";
    }
}

QNativeWifiEngine::QNativeWifiEngine(QObject *parent)
:   QNetworkSessionEngine(parent), handle(0)
{
    DWORD clientVersion;

    DWORD result = local_WlanOpenHandle(1, 0, &clientVersion, &handle);
    if (result != ERROR_SUCCESS) {
        if (result != ERROR_SERVICE_NOT_ACTIVE)
            qWarning("%s: WlanOpenHandle failed with error %ld\n", __FUNCTION__, result);

        return;
    }

    result = local_WlanRegisterNotification(handle, WLAN_NOTIFICATION_SOURCE_ALL, true,
                                            WLAN_NOTIFICATION_CALLBACK(qNotificationCallback),
                                            this, 0, 0);
    if (result != ERROR_SUCCESS)
        qWarning("%s: WlanRegisterNotification failed with error %ld\n", __FUNCTION__, result);

    // On Windows XP SP2 and SP3 only connection and disconnection notifications are available.
    // We need to poll for changes in available wireless networks.
    connect(&pollTimer, SIGNAL(timeout()), this, SIGNAL(configurationsChanged()));
    pollTimer.setInterval(10000);
}

QNativeWifiEngine::~QNativeWifiEngine()
{
    local_WlanCloseHandle(handle, 0);
}

QList<QNetworkConfigurationPrivate *> QNativeWifiEngine::getConfigurations(bool *ok)
{
    if (ok)
        *ok = false;

    QList<QNetworkConfigurationPrivate *> foundConfigurations;

    // enumerate interfaces
    WLAN_INTERFACE_INFO_LIST *interfaceList;
    DWORD result = local_WlanEnumInterfaces(handle, 0, &interfaceList);
    if (result != ERROR_SUCCESS) {
        qWarning("%s: WlanEnumInterfaces failed with error %ld\n", __FUNCTION__, result);
        return foundConfigurations;
    }

    for (unsigned int i = 0; i < interfaceList->dwNumberOfItems; ++i) {
        const WLAN_INTERFACE_INFO &interface = interfaceList->InterfaceInfo[i];

        WLAN_AVAILABLE_NETWORK_LIST *networkList;
        result = local_WlanGetAvailableNetworkList(handle, &interface.InterfaceGuid,
                                                   3, 0, &networkList);
        if (result != ERROR_SUCCESS) {
            qWarning("%s: WlanGetAvailableNetworkList failed with error %ld\n",
                     __FUNCTION__, result);
            continue;
        }

        QStringList seenNetworks;

        for (unsigned int j = 0; j < networkList->dwNumberOfItems; ++j) {
            WLAN_AVAILABLE_NETWORK &network = networkList->Network[j];

            QString networkName;

            if (network.strProfileName[0] != 0) {
                networkName = QString::fromWCharArray(network.strProfileName);
            } else {
                networkName = QByteArray(reinterpret_cast<char *>(network.dot11Ssid.ucSSID),
                                         network.dot11Ssid.uSSIDLength);
            }

            // don't add duplicate networks
            if (seenNetworks.contains(networkName))
                continue;
            else
                seenNetworks.append(networkName);

            QNetworkConfigurationPrivate *cpPriv = new QNetworkConfigurationPrivate;

            cpPriv->isValid = true;

            cpPriv->name = networkName;
            cpPriv->id = QString::number(qHash(QLatin1String("WLAN:") + cpPriv->name));

            if (!(network.dwFlags & WLAN_AVAILABLE_NETWORK_HAS_PROFILE))
                cpPriv->state = QNetworkConfiguration::Undefined;

            if (network.strProfileName[0] != 0) {
                if (network.bNetworkConnectable) {
                    if (network.dwFlags & WLAN_AVAILABLE_NETWORK_CONNECTED)
                        cpPriv->state = QNetworkConfiguration::Active;
                    else
                        cpPriv->state = QNetworkConfiguration::Discovered;
                } else {
                    cpPriv->state = QNetworkConfiguration::Defined;
                }
            }

            cpPriv->type = QNetworkConfiguration::InternetAccessPoint;
            cpPriv->bearer = QLatin1String("WLAN");


            foundConfigurations.append(cpPriv);
        }

        local_WlanFreeMemory(networkList);
    }

    local_WlanFreeMemory(interfaceList);

    if (ok)
        *ok = true;

    pollTimer.start();

    return foundConfigurations;
}

QString QNativeWifiEngine::getInterfaceFromId(const QString &id)
{
    // enumerate interfaces
    WLAN_INTERFACE_INFO_LIST *interfaceList;
    DWORD result = local_WlanEnumInterfaces(handle, 0, &interfaceList);
    if (result != ERROR_SUCCESS) {
        qWarning("%s: WlanEnumInterfaces failed with error %ld\n", __FUNCTION__, result);
        return QString();
    }

    for (unsigned int i = 0; i < interfaceList->dwNumberOfItems; ++i) {
        const WLAN_INTERFACE_INFO &interface = interfaceList->InterfaceInfo[i];

        DWORD dataSize;
        WLAN_CONNECTION_ATTRIBUTES *connectionAttributes;
        result = local_WlanQueryInterface(handle, &interface.InterfaceGuid,
                                          wlan_intf_opcode_current_connection, 0, &dataSize,
                                          reinterpret_cast<PVOID *>(&connectionAttributes), 0);
        if (result != ERROR_SUCCESS) {
            if (result != ERROR_INVALID_STATE)
                qWarning("%s: WlanQueryInterface failed with error %ld\n", __FUNCTION__, result);

            continue;
        }

        if (qHash(QLatin1String("WLAN:") +
                  QString::fromWCharArray(connectionAttributes->strProfileName)) == id.toUInt()) {
            QString guid("{%1-%2-%3-%4%5-%6%7%8%9%10%11}");

            guid = guid.arg(interface.InterfaceGuid.Data1, 8, 16, QChar('0'));
            guid = guid.arg(interface.InterfaceGuid.Data2, 4, 16, QChar('0'));
            guid = guid.arg(interface.InterfaceGuid.Data3, 4, 16, QChar('0'));
            for (int i = 0; i < 8; ++i)
                guid = guid.arg(interface.InterfaceGuid.Data4[i], 2, 16, QChar('0'));

            local_WlanFreeMemory(connectionAttributes);

            return guid.toUpper();
        }

        local_WlanFreeMemory(connectionAttributes);
        local_WlanFreeMemory(interfaceList);
    }

    return QString();
}

bool QNativeWifiEngine::hasIdentifier(const QString &id)
{
    // enumerate interfaces
    WLAN_INTERFACE_INFO_LIST *interfaceList;
    DWORD result = local_WlanEnumInterfaces(handle, 0, &interfaceList);
    if (result != ERROR_SUCCESS) {
        qWarning("%s: WlanEnumInterfaces failed with error %ld\n", __FUNCTION__, result);
        return false;
    }

    for (unsigned int i = 0; i < interfaceList->dwNumberOfItems; ++i) {
        const WLAN_INTERFACE_INFO &interface = interfaceList->InterfaceInfo[i];

        WLAN_AVAILABLE_NETWORK_LIST *networkList;
        result = local_WlanGetAvailableNetworkList(handle, &interface.InterfaceGuid,
                                                   3, 0, &networkList);
        if (result != ERROR_SUCCESS) {
            qWarning("%s: WlanGetAvailableNetworkList failed with error %ld\n",
                     __FUNCTION__, result);
            continue;
        }

        for (unsigned int j = 0; j < networkList->dwNumberOfItems; ++j) {
            WLAN_AVAILABLE_NETWORK &network = networkList->Network[j];

            QString networkName;

            if (network.strProfileName[0] != 0) {
                networkName = QString::fromWCharArray(network.strProfileName);
            } else {
                networkName = QByteArray(reinterpret_cast<char *>(network.dot11Ssid.ucSSID),
                                         network.dot11Ssid.uSSIDLength);
            }

            if (qHash(QLatin1String("WLAN:") + networkName) == id.toUInt()) {
                local_WlanFreeMemory(networkList);
                local_WlanFreeMemory(interfaceList);
                return true;
            }
        }

        local_WlanFreeMemory(networkList);
    }

    local_WlanFreeMemory(interfaceList);

    return false;
}

/*QString QNativeWifiEngine::bearerName(const QString &)
{
    return QLatin1String("WLAN");
}*/

void QNativeWifiEngine::connectToId(const QString &id)
{
    WLAN_INTERFACE_INFO_LIST *interfaceList;
    DWORD result = local_WlanEnumInterfaces(handle, 0, &interfaceList);
    if (result != ERROR_SUCCESS) {
        qWarning("%s: WlanEnumInterfaces failed with error %ld\n", __FUNCTION__, result);
        emit connectionError(id, InterfaceLookupError);
        return;
    }

    QString profile;

    for (unsigned int i = 0; i < interfaceList->dwNumberOfItems; ++i) {
        const WLAN_INTERFACE_INFO &interface = interfaceList->InterfaceInfo[i];

        WLAN_AVAILABLE_NETWORK_LIST *networkList;
        result = local_WlanGetAvailableNetworkList(handle, &interface.InterfaceGuid,
                                                   3, 0, &networkList);
        if (result != ERROR_SUCCESS) {
            qWarning("%s: WlanGetAvailableNetworkList failed with error %ld\n",
                     __FUNCTION__, result);
            continue;
        }

        for (unsigned int j = 0; j < networkList->dwNumberOfItems; ++j) {
            WLAN_AVAILABLE_NETWORK &network = networkList->Network[j];

            profile = QString::fromWCharArray(network.strProfileName);

            if (qHash(QLatin1String("WLAN:") + profile) == id.toUInt())
                break;
            else
                profile.clear();
        }

        local_WlanFreeMemory(networkList);

        if (!profile.isEmpty()) {
            WLAN_CONNECTION_PARAMETERS parameters;
            parameters.wlanConnectionMode = wlan_connection_mode_profile;
            parameters.strProfile = reinterpret_cast<LPCWSTR>(profile.utf16());
            parameters.pDot11Ssid = 0;
            parameters.pDesiredBssidList = 0;
            parameters.dot11BssType = dot11_BSS_type_any;
            parameters.dwFlags = 0;

            DWORD result = local_WlanConnect(handle, &interface.InterfaceGuid, &parameters, 0);
            if (result != ERROR_SUCCESS) {
                qWarning("%s: WlanConnect failed with error %ld\n", __FUNCTION__, result);
                emit connectionError(id, ConnectError);
                break;
            }

            break;
        }
    }

    local_WlanFreeMemory(interfaceList);

    if (profile.isEmpty())
        emit connectionError(id, InterfaceLookupError);
}

void QNativeWifiEngine::disconnectFromId(const QString &id)
{
    QString interface = getInterfaceFromId(id);

    if (interface.isEmpty()) {
        emit connectionError(id, InterfaceLookupError);
        return;
    }

    QStringList split = interface.mid(1, interface.length() - 2).split('-');

    GUID guid;
    guid.Data1 = split.at(0).toUInt(0, 16);
    guid.Data2 = split.at(1).toUShort(0, 16);
    guid.Data3 = split.at(2).toUShort(0, 16);
    guid.Data4[0] = split.at(3).left(2).toUShort(0, 16);
    guid.Data4[1] = split.at(3).right(2).toUShort(0, 16);
    for (int i = 0; i < 6; ++i)
        guid.Data4[i + 2] = split.at(4).mid(i*2, 2).toUShort(0, 16);

    DWORD result = local_WlanDisconnect(handle, &guid, 0);
    if (result != ERROR_SUCCESS) {
        qWarning("%s: WlanDisconnect failed with error %ld\n", __FUNCTION__, result);
        emit connectionError(id, DisconnectionError);
        return;
    }
}

void QNativeWifiEngine::requestUpdate()
{
    // enumerate interfaces
    WLAN_INTERFACE_INFO_LIST *interfaceList;
    DWORD result = local_WlanEnumInterfaces(handle, 0, &interfaceList);
    if (result != ERROR_SUCCESS) {
        qWarning("%s: WlanEnumInterfaces failed with error %ld\n", __FUNCTION__, result);
        return;
    }

    for (unsigned int i = 0; i < interfaceList->dwNumberOfItems; ++i) {
        result = local_WlanScan(handle, &interfaceList->InterfaceInfo[i].InterfaceGuid, 0, 0, 0);
        if (result != ERROR_SUCCESS)
            qWarning("%s: WlanScan failed with error %ld\n", __FUNCTION__, result);
    }
}

QNativeWifiEngine *QNativeWifiEngine::instance()
{
    resolveLibrary();

    // native wifi dll not available
    if (!local_WlanOpenHandle)
        return 0;

    QNativeWifiEngine *engine = nativeWifiEngine();

    // could not initialise subsystem
    if (engine && engine->handle == 0)
        return 0;

    return engine;
}

#include "moc_qnativewifiengine_win_p.cpp"

QTM_END_NAMESPACE

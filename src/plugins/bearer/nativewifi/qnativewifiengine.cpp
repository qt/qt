/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qnativewifiengine.h"
#include "platformdefs.h"

#include <QtNetwork/private/qnetworkconfiguration_p.h>

#include <QtCore/qstringlist.h>

#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

Q_GLOBAL_STATIC(QNativeWifiEngine, nativeWifiEngine)

WlanOpenHandleProto local_WlanOpenHandle = 0;
WlanRegisterNotificationProto local_WlanRegisterNotification = 0;
WlanEnumInterfacesProto local_WlanEnumInterfaces = 0;
WlanGetAvailableNetworkListProto local_WlanGetAvailableNetworkList = 0;
WlanQueryInterfaceProto local_WlanQueryInterface = 0;
WlanConnectProto local_WlanConnect = 0;
WlanDisconnectProto local_WlanDisconnect = 0;
WlanScanProto local_WlanScan = 0;
WlanFreeMemoryProto local_WlanFreeMemory = 0;
WlanCloseHandleProto local_WlanCloseHandle = 0;



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

QString QNativeWifiEngine::bearerName(const QString &)
{
    return QLatin1String("WLAN");
}

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

QT_END_NAMESPACE

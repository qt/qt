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

#include "qgenericengine_p.h"
#include "qnetworkconfiguration_p.h"

#include <QtCore/qthread.h>
#include <QtCore/qmutex.h>
#include <QtCore/qcoreapplication.h>
#include <QtCore/qstringlist.h>

#include <QtCore/qdebug.h>

#ifdef Q_OS_WIN
#include "qnetworksessionengine_win_p.h"
#endif

#ifdef Q_OS_LINUX
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/if_arp.h>
#endif

QTM_BEGIN_NAMESPACE

Q_GLOBAL_STATIC(QGenericEngine, genericEngine)

static QString qGetInterfaceType(const QString &interface)
{
#ifdef Q_OS_WIN32
    unsigned long oid;
    DWORD bytesWritten;

    NDIS_MEDIUM medium;
    NDIS_PHYSICAL_MEDIUM physicalMedium;

    HANDLE handle = CreateFile((TCHAR *)QString("\\\\.\\%1").arg(interface).utf16(), 0,
                               FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if (handle == INVALID_HANDLE_VALUE)
        return QLatin1String("Unknown");

    oid = OID_GEN_MEDIA_SUPPORTED;
    bytesWritten = 0;
    bool result = DeviceIoControl(handle, IOCTL_NDIS_QUERY_GLOBAL_STATS, &oid, sizeof(oid),
                                  &medium, sizeof(medium), &bytesWritten, 0);
    if (!result) {
        CloseHandle(handle);
        return QLatin1String("Unknown");
    }

    oid = OID_GEN_PHYSICAL_MEDIUM;
    bytesWritten = 0;
    result = DeviceIoControl(handle, IOCTL_NDIS_QUERY_GLOBAL_STATS, &oid, sizeof(oid),
                             &physicalMedium, sizeof(physicalMedium), &bytesWritten, 0);
    if (!result) {
        CloseHandle(handle);

        if (medium == NdisMedium802_3)
            return QLatin1String("Ethernet");
        else
            return QLatin1String("Unknown");
    }

    CloseHandle(handle);

    if (medium == NdisMedium802_3) {
        switch (physicalMedium) {
        case NdisPhysicalMediumWirelessLan:
            return QLatin1String("WLAN");
        case NdisPhysicalMediumBluetooth:
            return QLatin1String("Bluetooth");
        case NdisPhysicalMediumWiMax:
            return QLatin1String("WiMAX");
        default:
#ifdef BEARER_MANAGEMENT_DEBUG
            qDebug() << "Physical Medium" << physicalMedium;
#endif
            return QLatin1String("Ethernet");
        }
    }

#ifdef BEARER_MANAGEMENT_DEBUG
    qDebug() << medium << physicalMedium;
#endif
#elif defined(Q_OS_LINUX)
    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    ifreq request;
    strncpy(request.ifr_name, interface.toLocal8Bit().data(), sizeof(request.ifr_name));
    if (ioctl(sock, SIOCGIFHWADDR, &request) >= 0) {
        switch (request.ifr_hwaddr.sa_family) {
        case ARPHRD_ETHER:
            return QLatin1String("Ethernet");
        }
    }

    close(sock);
#else
    Q_UNUSED(interface);
#endif

    return QLatin1String("Unknown");
}

QGenericEngine::QGenericEngine(QObject *parent)
:   QNetworkSessionEngine(parent)
{
    connect(&pollTimer, SIGNAL(timeout()), this, SIGNAL(configurationsChanged()));
    pollTimer.setInterval(10000);
}

QGenericEngine::~QGenericEngine()
{
}

QList<QNetworkConfigurationPrivate *> QGenericEngine::getConfigurations(bool *ok)
{
    if (ok)
        *ok = true;

    QList<QNetworkConfigurationPrivate *> foundConfigurations;

    // Immediately after connecting with a wireless access point
    // QNetworkInterface::allInterfaces() will sometimes return an empty list. Calling it again a
    // second time results in a non-empty list. If we loose interfaces we will end up removing
    // network configurations which will break current sessions.
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    if (interfaces.isEmpty())
        interfaces = QNetworkInterface::allInterfaces();

    // create configuration for each interface
    while (!interfaces.isEmpty()) {
        QNetworkInterface interface = interfaces.takeFirst();

        if (!interface.isValid())
            continue;

        // ignore loopback interface
        if (interface.flags() & QNetworkInterface::IsLoopBack)
            continue;

        // ignore WLAN interface handled in seperate engine
        if (qGetInterfaceType(interface.name()) == "WLAN")
            continue;

        QNetworkConfigurationPrivate *cpPriv = new QNetworkConfigurationPrivate;
        const QString humanReadableName = interface.humanReadableName();
        cpPriv->name = humanReadableName.isEmpty() ? interface.name() : humanReadableName;
        cpPriv->isValid = true;

        uint identifier;
        if (interface.index())
            identifier = qHash(QLatin1String("NLA:") + QString::number(interface.index()));
        else
            identifier = qHash(QLatin1String("NLA:") + interface.hardwareAddress());

        cpPriv->id = QString::number(identifier);
        cpPriv->state = QNetworkConfiguration::Discovered;
        cpPriv->type = QNetworkConfiguration::InternetAccessPoint;
        if (interface.name().isEmpty())
            cpPriv->bearer = QLatin1String("Unknown");
        else
            cpPriv->bearer = qGetInterfaceType(interface.name());

        if (interface.flags() & QNetworkInterface::IsUp)
            cpPriv->state |= QNetworkConfiguration::Active;

        configurationInterface[identifier] = interface.name();

        foundConfigurations.append(cpPriv);
    }

    pollTimer.start();

    return foundConfigurations;
}

QString QGenericEngine::getInterfaceFromId(const QString &id)
{
    return configurationInterface.value(id.toUInt());
}

bool QGenericEngine::hasIdentifier(const QString &id)
{
    return configurationInterface.contains(id.toUInt());
}

/*QString QGenericEngine::bearerName(const QString &id)
{
    QString interface = getInterfaceFromId(id);

    if (interface.isEmpty())
        return QLatin1String("Unknown");

    return qGetInterfaceType(interface);
}*/

void QGenericEngine::connectToId(const QString &id)
{
    emit connectionError(id, OperationNotSupported);
}

void QGenericEngine::disconnectFromId(const QString &id)
{
    emit connectionError(id, OperationNotSupported);
}

void QGenericEngine::requestUpdate()
{
    emit configurationsChanged();
}

QGenericEngine *QGenericEngine::instance()
{
    return genericEngine();
}

#include "moc_qgenericengine_p.cpp"
QTM_END_NAMESPACE


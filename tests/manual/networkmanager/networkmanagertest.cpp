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

#include <QDBusConnection>
#include <QDBusError>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDBusReply>
#include <QtDBus>
#include <QHostAddress>
#include <QDebug>
#include <NetworkManager/NetworkManager.h>
#include <QApplication>
#include <QMainWindow>
#include "nmview.h"

#include <arpa/inet.h>

typedef QMap< QString, QMap<QString,QVariant> > SettingsMap;
Q_DECLARE_METATYPE(SettingsMap)

void printConnectionDetails(const QString& service)
{
    QDBusConnection dbc = QDBusConnection::systemBus();
    if (!dbc.isConnected()) {
        qWarning() << "Unable to connect to D-Bus:" << dbc.lastError();
        return;
    }
    QDBusInterface allCons(service, NM_DBUS_PATH_SETTINGS, NM_DBUS_IFACE_SETTINGS, dbc);
    if (allCons.isValid()) {
        QDBusReply<QList<QDBusObjectPath> > reply = allCons.call("ListConnections");
        if ( reply.isValid() ) {
            qWarning() << "Known connections:";
            QList<QDBusObjectPath> list = reply.value();
            foreach(QDBusObjectPath path, list) {
                qWarning() << "  " << path.path();
                QDBusInterface sysIface(NM_DBUS_SERVICE_SYSTEM_SETTINGS, path.path(), NM_DBUS_IFACE_SETTINGS_CONNECTION, dbc);
                if (sysIface.isValid()) {
                    QDBusMessage r = sysIface.call("GetSettings");
                    QDBusReply< SettingsMap > rep = sysIface.call("GetSettings");
                    qWarning() << "     GetSettings:" << r.arguments() << r.signature() << rep.isValid() << sysIface.lastError();
                    QMap< QString, QMap<QString,QVariant> > map = rep.value();
                    QList<QString> list = map.keys();
                    foreach (QString key, list) {
                        QMap<QString,QVariant> innerMap = map[key];
                        qWarning() << "       Key: " << key;
                        QMap<QString,QVariant>::const_iterator i = innerMap.constBegin();
                        while (i != innerMap.constEnd()) {
                            QString k = i.key();
                            qWarning() << "          Key: " << k << " Entry: " << i.value();
                            if (k == "addresses" && i.value().canConvert<QDBusArgument>()) {
                                QDBusArgument arg = i.value().value<QDBusArgument>();
                                arg.beginArray();
                                while (!arg.atEnd()) {
                                    QDBusVariant addr;
                                    arg >> addr;
                                    uint ip = addr.variant().toUInt();
                                    qWarning() << ip;
                                    qWarning() << "        " << QHostAddress(htonl(ip)).toString();
                                }

                            }
                            i++;
                        }
                    }
                }
            }
        }
    }


}

void readConnectionManagerDetails()
{
    qDBusRegisterMetaType<SettingsMap>();
    QDBusConnection dbc = QDBusConnection::systemBus();
    if (!dbc.isConnected()) {
        qWarning() << "Unable to connect to D-Bus:" << dbc.lastError();
        return;
    }
    
    QDBusInterface iface(NM_DBUS_SERVICE, NM_DBUS_PATH, NM_DBUS_INTERFACE, dbc);
    if (!iface.isValid()) {
        qWarning() << "Could not find NetworkManager";
        return;
    }

    uint state = iface.property("State").toUInt();
    switch(state) {
        case NM_STATE_UNKNOWN:
            qWarning() << "State: Unknown"; break;
        case NM_STATE_ASLEEP:
            qWarning() << "State: Asleep"; break;
        case NM_STATE_CONNECTING:
            qWarning() << "State: Connecting"; break;
        case NM_STATE_CONNECTED:
            qWarning() << "State: Connected"; break;
        case NM_STATE_DISCONNECTED:
            qWarning() << "State: Disconnected"; break;
    }
    //get list of network devices
    QDBusReply<QList<QDBusObjectPath> > reply = iface.call("GetDevices");
    if ( reply.isValid() ) {
        qWarning() << "Current devices:";
        QList<QDBusObjectPath> list = reply.value();
        foreach(QDBusObjectPath path, list) {
            qWarning() << "  " << path.path();
            QDBusInterface devIface(NM_DBUS_SERVICE, path.path(), NM_DBUS_INTERFACE_DEVICE, dbc);
            if (devIface.isValid()) {
                qWarning() << "     Managed: " << devIface.property("Managed").toBool();
                qWarning() << "     Interface: " << devIface.property("Interface").toString();
                qWarning() << "     HAL UDI: " << devIface.property("Udi").toString();
                qWarning() << "     Driver: " << devIface.property("Driver").toString();
                QVariant v = devIface.property("DeviceType");
                switch(v.toUInt()) {
                    case DEVICE_TYPE_UNKNOWN:
                        qWarning() << "     DeviceType: Unknown" ;
                        break;
                    case DEVICE_TYPE_802_3_ETHERNET: 
                        qWarning() << "     DeviceType: Ethernet" ;
                        break;
                    case DEVICE_TYPE_802_11_WIRELESS:
                        qWarning() << "     DeviceType: Wireless" ;
                        break;
                    case DEVICE_TYPE_GSM:
                        qWarning() << "     DeviceType: GSM" ;
                        break;
                    case DEVICE_TYPE_CDMA:
                        qWarning() << "     DeviceType: CDMA" ;
                        break;
                    
                }
                v = devIface.property("State");
                switch(v.toUInt()) {
                    case NM_DEVICE_STATE_UNKNOWN:
                        qWarning() << "     State: Unknown" ; break;
                    case NM_DEVICE_STATE_UNMANAGED:
                        qWarning() << "     State: Unmanaged" ; break;
                    case NM_DEVICE_STATE_UNAVAILABLE:
                        qWarning() << "     State: Unavailable" ; break;
                    case NM_DEVICE_STATE_DISCONNECTED:
                        qWarning() << "     State: Disconnected" ; break;
                    case NM_DEVICE_STATE_PREPARE:
                        qWarning() << "     State: Preparing" ; break;
                    case NM_DEVICE_STATE_CONFIG:
                        qWarning() << "     State: Being configured" ; break;
                    case NM_DEVICE_STATE_NEED_AUTH:
                        qWarning() << "     State: Awaiting secrets" ; break;
                    case NM_DEVICE_STATE_IP_CONFIG:
                        qWarning() << "     State: IP requested" ; break;
                    case NM_DEVICE_STATE_ACTIVATED:
                        qWarning() << "     State: Activated" ; break;
                    case NM_DEVICE_STATE_FAILED:
                        qWarning() << "     State: FAILED" ; break;
                }
                quint32 ip = devIface.property("Ip4Address").toUInt();
                qWarning() << "     IP4Address: " << QHostAddress(htonl(ip)).toString();
                if (v.toUInt() == NM_DEVICE_STATE_ACTIVATED) {
                    QString path = devIface.property("Ip4Config").value<QDBusObjectPath>().path();
                    qWarning() << "     IP4Config: " << path;
                    QDBusInterface ipIface(NM_DBUS_SERVICE, path, NM_DBUS_INTERFACE_IP4_CONFIG, dbc);
                    if (ipIface.isValid()) {
                        qWarning() << "        Hostname: " << ipIface.property("Hostname").toString();
                        qWarning() << "        Domains: " << ipIface.property("Domains").toStringList();
                        qWarning() << "        NisDomain: " << ipIface.property("NisDomain").toString();
                        QDBusArgument arg=  ipIface.property("Addresses").value<QDBusArgument>();
                        //qWarning() << "        " << arg.currentType();
                        qWarning() << "        Addresses: " << ipIface.property("Addresses");
                        qWarning() << "        Nameservers: " << ipIface.property("Nameservers");
                        qWarning() << "        NisServers: " << ipIface.property("NisServers");
                    }

                }

            }
        }
    }

    //get list of active connections
    QVariant prop = iface.property("ActiveConnections");
    QList<QDBusObjectPath> connections = prop.value<QList<QDBusObjectPath> >();
    QString activePath;
    if ( connections.count() )
        qWarning() << "Active connections:";
    foreach(QDBusObjectPath path, connections) {
        qWarning() << "  " << path.path();
        activePath = path.path();
        QString serviceName;
        QDBusInterface conIface(NM_DBUS_SERVICE, path.path(), NM_DBUS_INTERFACE_ACTIVE_CONNECTION, dbc);
        if (conIface.isValid()) {
            qWarning() << "     default connection: " << conIface.property("Default").toBool();
            serviceName = conIface.property("ServiceName").toString();
            qWarning() << "     service name: " << serviceName;
            qWarning() << "     connection path: " << conIface.property("Connection").value<QDBusObjectPath>().path();
            qWarning() << "     specific object:" << conIface.property("SpecificObject").value<QDBusObjectPath>().path();
            qWarning() << "     sharedServiceName: " << conIface.property("SharedServiceName").toString();
            QList<QDBusObjectPath> devs = conIface.property("Devices").value<QList<QDBusObjectPath> >();
            qWarning() << "     devices: ";
            foreach(QDBusObjectPath p, devs)
                qWarning() << "         " << path.path();
            QVariant v = conIface.property("State");
            switch (v.toInt()) {
                case NM_ACTIVE_CONNECTION_STATE_UNKNOWN:
                    qWarning()<< "     State: unknown"; break;
                case NM_ACTIVE_CONNECTION_STATE_ACTIVATING:
                    qWarning()<< "     State: activating"; break;
                case NM_ACTIVE_CONNECTION_STATE_ACTIVATED:
                    qWarning()<< "     State: activated"; break;
            }
        } else {
            qWarning() << conIface.lastError();
        }

    }

    printConnectionDetails(NM_DBUS_SERVICE_SYSTEM_SETTINGS);
    printConnectionDetails(NM_DBUS_SERVICE_USER_SETTINGS);


    //turn active connection off
    /*QDBusObjectPath dbop("/org/freedesktop/NetworkManager/ActiveConnection/1");
    QVariant asd = QVariant::fromValue(dbop);
    iface.call(QLatin1String("DeactivateConnection"), asd);
    qWarning() << iface.lastError();*/

    /*QDBusObjectPath p1device("/org/freedesktop/Hal/devices/net_00_60_6e_82_02_65");
    QVariant p1v = QVariant::fromValue(p1device);
    QDBusObjectPath p1con("/org/freedesktop/NetworkManagerSettings/0");
    QVariant p1c = QVariant::fromValue(p1con);
    QDBusObjectPath p1sp("");
    QVariant p1sp1 = QVariant::fromValue(p1sp);
    iface.call(QLatin1String("ActivateConnection"), 
            QString("/org/freedesktop/NetworkManagerSystemSettings"), p1c,p1v, p1v );
    qWarning() << iface.lastError();
    */
}

int main( int argc, char** argv)
{
    QApplication app(argc, argv);
    //readConnectionManagerDetails();
    QMainWindow main;
    NMView view;
    main.setCentralWidget(&view);
    main.show();
    return app.exec();

}

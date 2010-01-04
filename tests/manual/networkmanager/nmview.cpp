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

#include "nmview.h"

#include <QLabel>
#include <QLayout>
#include <QListWidget>
#include <QDebug>
#include <QPushButton>

#include <QtDBus>
#include <QtNetwork>
#include <NetworkManager/NetworkManager.h>
#include <arpa/inet.h>

#include <QDBusConnection>
#include <QDBusError>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDBusReply>

#include <QHostAddress>
#include <QNetworkInterface>
#include <QTreeWidgetItem>
#include <QMessageBox>

//#include "ui_dialog.h"

#include <qnetworkconfiguration.h>
#include <qnetworkconfigmanager.h>
#include <qnetworksession.h>
#include <qnetworkmanagerservice_p.h>

//static QDBusConnection dbc = QDBusConnection::systemBus();
//typedef QMap< QString, QMap<QString,QVariant> > SettingsMap;
//Q_DECLARE_METATYPE(SettingsMap)


NMView::NMView(QDialog* parent)
    : QDialog(parent, 0), dbc(QDBusConnection::systemBus())
{
    init();

    if (!dbc.isConnected()) {
        qWarning() << "Unable to connect to D-Bus:" << dbc.lastError();
        return;
    }
    updateConnections();
    getDevices();
}

NMView::~NMView()
{
}
void NMView::init()
{
    setupUi(this);
    sess = 0;
//    readSettings();
    connect(startButton, SIGNAL(clicked()), this, SLOT(activate()));
    connect(stopButton, SIGNAL(clicked()), this, SLOT(deactivate()));
    connect(scanButton, SIGNAL(clicked()), this, SLOT(findAccessPoints()));
    connect(updateButton, SIGNAL(clicked()), this, SLOT(update()));

    connect(cons, SIGNAL(itemActivated(QTreeWidgetItem*, int)),
            this,SLOT(connectionItemActivated(QTreeWidgetItem*, int)));


    connect(devicesTreeWidget, SIGNAL(itemActivated(QTreeWidgetItem*, int)),
            this, SLOT(deviceItemActivated(QTreeWidgetItem*, int)));

    cons->header()->setResizeMode(QHeaderView::ResizeToContents);
    devicesTreeWidget->header()->setResizeMode(QHeaderView::ResizeToContents);
    accessPointsTreeWidget->header()->setResizeMode(QHeaderView::ResizeToContents);
//    accessPointsTreeWidget->header()->setResizeMode(QHeaderView::ResizeToContents);

   manager = new QNetworkConfigurationManager(this);

   connect(manager, SIGNAL(updateCompleted()), this, SLOT(updateCompleted()));

   connect(manager, SIGNAL(configurationAdded(const QNetworkConfiguration &)),
           this, SLOT(configurationAdded(const QNetworkConfiguration&)));

   devicesTreeWidget->header()->hideSection(0);
   devicesTreeWidget->header()->hideSection(1);
   devicesTreeWidget->header()->hideSection(3);
   devicesTreeWidget->header()->hideSection(5);
   devicesTreeWidget->header()->hideSection(6);
   devicesTreeWidget->header()->hideSection(7);
   devicesTreeWidget->header()->hideSection(8);
   devicesTreeWidget->header()->hideSection(9);
}

void NMView::updateConnections()
{
    cons->clear();
    manager->updateConfigurations();
    QList<QNetworkConfiguration> configs = manager->allConfigurations();
    foreach(QNetworkConfiguration p, configs) {
        QStringList strlist;
        strlist << p.name();
        strlist << stateToString(p.state());
        strlist << p.identifier();
        strlist << typeToString(p.type());
        QTreeWidgetItem *item;
        item = new QTreeWidgetItem(strlist);
        cons->insertTopLevelItem(0, item);
    }
    cons->sortItems ( 1, Qt::AscendingOrder);
}

void NMView::getActiveConnections()
{
    QList<QNetworkConfiguration> configs = manager->allConfigurations(QNetworkConfiguration::Active);
    foreach(QNetworkConfiguration p, configs) {
        qWarning() << __FUNCTION__ << p.name() << p.identifier();
    }

//     QDBusInterface iface(NM_DBUS_SERVICE, NM_DBUS_PATH, NM_DBUS_INTERFACE, dbc);
//     if (!iface.isValid()) {
//         qWarning() << "Could not find NetworkManager";
//         return;
//     }

//     QVariant prop = iface.property("ActiveConnections");
//     QList<QDBusObjectPath> connections = prop.value<QList<QDBusObjectPath> >();
//     foreach(QDBusObjectPath path, connections) {
//         QDBusInterface conDetails(NM_DBUS_SERVICE, path.path(), NM_DBUS_INTERFACE_ACTIVE_CONNECTION, dbc);
//         if (conDetails.isValid()) {
//             QString desc = path.path();
//            conDetails.property("Connection Path").value<QDBusObjectPath>().path();

//            QList<QDBusObjectPath> devices = conDetails.property("Devices").value<QList<QDBusObjectPath> >();
//
//            foreach(QDBusObjectPath devpath, devices) {
//                QDBusInterface devIface(NM_DBUS_SERVICE, devpath.path(), NM_DBUS_INTERFACE_DEVICE, dbc);
//                desc += " "+devIface.property("Interface").toString();
//                desc +=  " "+QHostAddress(htonl(devIface.property("Ip4Address").toUInt())).toString();

//            }

//            qWarning() << conDetails.property("Connection").value<QDBusObjectPath>().path() << path.path();

//            QListWidgetItem* item = new QListWidgetItem(desc,  activeCons);
//            item->setData(Qt::UserRole, path.path());
//      }
//    }
}

void NMView::update()
{
//    QNetworkManagerInterface *dbIface;
//    dbIface = new QNetworkManagerInterface;
//    QList <QDBusObjectPath> connections = dbIface->activeConnections();
//
//    foreach(QDBusObjectPath conpath, connections) {
//        QNetworkManagerConnectionActive *aConn;
//        aConn = new QNetworkManagerConnectionActive(conpath.path());
//        // in case of accesspoint, specificObject will hold the accessPOintObjectPath
//        qWarning() << aConn->connection().path() << aConn->specificObject().path() << aConn->devices().count();
//        QList <QDBusObjectPath>  devs = aConn->devices();
//        foreach(QDBusObjectPath device, devs) {
//            qWarning() << "    " << device.path();
//        }
//    }

    QStringList connectionServices;
    connectionServices << NM_DBUS_SERVICE_SYSTEM_SETTINGS;
    connectionServices << NM_DBUS_SERVICE_USER_SETTINGS;
    foreach (QString service, connectionServices) {
        QDBusInterface allCons(service,
                               NM_DBUS_PATH_SETTINGS,
                               NM_DBUS_IFACE_SETTINGS,
                               dbc);
        if (allCons.isValid()) {
            QDBusReply<QList<QDBusObjectPath> > reply = allCons.call("ListConnections");
            if ( reply.isValid() ) {
                QList<QDBusObjectPath> list = reply.value();
                foreach(QDBusObjectPath path, list) {
                    QDBusInterface sysIface(service,
                                            path.path(),
                                            NM_DBUS_IFACE_SETTINGS_CONNECTION,
                                            dbc);
                    if (sysIface.isValid()) {
                        qWarning() << "";
                        qWarning() << path.path();

                        //                        QDBusMessage r = sysIface.call("GetSettings");
                        QDBusReply< QNmSettingsMap > rep = sysIface.call("GetSettings");

                        QMap< QString, QMap<QString,QVariant> > map = rep.value();
                        QList<QString> list = map.keys();
                        foreach (QString key, list) {
                            QMap<QString,QVariant> innerMap = map[key];
                            qWarning() << "       Key: " << key;
                            QMap<QString,QVariant>::const_iterator i = innerMap.constBegin();

                            while (i != innerMap.constEnd()) {
                                QString k = i.key();
                                qWarning() << "          Key: " << k << " Entry: " << i.value();
                                i++;
                            }//end innerMap
                        }//end foreach key
                    }//end settings connection
                } // foreach path
            } //end ListConnections
        } //end settingsInterface
    }// end services
    QDBusInterface iface(NM_DBUS_SERVICE,
                         NM_DBUS_PATH,
                         NM_DBUS_INTERFACE,
                         dbc);
    if (iface.isValid()) {
        QVariant prop = iface.property("ActiveConnections");
        QList<QDBusObjectPath> connections = prop.value<QList<QDBusObjectPath> >();
        foreach(QDBusObjectPath conpath, connections) {
            qWarning() << "Active connection" << conpath.path();
            QDBusInterface conDetails(NM_DBUS_SERVICE,
                                      conpath.path(),
                                      NM_DBUS_INTERFACE_ACTIVE_CONNECTION,
                                      dbc);
            if (conDetails.isValid()) {

                QVariant prop = conDetails.property("Connection");
                QDBusObjectPath connection = prop.value<QDBusObjectPath>();
                qWarning() << conDetails.property("Default").toBool() << connection.path();

//                QVariant Sprop = conDetails.property("Devices");
//                QList<QDBusObjectPath> so = Sprop.value<QList<QDBusObjectPath> >();
//                foreach(QDBusObjectPath device, so) {
//                    if(device.path() == devicePath) {
//                        path = connection.path();
//                    }
//                    break;
//                }
            }
        }
    }
qWarning() << "";
}

void NMView::deactivate()
{
    QList<QNetworkConfiguration> configs = manager->allConfigurations(QNetworkConfiguration::Active);
    foreach(QNetworkConfiguration p, configs) {
        qWarning() << "Active cons" << p.name();
        if(p.name() == cons->currentItem()->text(0)
            && p.identifier() == cons->currentItem()->text(2)) {
            qWarning() <<__FUNCTION__<< p.name();
            if(!sess) {
                sess = new QNetworkSession(p);

                connect(sess,  SIGNAL(stateChanged(QNetworkSession::State)),
                        this, SLOT(stateChanged(QNetworkSession::State)));

                connect(sess,  SIGNAL(error(QNetworkSession::SessionError)),
                        this,  SLOT(networkSessionError(QNetworkSession::SessionError)));
            }
            sess->close();
            delete sess;
            sess = 0;
        }
    }
}

void NMView::activate()
{
    qWarning() << __FUNCTION__ << cons->currentItem()->text(0);

    QList<QNetworkConfiguration> configs = manager->allConfigurations();
    foreach(QNetworkConfiguration p, configs) {
        if(p.name() == cons->currentItem()->text(0)) {

            sess = new QNetworkSession(p);

            connect(sess, SIGNAL(newConfigurationActivated()),
                    this, SLOT(getActiveConnections()));

            connect(sess,  SIGNAL(stateChanged(QNetworkSession::State)),
                    this, SLOT(stateChanged(QNetworkSession::State)));

            connect(sess,  SIGNAL(error(QNetworkSession::SessionError)),
                    this,  SLOT(networkSessionError(QNetworkSession::SessionError)));

            sess->open();
        }
    }
}

void NMView::getDevices()
{
    qWarning() << "";
    qWarning() << __FUNCTION__;
    devicesTreeWidget->clear();
    //qDBusRegisterMetaType<SettingsMap>();

    if (!dbc.isConnected()) {
        qWarning() << "Unable to connect to D-Bus:" << dbc.lastError();
        return;
    }
        QDBusInterface iface(NM_DBUS_SERVICE,
                             NM_DBUS_PATH,
                             NM_DBUS_INTERFACE,
                             dbc);
//NetworkManager interface
        if (!iface.isValid()) {
            qWarning() << "Could not find NetworkManager";
            return;
        }

//         uint state = iface.property("State").toUInt();
//         switch(state) {
//         case NM_STATE_UNKNOWN:
//             qWarning() << "State: Unknown"; break;
//         case NM_STATE_ASLEEP:
//             qWarning() << "State: Asleep"; break;
//         case NM_STATE_CONNECTING:
//             qWarning() << "State: Connecting"; break;
//         case NM_STATE_CONNECTED:
//             qWarning() << "State: Connected"; break;
//         case NM_STATE_DISCONNECTED:
//             qWarning() << "State: Disconnected"; break;
//         }

        //get list of network devices
        QTreeWidgetItem *item;
        QDBusReply<QList<QDBusObjectPath> > reply = iface.call("GetDevices");
        if ( reply.isValid() ) {
//            qWarning() << "Current devices:";
            QList<QDBusObjectPath> list = reply.value();
            foreach(QDBusObjectPath path, list) {
                QStringList devicesList;
                qWarning() << "  " << path.path();
                devicesList << path.path();

                QDBusInterface devIface(NM_DBUS_SERVICE,
                                        path.path(),
                                        NM_DBUS_INTERFACE_DEVICE,
                                        dbc);
                if (devIface.isValid()) {



                    ////// connect the dbus signal
//                     /*if(*/dbc.connect(NM_DBUS_SERVICE,
//                                    path.path(),
//                                    NM_DBUS_INTERFACE_DEVICE,
//                                    "StateChanged",
//                                    this,SLOT(deviceStateChanged(quint32)));
//                    {
//                        qWarning() << "XXXXXXXXXX dbus connect successful" <<  path.path();
//                    }


          //          qWarning() << "     Managed: " << devIface.property("Managed").toBool();
                    devicesList << devIface.property("Managed").toString();
 //                   qWarning() << "     Interface: " << devIface.property("Interface").toString();
                    devicesList <<  devIface.property("Interface").toString();
  //                  qWarning() << "     HAL UDI: " << devIface.property("Udi").toString();
   //                 qWarning() << "     Driver: " << devIface.property("Driver").toString();
                    devicesList  << devIface.property("Driver").toString();

                    QString x =  deviceTypeToString(devIface.property("DeviceType").toUInt());
 //                   qWarning() << "     Type:" << x;
                    devicesList << x;

                                 if( devIface.property("DeviceType").toUInt() == DEVICE_TYPE_802_11_WIRELESS) {
                                     qWarning() << "Device is WIFI";
                    //                //    findAccessPoints(path.path());
                                    }

                    QVariant v = devIface.property("State");
                    x = deviceStateToString(v.toUInt());
//                    qWarning() << "     State:" << x;
                    devicesList << x;

                    quint32 ip = devIface.property("Ip4Address").toUInt();
//                    qWarning() << "     IP4Address: " << QHostAddress(htonl(ip)).toString();
                    devicesList << QHostAddress(htonl(ip)).toString();


                    if (v.toUInt() == NM_DEVICE_STATE_ACTIVATED) {
                        QString path = devIface.property("Ip4Config").value<QDBusObjectPath>().path();
//                        qWarning() << "     IP4Config: " << path;
                        devicesList << path;
                        QDBusInterface ipIface(NM_DBUS_SERVICE,
                                               path,
                                               NM_DBUS_INTERFACE_IP4_CONFIG,
                                               dbc);
                        if (ipIface.isValid()) {
      //                      qWarning() << "        Hostname: " << ipIface.property("Hostname").toString();
                            devicesList << ipIface.property("Hostname").toString();
//                            qWarning() << "        Domains: " << ipIface.property("Domains").toStringList();
                            devicesList <<  ipIface.property("Domains").toStringList().join(", ");
//                            qWarning() << "        NisDomain: " << ipIface.property("NisDomain").toString();
                            QDBusArgument arg=  ipIface.property("Addresses").value<QDBusArgument>();
//                            qWarning() << "        Addresses: " << ipIface.property("Addresses");
//                            qWarning() << "        Nameservers: " << ipIface.property("Nameservers");
//                            qWarning() << "        NisServers: " << ipIface.property("NisServers");
                        }

                    }

                }
                item = new QTreeWidgetItem(devicesList);
                devicesTreeWidget->insertTopLevelItem(0, item);
            }
        }


//        netconfig();

}

void NMView::readSettings()
{
        QDBusInterface settingsiface(NM_DBUS_SERVICE_SYSTEM_SETTINGS,
                                     NM_DBUS_PATH_SETTINGS,
                                     NM_DBUS_IFACE_SETTINGS,
                                     dbc);
        //NetworkManagerSettings interface
        if (settingsiface.isValid()) {
            QDBusReply<QList<QDBusObjectPath> > reply = settingsiface.call("ListConnections");
            if ( reply.isValid() ) {
                QList<QDBusObjectPath> list = reply.value();
                foreach(QDBusObjectPath path, list) {
                    qWarning() <<__FUNCTION__ << path.path();

                }
            }
        }

        QDBusInterface iface(NM_DBUS_SERVICE, NM_DBUS_PATH, NM_DBUS_INTERFACE, dbc);
        if (!iface.isValid()) {
            qWarning() << "Could not find NetworkManager";
            return;
        }

        //   QStringList list = item->text().split(" -> ");

        QVariant prop = iface.property("ActiveConnections");
        QList<QDBusObjectPath> connections = prop.value<QList<QDBusObjectPath> >();
        foreach(QDBusObjectPath path, connections) {
            QDBusInterface conDetails(NM_DBUS_SERVICE,
                                      path.path(),
                                      NM_DBUS_INTERFACE_ACTIVE_CONNECTION,
                                      dbc);

            if (conDetails.isValid()) {
                QString desc = path.path();
                qWarning() << desc;
                //if ( item->text(0) == conDetails.property("Connection").value<QDBusObjectPath>().path() ) {
                    //                QListWidgetItem* item = new QTreeWidgetItem( desc,
                    //                                                             cons);
                    //                item->setData(Qt::UserRole, desc);
                    //                activeItemActivated( item );
                //}
            }
        }
}

void NMView::printConnectionDetails(const QString& service)
{
    Q_UNUSED(service);

//
//    qWarning() << __FUNCTION__ << service;
//
//    QDBusConnection dbc = QDBusConnection::systemBus();
//    if (!dbc.isConnected()) {
//        qWarning() << "Unable to connect to D-Bus:" << dbc.lastError();
//        return;
//    }
//    QDBusInterface allCons(service,
//                           NM_DBUS_PATH_SETTINGS,
//                           NM_DBUS_IFACE_SETTINGS,
//                           dbc);
//
//    if (allCons.isValid()) {
//        QDBusReply<QList<QDBusObjectPath> > reply = allCons.call("ListConnections");
//
//        if ( reply.isValid() ) {
//            qWarning() << "Known connections:";
//            QList<QDBusObjectPath> list = reply.value();
//
//            foreach(QDBusObjectPath path, list) {
//                qWarning() << "  " << path.path();
//
//                QDBusInterface sysIface(NM_DBUS_SERVICE_SYSTEM_SETTINGS,
//                                        path.path(),
//                                        NM_DBUS_IFACE_SETTINGS_CONNECTION,
//                                        dbc);
//
//                if (sysIface.isValid()) {
//                    QDBusMessage r = sysIface.call("GetSettings");
//                    QDBusReply< QSettingsMap > rep = sysIface.call("GetSettings");
//
//                    qWarning() << "     GetSettings:" << r.arguments() << r.signature() << rep.isValid() << sysIface.lastError();
//
//                    QMap< QString, QMap<QString,QVariant> > map = rep.value();
//                    QList<QString> list = map.keys();
//
//                    foreach (QString key, list) {
//                        QMap<QString,QVariant> innerMap = map[key];
//                        qWarning() << "       Key: " << key;
//                        QMap<QString,QVariant>::const_iterator i = innerMap.constBegin();
//
//                        while (i != innerMap.constEnd()) {
//                            QString k = i.key();
//                            qWarning() << "          Key: " << k << " Entry: " << i.value();
//
//                            if (k == "addresses" && i.value().canConvert<QDBusArgument>()) {
//                                QDBusArgument arg = i.value().value<QDBusArgument>();
//                                arg.beginArray();
//
//                                while (!arg.atEnd()) {
//                                    QDBusVariant addr;
//                                    arg >> addr;
//                                    uint ip = addr.variant().toUInt();
//                                    qWarning() << ip;
//                                    qWarning() << "        " << QHostAddress(htonl(ip)).toString();
//                                }
//
//                            }
//                            i++;
//                        }
//                    }
//                }
//            }
//        }
//    }
    qWarning() << "";
}

void NMView::getNetworkDevices()
{

}

void NMView::connectionItemActivated( QTreeWidgetItem * item, int col )
{
    Q_UNUSED(col);

    qWarning() <<__FUNCTION__<< item->text(0);
    QDBusInterface iface(NM_DBUS_SERVICE,
                         NM_DBUS_PATH,
                         NM_DBUS_INTERFACE,
                         dbc);
    if (!iface.isValid()) {
        qWarning() << "Could not find NetworkManager";
        return;
    }
    QVariant prop = iface.property("ActiveConnections");
    QList<QDBusObjectPath> connections = prop.value<QList<QDBusObjectPath> >();
    foreach(QDBusObjectPath path, connections) {
        QDBusInterface conDetails(NM_DBUS_SERVICE,
                                  path.path(),
                                  NM_DBUS_INTERFACE_ACTIVE_CONNECTION,
                                  dbc);

        if (conDetails.isValid()) {
            QString desc = path.path();
            qWarning() << desc;
            if ( item->text(0) == conDetails.property("Connection").value<QDBusObjectPath>().path() ) {
//                QListWidgetItem* item = new QTreeWidgetItem( desc,
//                                                             cons);
//                item->setData(Qt::UserRole, desc);
//                activeItemActivated( item );
            }
        }
    }
}



void NMView::deviceItemActivated( QTreeWidgetItem * item , int section)
{
    Q_UNUSED(item);
    Q_UNUSED(section);

  //  qWarning() << item->text(section) << item->text(4);
    //if(item->text(4) == "Wireless") findAccessPoints(item->text(0));
}


void NMView::netconfig()
{
//    qWarning() << __FUNCTION__;

//    qDBusRegisterMetaType<SettingsMap>();
    QDBusConnection dbc = QDBusConnection::systemBus();
    if (!dbc.isConnected()) {
        qWarning() << "Unable to connect to D-Bus:" << dbc.lastError();
        return;
    }

    QDBusInterface iface(NM_DBUS_SERVICE,
                         NM_DBUS_PATH,
                         NM_DBUS_IFACE_SETTINGS_CONNECTION,
                         dbc);
    if (!iface.isValid()) {
        qWarning() << "Could not find NetworkManager Settings";
        return;
    } else {
        QDBusReply<QList<QDBusObjectPath> > reply = iface.call("ListConnections");
            QList<QDBusObjectPath> list = reply.value();
//            qWarning() << reply.value();
            foreach(QDBusObjectPath path, list) {
                qWarning() << "  " << path.path();
            }
    }
}


void NMView::findAccessPoints()
{
    accessPointsTreeWidget->clear();

    QDBusInterface iface(NM_DBUS_SERVICE,
                         NM_DBUS_PATH,
                         NM_DBUS_INTERFACE,
                         dbc);

    QDBusReply<QList<QDBusObjectPath> > reply = iface.call("GetDevices");
    if ( reply.isValid() ) {
        QList<QDBusObjectPath> list = reply.value();
        foreach(QDBusObjectPath path, list) {
            QDBusInterface devIface(NM_DBUS_SERVICE,
                                    path.path(),
                                    NM_DBUS_INTERFACE_DEVICE,
                                    dbc);
            if (devIface.isValid()) {

                if( devIface.property("DeviceType").toUInt() == DEVICE_TYPE_802_11_WIRELESS) {

//                    qWarning() <<"deviface"<< devIface.path();
                    QDBusInterface wififace(NM_DBUS_SERVICE,
                                            devIface.path(),
                                            NM_DBUS_INTERFACE_DEVICE_WIRELESS,
                                            dbc);
                    if (!wififace.isValid()) {
                        qWarning() << "Could not find NetworkManager wireless interface";
                        return;
                    }

/////////////////////////
//                     if(dbc.connect(NM_DBUS_SERVICE,
//                                    path.path(),
//                                    NM_DBUS_INTERFACE_DEVICE_WIRELESS,
//                                    "PropertiesChanged",
//                                    this,SLOT(aPPropertiesChanged( QMap<QString,QVariant>))) ) {
//                         qWarning() << "PropertiesChanged connect";
//                     } else {
//                         qWarning() << "NOT connect";

//                     }
/////////////////////////
                    qWarning() <<"wififace"<< wififace.path();
                    QDBusReply<QList<QDBusObjectPath> > reply2 = wififace.call("GetAccessPoints");
                    if ( reply2.isValid() ) {
                        QTreeWidgetItem *item;
                        QList<QDBusObjectPath> list2 = reply2.value();
                        foreach(QDBusObjectPath path2, list2) {
                            QDBusInterface accessPointIface(NM_DBUS_SERVICE,
                                                            path2.path(),
                                                            NM_DBUS_INTERFACE_ACCESS_POINT,
                                                            dbc);
                            if (accessPointIface.isValid()) {
////                                qWarning() <<"path2" <<  path2.path();

//                                if(dbc.connect(NM_DBUS_SERVICE, //signal strength
//                                               path2.path(),
//                                               NM_DBUS_INTERFACE_ACCESS_POINT,
//                                               "PropertiesChanged",
//                                               this,SLOT(aPPropertiesChanged( QMap<QString,QVariant>))) ) {
//                                    qWarning() << "PropertiesChanged connect";
//                                } else {
//                                    qWarning() << "NOT connect";
//
//                                }

                                QStringList accessPoints;
                                accessPoints << accessPointIface.property("Ssid").toString();
                                accessPoints << QString::number(accessPointIface.property("Strength").toInt());
                                accessPoints << securityCapabilitiesToString(accessPointIface.property("WpaFlags" ).toInt());
                                accessPoints << securityCapabilitiesToString(accessPointIface.property("RsnFlags" ).toInt());
                                accessPoints << accessPointIface.property("Frequency" ).toString();
                                accessPoints << accessPointIface.property("HwAddress" ).toString();
                                accessPoints << deviceModeToString(accessPointIface.property("Mode" ).toInt());
                                accessPoints << accessPointIface.property("MaxBitrate" ).toString();
                                //HwAddress
                                //                if(accessPointIface.property("Flags" ).toInt() == NM_802_11_AP_FLAGS_PRIVACY ) {
                                //                    qWarning() << "Secure";
                                //                }
                                item = new QTreeWidgetItem(accessPoints);
                                accessPointsTreeWidget->insertTopLevelItem(0, item);
                            }
                        }
                    }
                }
            }
        }
    }
}

QString NMView::deviceStateToString(int state)
{
    QString ret;
    switch(state) {
    case NM_DEVICE_STATE_UNKNOWN:
        ret = "Unknown";
        break;
    case NM_DEVICE_STATE_UNMANAGED:
        ret =  "Unmanaged";
        break;
    case NM_DEVICE_STATE_UNAVAILABLE:
        ret =  "Unavailable";
        break;
    case NM_DEVICE_STATE_DISCONNECTED:
        ret = "Disconnected";
        break;
    case NM_DEVICE_STATE_PREPARE:
        ret = "Preparing to connect";
        break;
    case NM_DEVICE_STATE_CONFIG:
        ret = "Being configured";
        break;
    case NM_DEVICE_STATE_NEED_AUTH:
        ret = "Awaiting secrets";
        break;
    case NM_DEVICE_STATE_IP_CONFIG:
        ret = "IP requested";
        break;
    case NM_DEVICE_STATE_ACTIVATED:
        ret = "Activated";
        break;
    case NM_DEVICE_STATE_FAILED:
        ret = "FAILED";
        break;
    };
    return ret;
}

QString NMView::deviceTypeToString(int device)
{
    QString ret;
    switch(device) {
    case DEVICE_TYPE_UNKNOWN:
        ret = "Unknown";
        break;
    case DEVICE_TYPE_802_3_ETHERNET:
        ret = "Ethernet";
        break;
    case DEVICE_TYPE_802_11_WIRELESS:
        ret = "Wireless";
        break;
    case DEVICE_TYPE_GSM:
        ret = "GSM";
        break;
    case DEVICE_TYPE_CDMA:
        ret = "CDMA";
        break;
    };
    return ret;
}

QString NMView::securityCapabilitiesToString(int caps)
{
    int check = caps;
//    qWarning() << __FUNCTION__<< caps;
    QString ret;
    if( check == 0 )
        ret += "None.";
    if( (check & NM_802_11_AP_SEC_PAIR_WEP40))
        ret += " 40-bit WEP encryption. ";
    if( (check & NM_802_11_AP_SEC_PAIR_WEP104) )
        ret += " 104-bit WEP encryption. ";
    if( (check & NM_802_11_AP_SEC_PAIR_TKIP) )
        ret += " TKIP encryption. ";
    if( (check & NM_802_11_AP_SEC_PAIR_CCMP) )
        ret += " CCMP encryption. ";
    if( (check & NM_802_11_AP_SEC_GROUP_WEP40))
        ret += " 40-bit WEP cipher. ";
    if( (check & NM_802_11_AP_SEC_GROUP_WEP104))
        ret += " 104-bit WEP cipher. ";
    if( (check & NM_802_11_AP_SEC_GROUP_TKIP)  )
        ret += " TKIP cipher. ";
    if( (check & NM_802_11_AP_SEC_GROUP_CCMP)  )
        ret += " CCMP cipher. ";
    if( (check & NM_802_11_AP_SEC_KEY_MGMT_PSK))
        ret += " PSK key management. ";
    if( (check & NM_802_11_AP_SEC_KEY_MGMT_802_1X) )
        ret += " 802.1x key management. ";
    return ret;
}

QString NMView::deviceModeToString(int mode)
{
    QString ret;
    switch (mode) {
    case NM_802_11_MODE_UNKNOWN:
        ret = "Unknown";
        break;
    case NM_802_11_MODE_ADHOC:
        ret = " (Adhoc).";
        break;
    case NM_802_11_MODE_INFRA:
        ret = " (Infrastructure)";
    };
    return ret;
}

void NMView::netManagerState(quint32 state)
{
    qWarning() << __FUNCTION__ << state;// << old << reason;
    switch(state) {
    case NM_STATE_UNKNOWN:
        qWarning() << "The NetworkManager daemon is in an unknown state. ";
        break;
    case NM_STATE_ASLEEP:
        qWarning() << "The NetworkManager daemon is asleep and all interfaces managed by it are inactive. ";
        break;
    case NM_STATE_CONNECTING:
        qWarning() << "The NetworkManager daemon is connecting a device. FIXME: What does this mean when one device is active and another is connecting? ";
        break;
    case NM_STATE_CONNECTED:
        qWarning() <<"The NetworkManager daemon is connected. ";
        break;
    case NM_STATE_DISCONNECTED:
        qWarning() << "The NetworkManager daemon is disconnected.";
        break;

    };
}

void NMView::deviceStateChanged(quint32 state)
{
  qWarning() << __FUNCTION__ << state;
    switch(state) {
    case NM_DEVICE_STATE_UNKNOWN :
        qWarning() <<"The device is in an unknown state. ";
        break;
    case NM_DEVICE_STATE_UNMANAGED:
        qWarning() <<"The device is not managed by NetworkManager.";
        break;
    case NM_DEVICE_STATE_UNAVAILABLE:
        qWarning() <<"The device cannot be used (carrier off, rfkill, etc).";
        break;
    case NM_DEVICE_STATE_DISCONNECTED:
        qWarning() <<"The device is not connected.";
        break;
    case NM_DEVICE_STATE_PREPARE:
        qWarning() <<"The device is preparing to connect.";
        break;
    case NM_DEVICE_STATE_CONFIG:
        qWarning() <<"The device is being configured.";
        break;
    case NM_DEVICE_STATE_NEED_AUTH:
        qWarning() <<"The device is awaiting secrets necessary to continue connection.";
        break;
    case NM_DEVICE_STATE_IP_CONFIG:
        qWarning() <<"The IP settings of the device are being requested and configured.";
        break;
    case NM_DEVICE_STATE_ACTIVATED:
        qWarning() <<"The device is active.";
        break;
    case NM_DEVICE_STATE_FAILED:
        qWarning() <<"The device is in a failure state following an attempt to activate it.";
        break;
    };
}

void NMView::updateCompleted()
{
    qWarning() << __FUNCTION__;
}

void NMView::newConfigurationActivated()
{
    qWarning() << __FUNCTION__;
    getActiveConnections();
}

void NMView::stateChanged(QNetworkSession::State state)
{
  //  QString stateStringstr;
    switch (state) {
    case QNetworkSession::Invalid:
        stateString = "Invalid session";
        break;
    case QNetworkSession::NotAvailable:
        stateString = "Session is defined but not yet discovered";
        break;
    case QNetworkSession::Connecting:
        stateString = "Session is being established";
        break;
    case QNetworkSession::Connected:
        stateString = "Session is active and can be used for socket operations";
        cons->currentItem()->setText(1, "Active");
        break;
    case QNetworkSession::Closing:
        stateString = "Session is being shutdown";
        break;
    case QNetworkSession::Disconnected:
        qWarning() << __FUNCTION__;
        manager->updateConfigurations();
        stateString = "Session disconnected";
        cons->currentItem()->setText(1,"Discovered") ;
        break;
    case QNetworkSession::Roaming:
        stateString = "session is roaming from one AP to another";
        break;
    };
    qWarning() << __FUNCTION__ << state << stateString;
}

QString NMView::stateToString(int state)
{
    switch (state) {
    case QNetworkConfiguration::Undefined:
        return "Undefined";
        break;
    case QNetworkConfiguration::Defined:
        return "Defined";
        break;
    case QNetworkConfiguration::Discovered:
        return "Discovered";
        break;
    case QNetworkConfiguration::Active:
        return "Active";
        break;
    };
    return "";
}

void NMView::configurationAdded(const QNetworkConfiguration &config)
{
    qWarning() << __FUNCTION__<< config.name() << config.identifier();
}

QString NMView::typeToString(int type)
{
    switch(type) {
    case QNetworkConfiguration::InternetAccessPoint:
        return "Internet AccessPoint";
        break;
    case QNetworkConfiguration::ServiceNetwork:
        return "Service Network";
        break;
    };
    return QString();
}

void NMView::aPPropertiesChanged( QMap<QString,QVariant> map)
{
    //QMap<QString, QVariant>::const_iterator iterator = d->values.find(type + QLatin1String("Path"))
    QMapIterator<QString, QVariant> i(map);
    while (i.hasNext()) {
        i.next();
        if( i.key() == "State") {
            qWarning()  << __FUNCTION__ << i.key() << ": " << i.value().toUInt();
//            deviceStateChanged(i.value().toUInt());
        } else if( i.key() == "ActiveAccessPoint") {
            qWarning()  << __FUNCTION__ << i.key() << ": " << i.value().value<QDBusObjectPath>().path();
        } else if( i.key() == "Strength")
            qWarning()  << __FUNCTION__ << i.key() << ": " << i.value().toUInt();
        else
            qWarning()  << __FUNCTION__ << i.key() << ": " << i.value();
    }
}

void NMView::networkSessionError(QNetworkSession::SessionError error)
{
    QString errorStr;
    switch(error) {
    case QNetworkSession::RoamingError:
        errorStr = "Roaming error";
        break;
    case QNetworkSession::SessionAbortedError:
        errorStr = "Session aborted by user or system";
        break;
    default:
    case QNetworkSession::UnknownSessionError:
        errorStr = "Unidentified Error";
        break;
    };

    QMessageBox::warning(this, tr("NMView"), errorStr, QMessageBox::Ok);
}

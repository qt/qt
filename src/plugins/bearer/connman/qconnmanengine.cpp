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

#include "qconnmanengine.h"
#include "qconnmanservice_linux_p.h"
#include "../qnetworksession_impl.h"

#include <QtNetwork/private/qnetworkconfiguration_p.h>

#include <QtNetwork/qnetworksession.h>

#include <QtCore/qdebug.h>

#include <QtDBus/QtDBus>
#include <QtDBus/QDBusConnection>
//#include <QtDBus/QDBusError>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusReply>

#ifndef QT_NO_BEARERMANAGEMENT
#ifndef QT_NO_DBUS

QT_BEGIN_NAMESPACE

QConnmanEngine::QConnmanEngine(QObject *parent)
:   QBearerEngineImpl(parent),
    connmanManager(new QConnmanManagerInterface(this))
{
//    qWarning() << Q_FUNC_INFO;
}

QConnmanEngine::~QConnmanEngine()
{
}

bool QConnmanEngine::connmanAvailable() const
{
    QMutexLocker locker(&mutex);
    return connmanManager->isValid();
}

void QConnmanEngine::initialize()
{
    connect(connmanManager,SIGNAL(propertyChangedContext(QString,QString,QDBusVariant)),
            this,SLOT(propertyChangedContext(QString,QString,QDBusVariant)));

    foreach(const QString techPath, connmanManager->getTechnologies()) {
        QConnmanTechnologyInterface *tech;
        tech = new QConnmanTechnologyInterface(techPath, this);

        connect(tech,SIGNAL(propertyChangedContext(QString,QString,QDBusVariant)),
                this,SLOT(technologyPropertyChangedContext(QString,QString,QDBusVariant)));

        foreach(const QString devicePath,tech->getDevices()) {
            QConnmanDeviceInterface *dev;
            dev = new QConnmanDeviceInterface(devicePath);
            connect(dev,SIGNAL(propertyChangedContext(QString,QString,QDBusVariant)),
                    this,SLOT(devicePropertyChangedContext(QString,QString,QDBusVariant)));
            deviceMap.insert(techPath,QStringList() << devicePath);
        }
    }
    // Get current list of access points.
    getConfigurations();
}

QList<QNetworkConfigurationPrivate *> QConnmanEngine::getConfigurations()
{
    QMutexLocker locker(&mutex);
    foundConfigurations.clear();
    getNetworkListing();
    return foundConfigurations;
}

void QConnmanEngine::getNetworkListing()
{
    QMutexLocker locker(&mutex);

    QMapIterator<QString,QStringList> i(deviceMap);
    while(i.hasNext()) {
        i.next();
        QConnmanDeviceInterface dev(i.value().at(0));
        if(dev.isValid()) {
            foreach(const QString network,dev.getNetworks()) {
                addNetworkConfiguration(network);
            }
        }
    }
}

void QConnmanEngine::doRequestUpdate()
{
    getConfigurations();
    emit updateCompleted();
}

QString QConnmanEngine::getInterfaceFromId(const QString &id)
{
    QMutexLocker locker(&mutex);

    QString servicePath = serviceFromId(id);
    QString netPath = getNetworkForService(servicePath);

    QMapIterator<QString,QStringList> i(deviceMap);
    while(i.hasNext()) {
     i.next();
     if(i.value().count() > 0) {
         QConnmanDeviceInterface dev(i.value().at(0));
         foreach(const QString network, dev.getNetworks()) {
             if(network == netPath) {
                 return dev.getInterface();
             }
         }
     }
 }
    return QString();
}

bool QConnmanEngine::hasIdentifier(const QString &id)
{
    QMutexLocker locker(&mutex);
    return accessPointConfigurations.contains(id);
}

QString QConnmanEngine::bearerName(const QString &id)
{
    QMutexLocker locker(&mutex);
    QConnmanServiceInterface serv(serviceFromId(id));
    QString connectionType = serv.getType();

    if (connectionType == "ethernet")
        return QLatin1String("Ethernet");
    else if (connectionType == "wifi")
        return QLatin1String("WLAN");
    else if (connectionType == "cellular") {
        QString mode = serv.getMode();
        if(mode == "gprs" || mode == "edge") {
            return QLatin1String("2G");
        } else if(mode == "umts") {
            return QLatin1String("WCDMA");
        }
    }
    else if (connectionType == "wimax")
        return QLatin1String("WIMAX");

    return QString();
}

void QConnmanEngine::connectToId(const QString &id)
{
    QMutexLocker locker(&mutex);
    QConnmanServiceInterface serv(serviceFromId(id));
    if(!serv.isValid()) {
        emit connectionError(id, InterfaceLookupError);
    } else {
        serv.connect();
    }
}

void QConnmanEngine::disconnectFromId(const QString &id)
{
    QMutexLocker locker(&mutex);
    QConnmanServiceInterface serv(serviceFromId(id));
    if(!serv.isValid()) {
        emit connectionError(id, DisconnectionError);
    } else {
        serv.disconnect();
    }
}

void QConnmanEngine::requestUpdate()
{
    QMutexLocker locker(&mutex);
    QTimer::singleShot(0, this, SLOT(doRequestUpdate()));
}

QString QConnmanEngine::serviceFromId(const QString &id)
{
    QMutexLocker locker(&mutex);
    foreach(QString service, connmanManager->getServices()) {
        if (id == QString::number(qHash(service)))
            return service;
    }

    return QString();
}

QNetworkSession::State QConnmanEngine::sessionStateForId(const QString &id)
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

quint64 QConnmanEngine::bytesWritten(const QString &id)
{//TODO use connman counter API
    QMutexLocker locker(&mutex);
    quint64 result = 0;
    QString devFile = getInterfaceFromId(id);
    QFile tx("/sys/class/net/"+devFile+"/statistics/tx_bytes");
    if(tx.exists() && tx.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&tx);
        in >> result;
        tx.close();
    }
    return result;
}

quint64 QConnmanEngine::bytesReceived(const QString &id)
{//TODO use connman counter API
    QMutexLocker locker(&mutex);
    quint64 result = 0;
    QString devFile = getInterfaceFromId(id);
    QFile rx("/sys/class/net/"+devFile+"/statistics/rx_bytes");
    if(rx.exists() && rx.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&rx);
        in >> result;
        rx.close();
    }
    return result;
}

quint64 QConnmanEngine::startTime(const QString &/*id*/)
{
    // TODO
    QMutexLocker locker(&mutex);
    if (activeTime.isNull()) {
        return 0;
    }
    return activeTime.secsTo(QDateTime::currentDateTime());
}

QNetworkConfigurationManager::Capabilities QConnmanEngine::capabilities() const
{
    return QNetworkConfigurationManager::ForcedRoaming |
            QNetworkConfigurationManager::DataStatistics |
           QNetworkConfigurationManager::CanStartAndStopInterfaces;
}

QNetworkSessionPrivate *QConnmanEngine::createSessionBackend()
{
     return new QNetworkSessionPrivateImpl;
}

QNetworkConfigurationPrivatePointer QConnmanEngine::defaultConfiguration()
{
    return QNetworkConfigurationPrivatePointer();
}


QString QConnmanEngine::getServiceForNetwork(const QString &netPath)
{
    QMutexLocker locker(&mutex);
    QConnmanNetworkInterface network(netPath, this);
    foreach(QString service,connmanManager->getServices()) {
        QConnmanServiceInterface serv(service,this);
        if(serv.getName() == network.getName()
            && network.getSignalStrength() == serv.getSignalStrength()) {
            return service;
        }
    }
    return QString();
}

QString QConnmanEngine::getNetworkForService(const QString &servPath)
{
    QMutexLocker locker(&mutex);
    QMap<QString,QString> map;

    QMapIterator<QString,QStringList> i(deviceMap);
    while(i.hasNext()) {
        i.next();
        if(i.value().count() > 0) {
            QConnmanDeviceInterface device(i.value().at(0));
            QMap<QString,int> netMapStrength;

            foreach(const QString netPath, knownNetworks[device.getType()]) {
                QConnmanNetworkInterface network1(netPath, this);
                QString netname = network1.getName();
                qint32 sigStrength = network1.getSignalStrength();

                if(netMapStrength.contains(netname)
                    && netMapStrength.value(netname) < sigStrength) {
                    netMapStrength.remove(netname);
                    map.remove(netname);
                }
                netMapStrength.insert(netname, sigStrength);
                map.insert(netname,netPath);
            }
        }
    }

    QConnmanServiceInterface *serv;
    serv = new QConnmanServiceInterface(servPath);
    if(map.contains(serv->getName())) {
        return map.value(serv->getName());
    }
    return QString();
}

void QConnmanEngine::propertyChangedContext(const QString &path,const QString &item, const QDBusVariant &value)
{
//    qDebug() << __FUNCTION__ << path << item;
    QMutexLocker locker(&mutex);

    if(item == "Technologies") {
        QDBusArgument arg = qvariant_cast<QDBusArgument>(value.variant());
        QStringList newlist = qdbus_cast<QStringList>(arg);
        if(newlist.count() > 0) {
            QMap<QString,QConnmanTechnologyInterface *> oldtech = technologies;

            foreach(const QString listPath, newlist) {
                if(!oldtech.contains(listPath)) {
                    QConnmanTechnologyInterface *tech;
                    tech = new QConnmanTechnologyInterface(listPath,this);
                    connect(tech,SIGNAL(propertyChangedContext(QString,QString,QDBusVariant)),
                            this,SLOT(technologyPropertyChangedContext(QString,QString,QDBusVariant)));
                    technologies.insert(listPath, tech);
                }
            }

            foreach(const QString old, oldtech.keys()) {
                if(!newlist.contains(old)) {
                    QConnmanTechnologyInterface *tech = oldtech.value(old);
                    disconnect(tech,SIGNAL(propertyChangedContext(QString,QString,QDBusVariant)),
                               this,SLOT(technologyPropertyChangedContext(QString,QString,QDBusVariant)));

                    technologies.remove(old);
                    getNetworkListing();
                }
            }
        }
    }
}

void QConnmanEngine::servicePropertyChangedContext(const QString &path,const QString &item, const QDBusVariant &value)
{
//    qDebug() << __FUNCTION__ << path << item;
    QMutexLocker locker(&mutex);
     if(item == "State") {
        configurationChange(QString::number(qHash(path)));
        if(value.variant().toString() == "failure") {
            QConnmanServiceInterface serv(path);
            qDebug() <<__FUNCTION__ <<"Error" << serv.getError();
              emit connectionError(QString::number(qHash(path)), ConnectError);
        }
    }
}

void QConnmanEngine::networkPropertyChangedContext(const QString &path,const QString &item, const QDBusVariant &/*value*/)
{
//    qDebug() << __FUNCTION__ << path << item;
    QMutexLocker locker(&mutex);
}

void QConnmanEngine::devicePropertyChangedContext(const QString &path,const QString &item,const QDBusVariant &value)
{
//    qDebug() << __FUNCTION__ << path << item << value.variant();
    QMutexLocker locker(&mutex);
    if(item == "Networks") {
        QDBusArgument arg = qvariant_cast<QDBusArgument>(value.variant());
        QStringList remainingNetworks = qdbus_cast<QStringList>(arg);

        QConnmanDeviceInterface dev(path);
        QStringList oldnetworks = knownNetworks[dev.getType()];
        if(remainingNetworks.count() != oldnetworks.count()) {

            foreach(const QString netPath, remainingNetworks) {
                if(!oldnetworks.contains(netPath)) {
                    addNetworkConfiguration(netPath);
                }
            }

            foreach(const QString netPath, oldnetworks) {
                QString servicePath = getServiceForNetwork(netPath);
                if(!remainingNetworks.contains(netPath)) {
                    if(servicePath.isEmpty()) {
                        removeConfiguration(netPath);
                    }  else {
                        if(!remainingNetworks.contains(servicePath)) {
                            removeConfiguration(QString::number(qHash(servicePath)));
                        }
                    }
                }
            }
        }
    }
}

void QConnmanEngine::technologyPropertyChangedContext(const QString & path, const QString &item, const QDBusVariant &value)
{
//  qWarning() << __FUNCTION__ << path << item << value.variant();
//  if(item == "Devices") {
//      QDBusArgument arg = qvariant_cast<QDBusArgument>(value.variant());
//      QStringList list = qdbus_cast<QStringList>(arg);
//  }
  if(item == "State") {
      if(value.variant().toString() == "enabled") {
      }
      if(value.variant().toString() == "offline") {
          deviceMap.remove(path);
      }
      if(value.variant().toString() == "available") {
          QConnmanTechnologyInterface tech(connmanManager->getPathForTechnology(path));
          foreach(const QString devPath, tech.getDevices()) {
              QConnmanDeviceInterface *dev;
              dev = new QConnmanDeviceInterface(devPath,this);
              connect(dev,SIGNAL(propertyChangedContext(QString,QString,QDBusVariant)),
                      this,SLOT(devicePropertyChangedContext(QString,QString,QDBusVariant)));

              deviceMap.insert(path,QStringList() << devPath);
          }
      }
  }
}

void QConnmanEngine::configurationChange(const QString &id)
{
    QMutexLocker locker(&mutex);
    bool changed = false;

    if (accessPointConfigurations.contains(id)) {
        QNetworkConfigurationPrivatePointer ptr = accessPointConfigurations.value(id);

        QString servicePath = serviceFromId(id);
        QConnmanServiceInterface *serv;
        serv = new QConnmanServiceInterface(servicePath);
        QString networkName = serv->getName();
        QNetworkConfiguration::StateFlags curState = getStateForService(servicePath);

        ptr->mutex.lock();

        if (!ptr->isValid) {
            ptr->isValid = true;
            changed = true;
        }

        if (ptr->name != networkName) {
            ptr->name = networkName;
            changed = true;
        }

        if (ptr->state != curState) {
            ptr->state = curState;
            changed = true;
        }

        ptr->mutex.unlock();

        if (changed) {
            locker.unlock();
            emit configurationChanged(ptr);
            locker.relock();
        }

    }
     locker.unlock();
     emit updateCompleted();
}

QNetworkConfiguration::StateFlags QConnmanEngine::getStateForService(const QString &service)
{
    qWarning() << __FUNCTION__;
    QMutexLocker locker(&mutex);
    QConnmanServiceInterface serv(service);
    QNetworkConfiguration::StateFlags flag = QNetworkConfiguration::Defined;
    if(serv.isFavorite()) {
        flag = ( flag | QNetworkConfiguration::Discovered);
    } else {
        flag = QNetworkConfiguration::Undefined;
    }

    if(serv.getState() == "ready" || serv.getState() == "online") {
        flag = ( flag | QNetworkConfiguration::Active);
    }

    return flag;
}

QString QConnmanEngine::typeToBearer(const QString &type)
{
    QMutexLocker locker(&mutex);
    if(type == "wifi")
        return "WLAN";
    if(type == "ethernet")
        return "Ethernet";
    if(type == "bluetooth")
        return "Bluetooth";
    if(type == "cellular") {
        return "Cellular";
        // not handled: CDMA2000 HSPA
    }
    if(type == "wimax")
        return "WiMax";
//    if(type == "gps")
//    if(type == "vpn")

    return "Unknown";
}

void QConnmanEngine::removeConfiguration(const QString &netpath)
{
    QMutexLocker locker(&mutex);
    const QString id = QString::number(qHash(netpath));
    if (accessPointConfigurations.contains(id)) {
        QNetworkConfigurationPrivatePointer ptr = accessPointConfigurations.take(id);
        QConnmanDeviceInterface device(netpath.section("/",0,5),this);
        locker.unlock();
        knownNetworks[device.getType()].removeAll(netpath);
        emit configurationRemoved(ptr);
        locker.relock();
    }
}

void QConnmanEngine::addNetworkConfiguration(const QString &networkPath)
{
//    qWarning() << __FUNCTION__ << networkPath;
    QMutexLocker locker(&mutex);

    QConnmanNetworkInterface *network;
    network = new QConnmanNetworkInterface(networkPath, this);
    QString servicePath = getServiceForNetwork(networkPath);
    QConnmanServiceInterface *serv;

    QString id;
    if(servicePath.isEmpty()) {
        id = QString::number(qHash(networkPath));
    } else {
        id = QString::number(qHash(servicePath));
    }
    if (!accessPointConfigurations.contains(id)) {
        connect(network,SIGNAL(propertyChangedContext(QString,QString,QDBusVariant)),
                this,SLOT(networkPropertyChangedContext(QString,QString, QDBusVariant)));

        QNetworkConfigurationPrivate* cpPriv = new QNetworkConfigurationPrivate();

        QString networkName = network->getName();

        if(networkName.isEmpty())
            networkName = "Hidden Network";

        QString bearerName;

        QConnmanDeviceInterface device(networkPath.section("/",0,5),this);
        if(servicePath.isEmpty()) {
            bearerName = typeToBearer(device.getType());
        } else {
            serv = new QConnmanServiceInterface(servicePath,this);
            bearerName = typeToBearer(serv->getType());
            connect(serv,SIGNAL(propertyChangedContext(QString,QString,QDBusVariant)),
                    this,SLOT(servicePropertyChangedContext(QString,QString, QDBusVariant)));
        }
        knownNetworks[device.getType()]<< networkPath;

        if(bearerName == "Cellular") {
            QString mode = serv->getMode();
            if(mode == "gprs" || mode == "edge") {
                bearerName = "2G";
            } else if(mode == "umts") {
                bearerName = "WCDMA";
            }
            networkName = serv->getAPN();
        }

        cpPriv->name = networkName;
        cpPriv->isValid = true;
        cpPriv->id = id;
        cpPriv->type = QNetworkConfiguration::InternetAccessPoint;
        cpPriv->bearer = bearerName;

        if(network->getWifiSecurity() == "none") {
            cpPriv->purpose = QNetworkConfiguration::PublicPurpose;
        } else {
            cpPriv->purpose = QNetworkConfiguration::PrivatePurpose;
        }

        if(servicePath.isEmpty())
            cpPriv->state = QNetworkConfiguration::Undefined;
        else
            cpPriv->state = getStateForService(servicePath);

        QNetworkConfigurationPrivatePointer ptr(cpPriv);
        accessPointConfigurations.insert(ptr->id, ptr);
        foundConfigurations.append(cpPriv);
        locker.unlock();
        emit configurationAdded(ptr);
        locker.relock();
    }
}

QT_END_NAMESPACE

#endif // QT_NO_DBUS
#endif // QT_NO_BEARERMANAGEMENT

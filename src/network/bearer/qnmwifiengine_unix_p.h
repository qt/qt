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

#ifndef QNMWIFIENGINE_P_H
#define QNMWIFIENGINE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

//#include <QtCore/qtimer.h>

#include <QHash>
#include <QDBusPendingCallWatcher>
#include <QExplicitlySharedDataPointer>
#include <qnetworkmanagerservice_p.h>

#include "qnetworksessionengine_p.h"
#include <qnetworksession.h>
#include <qnetworkconfiguration.h>
#include <qnetworkconfigmanager_p.h>



QTM_BEGIN_NAMESPACE

class QNetworkConfigurationPrivate;

class QNmWifiEngine : public QNetworkSessionEngine
{
    Q_OBJECT

public:
    QNmWifiEngine(QObject *parent = 0);
    ~QNmWifiEngine();

    QList<QNetworkConfigurationPrivate *> getConfigurations(bool *ok = 0);
    QString getInterfaceFromId(const QString &id);
    bool hasIdentifier(const QString &id);

    QString bearerName(const QString &id);

    void connectToId(const QString &id);
    void disconnectFromId(const QString &id);

    void requestUpdate();

    static QNmWifiEngine *instance();

    QStringList knownSsids;
    inline void emitConfigurationsChanged() { emit configurationsChanged(); }
    QNetworkConfigurationPrivate * addAccessPoint(const QString &, QDBusObjectPath );

    QStringList getConnectionPathForId(const QString &uuid);
    //QString getConnectionPathForId(const QString &name = QString());
    quint64 sentDataForId(const QString &id) const;
    quint64 receivedDataForId(const QString &id) const;

private:
    bool updated;
    QString activatingConnectionPath;
    QStringList activeConnectionPaths;


    QMap<QString,QDBusObjectPath> availableAccessPoints;
    void scanForAccessPoints();

    QStringList devicePaths;

    void updateActiveConnectionsPaths();
    void updateKnownSsids();
    void accessPointConnections();
    void knownConnections();
    QString deviceConnectionPath(const QString &mac);

    QList<QNetworkConfigurationPrivate *> foundConfigurations;
	//    QHash<QString, QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> > allConfigurations;

    QNetworkManagerInterface *iface;

    QNetworkConfiguration::StateFlags getAPState(qint32 vState, bool isKnown);
    QNetworkConfiguration::StateFlags getStateFlag(quint32 nmstate);

    QString getActiveConnectionPath(const QString &identifier);
    QString getNameForConfiguration(QNetworkManagerInterfaceDevice *devIface);

    QNetworkConfiguration::StateFlags getStateForId(const QString &id);

    QNetworkInterface getBestInterface(quint32 type, const QString &conPath);

    QMap<QString, QString> configurationInterface;

    bool isAddressOfConnection(const QString &conPath, quint32 ipaddress);

private slots:
    void updateDeviceInterfaceState(const QString &, quint32);
    void addDevice(QDBusObjectPath path);
    void removeDevice(QDBusObjectPath path);

Q_SIGNALS:
    void configurationChanged(const QNetworkConfiguration& config);

private slots:
    void accessPointAdded( const QString &aPath, QDBusObjectPath oPath);
    void accessPointRemoved( const QString &aPath, QDBusObjectPath oPath);
    void cmpPropertiesChanged(const QString &, QMap<QString,QVariant> map);
    void newConnection(QDBusObjectPath);
    void settingsConnectionRemoved(const QString &);
    void slotActivationFinished(QDBusPendingCallWatcher*);
};

QTM_END_NAMESPACE

#endif



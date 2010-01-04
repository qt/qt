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

#ifndef QNETWORKSESSIONPRIVATE_H
#define QNETWORKSESSIONPRIVATE_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of the QLibrary class.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//

#include "qnetworkconfigmanager_maemo_p.h"
#include "qnetworksession.h"

#include <qnetworksession.h>
#include <QNetworkInterface>
#include <QDateTime>

#include <icd/dbus_api.h>

QTM_BEGIN_NAMESPACE

class QNetworkSessionPrivate : public QObject
{
    Q_OBJECT
public:
    QNetworkSessionPrivate() : 
	    tx_data(0), rx_data(0), m_activeTime(0), isOpen(false),
	    connectFlags(ICD_CONNECTION_FLAG_USER_EVENT)
    {
    }

    ~QNetworkSessionPrivate()
    {
	cleanupSession();
    }

    //called by QNetworkSession constructor and ensures
    //that the state is immediately updated (w/o actually opening
    //a session). Also this function should take care of 
    //notification hooks to discover future state changes.
    void syncStateWithInterface();

    QNetworkInterface currentInterface() const;
    QVariant sessionProperty(const QString& key) const;
    void setSessionProperty(const QString& key, const QVariant& value);

    void open();
    void close();
    void stop();
    void migrate();
    void accept();
    void ignore();
    void reject();

    QString errorString() const; //must return translated string
    QNetworkSession::SessionError error() const;

    quint64 bytesWritten() const;
    quint64 bytesReceived() const;
    quint64 activeTime() const;

private:
    void updateStateFromServiceNetwork();
    void updateStateFromActiveConfig();

Q_SIGNALS:
    //releases any pending waitForOpened() calls
    void quitPendingWaitsForOpened();

private Q_SLOTS:
    void do_open();
    void networkConfigurationsChanged();
    void configurationChanged(const QNetworkConfiguration &config);

private:
    QNetworkConfigurationManager manager;

    quint64 tx_data;
    quint64 rx_data;
    quint64 m_activeTime;

    // The config set on QNetworkSession.
    QNetworkConfiguration publicConfig;

    // If publicConfig is a ServiceNetwork this is a copy of publicConfig.
    // If publicConfig is an UserChoice that is resolved to a ServiceNetwork this is the actual
    // ServiceNetwork configuration.
    QNetworkConfiguration serviceConfig;

    // This is the actual active configuration currently in use by the session.
    // Either a copy of publicConfig or one of serviceConfig.children().
    QNetworkConfiguration activeConfig;

    QNetworkConfiguration& copyConfig(QNetworkConfiguration &fromConfig, QNetworkConfiguration &toConfig, bool deepCopy = true);
    void clearConfiguration(QNetworkConfiguration &config);
    void cleanupAnyConfiguration();

    QNetworkSession::State state;
    bool isOpen;
    bool opened;
    icd_connection_flags connectFlags;

    QNetworkSession::SessionError lastError;

    QNetworkSession* q;
    friend class QNetworkSession;

    QDateTime startTime;
    QString currentNetworkInterface;
    friend class IcdListener;
    void updateState(QNetworkSession::State);
    void updateIdentifier(QString &newId);
    quint64 getStatistics(bool sent) const;
    void cleanupSession(void);
};

QTM_END_NAMESPACE

#endif //QNETWORKSESSIONPRIVATE_H


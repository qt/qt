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

#include "qnetworksession_impl.h"
#include "qnetworksessionengine_impl.h"

#include <QtNetwork/qnetworksession.h>
#include <QtNetwork/private/qnetworkconfigmanager_p.h>

#include <QtCore/qstringlist.h>
#include <QtCore/qdebug.h>
#include <QtCore/qmutex.h>

#include <QtNetwork/qnetworkinterface.h>

QT_BEGIN_NAMESPACE

static QNetworkSessionEngineImpl *getEngineFromId(const QString &id)
{
    QNetworkConfigurationManagerPrivate *priv = qNetworkConfigurationManagerPrivate();

    foreach (QNetworkSessionEngine *engine, priv->sessionEngines) {
        QNetworkSessionEngineImpl *engineImpl = qobject_cast<QNetworkSessionEngineImpl *>(engine);
        if (engineImpl && engineImpl->hasIdentifier(id))
            return engineImpl;
    }

    return 0;
}

class QNetworkSessionManagerPrivate : public QObject
{
    Q_OBJECT

public:
    QNetworkSessionManagerPrivate(QObject *parent = 0);
    ~QNetworkSessionManagerPrivate();

    void forceSessionClose(const QNetworkConfiguration &config);

Q_SIGNALS:
    void forcedSessionClose(const QNetworkConfiguration &config);
};

#include "qnetworksession_impl.moc"

Q_GLOBAL_STATIC(QNetworkSessionManagerPrivate, sessionManager);

QNetworkSessionManagerPrivate::QNetworkSessionManagerPrivate(QObject *parent)
:   QObject(parent)
{
}

QNetworkSessionManagerPrivate::~QNetworkSessionManagerPrivate()
{
}

void QNetworkSessionManagerPrivate::forceSessionClose(const QNetworkConfiguration &config)
{
    emit forcedSessionClose(config);
}

void QNetworkSessionPrivateImpl::syncStateWithInterface()
{
    connect(&manager, SIGNAL(updateCompleted()), this, SLOT(networkConfigurationsChanged()));
    connect(&manager, SIGNAL(configurationChanged(QNetworkConfiguration)),
            this, SLOT(configurationChanged(QNetworkConfiguration)));
    connect(sessionManager(), SIGNAL(forcedSessionClose(QNetworkConfiguration)),
            this, SLOT(forcedSessionClose(QNetworkConfiguration)));

    opened = false;
    isOpen = false;
    state = QNetworkSession::Invalid;
    lastError = QNetworkSession::UnknownSessionError;

    qRegisterMetaType<QNetworkSessionEngineImpl::ConnectionError>
        ("QNetworkSessionEngineImpl::ConnectionError");

    switch (publicConfig.type()) {
    case QNetworkConfiguration::InternetAccessPoint:
        activeConfig = publicConfig;
        engine = getEngineFromId(activeConfig.identifier());
        if (engine) {
            connect(engine, SIGNAL(connectionError(QString,QNetworkSessionEngine::ConnectionError)),
                    this, SLOT(connectionError(QString,QNetworkSessionEngine::ConnectionError)),
                    Qt::QueuedConnection);
        }
        break;
    case QNetworkConfiguration::ServiceNetwork:
        serviceConfig = publicConfig;
        // Defer setting engine and signals until open().
        // fall through
    case QNetworkConfiguration::UserChoice:
        // Defer setting serviceConfig and activeConfig until open().
        // fall through
    default:
        engine = 0;
    }

    networkConfigurationsChanged();
}

void QNetworkSessionPrivateImpl::open()
{
    if (serviceConfig.isValid()) {
        lastError = QNetworkSession::OperationNotSupportedError;
        emit QNetworkSessionPrivate::error(lastError);
    } else if (!isOpen) {
        if ((activeConfig.state() & QNetworkConfiguration::Discovered) !=
            QNetworkConfiguration::Discovered) {
            lastError =QNetworkSession::InvalidConfigurationError;
            emit QNetworkSessionPrivate::error(lastError);
            return;
        }
        opened = true;

        if ((activeConfig.state() & QNetworkConfiguration::Active) != QNetworkConfiguration::Active &&
            (activeConfig.state() & QNetworkConfiguration::Discovered) == QNetworkConfiguration::Discovered) {
            state = QNetworkSession::Connecting;
            emit stateChanged(state);

            engine->connectToId(activeConfig.identifier());
        }

        isOpen = (activeConfig.state() & QNetworkConfiguration::Active) == QNetworkConfiguration::Active;
        if (isOpen)
            emit quitPendingWaitsForOpened();
    }
}

void QNetworkSessionPrivateImpl::close()
{
    if (serviceConfig.isValid()) {
        lastError = QNetworkSession::OperationNotSupportedError;
        emit QNetworkSessionPrivate::error(lastError);
    } else if (isOpen) {
        opened = false;
        isOpen = false;
        emit closed();
    }
}

void QNetworkSessionPrivateImpl::stop()
{
    if (serviceConfig.isValid()) {
        lastError = QNetworkSession::OperationNotSupportedError;
        emit QNetworkSessionPrivate::error(lastError);
    } else {
        if ((activeConfig.state() & QNetworkConfiguration::Active) == QNetworkConfiguration::Active) {
            state = QNetworkSession::Closing;
            emit stateChanged(state);

            engine->disconnectFromId(activeConfig.identifier());

            sessionManager()->forceSessionClose(activeConfig);
        }

        opened = false;
        isOpen = false;
        emit closed();
    }
}

void QNetworkSessionPrivateImpl::migrate()
{
    qWarning("This platform does not support roaming (%s).", __FUNCTION__);
}

void QNetworkSessionPrivateImpl::accept()
{
    qWarning("This platform does not support roaming (%s).", __FUNCTION__);
}

void QNetworkSessionPrivateImpl::ignore()
{
    qWarning("This platform does not support roaming (%s).", __FUNCTION__);
}

void QNetworkSessionPrivateImpl::reject()
{
    qWarning("This platform does not support roaming (%s).", __FUNCTION__);
}

QNetworkInterface QNetworkSessionPrivateImpl::currentInterface() const
{
    if (!publicConfig.isValid() || !engine || state != QNetworkSession::Connected)
        return QNetworkInterface();

    QString interface = engine->getInterfaceFromId(activeConfig.identifier());

    if (interface.isEmpty())
        return QNetworkInterface();
    return QNetworkInterface::interfaceFromName(interface);
}

QVariant QNetworkSessionPrivateImpl::sessionProperty(const QString& /*key*/) const
{
    return QVariant();
}

void QNetworkSessionPrivateImpl::setSessionProperty(const QString& /*key*/, const QVariant& /*value*/)
{
}

QString QNetworkSessionPrivateImpl::errorString() const
{
    switch (lastError) {
    case QNetworkSession::UnknownSessionError:
        return tr("Unknown session error.");
    case QNetworkSession::SessionAbortedError:
        return tr("The session was aborted by the user or system.");
    case QNetworkSession::OperationNotSupportedError:
        return tr("The requested operation is not supported by the system.");
    case QNetworkSession::InvalidConfigurationError:
        return tr("The specified configuration cannot be used.");
    case QNetworkSession::RoamingError:
        return tr("Roaming was aborted or is not possible.");

    }

    return QString();
}

QNetworkSession::SessionError QNetworkSessionPrivateImpl::error() const
{
    return lastError;
}

quint64 QNetworkSessionPrivateImpl::bytesWritten() const
{
#if defined(BACKEND_NM) && 0
    if( NetworkManagerAvailable() && state == QNetworkSession::Connected ) {
        if (publicConfig.type() == QNetworkConfiguration::ServiceNetwork) {
            foreach (const QNetworkConfiguration &config, publicConfig.children()) {
                if ((config.state() & QNetworkConfiguration::Active) == QNetworkConfiguration::Active) {
                    return static_cast<QNmWifiEngine*>(getEngineFromId(config.d->id))->sentDataForId(config.d->id);
                }
            }
        } else {
            return static_cast<QNmWifiEngine*>(getEngineFromId(activeConfig.d->id))->sentDataForId(activeConfig.d->id);
        }
    }
#endif
    return tx_data;
}

quint64 QNetworkSessionPrivateImpl::bytesReceived() const
{
#if defined(BACKEND_NM) && 0
    if( NetworkManagerAvailable() && state == QNetworkSession::Connected ) {
        if (publicConfig.type() == QNetworkConfiguration::ServiceNetwork) {
            foreach (const QNetworkConfiguration &config, publicConfig.children()) {
                if ((config.state() & QNetworkConfiguration::Active) == QNetworkConfiguration::Active) {
                    return static_cast<QNmWifiEngine*>(getEngineFromId(activeConfig.d->id))->receivedDataForId(config.d->id);
                }
            }
        } else {
            return static_cast<QNmWifiEngine*>(getEngineFromId(activeConfig.d->id))->receivedDataForId(activeConfig.d->id);
        }
    }
#endif
    return rx_data;
}

quint64 QNetworkSessionPrivateImpl::activeTime() const
{
#if defined(BACKEND_NM)
    if (startTime.isNull()) {
        return 0;
    }
    if(state == QNetworkSession::Connected )
        return startTime.secsTo(QDateTime::currentDateTime());
#endif
    return m_activeTime;
}

void QNetworkSessionPrivateImpl::updateStateFromServiceNetwork()
{
    QNetworkSession::State oldState = state;

    foreach (const QNetworkConfiguration &config, serviceConfig.children()) {
        if ((config.state() & QNetworkConfiguration::Active) != QNetworkConfiguration::Active)
            continue;

        if (activeConfig != config) {
            if (engine) {
                disconnect(engine, SIGNAL(connectionError(QString,QNetworkSessionEngine::ConnectionError)),
                           this, SLOT(connectionError(QString,QNetworkSessionEngine::ConnectionError)));
            }

            activeConfig = config;
            engine = getEngineFromId(activeConfig.identifier());
            if (engine) {
                connect(engine, SIGNAL(connectionError(QString,QNetworkSessionEngine::ConnectionError)),
                        this, SLOT(connectionError(QString,QNetworkSessionEngine::ConnectionError)),
                        Qt::QueuedConnection);
            }
            emit newConfigurationActivated();
        }

        state = QNetworkSession::Connected;
        qDebug() << oldState << "->" << state;
        if (state != oldState)
            emit stateChanged(state);

        return;
    }

    if (serviceConfig.children().isEmpty())
        state = QNetworkSession::NotAvailable;
    else
        state = QNetworkSession::Disconnected;

    qDebug() << oldState << "->" << state;
    if (state != oldState)
        emit stateChanged(state);
}

void QNetworkSessionPrivateImpl::updateStateFromActiveConfig()
{
    if (!engine)
        return;

    QNetworkSession::State oldState = state;

    state = engine->sessionStateForId(activeConfig.identifier());

    bool oldActive = isOpen;
    isOpen = (state == QNetworkSession::Connected) ? opened : false;

    if (!oldActive && isOpen)
        emit quitPendingWaitsForOpened();
    if (oldActive && !isOpen)
        emit closed();

    if (oldState != state)
        emit stateChanged(state);
}

void QNetworkSessionPrivateImpl::networkConfigurationsChanged()
{
    if (serviceConfig.isValid())
        updateStateFromServiceNetwork();
    else
        updateStateFromActiveConfig();
#if defined(BACKEND_NM) && 0
        setActiveTimeStamp();
#endif
}

void QNetworkSessionPrivateImpl::configurationChanged(const QNetworkConfiguration &config)
{
    if (serviceConfig.isValid() && (config == serviceConfig || config == activeConfig))
        updateStateFromServiceNetwork();
    else if (config == activeConfig)
        updateStateFromActiveConfig();
}

void QNetworkSessionPrivateImpl::forcedSessionClose(const QNetworkConfiguration &config)
{
    if (activeConfig == config) {
        opened = false;
        isOpen = false;

        emit closed();

        lastError = QNetworkSession::SessionAbortedError;
        emit QNetworkSessionPrivate::error(lastError);
    }
}

void QNetworkSessionPrivateImpl::connectionError(const QString &id,
                                                 QNetworkSessionEngineImpl::ConnectionError error)
{
    if (activeConfig.identifier() == id) {
        networkConfigurationsChanged();
        switch (error) {
        case QNetworkSessionEngineImpl::OperationNotSupported:
            lastError = QNetworkSession::OperationNotSupportedError;
            opened = false;
            break;
        case QNetworkSessionEngineImpl::InterfaceLookupError:
        case QNetworkSessionEngineImpl::ConnectError:
        case QNetworkSessionEngineImpl::DisconnectionError:
        default:
            lastError = QNetworkSession::UnknownSessionError;
        }

        emit quitPendingWaitsForOpened();
        emit QNetworkSessionPrivate::error(lastError);
    }
}

#if defined(BACKEND_NM) && 0
void QNetworkSessionPrivateImpl::setActiveTimeStamp()
{
    if(NetworkManagerAvailable()) {
        startTime = QDateTime();
        return;
    }
    QString connectionIdent = q->configuration().identifier();
    QString interface = currentInterface().hardwareAddress().toLower();
    QString devicePath = "/org/freedesktop/Hal/devices/net_" + interface.replace(":","_");

    QString path;
    QString serviceName;
    QNetworkManagerInterface * ifaceD;
    ifaceD = new QNetworkManagerInterface();

    QList<QDBusObjectPath> connections = ifaceD->activeConnections();
    foreach(QDBusObjectPath conpath, connections) {
        QNetworkManagerConnectionActive *conDetails;
        conDetails = new QNetworkManagerConnectionActive(conpath.path());
        QDBusObjectPath connection = conDetails->connection();
        serviceName = conDetails->serviceName();
        QList<QDBusObjectPath> so = conDetails->devices();
        foreach(QDBusObjectPath device, so) {

            if(device.path() == devicePath) {
                path = connection.path();
            }
            break;
        }
    }
if(serviceName.isEmpty())
    return;
    QNetworkManagerSettings *settingsiface;
    settingsiface = new QNetworkManagerSettings(serviceName);
    QList<QDBusObjectPath> list = settingsiface->listConnections();
    foreach(QDBusObjectPath path, list) {
        QNetworkManagerSettingsConnection *sysIface;
        sysIface = new QNetworkManagerSettingsConnection(serviceName, path.path());
        startTime = QDateTime::fromTime_t(sysIface->getTimestamp());
        //                    isOpen = (publicConfig.state() & QNetworkConfiguration::Active) == QNetworkConfiguration::Active;
    }
    if(startTime.isNull())
        startTime = QDateTime::currentDateTime();
}
#endif

QT_END_NAMESPACE

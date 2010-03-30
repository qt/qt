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
#include "qicdengine.h"

#include <QHash>

#include <dbus/dbus.h>
#include <dbus/dbus-glib-lowlevel.h>

#include <maemo_icd.h>
#include <iapconf.h>
#include <proxyconf.h>

#include <sys/types.h>
#include <ifaddrs.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

QT_BEGIN_NAMESPACE

static QHash<QString, QVariant> properties;

static QString get_network_interface();
static DBusConnection *dbus_connection;
static DBusHandlerResult signal_handler(DBusConnection *connection,
					DBusMessage *message,
					void *user_data);

#define ICD_DBUS_MATCH		"type='signal'," \
				"interface='" ICD_DBUS_INTERFACE "'," \
				"path='" ICD_DBUS_PATH "'"


static inline DBusConnection *get_dbus_conn(DBusError *error)
{
    DBusConnection *conn = dbus_bus_get(DBUS_BUS_SYSTEM, error);
#ifdef BEARER_MANAGEMENT_DEBUG
    qDebug() << "Listening to bus" << dbus_bus_get_unique_name(conn);
#endif

    return conn;
}


/* Helper class that monitors the Icd status messages and
 * can change the IAP status accordingly. This is a singleton.
 */
class IcdListener : public QObject
{
    Q_OBJECT

public:
    IcdListener() : first_call(true) { }
    friend DBusHandlerResult signal_handler(DBusConnection *connection,
					    DBusMessage *message,
					    void *user_data);
    void setup(QNetworkSessionPrivateImpl *d);
    void cleanup();
    void cleanupSession(QNetworkSessionPrivateImpl *ptr);

    enum IapConnectionStatus {
	/* The IAP was connected */
	CONNECTED = 0,
	/* The IAP was disconnected */
	DISCONNECTED,
	/* The IAP is disconnecting */
	DISCONNECTING,
	/* The IAP has a network address, but is not yet fully connected */
	NETWORK_UP
    };

private:
    void icdSignalReceived(QString&, QString&, QString&);
    bool first_call;
    QHash<QString, QNetworkSessionPrivateImpl *> sessions;
};

Q_GLOBAL_STATIC(IcdListener, icdListener);
 

static DBusHandlerResult signal_handler(DBusConnection *,
					DBusMessage *message,
					void *user_data)
{
    if (dbus_message_is_signal(message,
				ICD_DBUS_INTERFACE,
				ICD_STATUS_CHANGED_SIG)) {

	IcdListener *icd = (IcdListener *)user_data;
	DBusError error;
	dbus_error_init(&error);

	char *iap_id = 0;
	char *network_type = 0;
	char *state = 0;

	if (dbus_message_get_args(message, &error,
				    DBUS_TYPE_STRING, &iap_id,
				    DBUS_TYPE_STRING, &network_type,
				    DBUS_TYPE_STRING, &state,
				    DBUS_TYPE_INVALID) == FALSE) {
#ifdef BEARER_MANAGEMENT_DEBUG
        qDebug() << QString("Failed to parse icd status signal: %1").arg(error.message);
#endif
        } else {
	    QString _iap_id(iap_id);
	    QString _network_type(network_type);
	    QString _state(state);

	    icd->icdSignalReceived(_iap_id, _network_type, _state);
	}

	dbus_error_free(&error);
        return DBUS_HANDLER_RESULT_HANDLED;
    }

    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}


void IcdListener::setup(QNetworkSessionPrivateImpl *d)
{
    if (first_call) {
	// We use the old Icd dbus interface like in ConIC
	DBusError error;
	dbus_error_init(&error);

	dbus_connection = get_dbus_conn(&error);
	if (dbus_error_is_set(&error)) {
	    dbus_error_free(&error);
	    return;
	}

	static struct DBusObjectPathVTable icd_vtable;
	icd_vtable.message_function = signal_handler;

	dbus_bus_add_match(dbus_connection, ICD_DBUS_MATCH, &error);
	if (dbus_error_is_set(&error)) {
	    dbus_error_free(&error);
	    return;
	}

	if (dbus_connection_register_object_path(dbus_connection,
						    ICD_DBUS_PATH,
						    &icd_vtable,
						    (void*)this) == FALSE) {
	    dbus_error_free(&error);
	    return;
	}

#ifdef BEARER_MANAGEMENT_DEBUG
	qDebug() << "Listening" << ICD_STATUS_CHANGED_SIG << "signal from" << ICD_DBUS_SERVICE;
#endif
	first_call = false;
	dbus_error_free(&error);
    }

    QString id = d->activeConfig.identifier();
    if (!sessions.contains(id)) {
    QNetworkSessionPrivateImpl *ptr = d;
	sessions.insert(id, ptr);
    }
}


void IcdListener::icdSignalReceived(QString& iap_id,
#ifdef BEARER_MANAGEMENT_DEBUG
				    QString& network_type,
#else
				    QString&,
#endif
				    QString& state)
{
    if (iap_id == OSSO_IAP_SCAN) // icd sends scan status signals which we will ignore
	return;

#ifdef BEARER_MANAGEMENT_DEBUG
    qDebug() << "Status received:" << iap_id << "type" << network_type << "state" << state;
#endif

    if (!sessions.contains(iap_id)) {
#ifdef BEARER_MANAGEMENT_DEBUG
	qDebug() << "No session for IAP" << iap_id;
#endif
	return;
    }

    QNetworkSessionPrivateImpl *session = sessions.value(iap_id);
    QNetworkConfiguration ap_conf =
        QNetworkConfigurationManager().configurationFromIdentifier(iap_id);
    if (!ap_conf.isValid()) {
#ifdef BEARER_MANAGEMENT_DEBUG
	qDebug() << "Unknown IAP" << iap_id;
#endif
	return;
    }

    IapConnectionStatus status;

    if (state == "IDLE") {
	status = DISCONNECTED;
    } else if (state == "CONNECTED") {
	status = CONNECTED;
    } else if (state == "NETWORKUP") {
	status = NETWORK_UP;
    } else {
	//qDebug() << "Unknown state" << state;
	return;
    }

    if (status == DISCONNECTED) {
	if (ap_conf.state() == QNetworkConfiguration::Active) {
	    /* The IAP was just disconnected by Icd */
	    session->updateState(QNetworkSession::Disconnected);
	} else {
#ifdef BEARER_MANAGEMENT_DEBUG
	    qDebug() << "Got a network disconnect when in state" << ap_conf.state();
#endif
	}
    } else if (status == CONNECTED) {
	/* The IAP was just connected by Icd */
	session->updateState(QNetworkSession::Connected);
	session->updateIdentifier(iap_id);

	if (session->publicConfig.identifier() == OSSO_IAP_ANY) {
#ifdef BEARER_MANAGEMENT_DEBUG
	    qDebug() << "IAP" << iap_id << "connected when connecting to" << OSSO_IAP_ANY;
#endif
	} else {
#ifdef BEARER_MANAGEMENT_DEBUG
	    qDebug() << "IAP" << iap_id << "connected";
#endif
	}
    }

    return;
}


void IcdListener::cleanup()
{
    if (!first_call) {
	dbus_bus_remove_match(dbus_connection, ICD_DBUS_MATCH, NULL);
	dbus_connection_unref(dbus_connection);
    }
}


void IcdListener::cleanupSession(QNetworkSessionPrivateImpl *ptr)
{
    if (ptr->publicConfig.type() == QNetworkConfiguration::UserChoice)
        (void)sessions.take(ptr->activeConfig.identifier());
    else
        (void)sessions.take(ptr->publicConfig.identifier());
}


void QNetworkSessionPrivateImpl::cleanupSession(void)
{
    icdListener()->cleanupSession(this);

    QObject::disconnect(q, SIGNAL(stateChanged(QNetworkSession::State)), this, SLOT(updateProxies(QNetworkSession::State)));
}


void QNetworkSessionPrivateImpl::updateState(QNetworkSession::State newState)
{
    if (newState == state)
        return;

    state = newState;

	if (state == QNetworkSession::Disconnected) {
        isOpen = false;
	    currentNetworkInterface.clear();
        if (publicConfig.type() == QNetworkConfiguration::UserChoice) {
            IcdNetworkConfigurationPrivate *icdConfig =
                toIcdConfig(privateConfiguration(activeConfig));

            icdConfig->mutex.lock();
            icdConfig->state = QNetworkConfiguration::Defined;
            icdConfig->mutex.unlock();
        }

        IcdNetworkConfigurationPrivate *icdConfig =
            toIcdConfig(privateConfiguration(publicConfig));

        icdConfig->mutex.lock();
        icdConfig->state = QNetworkConfiguration::Defined;
        icdConfig->mutex.unlock();

	} else if (state == QNetworkSession::Connected) {
        isOpen = true;
	    if (publicConfig.type() == QNetworkConfiguration::UserChoice) {
            IcdNetworkConfigurationPrivate *icdConfig =
                toIcdConfig(privateConfiguration(activeConfig));

            icdConfig->mutex.lock();
            icdConfig->state = QNetworkConfiguration::Active;
            icdConfig->type = QNetworkConfiguration::InternetAccessPoint;
            icdConfig->mutex.unlock();
	    }

        IcdNetworkConfigurationPrivate *icdConfig =
            toIcdConfig(privateConfiguration(publicConfig));

        icdConfig->mutex.lock();
        icdConfig->state = QNetworkConfiguration::Active;
        icdConfig->mutex.unlock();
	}

    emit stateChanged(newState);
}


void QNetworkSessionPrivateImpl::updateIdentifier(QString &newId)
{
    if (publicConfig.type() == QNetworkConfiguration::UserChoice) {
        IcdNetworkConfigurationPrivate *icdConfig =
            toIcdConfig(privateConfiguration(activeConfig));

        icdConfig->mutex.lock();
        icdConfig->network_attrs |= ICD_NW_ATTR_IAPNAME;
        icdConfig->id = newId;
        icdConfig->mutex.unlock();
    } else {
        IcdNetworkConfigurationPrivate *icdConfig =
            toIcdConfig(privateConfiguration(publicConfig));

        icdConfig->mutex.lock();
        icdConfig->network_attrs |= ICD_NW_ATTR_IAPNAME;
        if (icdConfig->id != newId)
            icdConfig->id = newId;
        icdConfig->mutex.unlock();
    }
}


quint64 QNetworkSessionPrivateImpl::getStatistics(bool sent) const
{
    /* This could be also implemented by using the Maemo::Icd::statistics()
     * that gets the statistics data for a specific IAP. Change if
     * necessary.
     */
    Maemo::Icd icd;
    QList<Maemo::IcdStatisticsResult> stats_results;
    quint64 counter_rx = 0, counter_tx = 0;

    if (!icd.statistics(stats_results)) {
	return 0;
    }

    foreach (const Maemo::IcdStatisticsResult &res, stats_results) {
	if (res.params.network_attrs & ICD_NW_ATTR_IAPNAME) {
	    /* network_id is the IAP UUID */
	    if (QString(res.params.network_id.data()) == activeConfig.identifier()) {
		counter_tx = res.bytes_sent;
		counter_rx = res.bytes_received;
	    }
	} else {
	    /* We probably will never get to this branch */
        IcdNetworkConfigurationPrivate *icdConfig =
            toIcdConfig(privateConfiguration(activeConfig));

        icdConfig->mutex.lock();
        if (res.params.network_id == icdConfig->network_id) {
            counter_tx = res.bytes_sent;
            counter_rx = res.bytes_received;
	    }
        icdConfig->mutex.unlock();
	}
    }

    if (sent)
	return counter_tx;
    else
	return counter_rx;
}


quint64 QNetworkSessionPrivateImpl::bytesWritten() const
{
    return getStatistics(true);
}

quint64 QNetworkSessionPrivateImpl::bytesReceived() const
{
    return getStatistics(false);
}

quint64 QNetworkSessionPrivateImpl::activeTime() const
{
    if (startTime.isNull()) {
        return 0;
    }
    return startTime.secsTo(QDateTime::currentDateTime());
}


QNetworkConfiguration& QNetworkSessionPrivateImpl::copyConfig(QNetworkConfiguration &fromConfig,
                                                              QNetworkConfiguration &toConfig,
                                                              bool deepCopy)
{
    IcdNetworkConfigurationPrivate *cpPriv;
    if (deepCopy) {
        cpPriv = new IcdNetworkConfigurationPrivate;
        setPrivateConfiguration(toConfig, QNetworkConfigurationPrivatePointer(cpPriv));
    } else {
        cpPriv = toIcdConfig(privateConfiguration(toConfig));
    }

    IcdNetworkConfigurationPrivate *fromPriv = toIcdConfig(privateConfiguration(fromConfig));

    QMutexLocker toLocker(&cpPriv->mutex);
    QMutexLocker fromLocker(&fromPriv->mutex);

    cpPriv->name = fromPriv->name;
    cpPriv->isValid = fromPriv->isValid;
    // Note that we do not copy id field here as the publicConfig does
    // not contain a valid IAP id.
    cpPriv->state = fromPriv->state;
    cpPriv->type = fromPriv->type;
    cpPriv->roamingSupported = fromPriv->roamingSupported;
    cpPriv->purpose = fromPriv->purpose;
    cpPriv->network_id = fromPriv->network_id;
    cpPriv->iap_type = fromPriv->iap_type;
    cpPriv->network_attrs = fromPriv->network_attrs;
    cpPriv->service_type = fromPriv->service_type;
    cpPriv->service_id = fromPriv->service_id;
    cpPriv->service_attrs = fromPriv->service_attrs;

    return toConfig;
}


/* This is called by QNetworkSession constructor and it updates the current
 * state of the configuration.
 */
void QNetworkSessionPrivateImpl::syncStateWithInterface()
{
    /* Start to listen Icd status messages. */
    icdListener()->setup(this);

    /* Initially we are not active although the configuration might be in
     * connected state.
     */
    isOpen = false;
    opened = false;

    connect(&manager, SIGNAL(updateCompleted()), this, SLOT(networkConfigurationsChanged()));

    connect(&manager, SIGNAL(configurationChanged(QNetworkConfiguration)),
            this, SLOT(configurationChanged(QNetworkConfiguration)));

    QObject::connect(q, SIGNAL(stateChanged(QNetworkSession::State)), this, SLOT(updateProxies(QNetworkSession::State)));

    state = QNetworkSession::Invalid;
    lastError = QNetworkSession::UnknownSessionError;

    switch (publicConfig.type()) {
    case QNetworkConfiguration::InternetAccessPoint:
	activeConfig = publicConfig;
        break;
    case QNetworkConfiguration::ServiceNetwork:
	serviceConfig = publicConfig;
	break;
    case QNetworkConfiguration::UserChoice:
	// active config will contain correct data after open() has succeeded
	copyConfig(publicConfig, activeConfig);

	/* We create new configuration that holds the actual configuration
	 * returned by icd. This way publicConfig still contains the
	 * original user specified configuration.
	 *
	 * Note that the new activeConfig configuration is not inserted
	 * to configurationManager as manager class will get the newly
	 * connected configuration from gconf when the IAP is saved.
	 * This configuration manager update is done by IapMonitor class.
	 * If the ANY connection fails in open(), then the configuration
	 * data is not saved to gconf and will not be added to
	 * configuration manager IAP list.
	 */
#ifdef BEARER_MANAGEMENT_DEBUG
	qDebug()<<"New configuration created for" << publicConfig.identifier();
#endif
	break;
    default:
	/* Invalid configuration, no point continuing */
	return;
    }

    if (!activeConfig.isValid())
	return;

    /* Get the initial state from icd */
    Maemo::Icd icd;
    QList<Maemo::IcdStateResult> state_results;

    /* Update the active config from first connection, this is ok as icd
     * supports only one connection anyway.
     */
    if (icd.state(state_results) && !state_results.isEmpty()) {

	/* If we did not get full state back, then we are not
	 * connected and can skip the next part.
	 */
	if (!(state_results.first().params.network_attrs == 0 &&
		state_results.first().params.network_id.isEmpty())) {

	    /* If we try to connect to specific IAP and we get results back
	     * that tell the icd is actually connected to another IAP,
	     * then do not update current state etc.
	     */
	    if (publicConfig.type() == QNetworkConfiguration::UserChoice ||
            publicConfig.identifier() == state_results.first().params.network_id) {

		switch (state_results.first().state) {
		case ICD_STATE_DISCONNECTED:
		    state = QNetworkSession::Disconnected;
            {
                QNetworkConfigurationPrivatePointer ptr = privateConfiguration(activeConfig);
                if (ptr) {
                    ptr->mutex.lock();
                    ptr->isValid = true;
                    ptr->mutex.unlock();
                }
            }
		    break;
		case ICD_STATE_CONNECTING:
		    state = QNetworkSession::Connecting;
            {
                QNetworkConfigurationPrivatePointer ptr = privateConfiguration(activeConfig);
                if (ptr) {
                    ptr->mutex.lock();
                    ptr->isValid = true;
                    ptr->mutex.unlock();
                }
            }
		    break;
		case ICD_STATE_CONNECTED:
		    {
			if (!state_results.first().error.isEmpty())
			    break;
                        
            const QString id = state_results.first().params.network_id;

            QNetworkConfiguration config = manager.configurationFromIdentifier(id);
            if (config.isValid()) {
                //we don't want the copied data if the config is already known by the manager
                //just reuse it so that existing references to the old data get the same update
                setPrivateConfiguration(activeConfig, privateConfiguration(config));
            }

            QNetworkConfigurationPrivatePointer ptr = privateConfiguration(activeConfig);

            QMutexLocker configLocker(&ptr->mutex);

			state = QNetworkSession::Connected;
            toIcdConfig(ptr)->network_id = state_results.first().params.network_id;
            ptr->id = toIcdConfig(ptr)->network_id;
            toIcdConfig(ptr)->network_attrs = state_results.first().params.network_attrs;
            toIcdConfig(ptr)->iap_type = state_results.first().params.network_type;
            toIcdConfig(ptr)->service_type = state_results.first().params.service_type;
            toIcdConfig(ptr)->service_id = state_results.first().params.service_id;
            toIcdConfig(ptr)->service_attrs = state_results.first().params.service_attrs;
            ptr->type = QNetworkConfiguration::InternetAccessPoint;
            ptr->state = QNetworkConfiguration::Active;
            ptr->isValid = true;
			currentNetworkInterface = get_network_interface();

            Maemo::IAPConf iap_name(ptr->id);
			QString name_value = iap_name.value("name").toString();
			if (!name_value.isEmpty())
                ptr->name = name_value;
			else
                ptr->name = ptr->id;


			// Add the new active configuration to manager or update the old config
            if (!engine->hasIdentifier(ptr->id)) {
                configLocker.unlock();
                engine->addSessionConfiguration(ptr);
            } else {
                configLocker.unlock();
                engine->changedSessionConfiguration(ptr);
            }
            }
		    break;

		case ICD_STATE_DISCONNECTING:
		    state = QNetworkSession::Closing;
            {
                QNetworkConfigurationPrivatePointer ptr = privateConfiguration(activeConfig);
                if (ptr) {
                    ptr->mutex.lock();
                    ptr->isValid = true;
                    ptr->mutex.unlock();
                }
            }
		    break;
		default:
		    break;
		}
	    }
	} else {
#ifdef BEARER_MANAGEMENT_DEBUG
	    qDebug() << "status_req tells icd is not connected";
#endif
	}
    } else {
#ifdef BEARER_MANAGEMENT_DEBUG
	qDebug() << "status_req did not return any results from icd";
#endif
    }

    networkConfigurationsChanged();
}


void QNetworkSessionPrivateImpl::networkConfigurationsChanged()
{
    if (serviceConfig.isValid())
        updateStateFromServiceNetwork();
    else
        updateStateFromActiveConfig();
}


void QNetworkSessionPrivateImpl::updateStateFromServiceNetwork()
{
    QNetworkSession::State oldState = state;

    foreach (const QNetworkConfiguration &config, serviceConfig.children()) {
        if ((config.state() & QNetworkConfiguration::Active) != QNetworkConfiguration::Active)
            continue;

        if (activeConfig != config) {
            activeConfig = config;
            emit newConfigurationActivated();
        }

        state = QNetworkSession::Connected;
        if (state != oldState)
            emit stateChanged(state);

        return;
    }

    if (serviceConfig.children().isEmpty())
        state = QNetworkSession::NotAvailable;
    else
        state = QNetworkSession::Disconnected;

    if (state != oldState)
        emit stateChanged(state);
}


void QNetworkSessionPrivateImpl::clearConfiguration(QNetworkConfiguration &config)
{
    IcdNetworkConfigurationPrivate *icdConfig = toIcdConfig(privateConfiguration(config));

    QMutexLocker locker(&icdConfig->mutex);

    icdConfig->network_id.clear();
    icdConfig->iap_type.clear();
    icdConfig->network_attrs = 0;
    icdConfig->service_type.clear();
    icdConfig->service_id.clear();
    icdConfig->service_attrs = 0;
}


void QNetworkSessionPrivateImpl::updateStateFromActiveConfig()
{
    QNetworkSession::State oldState = state;

    bool newActive = false;

    if (!activeConfig.isValid())
        return;

    if (!activeConfig.isValid()) {
        state = QNetworkSession::Invalid;
        clearConfiguration(activeConfig);
    } else if ((activeConfig.state() & QNetworkConfiguration::Active) == QNetworkConfiguration::Active) {
        state = QNetworkSession::Connected;
        newActive = opened;
    } else if ((activeConfig.state() & QNetworkConfiguration::Discovered) == QNetworkConfiguration::Discovered) {
        state = QNetworkSession::Disconnected;
    } else if ((activeConfig.state() & QNetworkConfiguration::Defined) == QNetworkConfiguration::Defined) {
        state = QNetworkSession::NotAvailable;
    } else if ((activeConfig.state() & QNetworkConfiguration::Undefined) == QNetworkConfiguration::Undefined) {
        state = QNetworkSession::NotAvailable;
        //clearConfiguration(activeConfig);
    }

    bool oldActive = isOpen;
    isOpen = newActive;

    if (!oldActive && isOpen)
        emit quitPendingWaitsForOpened();

    if (oldActive && !isOpen)
        emit closed();

    if (oldState != state) {
        emit stateChanged(state);

	if (state == QNetworkSession::Disconnected) {
#ifdef BEARER_MANAGEMENT_DEBUG
	    //qDebug()<<"session aborted error emitted for"<<activeConfig.identifier();
#endif
	    lastError = QNetworkSession::SessionAbortedError;
        emit QNetworkSessionPrivate::error(lastError);
	}
    }

#ifdef BEARER_MANAGEMENT_DEBUG
    //qDebug()<<"oldState ="<<oldState<<" state ="<<state<<" oldActive ="<<oldActive<<" newActive ="<<newActive<<" opened ="<<opened;
#endif
}


void QNetworkSessionPrivateImpl::configurationChanged(const QNetworkConfiguration &config)
{
    if (serviceConfig.isValid() && (config == serviceConfig || config == activeConfig))
        updateStateFromServiceNetwork();
    else if (config == activeConfig)
        updateStateFromActiveConfig();
}


static QString get_network_interface()
{
    Maemo::Icd icd;
    QList<Maemo::IcdAddressInfoResult> addr_results;
    uint ret;
    QString iface;

    ret = icd.addrinfo(addr_results);
    if (ret == 0) {
	/* No results */
#ifdef BEARER_MANAGEMENT_DEBUG
	qDebug() << "Cannot get addrinfo from icd, are you connected or is icd running?";
#endif
	return iface;
    }

    const char *address = addr_results.first().ip_info.first().address.toAscii().constData();
    struct in_addr addr;
    if (inet_aton(address, &addr) == 0) {
#ifdef BEARER_MANAGEMENT_DEBUG
	qDebug() << "address" << address << "invalid";
#endif
	return iface;
    }

    struct ifaddrs *ifaddr, *ifa;
    int family;

    if (getifaddrs(&ifaddr) == -1) {
#ifdef BEARER_MANAGEMENT_DEBUG
	qDebug() << "getifaddrs() failed";
#endif
	return iface;
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
	family = ifa->ifa_addr->sa_family;
	if (family != AF_INET) {
	    continue; /* Currently only IPv4 is supported by icd dbus interface */
	}
	if (((struct sockaddr_in *)ifa->ifa_addr)->sin_addr.s_addr == addr.s_addr) {
	    iface = QString(ifa->ifa_name);
	    break;
	}
    }

    freeifaddrs(ifaddr);
    return iface;
}


void QNetworkSessionPrivateImpl::open()
{
    if (serviceConfig.isValid()) {
        lastError = QNetworkSession::OperationNotSupportedError;
        emit QNetworkSessionPrivate::error(lastError);
    } else if (!isOpen) {

	if (publicConfig.type() == QNetworkConfiguration::UserChoice) {
	    /* Caller is trying to connect to default IAP.
	     * At this time we will not know the IAP details so we just
	     * connect and update the active config when the IAP is
	     * connected.
	     */
	    opened = true;
            state = QNetworkSession::Connecting;
            emit stateChanged(state);
	    QTimer::singleShot(0, this, SLOT(do_open()));
	    return;
	}

	/* User is connecting to one specific IAP. If that IAP is not
	 * in discovered state we cannot continue.
	 */
        if ((activeConfig.state() & QNetworkConfiguration::Discovered) !=
            QNetworkConfiguration::Discovered) {
            lastError =QNetworkSession::InvalidConfigurationError;
            emit QNetworkSessionPrivate::error(lastError);
            return;
        }
        opened = true;

        if ((activeConfig.state() & QNetworkConfiguration::Active) != QNetworkConfiguration::Active) {
            state = QNetworkSession::Connecting;
            emit stateChanged(state);

	    QTimer::singleShot(0, this, SLOT(do_open()));
	    return;
        }

        isOpen = (activeConfig.state() & QNetworkConfiguration::Active) == QNetworkConfiguration::Active;
        if (isOpen)
            emit quitPendingWaitsForOpened();
    } else {
	/* We seem to be active so inform caller */
	emit quitPendingWaitsForOpened();
    }
}


void QNetworkSessionPrivateImpl::do_open()
{
    icd_connection_flags flags = connectFlags;
    bool st;
    QString result;
    QString iap = publicConfig.identifier();

    if (state == QNetworkSession::Connected) {
#ifdef BEARER_MANAGEMENT_DEBUG
	qDebug() << "Already connected to" << activeConfig.identifier();
#endif
        emit stateChanged(QNetworkSession::Connected);
	emit quitPendingWaitsForOpened();
	return;
    }

    Maemo::IcdConnectResult connect_result;
    Maemo::Icd icd(ICD_LONG_CONNECT_TIMEOUT);
    QNetworkConfiguration config;
    if (publicConfig.type() == QNetworkConfiguration::UserChoice)
	config = activeConfig;
    else
	config = publicConfig;

    if (iap == OSSO_IAP_ANY) {
#ifdef BEARER_MANAGEMENT_DEBUG
	qDebug() << "connecting to default IAP" << iap;
#endif
	st = icd.connect(flags, connect_result);

    } else {

	QList<Maemo::ConnectParams> params;
	Maemo::ConnectParams param;

    IcdNetworkConfigurationPrivate *icdConfig = toIcdConfig(privateConfiguration(config));

    icdConfig->mutex.lock();
    param.connect.service_type = icdConfig->service_type;
    param.connect.service_attrs = icdConfig->service_attrs;
    param.connect.service_id = icdConfig->service_id;
    param.connect.network_type = icdConfig->iap_type;
    param.connect.network_attrs = icdConfig->network_attrs;
    if (icdConfig->network_attrs & ICD_NW_ATTR_IAPNAME)
	    param.connect.network_id = QByteArray(iap.toLatin1());
	else
        param.connect.network_id = icdConfig->network_id;
	params.append(param);
    icdConfig->mutex.unlock();

#ifdef BEARER_MANAGEMENT_DEBUG
	qDebug("connecting to %s/%s/0x%x/%s/0x%x/%s",
	    param.connect.network_id.data(),
	    param.connect.network_type.toAscii().constData(),
	    param.connect.network_attrs,
	    param.connect.service_type.toAscii().constData(),
	    param.connect.service_attrs,
	    param.connect.service_id.toAscii().constData());
#endif
	st = icd.connect(flags, params, connect_result);
    }

    if (st) {
	result = connect_result.connect.network_id.data();
	QString connected_iap = result;

	if (connected_iap.isEmpty()) {
#ifdef BEARER_MANAGEMENT_DEBUG
	    qDebug() << "connect to"<< iap << "failed, result is empty";
#endif
	    updateState(QNetworkSession::Disconnected);
        emit QNetworkSessionPrivate::error(QNetworkSession::InvalidConfigurationError);
	    if (publicConfig.type() == QNetworkConfiguration::UserChoice)
		cleanupAnyConfiguration();
	    return;
	}

	/* If the user tried to connect to some specific connection (foo)
	 * and we were already connected to some other connection (bar),
	 * then we cannot activate this session although icd has a valid
	 * connection to somewhere.
	 */
	if ((publicConfig.type() != QNetworkConfiguration::UserChoice) &&
	    (connected_iap != config.identifier())) {
	    updateState(QNetworkSession::Disconnected);
        emit QNetworkSessionPrivate::error(QNetworkSession::InvalidConfigurationError);
	    return;
	}

    IcdNetworkConfigurationPrivate *icdConfig = toIcdConfig(privateConfiguration(config));

	/* Did we connect to non saved IAP? */
    icdConfig->mutex.lock();
    if (!(icdConfig->network_attrs & ICD_NW_ATTR_IAPNAME)) {
	    /* Because the connection succeeded, the IAP is now known.
	     */
        icdConfig->network_attrs |= ICD_NW_ATTR_IAPNAME;
        icdConfig->id = connected_iap;
	}

	/* User might have changed the IAP name when a new IAP was saved */
    Maemo::IAPConf iap_name(icdConfig->id);
	QString name = iap_name.value("name").toString();
	if (!name.isEmpty())
        icdConfig->name = name;

    icdConfig->iap_type = connect_result.connect.network_type;

    icdConfig->isValid = true;
    icdConfig->state = QNetworkConfiguration::Active;
    icdConfig->type = QNetworkConfiguration::InternetAccessPoint;

    icdConfig->mutex.unlock();

	startTime = QDateTime::currentDateTime();
	updateState(QNetworkSession::Connected);

	currentNetworkInterface = get_network_interface();

#ifdef BEARER_MANAGEMENT_DEBUG
    qDebug() << "connected to" << result << config.name() << "at" << currentNetworkInterface;
#endif

	/* We first check if the configuration already exists in the manager
	 * and if it is not found there, we then insert it. Note that this
	 * is only done for user choice config only because it can be missing
	 * from config manager list.
	 */

    if (publicConfig.type() == QNetworkConfiguration::UserChoice) {
        if (!engine->hasIdentifier(result)) {
            engine->addSessionConfiguration(privateConfiguration(config));
        } else {
            QNetworkConfigurationPrivatePointer priv = engine->configuration(result);
            QNetworkConfiguration reference;
            setPrivateConfiguration(reference, priv);
            copyConfig(config, reference, false);
            config = reference;
            activeConfig = reference;
            engine->changedSessionConfiguration(privateConfiguration(config));
	    }
	}

	emit quitPendingWaitsForOpened();

    } else {
#ifdef BEARER_MANAGEMENT_DEBUG
	qDebug() << "connect to"<< iap << "failed, status:" << connect_result.status;
#endif
	updateState(QNetworkSession::Disconnected);
	if (publicConfig.type() == QNetworkConfiguration::UserChoice)
	    cleanupAnyConfiguration();
    emit QNetworkSessionPrivate::error(QNetworkSession::UnknownSessionError);
    }
}


void QNetworkSessionPrivateImpl::cleanupAnyConfiguration()
{
#ifdef BEARER_MANAGEMENT_DEBUG
    qDebug()<<"Removing configuration created for" << activeConfig.identifier();
#endif
    activeConfig = publicConfig;
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

	    Maemo::Icd icd;
#ifdef BEARER_MANAGEMENT_DEBUG
	    qDebug() << "stopping session" << publicConfig.identifier();
#endif
	    icd.disconnect(ICD_CONNECTION_FLAG_APPLICATION_EVENT);
	    startTime = QDateTime();

	    /* Note that the state will go disconnected in
	     * updateStateFromActiveConfig() which gets called after
	     * configurationChanged is emitted (below).
	     */

        QNetworkConfigurationPrivatePointer ptr = privateConfiguration(activeConfig);
        ptr->mutex.lock();
        ptr->state = QNetworkConfiguration::Discovered;
        ptr->mutex.unlock();
        engine->changedSessionConfiguration(ptr);

	    opened = false;
	    isOpen = false;

        } else {
	    opened = false;
	    isOpen = false;
        emit closed();
	}
    }
}


void QNetworkSessionPrivateImpl::migrate()
{
}


void QNetworkSessionPrivateImpl::accept()
{
}


void QNetworkSessionPrivateImpl::ignore()
{
}


void QNetworkSessionPrivateImpl::reject()
{
}

#ifndef QT_NO_NETWORKINTERFACE
QNetworkInterface QNetworkSessionPrivateImpl::currentInterface() const
{
    if (!publicConfig.isValid() || state != QNetworkSession::Connected)
        return QNetworkInterface();

    if (currentNetworkInterface.isEmpty())
        return QNetworkInterface();

    return QNetworkInterface::interfaceFromName(currentNetworkInterface);
}
#endif

void QNetworkSessionPrivateImpl::setSessionProperty(const QString& key, const QVariant& value)
{
    if (value.isValid()) {
	properties.insert(key, value);

	if (key == "ConnectInBackground") {
	    bool v = value.toBool();
	    if (v)
		connectFlags = ICD_CONNECTION_FLAG_APPLICATION_EVENT;
	    else
		connectFlags = ICD_CONNECTION_FLAG_USER_EVENT;
	}
    } else {
	properties.remove(key);

	/* Set default value when property is removed */
	if (key == "ConnectInBackground")
	    connectFlags = ICD_CONNECTION_FLAG_USER_EVENT;
    }
}


QVariant QNetworkSessionPrivateImpl::sessionProperty(const QString& key) const
{
    return properties.value(key);
}


QString QNetworkSessionPrivateImpl::errorString() const
{
    QString errorStr;
    switch(q->error()) {
    case QNetworkSession::RoamingError:
        errorStr = QNetworkSessionPrivateImpl::tr("Roaming error");
        break;
    case QNetworkSession::SessionAbortedError:
        errorStr = QNetworkSessionPrivateImpl::tr("Session aborted by user or system");
        break;
    default:
    case QNetworkSession::UnknownSessionError:
        errorStr = QNetworkSessionPrivateImpl::tr("Unidentified Error");
        break;
    }
    return errorStr;
}


QNetworkSession::SessionError QNetworkSessionPrivateImpl::error() const
{
    return QNetworkSession::UnknownSessionError;
}

void QNetworkSessionPrivateImpl::updateProxies(QNetworkSession::State newState)
{
    if ((newState == QNetworkSession::Connected) &&
	(newState != currentState))
	updateProxyInformation();
    else if ((newState == QNetworkSession::Disconnected) &&
	    (currentState == QNetworkSession::Closing))
	clearProxyInformation();

    currentState = newState;
}


void QNetworkSessionPrivateImpl::updateProxyInformation()
{
    Maemo::ProxyConf::update();
}


void QNetworkSessionPrivateImpl::clearProxyInformation()
{
    Maemo::ProxyConf::clear();
}

#include "qnetworksession_impl.moc"

QT_END_NAMESPACE

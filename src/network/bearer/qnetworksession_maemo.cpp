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

#include <QHash>

#include "qnetworksession_maemo_p.h"
#include <dbus/dbus.h>
#include <dbus/dbus-glib-lowlevel.h>

#include <maemo_icd.h>
#include <iapconf.h>

#include <sys/types.h>
#include <ifaddrs.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

QTM_BEGIN_NAMESPACE

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
    void setup(QNetworkSessionPrivate *d);
    void cleanup();
    void cleanupSession(QNetworkSessionPrivate *ptr);

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
    QHash<QString, QNetworkSessionPrivate* > sessions;
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
	    qWarning() << QString("Failed to parse icd status signal: %1").arg(error.message);
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


void IcdListener::setup(QNetworkSessionPrivate *d)
{
    if (first_call) {
	// We use the old Icd dbus interface like in ConIC
	DBusError error;
	dbus_error_init(&error);

	dbus_connection = get_dbus_conn(&error);
	if (dbus_error_is_set(&error)) {
	    qWarning() << "Cannot get dbus connection.";
	    dbus_error_free(&error);
	    return;
	}

	static struct DBusObjectPathVTable icd_vtable;
	icd_vtable.message_function = signal_handler;

	dbus_bus_add_match(dbus_connection, ICD_DBUS_MATCH, &error);
	if (dbus_error_is_set(&error)) {
	    qWarning() << "Cannot add match" << ICD_DBUS_MATCH;
	    dbus_error_free(&error);
	    return;
	}

	if (dbus_connection_register_object_path(dbus_connection,
						    ICD_DBUS_PATH,
						    &icd_vtable,
						    (void*)this) == FALSE) {
	    qWarning() << "Cannot register dbus signal handler, interface"<< ICD_DBUS_INTERFACE << "path" << ICD_DBUS_PATH;
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
	QNetworkSessionPrivate *ptr = d;
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

    QNetworkSessionPrivate *session = sessions.value(iap_id);
    QNetworkConfiguration ap_conf = session->manager.configurationFromIdentifier(iap_id);
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


void IcdListener::cleanupSession(QNetworkSessionPrivate *ptr)
{
    if (ptr->publicConfig.type() == QNetworkConfiguration::UserChoice)
        (void)sessions.take(ptr->activeConfig.identifier());
    else
        (void)sessions.take(ptr->publicConfig.identifier());
}


void QNetworkSessionPrivate::cleanupSession(void)
{
    icdListener()->cleanupSession(this);
}


void QNetworkSessionPrivate::updateState(QNetworkSession::State newState)
{
    if( newState != state) {
        state = newState;

	if (state == QNetworkSession::Disconnected) {
	    isActive = false;
	    currentNetworkInterface.clear();
	    if (publicConfig.type() == QNetworkConfiguration::UserChoice)
		activeConfig.d->state = QNetworkConfiguration::Defined;
	    publicConfig.d->state = QNetworkConfiguration::Defined;

	} else if (state == QNetworkSession::Connected) {
	    isActive = true;
	    if (publicConfig.type() == QNetworkConfiguration::UserChoice) {
		activeConfig.d->state = QNetworkConfiguration::Active;
		activeConfig.d->type = QNetworkConfiguration::InternetAccessPoint;
	    }
	    publicConfig.d->state = QNetworkConfiguration::Active;
	}

	emit q->stateChanged(newState);
    }
}


void QNetworkSessionPrivate::updateIdentifier(QString &newId)
{
    if (publicConfig.type() == QNetworkConfiguration::UserChoice) {
	activeConfig.d->network_attrs |= ICD_NW_ATTR_IAPNAME;
	activeConfig.d->id = newId;
    } else {
	publicConfig.d->network_attrs |= ICD_NW_ATTR_IAPNAME;
	if (publicConfig.d->id != newId) {
	    qWarning() << "Your config id changed from" << publicConfig.d->id << "to" << newId;
	    publicConfig.d->id = newId;
	}
    }
}


quint64 QNetworkSessionPrivate::getStatistics(bool sent) const
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

    foreach (Maemo::IcdStatisticsResult res, stats_results) {
	if (res.params.network_attrs & ICD_NW_ATTR_IAPNAME) {
	    /* network_id is the IAP UUID */
	    if (QString(res.params.network_id.data()) == activeConfig.identifier()) {
		counter_tx = res.bytes_sent;
		counter_rx = res.bytes_received;
	    }
	} else {
	    /* We probably will never get to this branch */
	    QNetworkConfigurationPrivate *d = activeConfig.d.data();
	    if (res.params.network_id == d->network_id) {
		counter_tx = res.bytes_sent;
		counter_rx = res.bytes_received;
	    }
	}
    }

    if (sent)
	return counter_tx;
    else
	return counter_rx;
}


quint64 QNetworkSessionPrivate::bytesWritten() const
{
    return getStatistics(true);
}

quint64 QNetworkSessionPrivate::bytesReceived() const
{
    return getStatistics(false);
}

quint64 QNetworkSessionPrivate::activeTime() const
{
    if (startTime.isNull()) {
        return 0;
    }
    return startTime.secsTo(QDateTime::currentDateTime());
}


QNetworkConfiguration& QNetworkSessionPrivate::copyConfig(QNetworkConfiguration &fromConfig, QNetworkConfiguration &toConfig, bool deepCopy)
{
    if (deepCopy) {
        QNetworkConfigurationPrivate* cpPriv = new QNetworkConfigurationPrivate();
        QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> ptr(cpPriv);
        toConfig.d = ptr;
    }

    toConfig.d->name = fromConfig.d->name;
    toConfig.d->isValid = fromConfig.d->isValid;
    // Note that we do not copy id field here as the publicConfig does
    // not contain a valid IAP id.
    toConfig.d->state = fromConfig.d->state;
    toConfig.d->type = fromConfig.d->type;
    toConfig.d->roamingSupported = fromConfig.d->roamingSupported;
    toConfig.d->purpose = fromConfig.d->purpose;
    toConfig.d->network_id = fromConfig.d->network_id;
    toConfig.d->iap_type = fromConfig.d->iap_type;
    toConfig.d->network_attrs = fromConfig.d->network_attrs;
    toConfig.d->service_type = fromConfig.d->service_type;
    toConfig.d->service_id = fromConfig.d->service_id;
    toConfig.d->service_attrs = fromConfig.d->service_attrs;
    toConfig.d->manager = fromConfig.d->manager;

    return toConfig;
}


/* This is called by QNetworkSession constructor and it updates the current
 * state of the configuration.
 */
void QNetworkSessionPrivate::syncStateWithInterface()
{
    /* Start to listen Icd status messages. */
    icdListener()->setup(this);

    /* Initially we are not active although the configuration might be in
     * connected state.
     */
    isActive = false;
    opened = false;

    QObject::connect(&manager, SIGNAL(updateCompleted()), this, SLOT(networkConfigurationsChanged()));

    if (publicConfig.d.data()) {
	QNetworkConfigurationManagerPrivate* mgr = (QNetworkConfigurationManagerPrivate*)publicConfig.d.data()->manager;
	if (mgr) {
	    QObject::connect(mgr, SIGNAL(configurationChanged(QNetworkConfiguration)),
			    this, SLOT(configurationChanged(QNetworkConfiguration)));
	} else {
	    qWarning()<<"Manager object not set when trying to connect configurationChanged signal. Your configuration object is not correctly setup, did you remember to call manager.updateConfigurations() before creating session object?";
	    state = QNetworkSession::Invalid;
	    lastError = QNetworkSession::UnknownSessionError;
	    return;
	}
    }

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
		    publicConfig.d->id == state_results.first().params.network_id) {

		switch (state_results.first().state) {
		case ICD_STATE_DISCONNECTED:
		    state = QNetworkSession::Disconnected;
		    if (activeConfig.d.data())
			activeConfig.d->isValid = true;
		    break;
		case ICD_STATE_CONNECTING:
		    state = QNetworkSession::Connecting;
		    if (activeConfig.d.data())
			activeConfig.d->isValid = true;
		    break;
		case ICD_STATE_CONNECTED:
		    {
			if (!state_results.first().error.isEmpty())
			    break;
                        
                        const QString id = state_results.first().params.network_id;

			QNetworkConfigurationManagerPrivate *mgr = (QNetworkConfigurationManagerPrivate*)activeConfig.d.data()->manager;
			if (mgr->accessPointConfigurations.contains(id)) {
                            //we don't want the copied data if the config is already known by the manager
                            //just reuse it so that existing references to the old data get the same update
			    QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv = mgr->accessPointConfigurations.value(activeConfig.d->id);
                            activeConfig.d = priv;
                        }


			state = QNetworkSession::Connected;
			activeConfig.d->network_id = state_results.first().params.network_id;
			activeConfig.d->id = activeConfig.d->network_id;
			activeConfig.d->network_attrs = state_results.first().params.network_attrs;
			activeConfig.d->iap_type = state_results.first().params.network_type;
			activeConfig.d->service_type = state_results.first().params.service_type;
			activeConfig.d->service_id = state_results.first().params.service_id;
			activeConfig.d->service_attrs = state_results.first().params.service_attrs;
			activeConfig.d->type = QNetworkConfiguration::InternetAccessPoint;
			activeConfig.d->state = QNetworkConfiguration::Active;
			activeConfig.d->isValid = true;
			currentNetworkInterface = get_network_interface();

			Maemo::IAPConf iap_name(activeConfig.d->id);
			QString name_value = iap_name.value("name").toString();
			if (!name_value.isEmpty())
			    activeConfig.d->name = name_value;
			else
			    activeConfig.d->name = activeConfig.d->id;


			// Add the new active configuration to manager or update the old config
			mgr = (QNetworkConfigurationManagerPrivate*)activeConfig.d.data()->manager;
			if (!(mgr->accessPointConfigurations.contains(activeConfig.d->id))) {
			    QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> ptr = activeConfig.d;
			    mgr->accessPointConfigurations.insert(activeConfig.d->id, ptr);

			    QNetworkConfiguration item;
			    item.d = ptr;
			    emit mgr->configurationAdded(item);

#ifdef BEARER_MANAGEMENT_DEBUG
			    //qDebug()<<"New configuration"<<activeConfig.d->id<<"added to manager in sync";
#endif

			} else {
			    mgr->configurationChanged((QNetworkConfigurationPrivate*)(activeConfig.d.data()));
#ifdef BEARER_MANAGEMENT_DEBUG
			    //qDebug()<<"Existing configuration"<<activeConfig.d->id<<"updated in manager in sync";
#endif
			}

		    }
		    break;

		case ICD_STATE_DISCONNECTING:
		    state = QNetworkSession::Closing;
		    if (activeConfig.d.data())
			activeConfig.d->isValid = true;
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


void QNetworkSessionPrivate::networkConfigurationsChanged()
{
    if (serviceConfig.isValid())
        updateStateFromServiceNetwork();
    else
        updateStateFromActiveConfig();
}


void QNetworkSessionPrivate::updateStateFromServiceNetwork()
{
    QNetworkSession::State oldState = state;

    foreach (const QNetworkConfiguration &config, serviceConfig.children()) {
        if ((config.state() & QNetworkConfiguration::Active) != QNetworkConfiguration::Active)
            continue;

        if (activeConfig != config) {
            activeConfig = config;
            emit q->newConfigurationActivated();
        }

        state = QNetworkSession::Connected;
        if (state != oldState)
            emit q->stateChanged(state);

        return;
    }

    if (serviceConfig.children().isEmpty())
        state = QNetworkSession::NotAvailable;
    else
        state = QNetworkSession::Disconnected;

    if (state != oldState)
        emit q->stateChanged(state);
}


void QNetworkSessionPrivate::clearConfiguration(QNetworkConfiguration &config)
{
    config.d->network_id.clear();
    config.d->iap_type.clear();
    config.d->network_attrs = 0;
    config.d->service_type.clear();
    config.d->service_id.clear();
    config.d->service_attrs = 0;
}


void QNetworkSessionPrivate::updateStateFromActiveConfig()
{
    QNetworkSession::State oldState = state;

    bool newActive = false;

    if (!activeConfig.d.data())
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
	clearConfiguration(activeConfig);
    }

    bool oldActive = isActive;
    isActive = newActive;

    if (!oldActive && isActive)
        emit quitPendingWaitsForOpened();

    if (oldActive && !isActive)
        emit q->closed();

    if (oldState != state) {
        emit q->stateChanged(state);

	if (state == QNetworkSession::Disconnected) {
#ifdef BEARER_MANAGEMENT_DEBUG
	    //qDebug()<<"session aborted error emitted for"<<activeConfig.identifier();
#endif
	    lastError = QNetworkSession::SessionAbortedError;
	    emit q->error(lastError);
	}
    }

#ifdef BEARER_MANAGEMENT_DEBUG
    //qDebug()<<"oldState ="<<oldState<<" state ="<<state<<" oldActive ="<<oldActive<<" newActive ="<<newActive<<" opened ="<<opened;
#endif
}


void QNetworkSessionPrivate::configurationChanged(const QNetworkConfiguration &config)
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


void QNetworkSessionPrivate::open()
{
    if (serviceConfig.isValid()) {
        lastError = QNetworkSession::OperationNotSupportedError;
        emit q->error(lastError);
    } else if (!isActive) {

	if (publicConfig.type() == QNetworkConfiguration::UserChoice) {
	    /* Caller is trying to connect to default IAP.
	     * At this time we will not know the IAP details so we just
	     * connect and update the active config when the IAP is
	     * connected.
	     */
	    opened = true;
            state = QNetworkSession::Connecting;
            emit q->stateChanged(state);
	    QTimer::singleShot(0, this, SLOT(do_open()));
	    return;
	}

	/* User is connecting to one specific IAP. If that IAP is not
	 * in discovered state we cannot continue.
	 */
        if ((activeConfig.state() & QNetworkConfiguration::Discovered) !=
            QNetworkConfiguration::Discovered) {
            lastError =QNetworkSession::InvalidConfigurationError;
            emit q->error(lastError);
            return;
        }
        opened = true;

        if ((activeConfig.state() & QNetworkConfiguration::Active) != QNetworkConfiguration::Active) {
            state = QNetworkSession::Connecting;
            emit q->stateChanged(state);

	    QTimer::singleShot(0, this, SLOT(do_open()));
	    return;
        }

        isActive = (activeConfig.state() & QNetworkConfiguration::Active) == QNetworkConfiguration::Active;
        if (isActive)
            emit quitPendingWaitsForOpened();
    } else {
	/* We seem to be active so inform caller */
	emit quitPendingWaitsForOpened();
    }
}


void QNetworkSessionPrivate::do_open()
{
    icd_connection_flags flags = connectFlags;
    bool st;
    QString result;
    QString iap = publicConfig.identifier();
    QString bearer_name;

    if (state == QNetworkSession::Connected) {
#ifdef BEARER_MANAGEMENT_DEBUG
	qDebug() << "Already connected to" << activeConfig.identifier();
#endif
        emit q->stateChanged(QNetworkSession::Connected);
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
	param.connect.service_type = config.d->service_type;
	param.connect.service_attrs = config.d->service_attrs;
	param.connect.service_id = config.d->service_id;
	param.connect.network_type = config.d->iap_type;
	param.connect.network_attrs = config.d->network_attrs;
	if (config.d->network_attrs & ICD_NW_ATTR_IAPNAME)
	    param.connect.network_id = QByteArray(iap.toLatin1());
	else
	    param.connect.network_id = config.d->network_id;
	params.append(param);

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
	    emit quitPendingWaitsForOpened();
	    emit q->error(QNetworkSession::InvalidConfigurationError);
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
	    emit quitPendingWaitsForOpened();
	    emit q->error(QNetworkSession::InvalidConfigurationError);
	    return;
	}


	/* Did we connect to non saved IAP? */
	if (!(config.d->network_attrs & ICD_NW_ATTR_IAPNAME)) {
	    /* Because the connection succeeded, the IAP is now known.
	     */
	    config.d->network_attrs |= ICD_NW_ATTR_IAPNAME;
	    config.d->id = connected_iap;
	}

	/* User might have changed the IAP name when a new IAP was saved */
	Maemo::IAPConf iap_name(config.d->id);
	QString name = iap_name.value("name").toString();
	if (!name.isEmpty())
	    config.d->name = name;

	bearer_name = connect_result.connect.network_type;
	if (bearer_name == "WLAN_INFRA" ||
	    bearer_name == "WLAN_ADHOC")
	    currentBearerName = "WLAN";
	else if (bearer_name == "GPRS")
	    currentBearerName = "HSPA";
	else
	    currentBearerName = bearer_name;

	config.d->isValid = true;
	config.d->state = QNetworkConfiguration::Active;
	config.d->type = QNetworkConfiguration::InternetAccessPoint;

	startTime = QDateTime::currentDateTime();
	updateState(QNetworkSession::Connected);

	currentNetworkInterface = get_network_interface();

#ifdef BEARER_MANAGEMENT_DEBUG
	qDebug() << "connected to" << result << config.d->name << "at" << currentNetworkInterface;
#endif

	/* We first check if the configuration already exists in the manager
	 * and if it is not found there, we then insert it. Note that this
	 * is only done for user choice config only because it can be missing
	 * from config manager list.
	 */

	if (publicConfig.d->type == QNetworkConfiguration::UserChoice) {

#ifdef BEARER_MANAGEMENT_DEBUG
#if 0
	    QList<QNetworkConfiguration> configs;
	    QNetworkConfigurationManagerPrivate *conPriv = (QNetworkConfigurationManagerPrivate*)config.d.data()->manager;
	    QList<QString> cpsIdents = conPriv->accessPointConfigurations.keys();
	    foreach( QString ii, cpsIdents) {
		QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> p = 
		    conPriv->accessPointConfigurations.value(ii);
		QNetworkConfiguration pt;
		pt.d = conPriv->accessPointConfigurations.value(ii);
		configs << pt;
	    }

	    int all = configs.count();
	    qDebug() << "All configurations:" << all;
	    foreach(QNetworkConfiguration p, configs) {
		qDebug() << p.name() <<":  isvalid->" <<p.isValid() << " type->"<< p.type() << 
		    " roaming->" << p.isRoamingAvailable() << "identifier->" << p.identifier() <<
		    " purpose->" << p.purpose() << " state->" << p.state();
	    }
#endif
#endif

	    QNetworkConfigurationManagerPrivate *mgr = (QNetworkConfigurationManagerPrivate*)config.d.data()->manager;
            if (!mgr->accessPointConfigurations.contains(result)) {
		QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> ptr = config.d;
		mgr->accessPointConfigurations.insert(result, ptr);

		QNetworkConfiguration item;
		item.d = ptr;
		emit mgr->configurationAdded(item);

#ifdef BEARER_MANAGEMENT_DEBUG
		//qDebug()<<"New configuration"<<result<<"added to manager in open";
#endif

	    } else {
		QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv = mgr->accessPointConfigurations.value(result);
		QNetworkConfiguration reference;
		reference.d = priv;
                copyConfig(config, reference, false);
                config = reference;
                activeConfig = reference;
		mgr->configurationChanged((QNetworkConfigurationPrivate*)(config.d.data()));

#ifdef BEARER_MANAGEMENT_DEBUG
		//qDebug()<<"Existing configuration"<<result<<"updated in manager in open";
#endif
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
	emit quitPendingWaitsForOpened();
	emit q->error(QNetworkSession::UnknownSessionError);
    }
}


void QNetworkSessionPrivate::cleanupAnyConfiguration()
{
#ifdef BEARER_MANAGEMENT_DEBUG
    qDebug()<<"Removing configuration created for" << activeConfig.d->id;
#endif
    activeConfig = publicConfig;
}


void QNetworkSessionPrivate::close()
{
    if (serviceConfig.isValid()) {
        lastError = QNetworkSession::OperationNotSupportedError;
        emit q->error(lastError);
    } else if (isActive) {
        opened = false;
        isActive = false;
        emit q->closed();
    }
}


void QNetworkSessionPrivate::stop()
{
    if (serviceConfig.isValid()) {
        lastError = QNetworkSession::OperationNotSupportedError;
        emit q->error(lastError);
    } else {
        if ((activeConfig.state() & QNetworkConfiguration::Active) == QNetworkConfiguration::Active) {
            state = QNetworkSession::Closing;
            emit q->stateChanged(state);

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

	    activeConfig.d->state = QNetworkConfiguration::Discovered;
	    QNetworkConfigurationManagerPrivate *mgr = (QNetworkConfigurationManagerPrivate*)activeConfig.d.data()->manager;
	    mgr->configurationChanged((QNetworkConfigurationPrivate*)activeConfig.d.data());

	    opened = false;
	    isActive = false;

        } else {
	    opened = false;
	    isActive = false;
	    emit q->closed();
	}
    }
}


void QNetworkSessionPrivate::migrate()
{
    qWarning("This platform does not support roaming (%s).", __FUNCTION__);
}


void QNetworkSessionPrivate::accept()
{
    qWarning("This platform does not support roaming (%s).", __FUNCTION__);
}


void QNetworkSessionPrivate::ignore()
{
    qWarning("This platform does not support roaming (%s).", __FUNCTION__);
}


void QNetworkSessionPrivate::reject()
{
    qWarning("This platform does not support roaming (%s).", __FUNCTION__);
}


QNetworkInterface QNetworkSessionPrivate::currentInterface() const
{
    if (!publicConfig.isValid() || state != QNetworkSession::Connected)
        return QNetworkInterface();

    if (currentNetworkInterface.isEmpty())
        return QNetworkInterface();

    return QNetworkInterface::interfaceFromName(currentNetworkInterface);
}


void QNetworkSessionPrivate::setSessionProperty(const QString& key, const QVariant& value)
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


QVariant QNetworkSessionPrivate::sessionProperty(const QString& key) const
{
    return properties.value(key);
}


QString QNetworkSessionPrivate::bearerName() const
{
    if (!publicConfig.isValid())
        return QString();

    return currentBearerName;
}


QString QNetworkSessionPrivate::errorString() const
{
    QString errorStr;
    switch(q->error()) {
    case QNetworkSession::RoamingError:
        errorStr = QObject::tr("Roaming error");
        break;
    case QNetworkSession::SessionAbortedError:
        errorStr = QObject::tr("Session aborted by user or system");
        break;
    default:
    case QNetworkSession::UnknownSessionError:
        errorStr = QObject::tr("Unidentified Error");
        break;
    }
    return errorStr;
}


QNetworkSession::SessionError QNetworkSessionPrivate::error() const
{
    return QNetworkSession::UnknownSessionError;
}

#include "qnetworksession_maemo.moc"
#include "moc_qnetworksession_maemo_p.cpp"

QTM_END_NAMESPACE

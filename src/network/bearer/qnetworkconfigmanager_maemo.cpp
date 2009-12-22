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

#include <string.h>

#include "qnetworkconfigmanager_maemo_p.h"

#include <QDebug>
#include <QtDBus>
#include <QHash>

#include <wlancond.h>
#include <libicd-network-wlan-dev.h>
#include <maemo_icd.h>
#include <iapconf.h>
#include <iapmonitor.h>

QTM_BEGIN_NAMESPACE

#define IAP "/system/osso/connectivity/IAP"
static int iap_prefix_len;
static void notify_iap(GConfClient *client, guint id,
		    GConfEntry *entry, gpointer user_data);


/* The IapAddTimer is a helper class that makes sure we update
 * the configuration only after all gconf additions to certain
 * iap are finished (after a certain timeout)
 */
class _IapAddTimer : public QObject
{
    Q_OBJECT

public:
    _IapAddTimer() {}
    ~_IapAddTimer()
    {
	if (timer.isActive()) {
	    QObject::disconnect(&timer, SIGNAL(timeout()), this, SLOT(timeout()));
	    timer.stop();
	}
    }

    void add(QString& iap_id, QNetworkConfigurationManagerPrivate *d);

    QString iap_id;
    QTimer timer;
    QNetworkConfigurationManagerPrivate *d;

public Q_SLOTS:
    void timeout();
};


void _IapAddTimer::add(QString& id, QNetworkConfigurationManagerPrivate *d_ptr)
{
    iap_id = id;
    d = d_ptr;

    if (timer.isActive()) {
	QObject::disconnect(&timer, SIGNAL(timeout()), this, SLOT(timeout()));
	timer.stop();
    }
    timer.setSingleShot(true);
    QObject::connect(&timer, SIGNAL(timeout()), this, SLOT(timeout()));
    timer.start(1500);
}


void _IapAddTimer::timeout()
{
    d->addConfiguration(iap_id);
}


class IapAddTimer {
    QHash<QString, _IapAddTimer* > timers;

public:
    IapAddTimer() {}
    ~IapAddTimer() {}

    void add(QString& iap_id, QNetworkConfigurationManagerPrivate *d);
    void del(QString& iap_id);
    void removeAll();
};


void IapAddTimer::removeAll()
{
    QHashIterator<QString, _IapAddTimer* > i(timers);
    while (i.hasNext()) {
	i.next();
	_IapAddTimer *t = i.value();
	delete t;
    }
    timers.clear();
}


void IapAddTimer::add(QString& iap_id, QNetworkConfigurationManagerPrivate *d)
{
    if (timers.contains(iap_id)) {
	_IapAddTimer *iap = timers.value(iap_id);
	iap->add(iap_id, d);
    } else {
	_IapAddTimer *iap = new _IapAddTimer;
	iap->add(iap_id, d);
	timers.insert(iap_id, iap);
    }
}

void IapAddTimer::del(QString& iap_id)
{
    if (timers.contains(iap_id)) {
	_IapAddTimer *iap = timers.take(iap_id);
	delete iap;
    }
}


class IapMonitor
{
public:
    IapMonitor() : first_call(true) { }
    friend void notify_iap(GConfClient *, guint,
			GConfEntry *entry, gpointer user_data);

    void setup(QNetworkConfigurationManagerPrivate *d);
    void cleanup();

private:
    bool first_call;

    void iapAdded(const char *key, GConfEntry *entry);
    void iapDeleted(const char *key, GConfEntry *entry);

    Maemo::IAPMonitor *iap;
    QNetworkConfigurationManagerPrivate *d;
    IapAddTimer timers;
};

Q_GLOBAL_STATIC(IapMonitor, iapMonitor);


/* Notify func that is called when IAP is added or deleted */
static void notify_iap(GConfClient *, guint,
		    GConfEntry *entry, gpointer user_data)
{
    const char *key = gconf_entry_get_key(entry);
    if (key && g_str_has_prefix(key, IAP)) {
	IapMonitor *ptr = (IapMonitor *)user_data;
	if (gconf_entry_get_value(entry)) {
	    ptr->iapAdded(key, entry);
	} else {
	    ptr->iapDeleted(key, entry);
	}
    }
}


void IapMonitor::setup(QNetworkConfigurationManagerPrivate *d_ptr)
{
    if (first_call) {
	d = d_ptr;
	iap_prefix_len = strlen(IAP);	
	iap = new Maemo::IAPMonitor(notify_iap, (gpointer)this);
	first_call = false;
    }
}


void IapMonitor::cleanup()
{
    if (!first_call) {
	delete iap;
	timers.removeAll();
	first_call = true;
    }
}


void IapMonitor::iapAdded(const char *key, GConfEntry * /*entry*/)
{
    //qDebug("Notify called for added element: %s=%s", gconf_entry_get_key(entry), gconf_value_to_string(gconf_entry_get_value(entry)));

    /* We cannot know when the IAP is fully added to gconf, so a timer is
     * installed instead. When the timer expires we hope that IAP is added ok.
     */
    QString iap_id = QString(key + iap_prefix_len + 1).section('/',0,0);
    timers.add(iap_id, d);
}


void IapMonitor::iapDeleted(const char *key, GConfEntry * /*entry*/)
{
    //qDebug("Notify called for deleted element: %s", gconf_entry_get_key(entry));

    /* We are only interested in IAP deletions so we skip the config entries
     */
    if (strstr(key + iap_prefix_len + 1, "/")) {	
	//qDebug("Deleting IAP config %s", key+iap_prefix_len);
	return;
    }

    QString iap_id = key + iap_prefix_len + 1;
    d->deleteConfiguration(iap_id);
}



void QNetworkConfigurationManagerPrivate::registerPlatformCapabilities()
{
    capFlags |= QNetworkConfigurationManager::CanStartAndStopInterfaces;
    capFlags |= QNetworkConfigurationManager::DataStatistics;
    capFlags |= QNetworkConfigurationManager::ForcedRoaming;
}


static inline QString network_attrs_to_security(uint network_attrs)
{
    uint cap = 0;
    nwattr2cap(network_attrs, &cap); /* from libicd-network-wlan-dev.h */
    if (cap & WLANCOND_OPEN)
	return "NONE";
    else if (cap & WLANCOND_WEP)
	return "WEP";
    else if (cap & WLANCOND_WPA_PSK)
	return "WPA_PSK";
    else if (cap & WLANCOND_WPA_EAP)
	return "WPA_EAP";
    return "";
}


struct SSIDInfo {
    QString iap_id;
    QString wlan_security;
};


void QNetworkConfigurationManagerPrivate::configurationChanged(QNetworkConfigurationPrivate *ptr)
{
    QNetworkConfiguration item;
    item.d = ptr;
    emit configurationChanged(item);
}

void QNetworkConfigurationManagerPrivate::deleteConfiguration(QString& iap_id)
{
    /* Called when IAPs are deleted in gconf, in this case we do not scan
     * or read all the IAPs from gconf because it might take too much power
     * (multiple applications would need to scan and read all IAPs from gconf)
     */
    if (accessPointConfigurations.contains(iap_id)) {
	QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv = accessPointConfigurations.take(iap_id);
	if (priv.data()) {
	    priv->isValid = false;
#ifdef BEARER_MANAGEMENT_DEBUG
	    qDebug() << "IAP" << iap_id << "was removed from storage.";
#endif

	    QNetworkConfiguration item;
	    item.d = priv;
	    emit configurationRemoved(item);
	    configChanged(priv.data(), false);
	} else
	    qWarning("Configuration not found for IAP %s", iap_id.toAscii().data());
    } else {
#ifdef BEARER_MANAGEMENT_DEBUG
	qDebug("IAP: %s, already missing from the known list", iap_id.toAscii().data());
#endif
    }
}


uint32_t QNetworkConfigurationManagerPrivate::getNetworkAttrs(bool is_iap_id,
							    QString& iap_id,
							    QString& iap_type,
							    QString security_method)
{
    guint network_attr = 0;
    dbus_uint32_t cap = 0;

    if (iap_type == "WLAN_INFRA")
	cap |= WLANCOND_INFRA;
    else if (iap_type == "WLAN_ADHOC")
	cap |= WLANCOND_ADHOC;

    if (security_method.isEmpty() && (cap & (WLANCOND_INFRA | WLANCOND_ADHOC))) {
	Maemo::IAPConf saved_ap(iap_id);
	security_method = saved_ap.value("wlan_security").toString();
    }

    if (!security_method.isEmpty()) {
	if (security_method == "WEP")
	    cap |= WLANCOND_WEP;
	else if (security_method == "WPA_PSK")
	    cap |= WLANCOND_WPA_PSK;
	else if (security_method == "WPA_EAP")
	    cap |= WLANCOND_WPA_EAP;
	else if (security_method == "NONE")
	    cap |= WLANCOND_OPEN;

	if (cap & (WLANCOND_WPA_PSK | WLANCOND_WPA_EAP)) {
	    Maemo::IAPConf saved_iap(iap_id);
	    bool wpa2_only = saved_iap.value("EAP_wpa2_only_mode").toBool();
	    if (wpa2_only) {
		cap |= WLANCOND_WPA2;
	    }
	}
    }

    cap2nwattr(cap, &network_attr);
    if (is_iap_id)
	network_attr |= ICD_NW_ATTR_IAPNAME;

    return (uint32_t)network_attr;
}


void QNetworkConfigurationManagerPrivate::addConfiguration(QString& iap_id)
{
    if (!accessPointConfigurations.contains(iap_id)) {
	Maemo::IAPConf saved_iap(iap_id);
	QString iap_type = saved_iap.value("type").toString();
	if (!iap_type.isEmpty()) {
	    QNetworkConfigurationPrivate* cpPriv = new QNetworkConfigurationPrivate();
	    cpPriv->name = saved_iap.value("name").toString();
	    if (cpPriv->name.isEmpty())
		cpPriv->name = iap_id;
	    cpPriv->isValid = true;
	    cpPriv->id = iap_id;
	    cpPriv->iap_type = iap_type;
	    cpPriv->network_attrs = getNetworkAttrs(true, iap_id, iap_type, QString());
	    cpPriv->service_id = saved_iap.value("service_id").toString();
	    cpPriv->service_type = saved_iap.value("service_type").toString();
	    if (iap_type.startsWith("WLAN")) {
		QByteArray ssid = saved_iap.value("wlan_ssid").toByteArray();
		if (ssid.isEmpty()) {
		    qWarning() << "Cannot get ssid for" << iap_id;
		}
	    }
	    cpPriv->type = QNetworkConfiguration::InternetAccessPoint;
	    cpPriv->state = QNetworkConfiguration::Defined;

	    QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> ptr(cpPriv);
	    accessPointConfigurations.insert(iap_id, ptr);

#ifdef BEARER_MANAGEMENT_DEBUG
	    qDebug("IAP: %s, name: %s, added to known list", iap_id.toAscii().data(), cpPriv->name.toAscii().data());
#endif

	    QNetworkConfiguration item;
	    item.d = ptr;
	    emit configurationAdded(item);
	    configChanged(ptr.data(), true);
	} else {
	    qWarning("IAP %s does not have \"type\" field defined, skipping this IAP.", iap_id.toAscii().data());
	}
    } else {
#ifdef BEARER_MANAGEMENT_DEBUG
	qDebug() << "IAP" << iap_id << "already in gconf.";
#endif

	/* Check if the data in gconf changed and update configuration
	 * accordingly
	 */
	QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> ptr = accessPointConfigurations.take(iap_id);
	if (ptr.data()) {
	    Maemo::IAPConf changed_iap(iap_id);
	    QString iap_type = changed_iap.value("type").toString();
	    bool update_needed = false; /* if IAP type or ssid changed, we need to change the state */

	    ptr->network_attrs = getNetworkAttrs(true, iap_id, iap_type, QString());
	    ptr->service_id = changed_iap.value("service_id").toString();
	    ptr->service_type = changed_iap.value("service_type").toString();

	    if (!iap_type.isEmpty()) {
		ptr->name = changed_iap.value("name").toString();
		if (ptr->name.isEmpty())
		    ptr->name = iap_id;
		ptr->isValid = true;
		if (ptr->iap_type != iap_type) {
		    ptr->iap_type = iap_type;
		    update_needed = true;
		}
		if (iap_type.startsWith("WLAN")) {
		    QByteArray ssid = changed_iap.value("wlan_ssid").toByteArray();
		    if (ssid.isEmpty()) {
			qWarning() << "Cannot get ssid for" << iap_id;
		    }
		    if (ptr->network_id != ssid) {
			ptr->network_id = ssid;
			update_needed = true;
		    }
		}
	    }
	    accessPointConfigurations.insert(iap_id, ptr);
	    if (update_needed) {
		ptr->type = QNetworkConfiguration::InternetAccessPoint;
		if (ptr->state != QNetworkConfiguration::Defined) {
		    ptr->state = QNetworkConfiguration::Defined;
		    configurationChanged(ptr.data());
		}
	    }
	} else {
	    qWarning("Cannot find IAP %s from current configuration although it should be there.", iap_id.toAscii().data());
	}
    }
}


void QNetworkConfigurationManagerPrivate::updateConfigurations()
{
    /* Contains known network id (like ssid) from storage */
    QMultiHash<QByteArray, SSIDInfo* > knownConfigs;

    /* All the scanned access points */
    QList<Maemo::IcdScanResult> scanned;

    /* Turn on IAP monitoring */
    iapMonitor()->setup(this);

    if (firstUpdate) {
	/* We create a default configuration which is a pseudo config */
	QNetworkConfigurationPrivate* cpPriv = new QNetworkConfigurationPrivate();
	cpPriv->name = "UserChoice";
	cpPriv->state = QNetworkConfiguration::Discovered;
	cpPriv->isValid = true;
	cpPriv->id = OSSO_IAP_ANY;
	cpPriv->type = QNetworkConfiguration::UserChoice;
	cpPriv->purpose = QNetworkConfiguration::UnknownPurpose;
	cpPriv->roamingSupported = false;
	cpPriv->manager = this;
	QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> ptr(cpPriv);
	userChoiceConfigurations.insert(cpPriv->id, ptr);
    }

    /* We return currently configured IAPs in the first run and do the WLAN
     * scan in subsequent runs.
     */
    QList<QString> all_iaps;
    Maemo::IAPConf::getAll(all_iaps);

    foreach (QString escaped_iap_id, all_iaps) {
	QByteArray ssid;

	/* The key that is returned by getAll() needs to be unescaped */
	gchar *unescaped_id = gconf_unescape_key(escaped_iap_id.toUtf8().data(), -1);
	QString iap_id = QString((char *)unescaped_id);
	g_free(unescaped_id);

	Maemo::IAPConf saved_ap(iap_id);
	bool is_temporary = saved_ap.value("temporary").toBool();
	if (is_temporary) {
#ifdef BEARER_MANAGEMENT_DEBUG
	    qDebug() << "IAP" << iap_id << "is temporary, skipping it.";
#endif
	    continue;
	}

	QString iap_type = saved_ap.value("type").toString();
	if (iap_type.startsWith("WLAN")) {
	    ssid = saved_ap.value("wlan_ssid").toByteArray();
	    if (ssid.isEmpty()) {
		qWarning() << "Cannot get ssid for" << iap_id;
		continue;
	    }

	    QString security_method = saved_ap.value("wlan_security").toString();
	    SSIDInfo *info = new SSIDInfo;
	    info->iap_id = iap_id;
	    info->wlan_security = security_method;
	    knownConfigs.insert(ssid, info);
	} else if (iap_type.isEmpty()) {
	    qWarning() << "IAP" << iap_id << "network type is not set! Skipping it";
	    continue;
	} else {
#ifdef BEARER_MANAGEMENT_DEBUG
	    qDebug() << "IAP" << iap_id << "network type is" << iap_type;
#endif
	    ssid.clear();
	}

	if (!accessPointConfigurations.contains(iap_id)) {
	    QNetworkConfigurationPrivate* cpPriv = new QNetworkConfigurationPrivate();
	    //cpPriv->name = iap_info.value().toString();
	    cpPriv->name = saved_ap.value("name").toString();
	    if (cpPriv->name.isEmpty())
		if (!ssid.isEmpty() && ssid.size() > 0)
		    cpPriv->name = ssid.data();
		else
		    cpPriv->name = iap_id;
	    cpPriv->isValid = true;
	    cpPriv->id = iap_id;
	    cpPriv->network_id = ssid;
	    cpPriv->network_attrs = getNetworkAttrs(true, iap_id, iap_type, QString());
	    cpPriv->iap_type = iap_type;
	    cpPriv->service_id = saved_ap.value("service_id").toString();
	    cpPriv->service_type = saved_ap.value("service_type").toString();
	    cpPriv->type = QNetworkConfiguration::InternetAccessPoint;
	    cpPriv->state = QNetworkConfiguration::Defined;
	    cpPriv->manager = this;

	    QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> ptr(cpPriv);
	    accessPointConfigurations.insert(iap_id, ptr);

#ifdef BEARER_MANAGEMENT_DEBUG
	    qDebug("IAP: %s, name: %s, ssid: %s, added to known list", iap_id.toAscii().data(), cpPriv->name.toAscii().data(), !ssid.isEmpty() ? ssid.data() : "-");
#endif
	} else {
#ifdef BEARER_MANAGEMENT_DEBUG
	    qDebug("IAP: %s, ssid: %s, already exists in the known list", iap_id.toAscii().data(), !ssid.isEmpty() ? ssid.data() : "-");
#endif
	}
    }

    if (!firstUpdate) {
	QStringList scannedNetworkTypes;
	QStringList networkTypesToScan;
	QString error;
	Maemo::Icd icd(ICD_SHORT_SCAN_TIMEOUT);

	scannedNetworkTypes = icd.scan(ICD_SCAN_REQUEST_ACTIVE,
				    networkTypesToScan,
				    scanned,
				    error);
	if (!error.isEmpty()) {
	    qWarning() << "Network scanning failed" << error;
	} else {
#ifdef BEARER_MANAGEMENT_DEBUG
	    if (!scanned.isEmpty())
		qDebug() << "Scan returned" << scanned.size() << "networks";
	    else
		qDebug() << "Scan returned nothing.";
#endif
	}
    }


    /* This is skipped in the first update as scanned size is zero */
    if (!scanned.isEmpty())
      for (int i=0; i<scanned.size(); ++i) {
	const Maemo::IcdScanResult ap = scanned.at(i); 

	if (ap.scan.network_attrs & ICD_NW_ATTR_IAPNAME) {
	    /* The network_id is IAP id, so the IAP is a known one */
	    QString iapid = ap.scan.network_id.data();
	    QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv = accessPointConfigurations.take(iapid);
	    if (priv.data()) {
		priv->state = QNetworkConfiguration::Discovered; /* Defined is set automagically */
		priv->network_attrs = ap.scan.network_attrs;
		priv->service_id = ap.scan.service_id;
		priv->service_type = ap.scan.service_type;
		priv->service_attrs = ap.scan.service_attrs;

		configurationChanged(priv.data());
		accessPointConfigurations.insert(iapid, priv);
#ifdef BEARER_MANAGEMENT_DEBUG
		qDebug("IAP: %s, ssid: %s, discovered", iapid.toAscii().data(), priv->network_id.data());
#endif

		if (!ap.scan.network_type.startsWith("WLAN"))
		    continue; // not a wlan AP

		/* Remove scanned AP from known configurations so that we can
		 * emit configurationRemoved signal later
		 */
		QList<SSIDInfo* > known_iaps = knownConfigs.values(priv->network_id);
	    rescan_list:
		if (!known_iaps.isEmpty()) {
		    for (int k=0; k<known_iaps.size(); ++k) {
			SSIDInfo *iap = known_iaps.at(k);

#ifdef BEARER_MANAGEMENT_DEBUG
			//qDebug() << "iap" << iap->iap_id << "security" << iap->wlan_security << "scan" << network_attrs_to_security(ap.scan.network_attrs);
#endif

			if (iap->wlan_security == 
			    network_attrs_to_security(ap.scan.network_attrs)) {
			    /* Remove IAP from the list */
			    knownConfigs.remove(priv->network_id, iap);
#ifdef BEARER_MANAGEMENT_DEBUG
			    qDebug() << "Removed IAP" << iap->iap_id << "from unknown config";
#endif
			    known_iaps.removeAt(k);
			    delete iap;
			    goto rescan_list;
			}
		    }
		}
	    } else {
		qWarning() << "IAP" << iapid << "is missing in configuration.";
	    }

	} else {
	    /* Non saved access point data */
	    QByteArray scanned_ssid = ap.scan.network_id;
	    QNetworkConfigurationPrivate* cpPriv = new QNetworkConfigurationPrivate();
	    QString hrs = scanned_ssid.data();

	    cpPriv->name = ap.network_name.isEmpty() ? hrs : ap.network_name;
	    cpPriv->isValid = true;
	    cpPriv->id = scanned_ssid.data();  // Note: id is now ssid, it should be set to IAP id if the IAP is saved
	    cpPriv->network_id = scanned_ssid;
	    cpPriv->iap_type = ap.scan.network_type;
	    cpPriv->network_attrs = ap.scan.network_attrs;
	    cpPriv->service_id = ap.scan.service_id;
	    cpPriv->service_type = ap.scan.service_type;
	    cpPriv->service_attrs = ap.scan.service_attrs;
	    cpPriv->manager = this;

	    cpPriv->type = QNetworkConfiguration::InternetAccessPoint;
	    cpPriv->state = QNetworkConfiguration::Undefined;

	    QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> ptr(cpPriv);
	    accessPointConfigurations.insert(cpPriv->id, ptr);

#ifdef BEARER_MANAGEMENT_DEBUG
	    qDebug() << "IAP with network id" << cpPriv->id << "was found in the scan.";
#endif

	    QNetworkConfiguration item;
	    item.d = ptr;
	    emit configurationAdded(item);
	}
      }


    /* Remove non existing iaps since last update */
    if (!firstUpdate) {
	QHashIterator<QByteArray, SSIDInfo* > i(knownConfigs);
	while (i.hasNext()) {
	    i.next();
	    SSIDInfo *iap = i.value();
	    QString iap_id = iap->iap_id;
	    //qDebug() << i.key() << ": " << iap_id;

	    QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv = accessPointConfigurations.take(iap_id);
	    if (priv.data()) {
		priv->isValid = false;
#ifdef BEARER_MANAGEMENT_DEBUG
		qDebug() << "IAP" << iap_id << "was removed as it was not found in scan.";
#endif

		QNetworkConfiguration item;
		item.d = priv;
		emit configurationRemoved(item);
		configChanged(priv.data(), false);

		//if we would have SNAP support we would have to remove the references
		//from existing ServiceNetworks to the removed access point configuration
	    }
	}
    }


    QMutableHashIterator<QByteArray, SSIDInfo* > i(knownConfigs);
    while (i.hasNext()) {
	i.next();
	SSIDInfo *iap = i.value();
	delete iap;
	i.remove();
    }

    if (!firstUpdate)
	emit configurationUpdateComplete();

    if (firstUpdate)
        firstUpdate = false;
}


QNetworkConfiguration QNetworkConfigurationManagerPrivate::defaultConfiguration()
{
    /* Here we just return [ANY] request to icd and let the icd decide which
     * IAP to connect.
     */
    QNetworkConfiguration item;
    if (userChoiceConfigurations.contains(OSSO_IAP_ANY))
        item.d = userChoiceConfigurations.value(OSSO_IAP_ANY);
    return item;
}


void QNetworkConfigurationManagerPrivate::performAsyncConfigurationUpdate()
{
    QTimer::singleShot(0, this, SLOT(updateConfigurations()));
}


void QNetworkConfigurationManagerPrivate::cleanup()
{
    iapMonitor()->cleanup();
}


void QNetworkConfigurationManagerPrivate::configChanged(QNetworkConfigurationPrivate *ptr, bool added)
{
    if (added) {
	if (ptr && ptr->state == QNetworkConfiguration::Active) {
	    onlineConfigurations++;
	    if (!firstUpdate && onlineConfigurations == 1)
		emit onlineStateChanged(true);
	}
    } else {
	if (ptr && ptr->state == QNetworkConfiguration::Active) {
	    onlineConfigurations--;
	    if (!firstUpdate && onlineConfigurations == 0)
		emit onlineStateChanged(false);
	    if (onlineConfigurations < 0)
		onlineConfigurations = 0;
	}
    }
}


#include "qnetworkconfigmanager_maemo.moc"
#include "moc_qnetworkconfigmanager_maemo_p.cpp"

QTM_END_NAMESPACE

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

#include "qicdengine.h"
#include "monitor.h"
#include "qnetworksession_impl.h"

#include <libicd-network-wlan-dev.h>
#include <maemo_icd.h>
#include <iapconf.h>

QT_BEGIN_NAMESPACE

IcdNetworkConfigurationPrivate::IcdNetworkConfigurationPrivate()
:   network_attrs(0), service_attrs(0)
{
}

IcdNetworkConfigurationPrivate::~IcdNetworkConfigurationPrivate()
{
}

QString IcdNetworkConfigurationPrivate::bearerName() const
{
    if (iap_type == QLatin1String("WLAN_INFRA") ||
        iap_type == QLatin1String("WLAN_ADHOC")) {
        return QLatin1String("WLAN");
    } else if (iap_type == QLatin1String("GPRS")) {
        return QLatin1String("HSPA");
    } else {
        return iap_type;
    }
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

QIcdEngine::QIcdEngine(QObject *parent)
:   QBearerEngine(parent), iapMonitor(new IapMonitor)
{
    /* Turn on IAP monitoring */
    iapMonitor->setup(this);

    doRequestUpdate();
}

QIcdEngine::~QIcdEngine()
{
}

bool QIcdEngine::hasIdentifier(const QString &id)
{
    QMutexLocker locker(&mutex);

    return accessPointConfigurations.contains(id) ||
           snapConfigurations.contains(id) ||
           userChoiceConfigurations.contains(id);
}

void QIcdEngine::requestUpdate()
{
    QMutexLocker locker(&mutex);

    QTimer::singleShot(0, this, SLOT(doRequestUpdate()));
}

static uint32_t getNetworkAttrs(bool is_iap_id,
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

void QIcdEngine::doRequestUpdate()
{
    QMutexLocker locker(&mutex);

    QStringList previous = accessPointConfigurations.keys();

    /* All the scanned access points */
    QList<Maemo::IcdScanResult> scanned;

    /* We create a default configuration which is a pseudo config */
    if (!userChoiceConfigurations.contains(OSSO_IAP_ANY)) {
        QNetworkConfigurationPrivatePointer ptr(new IcdNetworkConfigurationPrivate);

        ptr->name = QLatin1String("UserChoice");
        ptr->state = QNetworkConfiguration::Discovered;
        ptr->isValid = true;
        ptr->id = OSSO_IAP_ANY;
        ptr->type = QNetworkConfiguration::UserChoice;
        ptr->purpose = QNetworkConfiguration::UnknownPurpose;
        ptr->roamingSupported = false;

        userChoiceConfigurations.insert(ptr->id, ptr);

        locker.unlock();
        emit configurationAdded(ptr);
        locker.relock();
    }

    /* We return currently configured IAPs in the first run and do the WLAN
     * scan in subsequent runs.
     */
    QList<QString> all_iaps;
    Maemo::IAPConf::getAll(all_iaps);

    foreach (QString iap_id, all_iaps) {
        QByteArray ssid;

        previous.removeAll(iap_id);

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
            if (ssid.isEmpty())
                continue;

            QString security_method = saved_ap.value("wlan_security").toString();
        } else if (iap_type.isEmpty()) {
            continue;
        } else {
#ifdef BEARER_MANAGEMENT_DEBUG
            qDebug() << "IAP" << iap_id << "network type is" << iap_type;
#endif
            ssid.clear();
        }

        if (!accessPointConfigurations.contains(iap_id)) {
            IcdNetworkConfigurationPrivate *cpPriv = new IcdNetworkConfigurationPrivate;

            cpPriv->name = saved_ap.value("name").toString();
            if (cpPriv->name.isEmpty()) {
                    if (!ssid.isEmpty() && ssid.size() > 0)
                        cpPriv->name = ssid.data();
                    else
                        cpPriv->name = iap_id;
            }
            cpPriv->isValid = true;
            cpPriv->id = iap_id;
            cpPriv->network_id = ssid;
            cpPriv->network_attrs = getNetworkAttrs(true, iap_id, iap_type, QString());
            cpPriv->iap_type = iap_type;
            cpPriv->service_id = saved_ap.value("service_id").toString();
            cpPriv->service_type = saved_ap.value("service_type").toString();
            cpPriv->type = QNetworkConfiguration::InternetAccessPoint;
            cpPriv->state = QNetworkConfiguration::Defined;

            QNetworkConfigurationPrivatePointer ptr(cpPriv);
            accessPointConfigurations.insert(iap_id, ptr);

            locker.unlock();
            emit configurationAdded(ptr);
            locker.relock();

#ifdef BEARER_MANAGEMENT_DEBUG
            qDebug("IAP: %s, name: %s, ssid: %s, added to known list",
                   iap_id.toAscii().data(), ptr->name.toAscii().data(),
                   !ssid.isEmpty() ? ssid.data() : "-");
#endif
        } else {
#ifdef BEARER_MANAGEMENT_DEBUG
            qDebug("IAP: %s, ssid: %s, already exists in the known list",
                   iap_id.toAscii().data(), !ssid.isEmpty() ? ssid.data() : "-");
#endif
        }
    }

    if (sender()) {
        QStringList scannedNetworkTypes;
        QStringList networkTypesToScan;
        QString error;
        Maemo::Icd icd(ICD_SHORT_SCAN_TIMEOUT);

        scannedNetworkTypes = icd.scan(ICD_SCAN_REQUEST_ACTIVE,
                                       networkTypesToScan,
                                       scanned,
                                       error);
        if (!error.isEmpty()) {
#ifdef BEARER_MANAGEMENT_DEBUG
            qDebug() << "Network scanning failed" << error;
#endif
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
    if (!scanned.isEmpty()) {
        for (int i=0; i<scanned.size(); ++i) {
            const Maemo::IcdScanResult ap = scanned.at(i);

            QByteArray scanned_ssid = ap.scan.network_id;

            if (ap.scan.network_attrs & ICD_NW_ATTR_IAPNAME) {
                /* The network_id is IAP id, so the IAP is a known one */
                QString iapid = ap.scan.network_id.data();

                previous.removeAll(iapid);

                if (accessPointConfigurations.contains(iapid)) {
                    QNetworkConfigurationPrivatePointer ptr =
                        accessPointConfigurations.value(iapid);

                    bool changed = false;

                    ptr->mutex.lock();

                    if (!ptr->isValid) {
                        ptr->isValid = true;
                        changed = true;
                    }

                    if (ptr->state != QNetworkConfiguration::Discovered) {
                        ptr->state = QNetworkConfiguration::Discovered;
                        changed = true;
                    }

                    toIcdConfig(ptr)->network_attrs = ap.scan.network_attrs;
                    toIcdConfig(ptr)->service_id = ap.scan.service_id;
                    toIcdConfig(ptr)->service_type = ap.scan.service_type;
                    toIcdConfig(ptr)->service_attrs = ap.scan.service_attrs;

#ifdef BEARER_MANAGEMENT_DEBUG
                    qDebug("IAP: %s, ssid: %s, discovered",
                           iapid.toAscii().data(), scanned_ssid.data());
#endif

                    ptr->mutex.unlock();

                    if (changed) {
                        locker.unlock();
                        emit configurationChanged(ptr);
                        locker.relock();
                    }

                    if (!ap.scan.network_type.startsWith("WLAN"))
                        continue; // not a wlan AP
                }
            } else {
                IcdNetworkConfigurationPrivate *cpPriv = new IcdNetworkConfigurationPrivate;

                QString hrs = scanned_ssid.data();

                cpPriv->name = ap.network_name.isEmpty() ? hrs : ap.network_name;
                cpPriv->isValid = true;
                // Note: id is now ssid, it should be set to IAP id if the IAP is saved
                cpPriv->id = scanned_ssid.data();
                cpPriv->network_id = scanned_ssid;
                cpPriv->iap_type = ap.scan.network_type;
                if (cpPriv->iap_type.isEmpty())
                    cpPriv->iap_type = QLatin1String("WLAN");
                cpPriv->network_attrs = ap.scan.network_attrs;
                cpPriv->service_id = ap.scan.service_id;
                cpPriv->service_type = ap.scan.service_type;
                cpPriv->service_attrs = ap.scan.service_attrs;

                cpPriv->type = QNetworkConfiguration::InternetAccessPoint;
                cpPriv->state = QNetworkConfiguration::Undefined;

#ifdef BEARER_MANAGEMENT_DEBUG
                qDebug() << "IAP with network id" << cpPriv->id << "was found in the scan.";
#endif

                previous.removeAll(cpPriv->id);

                QNetworkConfigurationPrivatePointer ptr(cpPriv);
                accessPointConfigurations.insert(ptr->id, ptr);

                locker.unlock();
                emit configurationAdded(ptr);
                locker.relock();
            }
        }
    }

    while (!previous.isEmpty()) {
        QNetworkConfigurationPrivatePointer ptr =
            accessPointConfigurations.take(previous.takeFirst());

        locker.unlock();
        emit configurationRemoved(ptr);
        locker.relock();
    }

    if (sender()) {
        locker.unlock();
        emit updateCompleted();
    }
}

void QIcdEngine::deleteConfiguration(const QString &iap_id)
{
    QMutexLocker locker(&mutex);

    /* Called when IAPs are deleted in db, in this case we do not scan
     * or read all the IAPs from db because it might take too much power
     * (multiple applications would need to scan and read all IAPs from db)
     */
    QNetworkConfigurationPrivatePointer ptr = accessPointConfigurations.take(iap_id);
    if (ptr) {
#ifdef BEARER_MANAGEMENT_DEBUG
        qDebug() << "IAP" << iap_id << "was removed from storage.";
#endif

        locker.unlock();
        emit configurationRemoved(ptr);
    } else {
#ifdef BEARER_MANAGEMENT_DEBUG
        qDebug("IAP: %s, already missing from the known list", iap_id.toAscii().data());
#endif
    }
}

QNetworkConfigurationManager::Capabilities QIcdEngine::capabilities() const
{
    return QNetworkConfigurationManager::CanStartAndStopInterfaces |
           QNetworkConfigurationManager::DataStatistics |
           QNetworkConfigurationManager::ForcedRoaming;
}

QNetworkSessionPrivate *QIcdEngine::createSessionBackend()
{
    return new QNetworkSessionPrivateImpl(this);
}

QNetworkConfigurationPrivatePointer QIcdEngine::defaultConfiguration()
{
    QMutexLocker locker(&mutex);

    // Here we just return [ANY] request to icd and let the icd decide which IAP to connect.
    return userChoiceConfigurations.value(OSSO_IAP_ANY);
}

QT_END_NAMESPACE

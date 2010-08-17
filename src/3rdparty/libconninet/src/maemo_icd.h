/*
  libconninet - Internet Connectivity support library
  
  Copyright (C) 2009 Nokia Corporation. All rights reserved.

  Contact: Jukka Rissanen <jukka.rissanen@nokia.com>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public License
  version 2.1 as published by the Free Software Foundation.

  This library is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
  02110-1301 USA
*/

#ifndef MAEMO_ICD_H
#define MAEMO_ICD_H

#include <QObject>
#include <QStringList>
#include <QByteArray>
#include <QMetaType>
#include <QtDBus>
#include <QDBusArgument>

#include <glib.h>
#include <icd/dbus_api.h>
#include <icd/osso-ic.h>
#include <icd/osso-ic-dbus.h>

#include "dbusdispatcher.h"
#include <icd/network_api_defines.h>

#define ICD_LONG_SCAN_TIMEOUT (30*1000)  /* 30sec */
#define ICD_SHORT_SCAN_TIMEOUT (10*1000)  /* 10sec */
#define ICD_SHORT_CONNECT_TIMEOUT (10*1000) /* 10sec */
#define ICD_LONG_CONNECT_TIMEOUT (150*1000) /* 2.5min */

namespace Maemo {

struct CommonParams {
	QString service_type;
	uint service_attrs;
	QString service_id;
	QString network_type;
	uint network_attrs;
	QByteArray network_id;
};

struct ConnectParams {
	struct CommonParams connect;
};

struct IcdScanResult {
	uint status; // see #icd_scan_status
	uint timestamp; // when last seen
	QString service_name;
	uint service_priority; // within a service type
	QString network_name;
	uint network_priority;
	struct CommonParams scan;
	uint signal_strength; // quality, 0 (none) - 10 (good)
	QString station_id; // e.g. MAC address or similar id
	uint signal_dB; // use signal strength above unless you know what you are doing

	IcdScanResult() {
		status = timestamp = scan.service_attrs = service_priority =
			scan.network_attrs = network_priority = signal_strength =
			signal_dB = 0;
	}
};

struct IcdConnectResult {
	struct CommonParams connect;
	uint status;
};

struct IcdStateResult {
	struct CommonParams params;
	QString error;
	uint state;
};

struct IcdStatisticsResult {
	struct CommonParams params;
	uint time_active;  // in seconds
	enum icd_nw_levels signal_strength; // see network_api_defines.h in icd2-dev package
	uint bytes_sent;
	uint bytes_received;
};

struct IcdIPInformation {
	QString address;
	QString netmask;
	QString default_gateway;
	QString dns1;
	QString dns2;
	QString dns3;
};

struct IcdAddressInfoResult {
	struct CommonParams params;
	QList<IcdIPInformation> ip_info;
};

enum IcdDbusInterfaceVer {
	IcdOldDbusInterface = 0,  // use the old OSSO-IC interface
	IcdNewDbusInterface       // use the new Icd2 interface (default)
};


class IcdPrivate;
class Icd : public QObject
{
    Q_OBJECT

public:
    Icd(QObject *parent = 0);
    Icd(unsigned int timeout, QObject *parent = 0);
    Icd(unsigned int timeout, IcdDbusInterfaceVer ver, QObject *parent = 0);
    ~Icd();
    QString error();  // returns last error string

    /* Icd2 dbus API functions */
    QStringList scan(icd_scan_request_flags flags,
		     QStringList &network_types,
		     QList<IcdScanResult>& scan_results,
		     QString& error);
    void scanCancel();
    bool connect(icd_connection_flags flags, IcdConnectResult& result);
    bool connect(icd_connection_flags flags, QList<ConnectParams>& params,
		 IcdConnectResult& result);
    bool connect(icd_connection_flags flags, QString& iap, QString& result);
    void select(uint flags);
    void disconnect(uint connect_flags, QString& service_type,
		    uint service_attrs, QString& service_id,
		    QString& network_type, uint network_attrs,
		    QByteArray& network_id);
    void disconnect(uint connect_flags);

    uint state(QString& service_type, uint service_attrs,
	       QString& service_id, QString& network_type,
	       uint network_attrs, QByteArray& network_id,
	       IcdStateResult &state_result);
    uint statistics(QString& service_type, uint service_attrs,
		    QString& service_id, QString& network_type,
		    uint network_attrs, QByteArray& network_id,
		    IcdStatisticsResult& stats_result);
    uint addrinfo(QString& service_type, uint service_attrs,
		  QString& service_id, QString& network_type,
		  uint network_attrs, QByteArray& network_id,
		  IcdAddressInfoResult& addr_result);

    uint state(QList<IcdStateResult>& state_results);
    uint state_non_blocking(QList<IcdStateResult>& state_results);
    uint statistics(QList<IcdStatisticsResult>& stats_results);
    uint addrinfo(QList<IcdAddressInfoResult>& addr_results);
    uint addrinfo_non_blocking(QList<IcdAddressInfoResult>& addr_results);

private Q_SLOTS:
    void icdSignalReceived(const QString& interface, 
                        const QString& signal,
                        const QList<QVariant>& args);
    void icdCallReply(const QString& method, 
                   const QList<QVariant>& args,
                   const QString& error);

private:
    IcdPrivate *d;
    friend class IcdPrivate;
};

}  // Maemo namespace

#endif

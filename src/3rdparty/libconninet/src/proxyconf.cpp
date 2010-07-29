/*
  libconninet - Internet Connectivity support library

  Copyright (C) 2010 Nokia Corporation. All rights reserved.

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

#include <QDebug>
#include <QWriteLocker>
#include <QNetworkProxyFactory>
#include <QNetworkProxy>
#include <GConfItem>
#include <gconf/gconf-client.h>
#include "proxyconf.h"

#define CONF_PROXY "/system/proxy"
#define HTTP_PROXY "/system/http_proxy"


namespace Maemo {

class NetworkProxyFactory : QNetworkProxyFactory {
public:
    NetworkProxyFactory() { }
    QList<QNetworkProxy> queryProxy(const QNetworkProxyQuery &query = QNetworkProxyQuery());
};


QList<QNetworkProxy> NetworkProxyFactory::queryProxy(const QNetworkProxyQuery &query)
{
    ProxyConf proxy_conf;

    QList<QNetworkProxy> result = proxy_conf.flush(query);
    if (result.isEmpty())
        result << QNetworkProxy::NoProxy;

    return result;
}


class ProxyConfPrivate {
private:
    // proxy values from gconf
    QString mode;
    bool use_http_host;
    QString autoconfig_url;
    QString http_proxy;
    quint16 http_port;
    QList<QVariant> ignore_hosts;
    QString secure_host;
    quint16 secure_port;
    QString ftp_host;
    quint16 ftp_port;
    QString socks_host;
    quint16 socks_port;
    QString rtsp_host;
    quint16 rtsp_port;

    QVariant getValue(QString& name);
    QVariant getHttpValue(QString& name);
    QVariant getValue(const char *name);
    QVariant getHttpValue(const char *name);
    bool isHostExcluded(const QString &host);

public:
    QString prefix;
    QString http_prefix;

    void readProxyData();
    QList<QNetworkProxy> flush(const QNetworkProxyQuery &query);
};


QVariant ProxyConfPrivate::getValue(QString& name)
{
    GConfItem item(prefix + name);
    return item.value();
}

QVariant ProxyConfPrivate::getHttpValue(QString& name)
{
    GConfItem item(http_prefix + name);
    return item.value();
}


QVariant ProxyConfPrivate::getValue(const char *name)
{
    QString n = QString(name);
    return getValue(n);
}

QVariant ProxyConfPrivate::getHttpValue(const char *name)
{
    QString n = QString(name);
    return getHttpValue(n);
}


#define GET(var, type)				\
  do {						\
    var = getValue(#var).to##type ();		\
    /*qDebug()<<"var="<<var;*/			\
  } while(0)

#define GET2(var, name, type)			\
  do {						\
    var = getHttpValue(#name).to##type ();	\
    /*qDebug()<<"var="<<var;*/			\
  } while(0)


void ProxyConfPrivate::readProxyData()
{
    /* Read the proxy settings from /system/proxy* */
    GET2(http_proxy, host, String);
    GET2(http_port, port, Int);
    GET2(ignore_hosts, ignore_hosts, List);

    GET(mode, String);
    GET(autoconfig_url, String);
    GET(secure_host, String);
    GET(secure_port, Int);
    GET(ftp_host, String);
    GET(ftp_port, Int);
    GET(socks_host, String);
    GET(socks_port, Int);
    GET(rtsp_host, String);
    GET(rtsp_port, Int);

    if (http_proxy.isEmpty())
        use_http_host = false;
    else
        use_http_host = true;
}


bool ProxyConfPrivate::isHostExcluded(const QString &host)
{
    if (host.isEmpty())
        return true;

    if (ignore_hosts.isEmpty())
        return false;

    QHostAddress ipAddress;
    bool isIpAddress = ipAddress.setAddress(host);

    foreach (QVariant h, ignore_hosts) {
        QString entry = h.toString();
        if (isIpAddress && ipAddress.isInSubnet(QHostAddress::parseSubnet(entry))) {
            return true;  // excluded
        } else {
            // do wildcard matching
            QRegExp rx(entry, Qt::CaseInsensitive, QRegExp::Wildcard);
            if (rx.exactMatch(host))
                return true;
        }
    }

    // host was not excluded
    return false;
}


QList<QNetworkProxy> ProxyConfPrivate::flush(const QNetworkProxyQuery &query)
{
    QList<QNetworkProxy> result;

#if 0
    qDebug()<<"http_proxy" << http_proxy;
    qDebug()<<"http_port" << http_port;
    qDebug()<<"ignore_hosts" << ignore_hosts;
    qDebug()<<"use_http_host" << use_http_host;
    qDebug()<<"mode" << mode;
    qDebug()<<"autoconfig_url" << autoconfig_url;
    qDebug()<<"secure_host" << secure_host;
    qDebug()<<"secure_port" << secure_port;
    qDebug()<<"ftp_host" << ftp_host;
    qDebug()<<"ftp_port" << ftp_port;
    qDebug()<<"socks_host" << socks_host;
    qDebug()<<"socks_port" << socks_port;
    qDebug()<<"rtsp_host" << rtsp_host;
    qDebug()<<"rtsp_port" << rtsp_port;
#endif

    if (isHostExcluded(query.peerHostName()))
        return result;          // no proxy for this host

    if (mode == "auto") {
        // TODO: pac currently not supported, fix me
        return result;
    }

    if (mode == "manual") {
        bool isHttps = false;
	QString protocol = query.protocolTag().toLower();

	// try the protocol-specific proxy
	QNetworkProxy protocolSpecificProxy;

	if (protocol == QLatin1String("ftp")) {
	    if (!ftp_host.isEmpty()) {
	        protocolSpecificProxy.setType(QNetworkProxy::FtpCachingProxy);
		protocolSpecificProxy.setHostName(ftp_host);
		protocolSpecificProxy.setPort(ftp_port);
	    }
	} else if (protocol == QLatin1String("http")) {
	    if (!http_proxy.isEmpty()) {
	        protocolSpecificProxy.setType(QNetworkProxy::HttpProxy);
		protocolSpecificProxy.setHostName(http_proxy);
		protocolSpecificProxy.setPort(http_port);
	    }
	} else if (protocol == QLatin1String("https")) {
	    isHttps = true;
	    if (!secure_host.isEmpty()) {
	        protocolSpecificProxy.setType(QNetworkProxy::HttpProxy);
		protocolSpecificProxy.setHostName(secure_host);
		protocolSpecificProxy.setPort(secure_port);
	    }
	}

	if (protocolSpecificProxy.type() != QNetworkProxy::DefaultProxy)
	    result << protocolSpecificProxy;


        if (!socks_host.isEmpty()) {
	    QNetworkProxy proxy;
	    proxy.setType(QNetworkProxy::Socks5Proxy);
	    proxy.setHostName(socks_host);
	    proxy.setPort(socks_port);
	    result << proxy;
	}


	// Add the HTTPS proxy if present (and if we haven't added yet)
	if (!isHttps) {
	    QNetworkProxy https;
	    if (!secure_host.isEmpty()) {
	        https.setType(QNetworkProxy::HttpProxy);
		https.setHostName(secure_host);
		https.setPort(secure_port);
	    }

	    if (https.type() != QNetworkProxy::DefaultProxy &&
		https != protocolSpecificProxy)
	        result << https;
	}
    }

    return result;
}


ProxyConf::ProxyConf()
    : d_ptr(new ProxyConfPrivate)
{
    g_type_init();
    d_ptr->prefix = CONF_PROXY "/";
    d_ptr->http_prefix = HTTP_PROXY "/";
}

ProxyConf::~ProxyConf()
{
    delete d_ptr;
}


QList<QNetworkProxy> ProxyConf::flush(const QNetworkProxyQuery &query)
{
    d_ptr->readProxyData();
    return d_ptr->flush(query);
}


static int refcount = 0;
static QReadWriteLock lock;

void ProxyConf::update()
{
    QWriteLocker locker(&lock);
    NetworkProxyFactory *factory = new NetworkProxyFactory();
    QNetworkProxyFactory::setApplicationProxyFactory((QNetworkProxyFactory*)factory);
    refcount++;
}


void ProxyConf::clear(void)
{
    QWriteLocker locker(&lock);
    refcount--;
    if (refcount == 0)
        QNetworkProxyFactory::setApplicationProxyFactory(NULL);

    if (refcount<0)
        refcount = 0;
}


} // namespace Maemo

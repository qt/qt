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


#include <QtTest/QtTest>
#include <QDebug>
#include <QNetworkProxy>
#include <conn_settings.h>

#include "../src/proxyconf.h"

class Ut_ProxyConf : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void init();
    void cleanup();
    void initTestCase();
    void cleanupTestCase();

    // tests without the factory
    void proxy_ftp_no_factory_ok_auto();
    void proxy_ftp_no_factory_ok_manual();
    void proxy_http_no_factory_ok_manual();
    void proxy_https_no_factory_ok_manual();
    void proxy_socks_no_factory_ok_manual();
    void proxy_default_no_factory_ok_manual();

    // tests using the factory
    void proxy_ftp_factory_ok_auto();
    void proxy_ftp_factory_ok_manual();
    void proxy_http_factory_ok_manual();
    void proxy_https_factory_ok_manual();
    void proxy_socks_factory_ok_manual();
    void proxy_http_factory_ok_manual_clear();
    void proxy_default_factory_ok_manual();
    void proxy_http_factory_ok_manual_ignore_list();
    void proxy_default_factory_ok_manual_system();

private:
    Maemo::ProxyConf *pc;
};


void put(QString var, QString type, QString value)
{
    QProcess gconf;
    if (value.isEmpty())
        gconf.start(QString("gconftool-2 -u /system/proxy/"+var));
    else
        gconf.start(QString("gconftool-2 -s /system/proxy/"+var+" -t "+type+" "+value));
    gconf.waitForFinished();
}

void put_http(QString var, QString type, QString value)
{
    QProcess gconf;
    if (value.isEmpty())
        gconf.start(QString("gconftool-2 -u /system/http_proxy/"+var));
    else
        gconf.start(QString("gconftool-2 -s /system/http_proxy/"+var+" -t "+type+" "+value));
    gconf.waitForFinished();
}

void put_list(QString var, QString type, QList<QString> value)
{
    QProcess gconf;
    QString values = "[";
    foreach (QString str, value)
        values = values + str + ",";
    values.chop(1);
    values = values + "]";

    gconf.start(QString("gconftool-2 -s /system/http_proxy/"+var+" -t list --list-type="+type+" "+values));
    gconf.waitForFinished();
}


void Ut_ProxyConf::init()
{
    put_http("host", "string", "my.proxy.com");
    put_http("port", "int", "8080");

    QList<QString> list;
    list.append("foo.bar.com");
    list.append("192.168.19.69");
    list.append("192.168.20.0/24");
    list.append("bar.foo.com");
    put_list("ignore_hosts", "string", list);
    put_http("use_http_host", "boolean", "true");

    put("mode", "string", "auto");
    put("autoconfig_url", "string", "http://foo.bar.com/autoconf");
    put("secure_host", "string", "secure_host.com");
    put("secure_port", "int", "112");

    put("ftp_host", "string", "ftp.nuukia.com");
    put("ftp_port", "int", "2000");
    put("socks_host", "string", "socks.host.com");
    put("socks_port", "int", "10080");
    put("rtsp_host", "string", "rtsp.voice.com");
    put("rtsp_port", "int", "1554");

    pc = new Maemo::ProxyConf();
}

void Ut_ProxyConf::cleanup()
{
    delete pc;
    pc = 0;
}

void Ut_ProxyConf::initTestCase()
{
} 

void Ut_ProxyConf::cleanupTestCase()
{
}


void Ut_ProxyConf::proxy_ftp_no_factory_ok_auto()
{
    QList<QNetworkProxy> nplist;
    QNetworkProxyQuery query = QNetworkProxyQuery(QUrl("ftp://maps.google.com/"));

    nplist = pc->flush(query);
    QVERIFY(nplist.length()==0);
}


void Ut_ProxyConf::proxy_ftp_no_factory_ok_manual()
{
    QList<QNetworkProxy> nplist;
    QNetworkProxyQuery query = QNetworkProxyQuery(QUrl("ftp://maps.google.com/"));

    put("mode", "string", "manual");

    nplist = pc->flush(query);
    foreach (QNetworkProxy proxy, nplist) {
      qDebug() << "proxy: " << proxy.hostName() << "port" << proxy.port();
    }
    QVERIFY(nplist.length()==3);
    QVERIFY(nplist.first().type() == QNetworkProxy::FtpCachingProxy);
}


void Ut_ProxyConf::proxy_http_no_factory_ok_manual()
{
    QList<QNetworkProxy> nplist;
    QNetworkProxyQuery query = QNetworkProxyQuery(QUrl("http://maps.google.com/"));

    put("mode", "string", "manual");

    nplist = pc->flush(query);
    foreach (QNetworkProxy proxy, nplist) {
      qDebug() << "proxy: " << proxy.hostName() << "port" << proxy.port();
    }
    QVERIFY(nplist.length()==3);
    QVERIFY(nplist.first().type() == QNetworkProxy::HttpProxy);
}


void Ut_ProxyConf::proxy_https_no_factory_ok_manual()
{
    QList<QNetworkProxy> nplist;
    QNetworkProxyQuery query = QNetworkProxyQuery(QUrl("https://maps.google.com/"));

    put("mode", "string", "manual");

    nplist = pc->flush(query);
    foreach (QNetworkProxy proxy, nplist) {
      qDebug() << "proxy: " << proxy.hostName() << "port" << proxy.port();
    }
    QVERIFY(nplist.length()==2);
    QVERIFY(nplist.first().type() == QNetworkProxy::HttpProxy);
}


void Ut_ProxyConf::proxy_socks_no_factory_ok_manual()
{
    QList<QNetworkProxy> nplist;
    QNetworkProxyQuery query = QNetworkProxyQuery(QUrl("http://maps.google.com/"));

    put("mode", "string", "manual");
    put_http("host", "string", "");

    nplist = pc->flush(query);
    foreach (QNetworkProxy proxy, nplist) {
      qDebug() << "proxy: " << proxy.hostName() << "port" << proxy.port();
    }
    QVERIFY(nplist.length()==2);
    QVERIFY(nplist.first().type() == QNetworkProxy::Socks5Proxy);
}


void Ut_ProxyConf::proxy_default_no_factory_ok_manual()
{
    QList<QNetworkProxy> nplist;
    QNetworkProxyQuery query = QNetworkProxyQuery(QUrl("foobar://maps.google.com/"));

    put("mode", "string", "manual");
    put("socks_host", "string", "");
    put("secure_host", "string", "");

    nplist = pc->flush(query);
    foreach (QNetworkProxy proxy, nplist) {
      qDebug() << "proxy: " << proxy.hostName() << "port" << proxy.port();
    }
    QVERIFY(nplist.length()==0);
}


void Ut_ProxyConf::proxy_ftp_factory_ok_auto()
{
    QNetworkProxyQuery query = QNetworkProxyQuery(QUrl("ftp://maps.google.com/"));
    Maemo::ProxyConf::update();
    QList<QNetworkProxy> listOfProxies = QNetworkProxyFactory::proxyForQuery(query);
    QVERIFY(listOfProxies.length()==1);
    QVERIFY(listOfProxies.first().type() == QNetworkProxy::NoProxy);
    Maemo::ProxyConf::clear();
}


void Ut_ProxyConf::proxy_ftp_factory_ok_manual()
{
    QNetworkProxyQuery query = QNetworkProxyQuery(QUrl("ftp://maps.google.com/"));
    Maemo::ProxyConf::update();

    put("mode", "string", "manual");

    QList<QNetworkProxy> listOfProxies = QNetworkProxyFactory::proxyForQuery(query);

    foreach (QNetworkProxy proxy, listOfProxies) {
      qDebug() << "proxy: " << proxy.hostName() << "port" << proxy.port();
    }
    QVERIFY(listOfProxies.length()==3);
    QVERIFY(listOfProxies.first().type() == QNetworkProxy::FtpCachingProxy);
    Maemo::ProxyConf::clear();
}


void Ut_ProxyConf::proxy_http_factory_ok_manual()
{
    QNetworkProxyQuery query = QNetworkProxyQuery(QUrl("http://maps.google.com/"));
    Maemo::ProxyConf::update();

    put("mode", "string", "manual");

    QList<QNetworkProxy> listOfProxies = QNetworkProxyFactory::proxyForQuery(query);

    foreach (QNetworkProxy proxy, listOfProxies) {
      qDebug() << "proxy: " << proxy.hostName() << "port" << proxy.port();
    }
    QVERIFY(listOfProxies.length()==3);
    QVERIFY(listOfProxies.first().type() == QNetworkProxy::HttpProxy);
    Maemo::ProxyConf::clear();
}


void Ut_ProxyConf::proxy_https_factory_ok_manual()
{
    QNetworkProxyQuery query = QNetworkProxyQuery(QUrl("https://maps.google.com/"));
    Maemo::ProxyConf::update();

    put("mode", "string", "manual");

    QList<QNetworkProxy> listOfProxies = QNetworkProxyFactory::proxyForQuery(query);

    foreach (QNetworkProxy proxy, listOfProxies) {
      qDebug() << "proxy: " << proxy.hostName() << "port" << proxy.port();
    }
    QVERIFY(listOfProxies.length()==2);
    QVERIFY(listOfProxies.first().type() == QNetworkProxy::HttpProxy);
    Maemo::ProxyConf::clear();
}


void Ut_ProxyConf::proxy_socks_factory_ok_manual()
{
    QNetworkProxyQuery query = QNetworkProxyQuery(QUrl("http://maps.google.com/"));
    Maemo::ProxyConf::update();

    put("mode", "string", "manual");
    put_http("host", "string", "");

    QList<QNetworkProxy> listOfProxies = QNetworkProxyFactory::proxyForQuery(query);

    foreach (QNetworkProxy proxy, listOfProxies) {
      qDebug() << "proxy: " << proxy.hostName() << "port" << proxy.port();
    }
    QVERIFY(listOfProxies.length()==2);
    QVERIFY(listOfProxies.first().type() == QNetworkProxy::Socks5Proxy);
    Maemo::ProxyConf::clear();
}


void Ut_ProxyConf::proxy_http_factory_ok_manual_clear()
{
    QNetworkProxyQuery query = QNetworkProxyQuery(QUrl("http://maps.google.com/"));
    Maemo::ProxyConf::update();

    put("mode", "string", "manual");
    put_http("host", "string", "192.168.1.1");

    QList<QNetworkProxy> listOfProxies = QNetworkProxyFactory::proxyForQuery(query);

    foreach (QNetworkProxy proxy, listOfProxies) {
      qDebug() << "proxy: " << proxy.hostName() << "port" << proxy.port();
    }
    QVERIFY(listOfProxies.length()==3);
    QVERIFY(listOfProxies.first().type() == QNetworkProxy::HttpProxy);

    Maemo::ProxyConf::clear();
    listOfProxies = QNetworkProxyFactory::proxyForQuery(query);
    QVERIFY(listOfProxies.length()==1);
    QVERIFY(listOfProxies.first().type() == QNetworkProxy::NoProxy);
}


void Ut_ProxyConf::proxy_default_factory_ok_manual()
{
    QNetworkProxyQuery query = QNetworkProxyQuery(QUrl("foobar://maps.google.com/"));
    Maemo::ProxyConf::update();

    put("mode", "string", "manual");
    put("socks_host", "string", "");
    put("secure_host", "string", "");

    QList<QNetworkProxy> listOfProxies = QNetworkProxyFactory::proxyForQuery(query);

    QVERIFY(listOfProxies.length()==1);
    QVERIFY(listOfProxies.first().type() == QNetworkProxy::NoProxy);
    Maemo::ProxyConf::clear();
}


void Ut_ProxyConf::proxy_http_factory_ok_manual_ignore_list()
{
    QNetworkProxyQuery query = QNetworkProxyQuery(QUrl("http://192.168.19.70/"));
    Maemo::ProxyConf::update();

    put("mode", "string", "manual");

    QList<QNetworkProxy> listOfProxies = QNetworkProxyFactory::proxyForQuery(query);

    foreach (QNetworkProxy proxy, listOfProxies) {
      qDebug() << "proxy: " << proxy.hostName() << "port" << proxy.port();
    }
    QVERIFY(listOfProxies.length()==3);
    QVERIFY(listOfProxies.first().type() == QNetworkProxy::HttpProxy);

    query = QNetworkProxyQuery(QUrl("http://192.168.20.10/"));
    listOfProxies = QNetworkProxyFactory::proxyForQuery(query);
    QVERIFY(listOfProxies.length()==1);
    QVERIFY(listOfProxies.first().type() == QNetworkProxy::NoProxy);
    Maemo::ProxyConf::clear();
}


void Ut_ProxyConf::proxy_default_factory_ok_manual_system()
{
    QNetworkProxyQuery query = QNetworkProxyQuery(QUrl("http://maps.google.com/"));
    Maemo::ProxyConf::update();

    put("mode", "string", "manual");

    QList<QNetworkProxy> listOfProxies = QNetworkProxyFactory::systemProxyForQuery(query);

    QVERIFY(listOfProxies.length()==1);
    QVERIFY(listOfProxies.first().type() == QNetworkProxy::NoProxy);
    Maemo::ProxyConf::clear();
}




QTEST_MAIN(Ut_ProxyConf)

#include "ut_proxyconf.moc"

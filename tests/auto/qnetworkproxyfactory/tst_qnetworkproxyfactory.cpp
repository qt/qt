/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QtTest/QTest>
#include <QtTest/QTestEventLoop>

#include <qcoreapplication.h>
#include <qdebug.h>
#include <qnetworkproxy.h>
#include <QThread>

class tst_QNetworkProxyFactory : public QObject {
    Q_OBJECT
private slots:
    void systemProxyForQueryCalledFromThread();
    void systemProxyForQuery() const;

private:
    QString formatProxyName(const QNetworkProxy & proxy) const;
};

QString tst_QNetworkProxyFactory::formatProxyName(const QNetworkProxy & proxy) const
{
    QString proxyName;
    if (!proxy.user().isNull())
        proxyName.append("%1:%2@").arg(proxy.user(), proxy.password());
    proxyName.append("%1:%2").arg(proxy.hostName(), proxy.port());
    proxyName.append(" (type=%1, capabilities=%2)").arg(proxy.type(), proxy.capabilities());

    return proxyName;
}

void tst_QNetworkProxyFactory::systemProxyForQuery() const
{
    QNetworkProxyQuery query(QUrl(QString("http://www.abc.com")), QNetworkProxyQuery::UrlRequest);
    QList<QNetworkProxy> systemProxyList = QNetworkProxyFactory::systemProxyForQuery(query);
    bool pass = true;
    QNetworkProxy proxy;

    QList<QNetworkProxy> nativeProxyList;
    nativeProxyList << QNetworkProxy(QNetworkProxy::HttpProxy, QString("http://test.proxy.com"), 8080) << QNetworkProxy::NoProxy;

    foreach (proxy, systemProxyList) {
        if (!nativeProxyList.contains(proxy)) {
            qWarning() << "System proxy not found in native proxy list: " <<
                  formatProxyName(proxy);
            pass = false;
        }
    }

    foreach (proxy, nativeProxyList) {
        if (!systemProxyList.contains(proxy)) {
            qWarning() << "Native proxy not found in system proxy list: " <<
                  formatProxyName(proxy);
            pass = false;
        }
    }

    if (!pass)
        QFAIL("One or more system proxy lookup failures occurred.");
}

class QSPFQThread : public QThread
{
protected:
    virtual void run()
    {
        proxies = QNetworkProxyFactory::systemProxyForQuery(query);
    }
public:
    QNetworkProxyQuery query;
    QList<QNetworkProxy> proxies;
};

//regression test for QTBUG-18799
void tst_QNetworkProxyFactory::systemProxyForQueryCalledFromThread()
{
    QUrl url(QLatin1String("http://qt.nokia.com"));
    QNetworkProxyQuery query(url);
    QSPFQThread thread;
    thread.query = query;
    connect(&thread, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    thread.start();
    QTestEventLoop::instance().enterLoop(5);
    QVERIFY(thread.isFinished());
    QCOMPARE(thread.proxies, QNetworkProxyFactory::systemProxyForQuery(query));
}

QTEST_MAIN(tst_QNetworkProxyFactory)
#include "tst_qnetworkproxyfactory.moc"

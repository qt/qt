/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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
// This file contains benchmarks for QNetworkReply functions.

#include <QDebug>
#include <qtest.h>
#include <QtTest/QtTest>
#include <QtNetwork/qnetworkreply.h>
#include <QtNetwork/qnetworkrequest.h>
#include <QtNetwork/qnetworkaccessmanager.h>
#include "../../auto/network-settings.h"

class tst_qnetworkreply : public QObject
{
    Q_OBJECT
private slots:
    void httpLatency();

#ifndef QT_NO_OPENSSL
    void echoPerformance_data();
    void echoPerformance();
#endif
};

void tst_qnetworkreply::httpLatency()
{
    QNetworkAccessManager manager;
    QBENCHMARK{
        QNetworkRequest request(QUrl("http://" + QtNetworkSettings::serverName() + "/qtest/"));
        QNetworkReply* reply = manager.get(request);
        connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()), Qt::QueuedConnection);
        QTestEventLoop::instance().enterLoop(5);
        QVERIFY(!QTestEventLoop::instance().timeout());
        delete reply;
    }
}

#ifndef QT_NO_OPENSSL
void tst_qnetworkreply::echoPerformance_data()
{
     QTest::addColumn<bool>("ssl");
     QTest::newRow("no_ssl") << false;
     QTest::newRow("ssl") << true;
}

void tst_qnetworkreply::echoPerformance()
{
    QFETCH(bool, ssl);
    QNetworkAccessManager manager;
    QNetworkRequest request(QUrl((ssl ? "https://" : "http://") + QtNetworkSettings::serverName() + "/qtest/cgi-bin/echo.cgi"));

    QByteArray data;
    data.resize(1024*1024*10); // 10 MB
    // init with garbage. needed so ssl cannot compress it in an efficient way.
    for (int i = 0; i < data.size() / sizeof(int); i++) {
        int r = qrand();
        data.data()[i*sizeof(int)] = r;
    }

    QBENCHMARK{
        QNetworkReply* reply = manager.post(request, data);
        connect(reply, SIGNAL(sslErrors( const QList<QSslError> &)), reply, SLOT(ignoreSslErrors()));
        connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()), Qt::QueuedConnection);
        QTestEventLoop::instance().enterLoop(5);
        QVERIFY(!QTestEventLoop::instance().timeout());
        QVERIFY(reply->error() == QNetworkReply::NoError);
        delete reply;
    }
}
#endif

QTEST_MAIN(tst_qnetworkreply)

#include "tst_qnetworkreply.moc"

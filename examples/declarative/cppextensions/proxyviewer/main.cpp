/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the demonstration applications of the Qt Toolkit.
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

#include <QApplication>
#include <QNetworkAccessManager>
#include <QNetworkProxy>

#include <QDeclarativeEngine>
#include <QDeclarativeNetworkAccessManagerFactory>
#include <QDeclarativeView>


/*
   This example illustrates using a QNetworkAccessManagerFactory to
   create a QNetworkAccessManager with a proxy.

   Usage:
     proxyviewer [-host <proxy> -port <port>] [file]
*/

static QString proxyHost;
static int proxyPort = 0;

class MyNetworkAccessManagerFactory : public QDeclarativeNetworkAccessManagerFactory
{
public:
    virtual QNetworkAccessManager *create(QObject *parent);
};

QNetworkAccessManager *MyNetworkAccessManagerFactory::create(QObject *parent)
{
    QNetworkAccessManager *nam = new QNetworkAccessManager(parent);
    if (!proxyHost.isEmpty()) {
        qDebug() << "Created QNetworkAccessManager using proxy" << (proxyHost + ":" + QString::number(proxyPort));
        QNetworkProxy proxy(QNetworkProxy::HttpCachingProxy, proxyHost, proxyPort);
        nam->setProxy(proxy);
    }

    return nam;
}

int main(int argc, char ** argv)
{
    QUrl source("qrc:view.qml");

    QApplication app(argc, argv);

    for (int i = 1; i < argc; ++i) {
        QString arg(argv[i]);
        if (arg == "-host" && i < argc-1) {
            proxyHost = argv[++i];
        } else if (arg == "-port" && i < argc-1) {
            arg = argv[++i];
            proxyPort = arg.toInt();
        } else if (arg[0] != '-') {
            source = QUrl::fromLocalFile(arg);
        } else {
            qWarning() << "Usage: proxyviewer [-host <proxy> -port <port>] [file]";
            exit(1);
        }
    }

    QDeclarativeView view;
    view.engine()->setNetworkAccessManagerFactory(new MyNetworkAccessManagerFactory);

    view.setSource(source);
    view.show();

    return app.exec();
}


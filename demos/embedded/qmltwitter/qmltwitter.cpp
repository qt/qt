/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the demonstration applications of the Qt Toolkit.
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

#include <QtCore/QFileInfo>
#include <QtCore/QSettings>
#include <QtGui/QApplication>
#include <QtDeclarative/QDeclarativeView>
#include <QtDeclarative/QDeclarativeEngine>
#include <QtDeclarative/QDeclarativeNetworkAccessManagerFactory>
#include <QtNetwork/QNetworkConfiguration>
#include <QtNetwork/QNetworkConfigurationManager>
#include <QtNetwork/QNetworkAccessManager>

// Factory to create QNetworkAccessManagers that use the saved network configuration; otherwise
// the system default.
class NetworkAccessManagerFactory : public QDeclarativeNetworkAccessManagerFactory
{
public:
    ~NetworkAccessManagerFactory() { }

    QNetworkAccessManager *create(QObject *parent);
};

QNetworkAccessManager *NetworkAccessManagerFactory::create(QObject *parent)
{
    QNetworkAccessManager *accessManager = new QNetworkAccessManager(parent);

    QNetworkConfigurationManager manager;
    if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired) {
        // Get saved network configuration
        QSettings settings(QSettings::UserScope, QLatin1String("Trolltech"));
        settings.beginGroup(QLatin1String("QtNetwork"));
        const QString id = settings.value(QLatin1String("DefaultNetworkConfiguration")).toString();
        settings.endGroup();

        // If the saved network configuration is not currently discovered use the system default
        QNetworkConfiguration config = manager.configurationFromIdentifier(id);
        if ((config.state() & QNetworkConfiguration::Discovered) !=
            QNetworkConfiguration::Discovered) {
            config = manager.defaultConfiguration();
        }

        accessManager->setConfiguration(config);
    }

    return accessManager;
}

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

    NetworkAccessManagerFactory networkAccessManagerFactory;

    const QString mainQmlApp = QLatin1String("twitter.qml");
    QDeclarativeView view;
    view.engine()->setNetworkAccessManagerFactory(&networkAccessManagerFactory);
    view.setSource(QUrl(mainQmlApp));
    view.setResizeMode(QDeclarativeView::SizeRootObjectToView);
    QObject::connect(view.engine(), SIGNAL(quit()), &application, SLOT(quit()));
    
#if defined(Q_OS_SYMBIAN)
    view.showFullScreen();
#else // Q_OS_SYMBIAN
    view.setGeometry(QRect(100, 100, 360, 640));
    view.show();
#endif // Q_OS_SYMBIAN
    return application.exec();
}


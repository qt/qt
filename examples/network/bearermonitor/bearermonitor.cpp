/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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

#include "bearermonitor.h"
#include "sessionwidget.h"

#include <QDebug>
#include <QMessageBox>
#ifdef Q_OS_WIN
#include <winsock2.h>
#undef interface

#ifndef NS_NLA
#define NS_NLA 15
#endif
#endif

BearerMonitor::BearerMonitor(QWidget *parent)
:   QWidget(parent)
{
    setupUi(this);
#if defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
    newSessionButton->hide();
    deleteSessionButton->hide();
#else
    delete tabWidget->currentWidget();
    sessionGroup->hide();
#endif
#if defined (Q_OS_SYMBIAN) || defined(Q_OS_WINCE) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
    setWindowState(Qt::WindowMaximized);
#endif
    updateConfigurations();
#if !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
    onlineStateChanged(!manager.allConfigurations(QNetworkConfiguration::Active).isEmpty());
#endif
    QNetworkConfiguration defaultConfiguration = manager.defaultConfiguration();
    for (int i = 0; i < treeWidget->topLevelItemCount(); ++i) {
        QTreeWidgetItem *item = treeWidget->topLevelItem(i);

        if (item->data(0, Qt::UserRole).toString() == defaultConfiguration.identifier()) {
            treeWidget->setCurrentItem(item);
            showConfigurationFor(item);
            break;
        }
    }

    connect(&manager, SIGNAL(configurationAdded(const QNetworkConfiguration&)),
            this, SLOT(configurationAdded(const QNetworkConfiguration&)));
    connect(&manager, SIGNAL(configurationRemoved(const QNetworkConfiguration&)),
            this, SLOT(configurationRemoved(const QNetworkConfiguration&)));
    connect(&manager, SIGNAL(configurationChanged(const QNetworkConfiguration&)),
            this, SLOT(configurationChanged(const QNetworkConfiguration)));
    connect(&manager, SIGNAL(updateCompleted()), this, SLOT(updateConfigurations()));
    connect(&manager, SIGNAL(onlineStateChanged(bool)), this ,SLOT(onlineStateChanged(bool)));

#ifdef Q_OS_WIN
    connect(registerButton, SIGNAL(clicked()), this, SLOT(registerNetwork()));
    connect(unregisterButton, SIGNAL(clicked()), this, SLOT(unregisterNetwork()));
#else
    nlaGroup->hide();
#endif

    connect(treeWidget, SIGNAL(itemActivated(QTreeWidgetItem*,int)),
            this, SLOT(createSessionFor(QTreeWidgetItem*)));

    connect(treeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
            this, SLOT(showConfigurationFor(QTreeWidgetItem*)));

    connect(newSessionButton, SIGNAL(clicked()),
            this, SLOT(createNewSession()));
#if !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
    connect(deleteSessionButton, SIGNAL(clicked()),
            this, SLOT(deleteSession()));
#endif
    connect(scanButton, SIGNAL(clicked()),
            this, SLOT(performScan()));
}

BearerMonitor::~BearerMonitor()
{
}

static void updateItem(QTreeWidgetItem *item, const QNetworkConfiguration &config)
{
    item->setText(0, config.name());
    item->setData(0, Qt::UserRole, config.identifier());

    QFont font = item->font(1);
    font.setBold((config.state() & QNetworkConfiguration::Active) == QNetworkConfiguration::Active);
    item->setFont(0, font);
}

void BearerMonitor::configurationAdded(const QNetworkConfiguration &config, QTreeWidgetItem *parent)
{
    QTreeWidgetItem *item = new QTreeWidgetItem;
    updateItem(item, config);

    if (parent)
        parent->addChild(item);
    else
        treeWidget->addTopLevelItem(item);

    if (config.type() == QNetworkConfiguration::ServiceNetwork) {
        foreach (const QNetworkConfiguration &child, config.children())
            configurationAdded(child, item);
    }
}

void BearerMonitor::configurationRemoved(const QNetworkConfiguration &config)
{
    for (int i = 0; i < treeWidget->topLevelItemCount(); ++i) {
        QTreeWidgetItem *item = treeWidget->topLevelItem(i);

        if (item->data(0, Qt::UserRole).toString() == config.identifier()) {
            delete item;
            break;
        }
    }
}

void BearerMonitor::configurationChanged(const QNetworkConfiguration &config)
{
    for (int i = 0; i < treeWidget->topLevelItemCount(); ++i) {
        QTreeWidgetItem *item = treeWidget->topLevelItem(i);

        if (item->data(0, Qt::UserRole).toString() == config.identifier()) {
            updateItem(item, config);

            if (config.type() == QNetworkConfiguration::ServiceNetwork)
                updateSnapConfiguration(item, config);

            if (item == treeWidget->currentItem())
                showConfigurationFor(item);

            break;
        }
    }
}

void BearerMonitor::updateSnapConfiguration(QTreeWidgetItem *parent, const QNetworkConfiguration &snap)
{
    QMap<QString, QTreeWidgetItem *> itemMap;
    for (int i = 0; i < parent->childCount(); ++i) {
        QTreeWidgetItem *item = parent->child(i);
        itemMap.insert(item->data(0, Qt::UserRole).toString(), item);
    }

    QList<QNetworkConfiguration> allConfigurations = snap.children();

    while (!allConfigurations.isEmpty()) {
        QNetworkConfiguration config = allConfigurations.takeFirst();

        QTreeWidgetItem *item = itemMap.take(config.identifier());
        if (item) {
            updateItem(item, config);

            if (config.type() == QNetworkConfiguration::ServiceNetwork)
                updateSnapConfiguration(item, config);
        } else {
            configurationAdded(config, parent);
        }
    }

    foreach (const QString &id, itemMap.keys())
        delete itemMap.value(id);

    itemMap.clear();
}

void BearerMonitor::updateConfigurations()
{
    progressBar->hide();
    scanButton->show();

    QList<QTreeWidgetItem *> items = treeWidget->findItems(QLatin1String("*"), Qt::MatchWildcard);
    QMap<QString, QTreeWidgetItem *> itemMap;
    while (!items.isEmpty()) {
        QTreeWidgetItem *item = items.takeFirst();
        itemMap.insert(item->data(0, Qt::UserRole).toString(), item);
    }

    QList<QNetworkConfiguration> allConfigurations = manager.allConfigurations();

    while (!allConfigurations.isEmpty()) {
        QNetworkConfiguration config = allConfigurations.takeFirst();

        QTreeWidgetItem *item = itemMap.take(config.identifier());
        if (item) {
            updateItem(item, config);

            if (config.type() == QNetworkConfiguration::ServiceNetwork)
                updateSnapConfiguration(item, config);
        } else {
            configurationAdded(config);
        }
    }

    foreach (const QString &id, itemMap.keys())
        delete itemMap.value(id);
}

void BearerMonitor::onlineStateChanged(bool isOnline)
{
    if (isOnline)
#if defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
        QMessageBox::information(this, "Connection state changed", "Online", QMessageBox::Close);
    else
        QMessageBox::information(this, "Connection state changed", "Offline", QMessageBox::Close);
#else
        onlineState->setText(tr("Online"));
    else
        onlineState->setText(tr("Offline"));
#endif
}

#ifdef Q_OS_WIN
void BearerMonitor::registerNetwork()
{
    QTreeWidgetItem *item = treeWidget->currentItem();

    QNetworkConfiguration configuration =
        manager.configurationFromIdentifier(item->data(0, Qt::UserRole).toString());

    const QString name = configuration.name();

    qDebug() << "Registering" << name << "with system";

    WSAQUERYSET networkInfo;
    memset(&networkInfo, 0, sizeof(networkInfo));
    networkInfo.dwSize = sizeof(networkInfo);
    networkInfo.lpszServiceInstanceName = (LPWSTR)name.utf16();
    networkInfo.dwNameSpace = NS_NLA;

    if (WSASetService(&networkInfo, RNRSERVICE_REGISTER, 0) == SOCKET_ERROR)
        qDebug() << "WSASetService(RNRSERVICE_REGISTER) returned" << WSAGetLastError();
}

void BearerMonitor::unregisterNetwork()
{
    QTreeWidgetItem *item = treeWidget->currentItem();

    QNetworkConfiguration configuration =
        manager.configurationFromIdentifier(item->data(0, Qt::UserRole).toString());

    const QString name = configuration.name();

    qDebug() << "Unregistering" << name << "with system";

    WSAQUERYSET networkInfo;
    memset(&networkInfo, 0, sizeof(networkInfo));
    networkInfo.dwSize = sizeof(networkInfo);
    networkInfo.lpszServiceInstanceName = (LPWSTR)name.utf16();
    networkInfo.dwNameSpace = NS_NLA;

    if (WSASetService(&networkInfo, RNRSERVICE_DELETE, 0) == SOCKET_ERROR)
        qDebug() << "WSASetService(RNRSERVICE_DELETE) returned" << WSAGetLastError();
}
#endif

void BearerMonitor::showConfigurationFor(QTreeWidgetItem *item)
{
    QString identifier;

    if (item)
        identifier = item->data(0, Qt::UserRole).toString();

    QNetworkConfiguration conf = manager.configurationFromIdentifier(identifier);

    switch (conf.state()) {
    case QNetworkConfiguration::Active:
        configurationState->setText(tr("Active"));
        break;
    case QNetworkConfiguration::Discovered:
        configurationState->setText(tr("Discovered"));
        break;
    case QNetworkConfiguration::Defined:
        configurationState->setText(tr("Defined"));
        break;
    case QNetworkConfiguration::Undefined:
        configurationState->setText(tr("Undefined"));
        break;
    default:
        configurationState->setText(QString());
    }

    switch (conf.type()) {
    case QNetworkConfiguration::InternetAccessPoint:
        configurationType->setText(tr("Internet Access Point"));
        break;
    case QNetworkConfiguration::ServiceNetwork:
        configurationType->setText(tr("Service Network"));
        break;
    case QNetworkConfiguration::UserChoice:
        configurationType->setText(tr("User Choice"));
        break;
    case QNetworkConfiguration::Invalid:
        configurationType->setText(tr("Invalid"));
        break;
    default:
        configurationType->setText(QString());
    }

    switch (conf.purpose()) {
    case QNetworkConfiguration::UnknownPurpose:
        configurationPurpose->setText(tr("Unknown"));
        break;
    case QNetworkConfiguration::PublicPurpose:
        configurationPurpose->setText(tr("Public"));
        break;
    case QNetworkConfiguration::PrivatePurpose:
        configurationPurpose->setText(tr("Private"));
        break;
    case QNetworkConfiguration::ServiceSpecificPurpose:
        configurationPurpose->setText(tr("Service Specific"));
        break;
    default:
        configurationPurpose->setText(QString());
    }

    configurationIdentifier->setText(conf.identifier());

    configurationRoaming->setText(conf.isRoamingAvailable() ? tr("Available") : tr("Not available"));

    configurationChildren->setText(QString::number(conf.children().count()));

    configurationName->setText(conf.name());
}

void BearerMonitor::createSessionFor(QTreeWidgetItem *item)
{
    const QString identifier = item->data(0, Qt::UserRole).toString();

    QNetworkConfiguration conf = manager.configurationFromIdentifier(identifier);

    SessionWidget *session = new SessionWidget(conf);

    tabWidget->addTab(session, conf.name());

#if !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
    sessionGroup->show();
#endif

    sessionWidgets.append(session);
}

void BearerMonitor::createNewSession()
{
    QTreeWidgetItem *item = treeWidget->currentItem();

    createSessionFor(item);
}

#if !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
void BearerMonitor::deleteSession()
{
    SessionWidget *session = qobject_cast<SessionWidget *>(tabWidget->currentWidget());
    if (session) {
        sessionWidgets.removeAll(session);

        delete session;

        if (tabWidget->count() == 0)
            sessionGroup->hide();
    }
}
#endif

void BearerMonitor::performScan()
{
    scanButton->hide();
    progressBar->show();
    manager.updateConfigurations();
}

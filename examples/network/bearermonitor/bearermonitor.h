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

#ifndef BEARERMONITOR_H
#define BEARERMONITOR_H

#include <qnetworkconfigmanager.h>
#include <qnetworksession.h>
#if defined (Q_OS_SYMBIAN) || defined(Q_OS_WINCE)	
#include "ui_bearermonitor_240_320.h"
#else
#include "ui_bearermonitor_640_480.h"
#endif

QTM_USE_NAMESPACE

class SessionWidget;

class BearerMonitor : public QWidget, public Ui_BearerMonitor
{
    Q_OBJECT

public:
    BearerMonitor(QWidget *parent = 0);
    ~BearerMonitor();

private slots:
    void configurationAdded(const QNetworkConfiguration &config, QTreeWidgetItem *parent = 0);
    void configurationRemoved(const QNetworkConfiguration &config);
    void configurationChanged(const QNetworkConfiguration &config);
    void updateSnapConfiguration(QTreeWidgetItem *parent, const QNetworkConfiguration &snap);
    void updateConfigurations();

    void onlineStateChanged(bool isOnline);

#ifdef Q_OS_WIN
    void registerNetwork();
    void unregisterNetwork();
#endif

    void showConfigurationFor(QTreeWidgetItem *item);

    void createSessionFor(QTreeWidgetItem *item);
    void createNewSession();
    void deleteSession();

    void performScan();

private:
    QNetworkConfigurationManager manager;
    QList<SessionWidget *> sessionWidgets;
};

#endif //BEARERMONITOR_H

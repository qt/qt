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

#include <QWidget>
#include <QDBusConnection>
#include <qnetworkconfigmanager.h>
#include <qnetworksession.h>
#include <qnetworkconfiguration.h>

#include "ui_dialog.h"

QTM_USE_NAMESPACE

QT_BEGIN_NAMESPACE
class QListWidget;
class QTreeWidget;
class QTreeWidgetItem;
QT_END_NAMESPACE

class NMView : public QDialog, private Ui::Dialog
{
    Q_OBJECT
public:
    NMView(QDialog* parent = 0);
    virtual ~NMView();

private:
    void init();
    QString stateString;

    QDBusConnection dbc;
    void printConnectionDetails(const QString&);

    QString deviceStateToString(int state);
    QString deviceTypeToString(int device);
    QString securityCapabilitiesToString(int caps);
    QString deviceModeToString(int mode);
//QDBusInterface getInterface();
    QNetworkConfigurationManager *manager;
    QNetworkSession *sess;
    QString stateToString(int state);
    QString typeToString(int type);

private slots:
    void update();
    void deactivate();
    void activate();
    void getActiveConnections();
    void updateConnections();
    void getDevices();
 //   void readConnectionManagerDetails();
    void getNetworkDevices();
    void connectionItemActivated( QTreeWidgetItem *, int);
//    void activeItemActivated( QListWidgetItem *);
    void deviceItemActivated( QTreeWidgetItem *, int);
    void netconfig();
    void findAccessPoints();

    void netManagerState(quint32);
    void readSettings();
    void updateCompleted();
    void newConfigurationActivated();

    void stateChanged(QNetworkSession::State);
    void deviceStateChanged(quint32 state);

    void configurationAdded(const QNetworkConfiguration &config);
    void aPPropertiesChanged( QMap<QString,QVariant> map);
    void networkSessionError(QNetworkSession::SessionError);

};

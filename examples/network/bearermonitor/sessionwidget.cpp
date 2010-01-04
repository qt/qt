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

#include "sessionwidget.h"
#include "qnetworkconfigmanager.h"

SessionWidget::SessionWidget(const QNetworkConfiguration &config, QWidget *parent)
:   QWidget(parent)
{
    setupUi(this);

    session = new QNetworkSession(config, this);

    connect(session, SIGNAL(stateChanged(QNetworkSession::State)),
            this, SLOT(updateSession()));
    connect(session, SIGNAL(error(QNetworkSession::SessionError)),
            this, SLOT(updateSession()));

    updateSession();

    sessionId->setText(QString("0x%1").arg(qulonglong(session), 8, 16, QChar('0')));

    configuration->setText(session->configuration().name());

    connect(openSessionButton, SIGNAL(clicked()),
            this, SLOT(openSession()));
    connect(openSyncSessionButton, SIGNAL(clicked()),
            this, SLOT(openSyncSession()));
    connect(closeSessionButton, SIGNAL(clicked()),
            this, SLOT(closeSession()));
    connect(stopSessionButton, SIGNAL(clicked()),
            this, SLOT(stopSession()));
}

SessionWidget::~SessionWidget()
{
    delete session;
}

void SessionWidget::updateSession()
{
    updateSessionState(session->state());
    updateSessionError(session->error());

    if (session->configuration().type() == QNetworkConfiguration::InternetAccessPoint)
        bearer->setText(session->configuration().bearerName());
    else {
        QNetworkConfigurationManager mgr;
        QNetworkConfiguration c = mgr.configurationFromIdentifier(session->sessionProperty("ActiveConfiguration").toString());
        bearer->setText(c.bearerName());
    }

    interfaceName->setText(session->interface().humanReadableName());
    interfaceGuid->setText(session->interface().name());
}

void SessionWidget::openSession()
{
    session->open();
    updateSession();
}

void SessionWidget::openSyncSession()
{
    session->open();
    session->waitForOpened();
    updateSession();
}

void SessionWidget::closeSession()
{
    session->close();
    updateSession();
}

void SessionWidget::stopSession()
{
    session->stop();
    updateSession();
}

void SessionWidget::updateSessionState(QNetworkSession::State state)
{
    QString s = tr("%1 (%2)");

    switch (state) {
    case QNetworkSession::Invalid:
        s = s.arg(tr("Invalid"));
        break;
    case QNetworkSession::NotAvailable:
        s = s.arg(tr("Not Available"));
        break;
    case QNetworkSession::Connecting:
        s = s.arg(tr("Connecting"));
        break;
    case QNetworkSession::Connected:
        s = s.arg(tr("Connected"));
        break;
    case QNetworkSession::Closing:
        s = s.arg(tr("Closing"));
        break;
    case QNetworkSession::Disconnected:
        s = s.arg(tr("Disconnected"));
        break;
    case QNetworkSession::Roaming:
        s = s.arg(tr("Roaming"));
        break;
    default:
        s = s.arg(tr("Unknown"));
    }

    if (session->isOpen())
        s = s.arg(tr("Open"));
    else
        s = s.arg(tr("Closed"));

    sessionState->setText(s);
}

void SessionWidget::updateSessionError(QNetworkSession::SessionError error)
{
    lastError->setText(QString::number(error));
    errorString->setText(session->errorString());
}


/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt QML Debugger of the Qt Toolkit.
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
#include <QtCore/qtimer.h>
#include <QtCore/qdebug.h>
#include <QtCore/qsettings.h>

#include <QtGui/qlayout.h>
#include <QtGui/qpushbutton.h>
#include <QtGui/qlineedit.h>
#include <QtGui/qtabwidget.h>
#include <QtGui/qspinbox.h>
#include <QtGui/qlabel.h>

#include "canvasframerate.h"
#include "engine.h"
#include "qmldebugger.h"

QT_BEGIN_NAMESPACE

QmlDebugger::QmlDebugger(QWidget *parent)
: QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout;
    setLayout(layout);

    QHBoxLayout *connectLayout = new QHBoxLayout;
    layout->addLayout(connectLayout);
    connectLayout->addStretch(2);

    m_connectionState = new QLabel(this);
    connectLayout->addWidget(m_connectionState);
    m_host = new QLineEdit(this);
    connectLayout->addWidget(m_host);
    m_port = new QSpinBox(this);
    m_port->setMinimum(1024);
    m_port->setMaximum(20000);
    connectLayout->addWidget(m_port);
    m_connectButton = new QPushButton(tr("Connect"), this);
    QObject::connect(m_connectButton, SIGNAL(clicked()), 
                     this, SLOT(connectToHost()));
    connectLayout->addWidget(m_connectButton);
    m_disconnectButton = new QPushButton(tr("Disconnect"), this);
    QObject::connect(m_disconnectButton, SIGNAL(clicked()), 
                     this, SLOT(disconnectFromHost()));
    m_disconnectButton->setEnabled(false);
    connectLayout->addWidget(m_disconnectButton);

    m_tabs = new QTabWidget(this);
    layout->addWidget(m_tabs);

    CanvasFrameRate *cfr = new CanvasFrameRate(this);
    cfr->reset(&client);
    cfr->setSizeHint(QSize(800, 600));
    m_tabs->addTab(cfr, tr("Frame Rate"));

    m_enginePane = new EnginePane(&client, this);
    m_tabs->addTab(m_enginePane, tr("QML Engine"));

    QObject::connect(&client, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
                     this, SLOT(connectionStateChanged()));
    connectionStateChanged();

    QObject::connect(&client, SIGNAL(error(QAbstractSocket::SocketError)),
                     this, SLOT(connectionError(QAbstractSocket::SocketError)));

    QSettings settings;
    m_host->setText(settings.value("Host", "127.0.0.1").toString());
    m_port->setValue(settings.value("Port", 3768).toInt());
    
    connectToHost();
}

void QmlDebugger::setHost(const QString &host)
{
    m_host->setText(host);
}

void QmlDebugger::setPort(quint16 port)
{
    m_port->setValue(port);
}

void QmlDebugger::showEngineTab()
{
    m_tabs->setCurrentWidget(m_enginePane);
}

void QmlDebugger::closeEvent(QCloseEvent *event)
{
    QSettings settings;
    settings.setValue("Host", m_host->text());
    settings.setValue("Port", m_port->value());

    QWidget::closeEvent(event);
}

void QmlDebugger::connectionStateChanged()
{
    switch (client.state()) {
    default:
    case QAbstractSocket::UnconnectedState:
        m_connectionState->setText(tr("Disconnected"));
        m_connectButton->setEnabled(true);
        m_disconnectButton->setEnabled(false);
        break;
    case QAbstractSocket::HostLookupState:
        m_connectionState->setText(tr("Resolving"));
        m_connectButton->setEnabled(false);
        m_disconnectButton->setEnabled(true);
        break;
    case QAbstractSocket::ConnectingState:
        m_connectionState->setText(tr("Connecting"));
        m_connectButton->setEnabled(false);
        m_disconnectButton->setEnabled(true);
        break;
    case QAbstractSocket::ConnectedState:
        m_connectionState->setText(tr("Connected"));
        m_connectButton->setEnabled(false);
        m_disconnectButton->setEnabled(true);

        QTimer::singleShot(0, m_enginePane, SLOT(refreshEngines()));
        break;
    case QAbstractSocket::ClosingState:
        m_connectionState->setText(tr("Closing"));
        m_connectButton->setEnabled(false);
        m_disconnectButton->setEnabled(false);
        break;
    }
}

void QmlDebugger::connectionError(QAbstractSocket::SocketError socketError)
{
    qWarning() << "qmldebugger cannot connect:" << socketError
            << client.errorString();
}

void QmlDebugger::connectToHost()
{
    client.connectToHost(m_host->text(), m_port->value());
}

void QmlDebugger::disconnectFromHost()
{
    client.disconnectFromHost();
}

QT_END_NAMESPACE

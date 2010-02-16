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
#ifndef QMLDEBUGGER_H
#define QMLDEBUGGER_H

#include <private/qmldebugclient_p.h>
#include <QtNetwork/qtcpsocket.h>
#include <QtGui/qwidget.h>

QT_BEGIN_NAMESPACE

class QLabel;
class QLineEdit;
class QSpinBox;
class QPushButton;
class QTabWidget;

class EnginePane;

class QmlDebugger : public QWidget
{
    Q_OBJECT
public:
    QmlDebugger(QWidget * = 0);

    void setHost(const QString &host);
    void setPort(quint16 port);
    void showEngineTab();

public slots:
    void connectToHost();
    void disconnectFromHost();

protected:
    void closeEvent(QCloseEvent *);
    
private slots:
    void connectionStateChanged();
    void connectionError(QAbstractSocket::SocketError socketError);

private:
    QmlDebugConnection client;

    QLabel *m_connectionState;
    QLineEdit *m_host;
    QSpinBox *m_port;
    QPushButton *m_connectButton;
    QPushButton *m_disconnectButton;

    EnginePane *m_enginePane;
    QTabWidget *m_tabs;
};

QT_END_NAMESPACE

#endif

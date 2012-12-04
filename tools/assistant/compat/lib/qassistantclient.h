/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** All rights reserved.
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Assistant of the Qt Toolkit.
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

#ifndef QASSISTANTCLIENT_H
#define QASSISTANTCLIENT_H

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtCore/QProcess>
#include <QtCore/qglobal.h>
#include <QtAssistant/qassistantclient_global.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QTcpSocket;

class QT_ASSISTANT_CLIENT_EXPORT QAssistantClient : public QObject
{
    Q_OBJECT
    Q_PROPERTY( bool open READ isOpen )

public:
    QAssistantClient( const QString &path, QObject *parent = 0);
    ~QAssistantClient();

    bool isOpen() const;

    void setArguments( const QStringList &args );

public Q_SLOTS:
    virtual void openAssistant();
    virtual void closeAssistant();
    virtual void showPage( const QString &page );

Q_SIGNALS:
    void assistantOpened();
    void assistantClosed();
    void error( const QString &msg );

private Q_SLOTS:
    void socketConnected();
    void socketConnectionClosed();
    void readPort();
    void procError(QProcess::ProcessError err);
    void socketError();
    void readStdError();

private:
    QTcpSocket *socket;
    QProcess *proc;
    quint16 port;
    QString host, assistantCommand, pageBuffer;
    bool opened;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif

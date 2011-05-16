/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#include "private/qjsdebugservice_p.h"
#include "private/qjsdebuggeragent_p.h"

#include <QtCore/qdatastream.h>
#include <QtCore/qdebug.h>
#include <QtCore/qstringlist.h>
#include <QtDeclarative/qdeclarativeengine.h>

Q_GLOBAL_STATIC(QJSDebugService, serviceInstance)

QJSDebugService::QJSDebugService(QObject *parent)
    : QDeclarativeDebugService(QLatin1String("JSDebugger"), parent)
    , m_agent(0)
{
}

QJSDebugService::~QJSDebugService()
{
    delete m_agent;
}

QJSDebugService *QJSDebugService::instance()
{
    return serviceInstance();
}

void QJSDebugService::addEngine(QDeclarativeEngine *engine)
{
    Q_ASSERT(engine);
    Q_ASSERT(!m_engines.contains(engine));

    m_engines.append(engine);
}

void QJSDebugService::removeEngine(QDeclarativeEngine *engine)
{
    Q_ASSERT(engine);
    Q_ASSERT(m_engines.contains(engine));

    m_engines.removeAll(engine);
}

void QJSDebugService::statusChanged(Status status)
{
    if (status == Enabled && !m_engines.isEmpty() && !m_agent) {
        // Multiple engines are currently unsupported
        QDeclarativeEngine *engine = m_engines.first();
        m_agent = new QJSDebuggerAgent(engine, engine);

        connect(m_agent, SIGNAL(stopped(bool,QString)),
                this, SLOT(executionStopped(bool,QString)));

    } else if (status != Enabled && m_agent) {
        delete m_agent;
        m_agent = 0;
    }
}

void QJSDebugService::messageReceived(const QByteArray &message)
{
    if (!m_agent) {
        qWarning() << "QJSDebugService::messageReceived: No QJSDebuggerAgent available";
        return;
    }

    QDataStream ds(message);
    QByteArray command;
    ds >> command;
    if (command == "BREAKPOINTS") {
        JSAgentBreakpoints breakpoints;
        ds >> breakpoints;
        m_agent->setBreakpoints(breakpoints);

        //qDebug() << "BREAKPOINTS";
        //foreach (const JSAgentBreakpointData &bp, breakpoints)
        //    qDebug() << "BREAKPOINT: " << bp.fileUrl << bp.lineNumber;
    } else if (command == "WATCH_EXPRESSIONS") {
        QStringList watchExpressions;
        ds >> watchExpressions;
        m_agent->setWatchExpressions(watchExpressions);
    } else if (command == "STEPOVER") {
        m_agent->stepOver();
    } else if (command == "STEPINTO" || command == "INTERRUPT") {
        m_agent->stepInto();
    } else if (command == "STEPOUT") {
        m_agent->stepOut();
    } else if (command == "CONTINUE") {
        m_agent->continueExecution();
    } else if (command == "EXEC") {
        QByteArray id;
        QString expr;
        ds >> id >> expr;

        JSAgentWatchData data = m_agent->executeExpression(expr);

        QByteArray reply;
        QDataStream rs(&reply, QIODevice::WriteOnly);
        rs << QByteArray("RESULT") << id << data;
        sendMessage(reply);
    } else if (command == "EXPAND") {
        QByteArray requestId;
        quint64 objectId;
        ds >> requestId >> objectId;

        QList<JSAgentWatchData> result = m_agent->expandObjectById(objectId);

        QByteArray reply;
        QDataStream rs(&reply, QIODevice::WriteOnly);
        rs << QByteArray("EXPANDED") << requestId << result;
        sendMessage(reply);
    } else if (command == "ACTIVATE_FRAME") {
        int frameId;
        ds >> frameId;

        QList<JSAgentWatchData> locals = m_agent->localsAtFrame(frameId);

        QByteArray reply;
        QDataStream rs(&reply, QIODevice::WriteOnly);
        rs << QByteArray("LOCALS") << frameId << locals;
        sendMessage(reply);
    } else if (command == "SET_PROPERTY") {
        QByteArray id;
        qint64 objectId;
        QString property;
        QString value;
        ds >> id >> objectId >> property >> value;

        m_agent->setProperty(objectId, property, value);

        //TODO: feedback
    } else if (command == "PING") {
        int ping;
        ds >> ping;
        QByteArray reply;
        QDataStream rs(&reply, QIODevice::WriteOnly);
        rs << QByteArray("PONG") << ping;
        sendMessage(reply);
    } else {
        qDebug() << Q_FUNC_INFO << "Unknown command" << command;
    }

    QDeclarativeDebugService::messageReceived(message);
}

void QJSDebugService::executionStopped(bool becauseOfException,
                                       const QString &exception)
{
    const QList<JSAgentStackData> backtrace = m_agent->backtrace();
    const QList<JSAgentWatchData> watches = m_agent->watches();
    const QList<JSAgentWatchData> locals = m_agent->locals();

    QByteArray reply;
    QDataStream rs(&reply, QIODevice::WriteOnly);
    rs << QByteArray("STOPPED") << backtrace << watches << locals
       << becauseOfException << exception;
    sendMessage(reply);
}

/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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

#include <cstdio>

#include <QBuffer>
#include <QStringList>
#include <QtDebug>
#include <QtTest>
#include <QtGlobal>
#include <QXmlQuery>
#include <QXmlResultItems>

#include "ErrorHandler.h"

using namespace QPatternistSDK;

ErrorHandler *ErrorHandler::handler = 0;

void qMessageHandler(QtMsgType type, const char *description)
{
    if(type == QtDebugMsg)
    {
        std::fprintf(stderr, "%s\n", description);
        return;
    }

    QtMsgType t;

    switch(type)
    {
        case QtWarningMsg:
        {
            t = QtWarningMsg;
            break;
        }
        case QtCriticalMsg:
        {
            t = QtFatalMsg;
            break;
        }
        case QtFatalMsg:
        {
            /* We can't swallow Q_ASSERTs, we need to fail the hard way here.
             * But maybe not: when run from "patternistrunsingle" it could be an idea
             * to actually try to record it(but nevertheless fail somehow) such
             * that it gets reported. */
            std::fprintf(stderr, "Fatal error: %s\n", description);
            t = QtFatalMsg; /* Dummy, to silence a bogus compiler warning. */
            return;
        }
        case QtDebugMsg: /* This enum is handled above in the if-clause. */
        /* Fallthrough. */
        default:
        {
            Q_ASSERT(false);
            return;
        }
    }

    Q_ASSERT(ErrorHandler::handler);
    /* This message is hacky. Ideally, we should do it the same way
     * ReportContext::error() constructs messages, but this is just testing
     * code. */
    ErrorHandler::handler->message(t, QLatin1String("<p>") + QPatternist::escape(QLatin1String(description)) + QLatin1String("</p>"));
}

void ErrorHandler::installQtMessageHandler(ErrorHandler *const h)
{
    handler = h;

    if(h)
        qInstallMsgHandler(qMessageHandler);
    else
        qInstallMsgHandler(0);
}

void ErrorHandler::handleMessage(QtMsgType type,
                                 const QString &description,
                                 const QUrl &identifier,
                                 const QSourceLocation &)
{
    /* Don't use pDebug() in this function, it results in infinite
     * recursion. Talking from experience.. */

    Message msg;
    msg.setType(type);
    msg.setIdentifier(identifier);

    /* Let's remove all the XHTML markup. */
    QBuffer buffer;
    buffer.setData(description.toLatin1());
    buffer.open(QIODevice::ReadOnly);

    QXmlQuery query;
    query.bindVariable(QLatin1String("desc"), &buffer);
    query.setQuery(QLatin1String("string(doc($desc))"));

    QStringList result;
    const bool success = query.evaluateTo(&result);

    if(!description.startsWith(QLatin1String("\"Test-suite harness error:")))
    {
        const QString msg(QString::fromLatin1("Invalid description: %1").arg(description));
        QVERIFY2(success, qPrintable(msg));

        if(!success)
            QTextStream(stderr) << msg;
    }


    if(!result.isEmpty())
        msg.setDescription(result.first());

    m_messages.append(msg);
}

ErrorHandler::Message::List ErrorHandler::messages() const
{
    return m_messages;
}

void ErrorHandler::reset()
{
    m_messages.clear();
}

// vim: et:ts=4:sw=4:sts=4

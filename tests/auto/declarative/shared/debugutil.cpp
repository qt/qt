/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
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
** $QT_END_LICENSE$
**
****************************************************************************/
#include <QSignalSpy>
#include <QEventLoop>
#include <QTimer>

#include <private/qdeclarativedebugclient_p.h>
#include <private/qdeclarativedebugservice_p.h>

#include "debugutil_p.h"


bool QDeclarativeDebugTest::waitForSignal(QObject *receiver, const char *member, int timeout) {
    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    QObject::connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    QObject::connect(receiver, member, &loop, SLOT(quit()));
    timer.start(timeout);
    loop.exec();
    return timer.isActive();
}

QDeclarativeDebugTestService::QDeclarativeDebugTestService(const QString &s, QObject *parent)
    : QDeclarativeDebugService(s, parent)
{
}

void QDeclarativeDebugTestService::messageReceived(const QByteArray &ba)
{
    sendMessage(ba);
}

void QDeclarativeDebugTestService::statusChanged(Status)
{
    emit statusHasChanged();
}


QDeclarativeDebugTestClient::QDeclarativeDebugTestClient(const QString &s, QDeclarativeDebugConnection *c)
    : QDeclarativeDebugClient(s, c)
{
}

QByteArray QDeclarativeDebugTestClient::waitForResponse()
{
    lastMsg.clear();
    QDeclarativeDebugTest::waitForSignal(this, SIGNAL(serverMessage(QByteArray)));
    if (lastMsg.isEmpty()) {
        qWarning() << "tst_QDeclarativeDebugClient: no response from server!";
        return QByteArray();
    }
    return lastMsg;
}

void QDeclarativeDebugTestClient::statusChanged(Status stat)
{
    QCOMPARE(stat, status());
    emit statusHasChanged();
}

void QDeclarativeDebugTestClient::messageReceived(const QByteArray &ba)
{
    lastMsg = ba;
    emit serverMessage(ba);
}

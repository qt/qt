/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtNetwork module of the Qt Toolkit.
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

//#define QHOSTINFO_DEBUG

// Symbian Headers
#include <es_sock.h>
#include <in_sock.h>

// Qt Headers
#include <QByteArray>
#include <QUrl>
#include <QList>

#include "qplatformdefs.h"

#include "qhostinfo_p.h"
#include <private/qcore_symbian_p.h>

QT_BEGIN_NAMESPACE


QHostInfo QHostInfoAgent::fromName(const QString &hostName)
{
    QHostInfo results;

    // Connect to ESOCK
    RSocketServ socketServ(qt_symbianGetSocketServer());
    RHostResolver hostResolver;

    // Will return both IPv4 and IPv6
    // TODO: Pass RHostResolver.Open() the global RConnection
    int err = hostResolver.Open(socketServ, KAfInet, KProtocolInetUdp);
    if (err) {
        results.setError(QHostInfo::UnknownError);
        results.setErrorString(tr("Symbian error code: %1").arg(err));

        return results;
    }

    TNameEntry nameResult;

#if defined(QHOSTINFO_DEBUG)
    qDebug("QHostInfoAgent::fromName(%s) looking up...",
           hostName.toLatin1().constData());
#endif

    QHostAddress address;
    if (address.setAddress(hostName)) {
        // Reverse lookup

        TInetAddr IpAdd;
        IpAdd.Input(qt_QString2TPtrC(hostName));

        // Synchronous request. nameResult returns Host Name.
        hostResolver.GetByAddress(IpAdd, nameResult);
        if (err) {
            // TODO - Could there be other errors? Symbian docs don't say.
            if (err = KErrNotFound) {
                results.setError(QHostInfo::HostNotFound);
                results.setErrorString(tr("Host not found"));
            } else {
                results.setError(QHostInfo::UnknownError);
                results.setErrorString(tr("Symbian error code: %1").arg(err));
            }

            return results;
        }

        results.setHostName(qt_TDesC2QString(nameResult().iName));
        results.setAddresses(QList<QHostAddress>() << address);
        return results;
    }

    // IDN support
    QByteArray aceHostname = QUrl::toAce(hostName);
    results.setHostName(hostName);
    if (aceHostname.isEmpty()) {
        results.setError(QHostInfo::HostNotFound);
        results.setErrorString(hostName.isEmpty() ?
                               QCoreApplication::translate("QHostInfoAgent", "No host name given") :
                               QCoreApplication::translate("QHostInfoAgent", "Invalid hostname"));
        return results;
    }


    // Call RHostResolver::GetByAddress, and place all IPv4 addresses at the start and
    // the IPv6 addresses at the end of the address list in results.

    // Synchronous request.
    err = hostResolver.GetByName(qt_QString2TPtrC(QString::fromLatin1(aceHostname)), nameResult);
    if (err) {
        // TODO - Could there be other errors? Symbian docs don't say.
        if (err = KErrNotFound) {
            results.setError(QHostInfo::HostNotFound);
            results.setErrorString(tr("Host not found"));
        } else {
            results.setError(QHostInfo::UnknownError);
            results.setErrorString(tr("Symbian error code: %1").arg(err));
        }

        return results;
    }

    QList<QHostAddress> hostAddresses;

    TInetAddr hostAdd = nameResult().iAddr;
    // 39 is the maximum length of an IPv6 address.
    TBuf<39> ipAddr;

    // Fill ipAddr with the IP address from hostAdd
    hostAdd.Output(ipAddr);
    if (ipAddr.Length() > 0)
        hostAddresses.append(QHostAddress(qt_TDesC2QString(ipAddr)));

    // Check if there's more than one IP address linkd to this name
    while (hostResolver.Next(nameResult) == KErrNone) {
        hostAdd = nameResult().iAddr;
        hostAdd.Output(ipAddr);

        if (ipAddr.Length() > 0) {
           if (nameResult().iAddr.Family() == KAfInet) {
                // IPv4 - prepend
                hostAddresses.prepend(QHostAddress(qt_TDesC2QString(ipAddr)));
            } else {
                // IPv6 - append
                hostAddresses.append(QHostAddress(qt_TDesC2QString(ipAddr)));
            }
        }
    }

    hostResolver.Close();

    results.setAddresses(hostAddresses);
    return results;
}

QString QHostInfo::localHostName()
{
    // Connect to ESOCK
    RSocketServ socketServ(qt_symbianGetSocketServer());
    RHostResolver hostResolver;

    // Will return both IPv4 and IPv6
    // TODO: Pass RHostResolver.Open() the global RConnection
    int err = hostResolver.Open(socketServ, KAfInet, KProtocolInetUdp);
    if (err)
        return QString();

    THostName hostName;
    err = hostResolver.GetHostName(hostName);
    if (err)
        return QString();

    hostResolver.Close();

    return qt_TDesC2QString(hostName);
}

QString QHostInfo::localDomainName()
{
    // TODO - fill with code.
    return QString();
}

QT_END_NAMESPACE

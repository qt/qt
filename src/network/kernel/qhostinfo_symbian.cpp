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

#include "qplatformdefs.h"

#include "qhostinfo_p.h"

QT_BEGIN_NAMESPACE

QHostInfo QHostInfoAgent::fromName(const QString &hostName)
{
    QHostInfo results;

#if defined(QHOSTINFO_DEBUG)
    qDebug("QHostInfoAgent::fromName(%s) looking up...",
           hostName.toLatin1().constData());
#endif

    QHostAddress address;
    if (address.setAddress(hostName)) {
        // Reverse lookup
        // TODO
        results.setHostName("assume.it.works");
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

    // Call getaddrinfo, and place all IPv4 addresses at the start and
    // the IPv6 addresses at the end of the address list in results.
    /*
    results.setError(QHostInfo::HostNotFound);
    results.setErrorString(tr("Host not found"));
    } else {
        results.setError(QHostInfo::UnknownError);
        results.setErrorString(QString::fromLocal8Bit(gai_strerror(result)));
    }
    */
	
    return results;
}

QString QHostInfo::localHostName()
{
    return QString()
}

QString QHostInfo::localDomainName()
{
    return QString();
}

QT_END_NAMESPACE

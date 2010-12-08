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

#include "qnetworkproxy.h"

#ifndef QT_NO_NETWORKPROXY

QT_BEGIN_NAMESPACE

QList<QNetworkProxy> QNetworkProxyFactory::systemProxyForQuery(const QNetworkProxyQuery &)
{
    // TODO: Get the current QNetworkSession which has the Symbian RConnection we use
    // I am wondering if we already have a connected QNetworkSession when the code
    // is run that retrieves the proxy (for QNetworkAccessManager it's somewhere called
    // from createRequest() which might be too early...)

    // TODO: Get the proxy from that RConnection

    // The QNetworkProxyQuery could have a QNetworkSession and then take the RConnection
    // from there. If it does not have one, we have to use the "global RConnection".

    // See http://bugreports.qt.nokia.com/browse/QTBUG-13857 and http://bugreports.qt.nokia.com/browse/QTBUG-11016
    // and the mails we have received.

    // Default case: No network proxy found/needed
    return QList<QNetworkProxy>() << QNetworkProxy::NoProxy;
}

QT_END_NAMESPACE

#endif

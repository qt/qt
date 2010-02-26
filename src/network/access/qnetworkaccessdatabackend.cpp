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

#include "qnetworkaccessdatabackend_p.h"
#include "qnetworkrequest.h"
#include "qnetworkreply.h"
#include "qurlinfo.h"
#include "private/qdataurl_p.h"
#include <qcoreapplication.h>

QT_BEGIN_NAMESPACE

QNetworkAccessBackend *
QNetworkAccessDataBackendFactory::create(QNetworkAccessManager::Operation,
                                         const QNetworkRequest &request) const
{
    if (request.url().scheme() == QLatin1String("data"))
        return new QNetworkAccessDataBackend;

    return 0;
}

QNetworkAccessDataBackend::QNetworkAccessDataBackend()
{
}

QNetworkAccessDataBackend::~QNetworkAccessDataBackend()
{
}

void QNetworkAccessDataBackend::open()
{
    QUrl uri = request().url();

    if (operation() != QNetworkAccessManager::GetOperation &&
        operation() != QNetworkAccessManager::HeadOperation) {
        // data: doesn't support anything but GET
        const QString msg = QCoreApplication::translate("QNetworkAccessDataBackend",
                                                        "Operation not supported on %1")
                      .arg(uri.toString());
        error(QNetworkReply::ContentOperationNotPermittedError, msg);
        finished();
        return;
    }

    QPair<QString, QByteArray> decoded = qDecodeDataUrl(uri);

    if (! decoded.first.isNull()) {
        setHeader(QNetworkRequest::ContentTypeHeader, decoded.first);
        setHeader(QNetworkRequest::ContentLengthHeader, decoded.second.size());
        emit metaDataChanged();

        QByteDataBuffer list;
        list.append(decoded.second);
        decoded.second.clear(); // important because of implicit sharing!
        writeDownstreamData(list);

        finished();
        return;
    }

    // something wrong with this URI
    const QString msg = QCoreApplication::translate("QNetworkAccessDataBackend",
                                                    "Invalid URI: %1").arg(uri.toString());
    error(QNetworkReply::ProtocolFailure, msg);
    finished();
}

void QNetworkAccessDataBackend::closeDownstreamChannel()
{
}

void QNetworkAccessDataBackend::closeUpstreamChannel()
{
}

bool QNetworkAccessDataBackend::waitForDownstreamReadyRead(int)
{
    return false;
}

bool QNetworkAccessDataBackend::waitForUpstreamBytesWritten(int)
{
    return false;
}

QT_END_NAMESPACE

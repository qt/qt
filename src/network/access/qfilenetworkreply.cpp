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

#include "qfilenetworkreply_p.h"

#include "QtCore/qdatetime.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QFileInfo>
#include <QDebug>

QT_BEGIN_NAMESPACE

QFileNetworkReplyPrivate::QFileNetworkReplyPrivate()
    : QNetworkReplyPrivate(), realFileSize(0)
{
}

QFileNetworkReply::~QFileNetworkReply()
{
}

QFileNetworkReply::QFileNetworkReply(QObject *parent, const QNetworkRequest &req, const QNetworkAccessManager::Operation op)
    : QNetworkReply(*new QFileNetworkReplyPrivate(), parent)
{
    setRequest(req);
    setUrl(req.url());
    setOperation(op);
    QNetworkReply::open(QIODevice::ReadOnly);

    qRegisterMetaType<QNetworkReply::NetworkError>("QNetworkReply::NetworkError");

    QFileNetworkReplyPrivate *d = (QFileNetworkReplyPrivate*) d_func();

    QUrl url = req.url();
    if (url.host() == QLatin1String("localhost"))
        url.setHost(QString());

#if !defined(Q_OS_WIN)
    // do not allow UNC paths on Unix
    if (!url.host().isEmpty()) {
        // we handle only local files
        QString msg = QCoreApplication::translate("QNetworkAccessFileBackend", "Request for opening non-local file %1").arg(url.toString());
        setError(QNetworkReply::ProtocolInvalidOperationError, msg);
        QMetaObject::invokeMethod(this, "error", Qt::QueuedConnection,
            Q_ARG(QNetworkReply::NetworkError, QNetworkReply::ProtocolInvalidOperationError));
        QMetaObject::invokeMethod(this, "finished", Qt::QueuedConnection);
        return;
    }
#endif
    if (url.path().isEmpty())
        url.setPath(QLatin1String("/"));
    setUrl(url);


    QString fileName = url.toLocalFile();
    if (fileName.isEmpty()) {
        fileName = url.toString(QUrl::RemoveAuthority | QUrl::RemoveFragment | QUrl::RemoveQuery);
    }
    d->realFile.setFileName(fileName);

    QFileInfo fi(d->realFile);
    if (fi.isDir()) {
        QString msg = QCoreApplication::translate("QNetworkAccessFileBackend", "Cannot open %1: Path is a directory").arg(url.toString());
        setError(QNetworkReply::ContentOperationNotPermittedError, msg);
        QMetaObject::invokeMethod(this, "error", Qt::QueuedConnection,
            Q_ARG(QNetworkReply::NetworkError, QNetworkReply::ContentOperationNotPermittedError));
        QMetaObject::invokeMethod(this, "finished", Qt::QueuedConnection);
        return;
    }

    bool opened = d->realFile.open(QIODevice::ReadOnly | QIODevice::Unbuffered);

    // could we open the file?
    if (!opened) {
        QString msg = QCoreApplication::translate("QNetworkAccessFileBackend", "Error opening %1: %2")
                      .arg(d->realFile.fileName(), d->realFile.errorString());

        if (d->realFile.exists()) {
            setError(QNetworkReply::ContentAccessDenied, msg);
            QMetaObject::invokeMethod(this, "error", Qt::QueuedConnection,
                Q_ARG(QNetworkReply::NetworkError, QNetworkReply::ContentAccessDenied));
        } else {
            setError(QNetworkReply::ContentNotFoundError, msg);
            QMetaObject::invokeMethod(this, "error", Qt::QueuedConnection,
                Q_ARG(QNetworkReply::NetworkError, QNetworkReply::ContentNotFoundError));
        }
        QMetaObject::invokeMethod(this, "finished", Qt::QueuedConnection);
        return;
    }

    d->realFileSize = fi.size();
    setHeader(QNetworkRequest::LastModifiedHeader, fi.lastModified());
    setHeader(QNetworkRequest::ContentLengthHeader, d->realFileSize);

    QMetaObject::invokeMethod(this, "metaDataChanged", Qt::QueuedConnection);
    QMetaObject::invokeMethod(this, "downloadProgress", Qt::QueuedConnection,
        Q_ARG(qint64, d->realFileSize), Q_ARG(qint64, d->realFileSize));
    QMetaObject::invokeMethod(this, "readyRead", Qt::QueuedConnection);
    QMetaObject::invokeMethod(this, "finished", Qt::QueuedConnection);
}

bool QFileNetworkReplyPrivate::isFinished() const
{
    return true;
}

void QFileNetworkReply::close()
{
    Q_D(QFileNetworkReply);
    QNetworkReply::close();
    d->realFile.close();
}

void QFileNetworkReply::abort()
{
    Q_D(QFileNetworkReply);
    QNetworkReply::close();
    d->realFile.close();
}

qint64 QFileNetworkReply::bytesAvailable() const
{
    Q_D(const QFileNetworkReply);
    return QNetworkReply::bytesAvailable() + d->realFile.bytesAvailable();
}

bool QFileNetworkReply::isSequential () const
{
    return true;
}

qint64 QFileNetworkReply::size() const
{
    Q_D(const QFileNetworkReply);
    return d->realFileSize;
}

/*!
    \internal
*/
qint64 QFileNetworkReply::readData(char *data, qint64 maxlen)
{
    Q_D(QFileNetworkReply);
    qint64 ret = d->realFile.read(data, maxlen);
    if (ret == 0 && bytesAvailable() == 0)
        return -1; // everything had been read
    else
        return ret;
}


QT_END_NAMESPACE

#include "moc_qfilenetworkreply_p.cpp"


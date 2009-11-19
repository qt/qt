/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

QT_BEGIN_NAMESPACE

QFileNetworkReplyPrivate::QFileNetworkReplyPrivate()
    : QNetworkReplyPrivate(), realFileSize(0), finished(false)
{
}

QFileNetworkReply::QFileNetworkReply(QObject *parent, const QNetworkRequest &req)
    : QNetworkReply(*new QFileNetworkReplyPrivate(), parent)
{
    setRequest(req);
    setUrl(req.url());
    setOperation(QNetworkAccessManager::GetOperation);
    QMetaObject::invokeMethod(this, "_q_startOperation", Qt::QueuedConnection);
    QNetworkReply::open(QIODevice::ReadOnly);
}

QFileNetworkReply::~QFileNetworkReply()
{
}

// This code is mostly inspired by QNetworkAccessFileBackend
// We also use its translation context for error messages
void QFileNetworkReplyPrivate::_q_startOperation()
{
    Q_Q(QFileNetworkReply);

    QUrl url = q->url();
    if (url.host() == QLatin1String("localhost"))
        url.setHost(QString());

#if !defined(Q_OS_WIN)
    // do not allow UNC paths on Unix
    if (!url.host().isEmpty()) {
        // we handle only local files
        QString msg = QCoreApplication::translate("QNetworkAccessFileBackend", "Request for opening non-local file %1").arg(url.toString());
        q->setError(QNetworkReply::ProtocolInvalidOperationError, msg);
        emit q->error(QNetworkReply::ProtocolInvalidOperationError);
        doFinished();
        return;
    }
#endif
    if (url.path().isEmpty())
        url.setPath(QLatin1String("/"));
    q->setUrl(url);


    QString fileName = url.toLocalFile();
    if (fileName.isEmpty()) {
        fileName = url.toString(QUrl::RemoveAuthority | QUrl::RemoveFragment | QUrl::RemoveQuery);
    }
    realFile.setFileName(fileName);

    QFileInfo fi(realFile);
    if (fi.isDir()) {
        QString msg = QCoreApplication::translate("QNetworkAccessFileBackend", "Cannot open %1: Path is a directory").arg(url.toString());
        q->setError(QNetworkReply::ContentOperationNotPermittedError, msg);
        emit q->error(QNetworkReply::ContentOperationNotPermittedError);
        doFinished();
        return;
    }

    bool opened = realFile.open(QIODevice::ReadOnly | QIODevice::Unbuffered);

    // could we open the file?
    if (!opened) {
        QString msg = QCoreApplication::translate("QNetworkAccessFileBackend", "Error opening %1: %2")
                      .arg(realFile.fileName(), realFile.errorString());

        if (realFile.exists()) {
            q->setError(QNetworkReply::ContentAccessDenied, msg);
            emit q->error(QNetworkReply::ContentAccessDenied);
        } else {
            q->setError(QNetworkReply::ContentNotFoundError, msg);
            emit q->error(QNetworkReply::ContentNotFoundError);
        }
        doFinished();
        return;
    }

    realFileSize = fi.size();
    q->setHeader(QNetworkRequest::LastModifiedHeader, fi.lastModified());
    q->setHeader(QNetworkRequest::ContentLengthHeader, realFileSize);

    emit q->metaDataChanged();
    emit q->downloadProgress(realFileSize, realFileSize);
    emit q->readyRead();
    doFinished();
}

bool QFileNetworkReplyPrivate::isFinished() const
{
    return finished;
}

void QFileNetworkReplyPrivate::doFinished()
{
    Q_Q(QFileNetworkReply);
    finished = true;
    emit q->finished();
}


void QFileNetworkReply::close()
{
    Q_D(QFileNetworkReply);
    QNetworkReply::close();
    d->realFile.close();

    if (!d->finished)
        d->doFinished();
}

void QFileNetworkReply::abort()
{
    Q_D(QFileNetworkReply);
    QNetworkReply::close();
    d->realFile.close();

    if (!d->finished)
        d->doFinished();
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


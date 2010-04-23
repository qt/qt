/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QDECLARATIVEPIXMAPCACHE_H
#define QDECLARATIVEPIXMAPCACHE_H

#include <QtCore/QString>
#include <QtGui/QPixmap>
#include <QtCore/qurl.h>
#include <QtCore/QCoreApplication>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)
class QDeclarativeEngine;
class QNetworkReply;
class QDeclarativeImageReader;

class QDeclarativePixmapReplyPrivate;
class Q_DECLARATIVE_EXPORT QDeclarativePixmapReply : public QObject
{
    Q_OBJECT
public:
    ~QDeclarativePixmapReply();

    enum Status { Ready, Error, Unrequested, Loading };
    Status status() const;
    QString errorString() const;

    const QUrl &url() const;
    int forcedWidth() const;
    int forcedHeight() const;
    QSize implicitSize() const;

Q_SIGNALS:
    void finished();
    void downloadProgress(qint64, qint64);

protected:
    bool event(QEvent *event);

private:
    void addRef();
    bool release(bool defer=false);
    bool isLoading() const;
    void setLoading();

private:
    QDeclarativePixmapReply(QDeclarativeImageReader *reader, const QUrl &url, int req_width, int req_height);
    Q_DISABLE_COPY(QDeclarativePixmapReply)
    Q_DECLARE_PRIVATE(QDeclarativePixmapReply)
    friend class QDeclarativeImageRequestHandler;
    friend class QDeclarativeImageReader;
    friend class QDeclarativePixmapCache;
};

class Q_DECLARATIVE_EXPORT QDeclarativePixmapCache
{
    Q_DECLARE_TR_FUNCTIONS(QDeclarativePixmapCache)
public:
    static QDeclarativePixmapReply::Status get(const QUrl& url, QPixmap *pixmap, QString *errorString, QSize *impsize=0, bool async=false, int req_width=0, int req_height=0);
    static QDeclarativePixmapReply *request(QDeclarativeEngine *, const QUrl& url, int req_width=0, int req_height=0);
    static void cancel(const QUrl& url, QObject *obj);
    static int pendingRequests();
};



QT_END_NAMESPACE

QT_END_HEADER

#endif // QDECLARATIVEPIXMAPCACHE_H

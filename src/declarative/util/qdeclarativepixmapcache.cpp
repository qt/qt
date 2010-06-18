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

#include "private/qdeclarativepixmapcache_p.h"
#include "qdeclarativenetworkaccessmanagerfactory.h"
#include "qdeclarativeimageprovider.h"

#include <qdeclarativeengine.h>
#include <private/qdeclarativeglobal_p.h>
#include <private/qdeclarativeengine_p.h>

#include <QCoreApplication>
#include <QImageReader>
#include <QHash>
#include <QNetworkReply>
#include <QPixmapCache>
#include <QFile>
#include <QThread>
#include <QMutex>
#include <QBuffer>
#include <QWaitCondition>
#include <QtCore/qdebug.h>
#include <private/qobject_p.h>
#include <QSslError>

// Maximum number of simultaneous image requests to send.
static const int maxImageRequestCount = 8;

QT_BEGIN_NAMESPACE

class QDeclarativeImageReaderEvent : public QEvent
{
public:
    enum ReadError { NoError, Loading, Decoding };

    QDeclarativeImageReaderEvent(QDeclarativeImageReaderEvent::ReadError err, const QString &errStr, const QImage &img)
        : QEvent(QEvent::User), error(err), errorString(errStr), image(img) {}

    ReadError error;
    QString errorString;
    QImage image;
};

class QDeclarativeImageRequestHandler;
class QDeclarativeImageReader : public QThread
{
    Q_OBJECT
public:
    QDeclarativeImageReader(QDeclarativeEngine *eng);
    ~QDeclarativeImageReader();

    QDeclarativePixmapReply *getImage(const QUrl &url, int req_width, int req_height);
    void cancel(QDeclarativePixmapReply *rep);

    static QDeclarativeImageReader *instance(QDeclarativeEngine *engine);

protected:
    void run();

private:
    QList<QDeclarativePixmapReply*> jobs;
    QList<QDeclarativePixmapReply*> cancelled;
    QDeclarativeEngine *engine;
    QDeclarativeImageRequestHandler *handler;
    QObject *eventLoopQuitHack;
    QMutex mutex;

    static QHash<QDeclarativeEngine *,QDeclarativeImageReader*> readers;
    static QMutex readerMutex;
    friend class QDeclarativeImageRequestHandler;
};

QHash<QDeclarativeEngine *,QDeclarativeImageReader*> QDeclarativeImageReader::readers;
QMutex QDeclarativeImageReader::readerMutex;


class QDeclarativeImageRequestHandler : public QObject
{
    Q_OBJECT
public:
    QDeclarativeImageRequestHandler(QDeclarativeImageReader *read, QDeclarativeEngine *eng)
        : QObject(), accessManager(0), engine(eng), reader(read), redirectCount(0)
    {
        QCoreApplication::postEvent(this, new QEvent(QEvent::User));
    }

    QDeclarativePixmapReply *getImage(const QUrl &url, int req_width, int req_height);
    void cancel(QDeclarativePixmapReply *reply);

protected:
    bool event(QEvent *event);

private slots:
    void networkRequestDone();

private:
    QNetworkAccessManager *networkAccessManager() {
        if (!accessManager)
            accessManager = QDeclarativeEnginePrivate::get(engine)->createNetworkAccessManager(this);
        return accessManager;
    }

    QHash<QNetworkReply*,QDeclarativePixmapReply*> replies;
    QNetworkAccessManager *accessManager;
    QDeclarativeEngine *engine;
    QDeclarativeImageReader *reader;
    int redirectCount;

    static int replyDownloadProgress;
    static int replyFinished;
    static int downloadProgress;
    static int thisNetworkRequestDone;
};

//===========================================================================

static bool readImage(const QUrl& url, QIODevice *dev, QImage *image, QString *errorString, QSize *impsize, int req_width, int req_height)
{
    QImageReader imgio(dev);

    bool force_scale = false;
    if (url.path().endsWith(QLatin1String(".svg"),Qt::CaseInsensitive)) {
        imgio.setFormat("svg"); // QSvgPlugin::capabilities bug QTBUG-9053
        force_scale = true;
    }

    bool scaled = false;
    if (req_width > 0 || req_height > 0) {
        QSize s = imgio.size();
        if (req_width && (force_scale || req_width < s.width())) {
            if (req_height <= 0)
                s.setHeight(s.height()*req_width/s.width());
            s.setWidth(req_width); scaled = true;
        }
        if (req_height && (force_scale || req_height < s.height())) {
            if (req_width <= 0)
                s.setWidth(s.width()*req_height/s.height());
            s.setHeight(req_height); scaled = true;
        }
        if (scaled) { imgio.setScaledSize(s); }
    }

    if (impsize)
        *impsize = imgio.size();

    if (imgio.read(image)) {
        if (impsize && impsize->width() < 0)
            *impsize = image->size();
        return true;
    } else {
        if (errorString)
            *errorString = QDeclarativePixmapCache::tr("Error decoding: %1: %2").arg(url.toString())
                                .arg(imgio.errorString());
        return false;
    }
}


//===========================================================================

int QDeclarativeImageRequestHandler::replyDownloadProgress = -1;
int QDeclarativeImageRequestHandler::replyFinished = -1;
int QDeclarativeImageRequestHandler::downloadProgress = -1;
int QDeclarativeImageRequestHandler::thisNetworkRequestDone = -1;

typedef QHash<QUrl, QSize> QDeclarativePixmapSizeHash;
Q_GLOBAL_STATIC(QDeclarativePixmapSizeHash, qmlOriginalSizes);

bool QDeclarativeImageRequestHandler::event(QEvent *event)
{
    if (event->type() == QEvent::User) {
        if (replyDownloadProgress == -1) {
            replyDownloadProgress = QNetworkReply::staticMetaObject.indexOfSignal("downloadProgress(qint64,qint64)");
            replyFinished = QNetworkReply::staticMetaObject.indexOfSignal("finished()");
            downloadProgress = QDeclarativePixmapReply::staticMetaObject.indexOfSignal("downloadProgress(qint64,qint64)");
            thisNetworkRequestDone = QDeclarativeImageRequestHandler::staticMetaObject.indexOfSlot("networkRequestDone()");
        }

        while (1) {
            reader->mutex.lock();

            if (reader->cancelled.count()) {
                for (int i = 0; i < reader->cancelled.count(); ++i) {
                    QDeclarativePixmapReply *job = reader->cancelled.at(i);
                    // cancel any jobs already started
                    QNetworkReply *reply = replies.key(job, 0);
                    if (reply && reply->isRunning()) {
                        replies.remove(reply);
                        reply->close();
                    }
                    // remove from pending job list
                    for (int j = 0; j < reader->jobs.count(); ++j) {
                        if (reader->jobs.at(j) == job) {
                            reader->jobs.removeAt(j);
                            job->release(true);
                            break;
                        }
                    }
                }
                reader->cancelled.clear();
            }

            if (!reader->jobs.count() || replies.count() > maxImageRequestCount) {
                reader->mutex.unlock();
                break;
            }

            QDeclarativePixmapReply *runningJob = reader->jobs.takeLast();
            QUrl url = runningJob->url();
            reader->mutex.unlock();

            // fetch
            if (url.scheme() == QLatin1String("image")) {
                // Use QmlImageProvider
                QSize read_impsize;
                QImage image = QDeclarativeEnginePrivate::get(engine)->getImageFromProvider(url, &read_impsize, QSize(runningJob->forcedWidth(),runningJob->forcedHeight()));
                qmlOriginalSizes()->insert(url, read_impsize);
                QDeclarativeImageReaderEvent::ReadError errorCode = QDeclarativeImageReaderEvent::NoError;
                QString errorStr;
                if (image.isNull()) {
                    errorCode = QDeclarativeImageReaderEvent::Loading;
                    errorStr = QDeclarativePixmapCache::tr("Failed to get image from provider: %1").arg(url.toString());
                }
                QCoreApplication::postEvent(runningJob, new QDeclarativeImageReaderEvent(errorCode, errorStr, image));
            } else {
                QString lf = QDeclarativeEnginePrivate::urlToLocalFileOrQrc(url);
                if (!lf.isEmpty()) {
                    // Image is local - load/decode immediately
                    QImage image;
                    QDeclarativeImageReaderEvent::ReadError errorCode = QDeclarativeImageReaderEvent::NoError;
                    QString errorStr;
                    QFile f(lf);
                    if (f.open(QIODevice::ReadOnly)) {
                        QSize read_impsize;
                        if (readImage(url, &f, &image, &errorStr, &read_impsize, runningJob->forcedWidth(),runningJob->forcedHeight())) {
                            qmlOriginalSizes()->insert(url, read_impsize);
                        } else {
                            errorCode = QDeclarativeImageReaderEvent::Loading;
                        }
                    } else {
                        errorStr = QDeclarativePixmapCache::tr("Cannot open: %1").arg(url.toString());
                        errorCode = QDeclarativeImageReaderEvent::Loading;
                    }
                    QCoreApplication::postEvent(runningJob, new QDeclarativeImageReaderEvent(errorCode, errorStr, image));
                } else {
                    // Network resource
                    QNetworkRequest req(url);
                    req.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);
                    QNetworkReply *reply = networkAccessManager()->get(req);

                    QMetaObject::connect(reply, replyDownloadProgress, runningJob, downloadProgress);
                    QMetaObject::connect(reply, replyFinished, this, thisNetworkRequestDone);

                    replies.insert(reply, runningJob);
                }
            }
        }
        return true;
    }

    return QObject::event(event);
}

#define IMAGEREQUESTHANDLER_MAX_REDIRECT_RECURSION 16

void QDeclarativeImageRequestHandler::networkRequestDone()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    QDeclarativePixmapReply *job = replies.take(reply);

    redirectCount++;
    if (redirectCount < IMAGEREQUESTHANDLER_MAX_REDIRECT_RECURSION) {
        QVariant redirect = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
        if (redirect.isValid()) {
            QUrl url = reply->url().resolved(redirect.toUrl());
            QNetworkRequest req(url);
            req.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);

            reply->deleteLater();
            reply = networkAccessManager()->get(req);

            QMetaObject::connect(reply, replyDownloadProgress, job, downloadProgress);
            QMetaObject::connect(reply, replyFinished, this, thisNetworkRequestDone);

            replies.insert(reply, job);
            return;
        }
    }
    redirectCount=0;

    if (job) {
        QImage image;
        QDeclarativeImageReaderEvent::ReadError error;
        QString errorString;
        if (reply->error()) {
            error = QDeclarativeImageReaderEvent::Loading;
            errorString = reply->errorString();
        } else {
            QSize read_impsize;
            QByteArray all = reply->readAll();
            QBuffer buff(&all);
            buff.open(QIODevice::ReadOnly);
            if (readImage(reply->url(), &buff, &image, &errorString, &read_impsize, job->forcedWidth(), job->forcedHeight())) {
                qmlOriginalSizes()->insert(reply->url(), read_impsize);
                error = QDeclarativeImageReaderEvent::NoError;
            } else {
                error = QDeclarativeImageReaderEvent::Decoding;
            }
        }
        // send completion event to the QDeclarativePixmapReply
        QCoreApplication::postEvent(job, new QDeclarativeImageReaderEvent(error, errorString, image));
    }
    // kick off event loop again if we have dropped below max request count
    if (replies.count() == maxImageRequestCount)
        QCoreApplication::postEvent(this, new QEvent(QEvent::User));
    reply->deleteLater();
}

//===========================================================================

QDeclarativeImageReader::QDeclarativeImageReader(QDeclarativeEngine *eng)
    : QThread(eng), engine(eng), handler(0)
{
    eventLoopQuitHack = new QObject;
    eventLoopQuitHack->moveToThread(this);
    connect(eventLoopQuitHack, SIGNAL(destroyed(QObject*)), SLOT(quit()), Qt::DirectConnection);
    start(QThread::IdlePriority);
}

QDeclarativeImageReader::~QDeclarativeImageReader()
{
    readerMutex.lock();
    readers.remove(engine);
    readerMutex.unlock();

    eventLoopQuitHack->deleteLater();
    wait();
}

QDeclarativeImageReader *QDeclarativeImageReader::instance(QDeclarativeEngine *engine)
{
    readerMutex.lock();
    QDeclarativeImageReader *reader = readers.value(engine);
    if (!reader) {
        reader = new QDeclarativeImageReader(engine);
        readers.insert(engine, reader);
    }
    readerMutex.unlock();

    return reader;
}

QDeclarativePixmapReply *QDeclarativeImageReader::getImage(const QUrl &url, int req_width, int req_height)
{
    mutex.lock();
    QDeclarativePixmapReply *reply = new QDeclarativePixmapReply(this, url, req_width, req_height);
    reply->addRef();
    reply->setLoading();
    jobs.append(reply);
    if (jobs.count() == 1 && handler)
        QCoreApplication::postEvent(handler, new QEvent(QEvent::User));
    mutex.unlock();
    return reply;
}

void QDeclarativeImageReader::cancel(QDeclarativePixmapReply *reply)
{
    mutex.lock();
    if (reply->isLoading()) {
        // Add to cancel list to be cancelled in reader thread.
        cancelled.append(reply);
        if (cancelled.count() == 1 && handler)
            QCoreApplication::postEvent(handler, new QEvent(QEvent::User));
    }
    mutex.unlock();
}

void QDeclarativeImageReader::run()
{
    readerMutex.lock();
    handler = new QDeclarativeImageRequestHandler(this, engine);
    readerMutex.unlock();

    exec();

    delete handler;
    handler = 0;
}

//===========================================================================

/*!
    \internal
    \class QDeclarativePixmapCache
    \brief Enacapsultes a pixmap for QDeclarativeGraphics items.

    This class is NOT reentrant.
 */

typedef QHash<QUrl, QDeclarativePixmapReply *> QDeclarativePixmapReplyHash;
Q_GLOBAL_STATIC(QDeclarativePixmapReplyHash, qmlActivePixmapReplies);

class QDeclarativePixmapReplyPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QDeclarativePixmapReply)

public:
    QDeclarativePixmapReplyPrivate(QDeclarativeImageReader *r, const QUrl &u, int req_width, int req_height)
        : QObjectPrivate(), refCount(1), url(u), status(QDeclarativePixmapReply::Loading), loading(false), reader(r),
            forced_width(req_width), forced_height(req_height)
    {
    }

    int refCount;
    QUrl url;
    QPixmap pixmap; // ensure reference to pixmap so QPixmapCache does not discard
    QDeclarativePixmapReply::Status status;
    bool loading;
    QDeclarativeImageReader *reader;
    int forced_width, forced_height;
    QString errorString;
};


QDeclarativePixmapReply::QDeclarativePixmapReply(QDeclarativeImageReader *reader, const QUrl &url, int req_width, int req_height)
  : QObject(*new QDeclarativePixmapReplyPrivate(reader, url, req_width, req_height), 0)
{
}

QDeclarativePixmapReply::~QDeclarativePixmapReply()
{
}

const QUrl &QDeclarativePixmapReply::url() const
{
    Q_D(const QDeclarativePixmapReply);
    return d->url;
}

int QDeclarativePixmapReply::forcedWidth() const
{
    Q_D(const QDeclarativePixmapReply);
    return d->forced_width;
}

int QDeclarativePixmapReply::forcedHeight() const
{
    Q_D(const QDeclarativePixmapReply);
    return d->forced_height;
}

QSize QDeclarativePixmapReply::implicitSize() const
{
    Q_D(const QDeclarativePixmapReply);
    QDeclarativePixmapSizeHash::Iterator iter = qmlOriginalSizes()->find(d->url);
    if (iter != qmlOriginalSizes()->end())
        return *iter;
    else
        return QSize();
}

bool QDeclarativePixmapReply::event(QEvent *event)
{
    Q_D(QDeclarativePixmapReply);
    if (event->type() == QEvent::User) {
        d->loading = false;
        if (!release(true)) {
            QDeclarativeImageReaderEvent *de = static_cast<QDeclarativeImageReaderEvent*>(event);
            d->status = (de->error == QDeclarativeImageReaderEvent::NoError) ? Ready : Error;
            if (d->status == Ready)
                d->pixmap = QPixmap::fromImage(de->image);
            else
                d->errorString =  de->errorString;
            QByteArray key = d->url.toEncoded(QUrl::FormattingOption(0x100));
            QString strKey = QString::fromLatin1(key.constData(), key.count());
            QPixmapCache::insert(strKey, d->pixmap); // note: may fail (returns false)
            emit finished();
        }
        return true;
    }

    return QObject::event(event);
}

QString QDeclarativePixmapReply::errorString() const
{
    Q_D(const QDeclarativePixmapReply);
    return d->errorString;
}

QDeclarativePixmapReply::Status QDeclarativePixmapReply::status() const
{
    Q_D(const QDeclarativePixmapReply);
    return d->status;
}

bool QDeclarativePixmapReply::isLoading() const
{
    Q_D(const QDeclarativePixmapReply);
    return d->loading;
}

void QDeclarativePixmapReply::setLoading()
{
    Q_D(QDeclarativePixmapReply);
    d->loading = true;
}

void QDeclarativePixmapReply::addRef()
{
    Q_D(QDeclarativePixmapReply);
    ++d->refCount;
}

bool QDeclarativePixmapReply::release(bool defer)
{
    Q_D(QDeclarativePixmapReply);
    Q_ASSERT(d->refCount > 0);
    --d->refCount;
    if (d->refCount == 0) {
        qmlActivePixmapReplies()->remove(d->url);
        if (defer)
            deleteLater();
        else
            delete this;
        return true;
    } else if (d->refCount == 1 && d->loading) {
        // The only reference left is the reader thread.
        qmlActivePixmapReplies()->remove(d->url);
        d->reader->cancel(this);
    }

    return false;
}

/*!
    Finds the cached pixmap corresponding to \a url.
    If the image is a network resource and has not yet
    been retrieved and cached, request() must be called.

    Returns Ready, or Error if the image has been retrieved,
    otherwise the current retrieval status.

    If \a async is false the image will be loaded and decoded immediately;
    otherwise the image will be loaded and decoded in a separate thread.

    If \a req_width and \a req_height are non-zero, they are used for
    the size of the rendered pixmap rather than the intrinsic size of the image.
    Different request sizes add different cache items.

    Note that images sourced from the network will always be loaded and
    decoded asynchonously.
*/
QDeclarativePixmapReply::Status QDeclarativePixmapCache::get(const QUrl& url, QPixmap *pixmap, QString *errorString, QSize *impsize, bool async, int req_width, int req_height)
{
    QDeclarativePixmapReply::Status status = QDeclarativePixmapReply::Unrequested;
    QByteArray key = url.toEncoded(QUrl::FormattingOption(0x100));

    if (req_width > 0 || req_height > 0) {
        key += ':';
        key += QByteArray::number(req_width);
        key += 'x';
        key += QByteArray::number(req_height);
    }

    QString strKey = QString::fromLatin1(key.constData(), key.count());

#ifndef QT_NO_LOCALFILE_OPTIMIZED_QML
    if (!async) {
        QString lf = QDeclarativeEnginePrivate::urlToLocalFileOrQrc(url);
        if (!lf.isEmpty()) {
            status = QDeclarativePixmapReply::Ready;
            if (!QPixmapCache::find(strKey,pixmap)) {
                QFile f(lf);
                QSize read_impsize;
                if (f.open(QIODevice::ReadOnly)) {
                    QImage image;
                    if (readImage(url, &f, &image, errorString, &read_impsize, req_width, req_height)) {
                        *pixmap = QPixmap::fromImage(image);
                    } else {
                        *pixmap = QPixmap();
                        status = QDeclarativePixmapReply::Error;
                    }
                } else {
                    if (errorString)
                        *errorString = tr("Cannot open: %1").arg(url.toString());
                    *pixmap = QPixmap();
                    status = QDeclarativePixmapReply::Error;
                }
                if (status == QDeclarativePixmapReply::Ready) {
                    QPixmapCache::insert(strKey, *pixmap);
                    qmlOriginalSizes()->insert(url, read_impsize);
                }
                if (impsize)
                    *impsize = read_impsize;
            } else {
                if (impsize) {
                    QDeclarativePixmapSizeHash::Iterator iter = qmlOriginalSizes()->find(url);
                    if (iter != qmlOriginalSizes()->end())
                        *impsize = *iter;
                }
            }
            return status;
        }
    }
#endif

    QDeclarativePixmapReplyHash::Iterator iter = qmlActivePixmapReplies()->find(url);
    if (iter != qmlActivePixmapReplies()->end() && (*iter)->status() == QDeclarativePixmapReply::Ready) {
        // Must check this, since QPixmapCache::insert may have failed.
        *pixmap = (*iter)->d_func()->pixmap;
        status = (*iter)->status();
        (*iter)->release();
    } else if (QPixmapCache::find(strKey, pixmap)) {
        if (iter != qmlActivePixmapReplies()->end()) {
            status = (*iter)->status();
            if (errorString)
                *errorString = (*iter)->errorString();
            (*iter)->release();
        } else if (pixmap->isNull()) {
            status = QDeclarativePixmapReply::Error;
            if (errorString)
                *errorString = tr("Unknown Error loading %1").arg(url.toString());
        } else {
            status = QDeclarativePixmapReply::Ready;
        }
    } else if (iter != qmlActivePixmapReplies()->end()) {
        status = QDeclarativePixmapReply::Loading;
    }
    if (impsize) {
        QDeclarativePixmapSizeHash::Iterator iter = qmlOriginalSizes()->find(url);
        if (iter != qmlOriginalSizes()->end())
            *impsize = *iter;
    }

    return status;
}

/*!
    Starts a network request to load \a url.

    Returns a QDeclarativePixmapReply.  Caller should connect to QDeclarativePixmapReply::finished()
    and call get() when the image is available.

    The returned QDeclarativePixmapReply will be deleted when all request() calls are
    matched by a corresponding get() call.
*/
QDeclarativePixmapReply *QDeclarativePixmapCache::request(QDeclarativeEngine *engine, const QUrl &url, int req_width, int req_height)
{
    QDeclarativePixmapReplyHash::Iterator iter = qmlActivePixmapReplies()->find(url);
    if (iter == qmlActivePixmapReplies()->end()) {
        QDeclarativeImageReader *reader = QDeclarativeImageReader::instance(engine);
        QDeclarativePixmapReply *item = reader->getImage(url, req_width, req_height);
        iter = qmlActivePixmapReplies()->insert(url, item);
    } else {
        (*iter)->addRef();
    }

    return (*iter);
}

/*!
    Cancels a previous call to request().

    May also cancel loading (eg. if no other pending request).

    Any connections from the QDeclarativePixmapReply returned by request() to \a obj will be
    disconnected.
*/
void QDeclarativePixmapCache::cancel(const QUrl& url, QObject *obj)
{
    QDeclarativePixmapReplyHash::Iterator iter = qmlActivePixmapReplies()->find(url);
    if (iter == qmlActivePixmapReplies()->end())
        return;

    QDeclarativePixmapReply *reply = *iter;
    if (obj)
        QObject::disconnect(reply, 0, obj, 0);
    reply->release();
}

/*!
    This function is mainly for test verification. It returns the number of
    requests that are still unfinished.
*/
int QDeclarativePixmapCache::pendingRequests()
{
    return qmlActivePixmapReplies()->count();
}

QT_END_NAMESPACE

#include <qdeclarativepixmapcache.moc>

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

#include "qdeclarativepixmapcache_p.h"
#include "qdeclarativenetworkaccessmanagerfactory.h"
#include "qdeclarativeimageprovider.h"

#include "qfxperf_p_p.h"

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
#include <QWaitCondition>
#include <QtCore/qdebug.h>
#include <private/qobject_p.h>
#include <QSslError>

#ifdef Q_OS_LINUX
#include <pthread.h>
#include <linux/sched.h>
#endif

// Maximum number of simultaneous image requests to send.
static const int maxImageRequestCount = 8;

QT_BEGIN_NAMESPACE

#if (QT_VERSION < QT_VERSION_CHECK(4, 7, 0))
inline uint qHash(const QUrl &uri)
{
    return qHash(uri.toEncoded(QUrl::FormattingOption(0x100)));
}
#endif

static QString toLocalFileOrQrc(const QUrl& url)
{
    QString r = url.toLocalFile();
    if (r.isEmpty() && url.scheme() == QLatin1String("qrc"))
        r = QLatin1Char(':') + url.path();
    return r;
}

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

    QDeclarativePixmapReply *getImage(const QUrl &url);
    void cancel(QDeclarativePixmapReply *rep);

    static QDeclarativeImageReader *instance(QDeclarativeEngine *engine);

protected:
    void run();

private:
    QList<QDeclarativePixmapReply*> jobs;
    QList<QDeclarativePixmapReply*> cancelled;
    QDeclarativeEngine *engine;
    QDeclarativeImageRequestHandler *handler;
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
        : QObject(), accessManager(0), engine(eng), reader(read)
    {
        QCoreApplication::postEvent(this, new QEvent(QEvent::User));
    }

    QDeclarativePixmapReply *getImage(const QUrl &url);
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
};

//===========================================================================

bool QDeclarativeImageRequestHandler::event(QEvent *event)
{
    if (event->type() == QEvent::User) {
        static int replyDownloadProgress = -1;
        static int replyFinished = -1;
        static int downloadProgress = -1;
        static int thisNetworkRequestDone = -1;

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
                QImage image = QDeclarativeEnginePrivate::get(engine)->getImageFromProvider(url);
                QDeclarativeImageReaderEvent::ReadError errorCode = QDeclarativeImageReaderEvent::NoError;
                QString errorStr;
                if (image.isNull()) {
                    errorCode = QDeclarativeImageReaderEvent::Loading;
                    errorStr = QLatin1String("Failed to get image from provider: ") + url.toString();
                }
                QCoreApplication::postEvent(runningJob, new QDeclarativeImageReaderEvent(errorCode, errorStr, image));
            } else {
                QString lf = toLocalFileOrQrc(url);
                if (!lf.isEmpty()) {
                    // Image is local - load/decode immediately
                    QImage image;
                    QDeclarativeImageReaderEvent::ReadError errorCode = QDeclarativeImageReaderEvent::NoError;
                    QString errorStr;
                    QFile f(lf);
                    if (f.open(QIODevice::ReadOnly)) {
                        QImageReader imgio(&f);
                        if (!imgio.read(&image)) {
                            errorStr = QLatin1String("Error decoding: ") + url.toString()
                                          + QLatin1String(" \"") + imgio.errorString() + QLatin1String("\"");
                            errorCode = QDeclarativeImageReaderEvent::Loading;
                        }
                    } else {
                        errorStr = QLatin1String("Cannot open: ") + url.toString();
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

void QDeclarativeImageRequestHandler::networkRequestDone()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    QDeclarativePixmapReply *job = replies.take(reply);
    if (job) {
        QImage image;
        QDeclarativeImageReaderEvent::ReadError error;
        QString errorString;
        if (reply->error()) {
            error = QDeclarativeImageReaderEvent::Loading;
            errorString = reply->errorString();
        } else {
            QImageReader imgio(reply);
            if (imgio.read(&image)) {
                error = QDeclarativeImageReaderEvent::NoError;
            } else {
                errorString = QLatin1String("Error decoding: ") + reply->url().toString()
                              + QLatin1String(" \"") + imgio.errorString() + QLatin1String("\"");
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
    start(QThread::LowPriority);
}

QDeclarativeImageReader::~QDeclarativeImageReader()
{
    readerMutex.lock();
    readers.remove(engine);
    readerMutex.unlock();

    quit();
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

QDeclarativePixmapReply *QDeclarativeImageReader::getImage(const QUrl &url)
{
    mutex.lock();
    QDeclarativePixmapReply *reply = new QDeclarativePixmapReply(this, url);
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
#if defined(Q_OS_LINUX) && defined(SCHED_IDLE)
    struct sched_param param;
    int policy;

    pthread_getschedparam(pthread_self(), &policy, &param);
    pthread_setschedparam(pthread_self(), SCHED_IDLE, &param);
#endif

    handler = new QDeclarativeImageRequestHandler(this, engine);

    exec();

    delete handler;
    handler = 0;
}

//===========================================================================

static bool readImage(QIODevice *dev, QPixmap *pixmap, QString &errorString)
{
    QImageReader imgio(dev);

//#define QT_TEST_SCALED_SIZE
#ifdef QT_TEST_SCALED_SIZE
    /*
    Some mechanism is needed for loading images at a limited size, especially
    for remote images. Loading only thumbnails of remote progressive JPEG
    images can be efficient. (Qt jpeg handler does not do so currently)
    */

    QSize limit(60,60);
    QSize sz = imgio.size();
    if (sz.width() > limit.width() || sz.height() > limit.height()) {
        sz.scale(limit,Qt::KeepAspectRatio);
        imgio.setScaledSize(sz);
    }
#endif

    QImage img;
    if (imgio.read(&img)) {
#ifdef QT_TEST_SCALED_SIZE
        if (!sz.isValid())
            img = img.scaled(limit,Qt::KeepAspectRatio);
#endif
        *pixmap = QPixmap::fromImage(img);
        return true;
    } else {
        errorString = imgio.errorString();
        return false;
    }
}

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
    QDeclarativePixmapReplyPrivate(QDeclarativeImageReader *r, const QUrl &u)
        : QObjectPrivate(), refCount(1), url(u), status(QDeclarativePixmapReply::Loading), loading(false), reader(r) {
    }

    int refCount;
    QUrl url;
    QPixmap pixmap; // ensure reference to pixmap so QPixmapCache does not discard
    QDeclarativePixmapReply::Status status;
    bool loading;
    QDeclarativeImageReader *reader;
};


QDeclarativePixmapReply::QDeclarativePixmapReply(QDeclarativeImageReader *reader, const QUrl &url)
  : QObject(*new QDeclarativePixmapReplyPrivate(reader, url), 0)
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
                qWarning() << de->errorString;
            QByteArray key = d->url.toEncoded(QUrl::FormattingOption(0x100));
            QString strKey = QString::fromLatin1(key.constData(), key.count());
            QPixmapCache::insert(strKey, d->pixmap); // note: may fail (returns false)
            emit finished();
        }
        return true;
    }

    return QObject::event(event);
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

    Note that images sourced from the network will always be loaded and
    decoded asynchonously.
*/
QDeclarativePixmapReply::Status QDeclarativePixmapCache::get(const QUrl& url, QPixmap *pixmap, bool async)
{
    QDeclarativePixmapReply::Status status = QDeclarativePixmapReply::Unrequested;
    QByteArray key = url.toEncoded(QUrl::FormattingOption(0x100));
    QString strKey = QString::fromLatin1(key.constData(), key.count());

#ifndef QT_NO_LOCALFILE_OPTIMIZED_QML
    if (!async) {
        QString lf = toLocalFileOrQrc(url);
        if (!lf.isEmpty()) {
            status = QDeclarativePixmapReply::Ready;
            if (!QPixmapCache::find(strKey,pixmap)) {
                QFile f(lf);
                if (f.open(QIODevice::ReadOnly)) {
                    QString errorString;
                    if (!readImage(&f, pixmap, errorString)) {
                        errorString = QLatin1String("Error decoding: ") + url.toString()
                                      + QLatin1String(" \"") + errorString + QLatin1String("\"");
                        qWarning() << errorString;
                        *pixmap = QPixmap();
                        status = QDeclarativePixmapReply::Error;
                    }
                } else {
                    qWarning() << "Cannot open" << url;
                    *pixmap = QPixmap();
                    status = QDeclarativePixmapReply::Error;
                }
                if (status == QDeclarativePixmapReply::Ready)
                    QPixmapCache::insert(strKey, *pixmap);
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
            (*iter)->release();
        } else {
            status = pixmap->isNull() ? QDeclarativePixmapReply::Error : QDeclarativePixmapReply::Ready;
        }
    } else if (iter != qmlActivePixmapReplies()->end()) {
        status = QDeclarativePixmapReply::Loading;
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
QDeclarativePixmapReply *QDeclarativePixmapCache::request(QDeclarativeEngine *engine, const QUrl &url)
{
    QDeclarativePixmapReplyHash::Iterator iter = qmlActivePixmapReplies()->find(url);
    if (iter == qmlActivePixmapReplies()->end()) {
        QDeclarativeImageReader *reader = QDeclarativeImageReader::instance(engine);
        QDeclarativePixmapReply *item = reader->getImage(url);
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

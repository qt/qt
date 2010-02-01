/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qmlpixmapcache_p.h"
#include "qmlnetworkaccessmanagerfactory.h"

#include "qfxperf_p_p.h"

#include <qmlengine.h>
#include <private/qmlglobal_p.h>

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

inline uint qHash(const QUrl &uri)
{
    return qHash(uri.toEncoded(QUrl::FormattingOption(0x100)));
}

class QmlImageReader : public QThread
{
    Q_OBJECT
public:
    QmlImageReader(QmlEngine *eng);
    ~QmlImageReader();

    QmlPixmapReply *getImage(const QUrl &url);
    void cancel(QmlPixmapReply *rep);

    static QmlImageReader *instance(QmlEngine *engine);

protected:
    void run();
    bool event(QEvent *event);

private slots:
    void networkRequestDone();
    void namInvalidated() {
        accessManagerValid = false;
    }

private:
    QNetworkAccessManager *networkAccessManager() {
        if (!accessManagerValid) {
            delete accessManager;
            accessManager = 0;
        }
        if (!accessManager) {
            if (engine && engine->networkAccessManagerFactory()) {
                connect(engine->networkAccessManagerFactory(), SIGNAL(invalidated())
                        , this, SLOT(namInvalidated()), Qt::UniqueConnection);
                accessManager = engine->networkAccessManagerFactory()->create(this);
            } else {
                accessManager = new QNetworkAccessManager(this);
            }
            accessManagerValid = true;
        }
        return accessManager;
    }

    QList<QmlPixmapReply*> jobs;
    QList<QmlPixmapReply*> cancelled;
    QHash<QNetworkReply*,QmlPixmapReply*> replies;
    QNetworkAccessManager *accessManager;
    bool accessManagerValid;
    QmlEngine *engine;
    QMutex mutex;
    static QHash<QmlEngine *,QmlImageReader*> readers;
};

QHash<QmlEngine *,QmlImageReader*> QmlImageReader::readers;

class QmlImageReaderEvent : public QEvent
{
public:
    enum ReadError { NoError, Loading, Decoding };

    QmlImageReaderEvent(QmlImageReaderEvent::ReadError err, QImage &img)
        : QEvent(QEvent::User), error(err), image(img) {}

    ReadError error;
    QImage image;
};


QmlImageReader::QmlImageReader(QmlEngine *eng)
    : QThread(eng), accessManager(0), accessManagerValid(false), engine(eng)
{
    start(QThread::LowPriority);
}

QmlImageReader::~QmlImageReader()
{
}

QmlImageReader *QmlImageReader::instance(QmlEngine *engine)
{
    QmlImageReader *reader = readers.value(engine);
    if (!reader) {
        static QMutex rmutex;
        rmutex.lock();
        reader = new QmlImageReader(engine);
        readers.insert(engine, reader);
        rmutex.unlock();
    }

    return reader;
}

QmlPixmapReply *QmlImageReader::getImage(const QUrl &url)
{
    mutex.lock();
    QmlPixmapReply *reply = new QmlPixmapReply(engine, url);
    jobs.append(reply);
    if (jobs.count() == 1)
        QCoreApplication::postEvent(this, new QEvent(QEvent::User));
    mutex.unlock();
    return reply;
}

void QmlImageReader::cancel(QmlPixmapReply *reply)
{
    mutex.lock();
    if (reply->isLoading()) {
        // Already requested.  Add to cancel list to be cancelled in reader thread.
        cancelled.append(reply);
        if (cancelled.count() == 1)
            QCoreApplication::postEvent(this, new QEvent(QEvent::User));
    } else {
        // Not yet processed - just remove from waiting list
        QList<QmlPixmapReply*>::iterator it = jobs.begin();
        while (it != jobs.end()) {
            QmlPixmapReply *job = *it;
            if (job == reply) {
                jobs.erase(it);
                break;
            }
            ++it;
        }
    }
    mutex.unlock();
}

void QmlImageReader::run()
{
#ifdef Q_OS_LINUX
    struct sched_param param;
    int policy;

    pthread_getschedparam(pthread_self(), &policy, &param);
    pthread_setschedparam(pthread_self(), SCHED_IDLE, &param);
#endif

    exec();
}

bool QmlImageReader::event(QEvent *event)
{
    if (event->type() == QEvent::User) {
        static int replyDownloadProgress = -1;
        static int replyFinished = -1;
        static int downloadProgress = -1;
        static int thisNetworkRequestDone = -1;

        if (replyDownloadProgress == -1) {
            replyDownloadProgress = QNetworkReply::staticMetaObject.indexOfSignal("downloadProgress(qint64,qint64)");
            replyFinished = QNetworkReply::staticMetaObject.indexOfSignal("finished()");
            downloadProgress = QmlPixmapReply::staticMetaObject.indexOfSignal("downloadProgress(qint64,qint64)");
            thisNetworkRequestDone = QmlImageReader::staticMetaObject.indexOfSlot("networkRequestDone()");
        }

        while (1) {
            mutex.lock();

            if (cancelled.count()) {
                for (int i = 0; i < cancelled.count(); ++i) {
                    QmlPixmapReply *job = cancelled.at(i);
                    QNetworkReply *reply = replies.key(job, 0);
                    if (reply && reply->isRunning()) {
                        replies.remove(reply);
                        reply->close();
                        job->release(true);
                    }
                }
                cancelled.clear();
            }

            if (!accessManagerValid) {
                // throw away existing requests and reschedule.
                QHash<QNetworkReply*,QmlPixmapReply*>::iterator it = replies.begin();
                for (; it != replies.end(); ++it) {
                    delete it.key();
                    jobs.prepend(*it);
                }
                replies.clear();
            }

            if (!jobs.count() || replies.count() > maxImageRequestCount) {
                mutex.unlock();
                break;
            }

            QmlPixmapReply *runningJob = jobs.takeFirst();
            runningJob->addRef();
            runningJob->setLoading();
            QUrl url = runningJob->url();
            mutex.unlock();

            // fetch
            QNetworkRequest req(url);
            req.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);
            QNetworkReply *reply = networkAccessManager()->get(req);

            QMetaObject::connect(reply, replyDownloadProgress, runningJob, downloadProgress);
            QMetaObject::connect(reply, replyFinished, this, thisNetworkRequestDone);

            replies.insert(reply, runningJob);
        }
        return true;
    }

    return QObject::event(event);
}

void QmlImageReader::networkRequestDone()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    QmlPixmapReply *job = replies.take(reply);
    if (job) {
        QImage image;
        QmlImageReaderEvent::ReadError error;
        if (reply->error()) {
            error = QmlImageReaderEvent::Loading;
        } else {
            QImageReader imgio(reply);
            if (imgio.read(&image)) {
                error = QmlImageReaderEvent::NoError;
            } else {
                error = QmlImageReaderEvent::Decoding;
            }
        }
        // send completion event to the QmlPixmapReply
        QCoreApplication::postEvent(job, new QmlImageReaderEvent(error, image));
    }
    // kick off event loop again if we have dropped below max request count
    if (replies.count() == maxImageRequestCount)
        QCoreApplication::postEvent(this, new QEvent(QEvent::User));
    reply->deleteLater();
}

static bool readImage(QIODevice *dev, QPixmap *pixmap)
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
        qWarning() << imgio.errorString();
        return false;
    }
}

/*!
    \internal
    \class QmlPixmapCache
    \brief Enacapsultes a pixmap for QmlGraphics items.

    This class is NOT reentrant.
 */

static QString toLocalFileOrQrc(const QUrl& url)
{
    QString r = url.toLocalFile();
    if (r.isEmpty() && url.scheme() == QLatin1String("qrc"))
        r = QLatin1Char(':') + url.path();
    return r;
}

typedef QHash<QUrl, QmlPixmapReply *> QmlPixmapReplyHash;
Q_GLOBAL_STATIC(QmlPixmapReplyHash, qmlActivePixmapReplies);

class QmlPixmapReplyPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QmlPixmapReply)

public:
    QmlPixmapReplyPrivate(QmlEngine *e, const QUrl &u)
        : QObjectPrivate(), refCount(1), url(u), status(QmlPixmapReply::Loading), loading(false), engine(e) {
    }

    int refCount;
    QUrl url;
    QPixmap pixmap; // ensure reference to pixmap so QPixmapCache does not discard
    QmlPixmapReply::Status status;
    bool loading;
    QmlEngine *engine;
};


QmlPixmapReply::QmlPixmapReply(QmlEngine *engine, const QUrl &url)
  : QObject(*new QmlPixmapReplyPrivate(engine, url), 0)
{
}

QmlPixmapReply::~QmlPixmapReply()
{
}

const QUrl &QmlPixmapReply::url() const
{
    Q_D(const QmlPixmapReply);
    return d->url;
}

bool QmlPixmapReply::event(QEvent *event)
{
    Q_D(QmlPixmapReply);
    if (event->type() == QEvent::User) {
        d->loading = false;
        if (!release(true)) {
            QmlImageReaderEvent *de = static_cast<QmlImageReaderEvent*>(event);
            d->status = (de->error == QmlImageReaderEvent::NoError) ? Ready : Error;
            if (d->status == Ready)
                d->pixmap = QPixmap::fromImage(de->image);
            QByteArray key = d->url.toEncoded(QUrl::FormattingOption(0x100));
            QString strKey = QString::fromLatin1(key.constData(), key.count());
            QPixmapCache::insert(strKey, d->pixmap); // note: may fail (returns false)
            emit finished();
        }
        return true;
    }

    return QObject::event(event);
}

QmlPixmapReply::Status QmlPixmapReply::status() const
{
    Q_D(const QmlPixmapReply);
    return d->status;
}

bool QmlPixmapReply::isLoading() const
{
    Q_D(const QmlPixmapReply);
    return d->loading;
}

void QmlPixmapReply::setLoading()
{
    Q_D(QmlPixmapReply);
    d->loading = true;
}

void QmlPixmapReply::addRef()
{
    Q_D(QmlPixmapReply);
    ++d->refCount;
}

bool QmlPixmapReply::release(bool defer)
{
    Q_D(QmlPixmapReply);
    Q_ASSERT(d->refCount > 0);
    --d->refCount;
    if (d->refCount == 0) {
        qmlActivePixmapReplies()->remove(d->url);
        if (d->status == Loading && !d->loading)
            QmlImageReader::instance(d->engine)->cancel(this);
        if (defer)
            deleteLater();
        else
            delete this;
        return true;
    } else if (d->refCount == 1 && d->loading) {
        // The only reference left is the reader thread.
        qmlActivePixmapReplies()->remove(d->url);
        QmlImageReader::instance(d->engine)->cancel(this);
    }

    return false;
}

/*!
    Finds the cached pixmap corresponding to \a url.
    If the image is a network resource and has not yet
    been retrieved and cached, request() must be called.

    Returns Ready, or Error if the image has been retrieved,
    otherwise the current retrieval status.
*/
QmlPixmapReply::Status QmlPixmapCache::get(const QUrl& url, QPixmap *pixmap)
{
    QmlPixmapReply::Status status = QmlPixmapReply::Unrequested;

#ifndef QT_NO_LOCALFILE_OPTIMIZED_QML
    QString lf = toLocalFileOrQrc(url);
    if (!lf.isEmpty()) {
        status = QmlPixmapReply::Ready;
        if (!QPixmapCache::find(lf,pixmap)) {
            QFile f(lf);
            if (f.open(QIODevice::ReadOnly)) {
                if (!readImage(&f, pixmap)) {
                    qWarning() << "Format error loading" << url;
                    *pixmap = QPixmap();
                    status = QmlPixmapReply::Error;
                }
            } else {
                qWarning() << "Cannot open" << url;
                *pixmap = QPixmap();
                status = QmlPixmapReply::Error;
            }
            if (status == QmlPixmapReply::Ready)
                QPixmapCache::insert(lf, *pixmap);
        }
        return status;
    }
#endif

    QByteArray key = url.toEncoded(QUrl::FormattingOption(0x100));
    QString strKey = QString::fromLatin1(key.constData(), key.count());
    QmlPixmapReplyHash::Iterator iter = qmlActivePixmapReplies()->find(url);
    if (iter != qmlActivePixmapReplies()->end() && (*iter)->status() == QmlPixmapReply::Ready) {
        // Must check this, since QPixmapCache::insert may have failed.
        *pixmap = (*iter)->d_func()->pixmap;
        status = (*iter)->status();
        (*iter)->release();
    } else if (QPixmapCache::find(strKey, pixmap)) {
        if (iter != qmlActivePixmapReplies()->end()) {
            status = (*iter)->status();
            (*iter)->release();
        } else {
            status = pixmap->isNull() ? QmlPixmapReply::Error : QmlPixmapReply::Ready;
        }
    } else if (iter != qmlActivePixmapReplies()->end()) {
        status = QmlPixmapReply::Loading;
    }

    return status;
}

/*!
    Starts a network request to load \a url.

    Returns a QmlPixmapReply.  Caller should connect to QmlPixmapReply::finished()
    and call get() when the image is available.

    The returned QmlPixmapReply will be deleted when all request() calls are
    matched by a corresponding get() call.
*/
QmlPixmapReply *QmlPixmapCache::request(QmlEngine *engine, const QUrl &url)
{
    QmlPixmapReplyHash::Iterator iter = qmlActivePixmapReplies()->find(url);
    if (iter == qmlActivePixmapReplies()->end()) {
        QmlImageReader *reader = QmlImageReader::instance(engine);
        QmlPixmapReply *item = reader->getImage(url);
        iter = qmlActivePixmapReplies()->insert(url, item);
    } else {
        (*iter)->addRef();
    }

    return (*iter);
}

/*!
    Cancels a previous call to request().

    May also cancel loading (eg. if no other pending request).

    Any connections from the QmlPixmapReply returned by request() to \a obj will be
    disconnected.
*/
void QmlPixmapCache::cancel(const QUrl& url, QObject *obj)
{
    QmlPixmapReplyHash::Iterator iter = qmlActivePixmapReplies()->find(url);
    if (iter == qmlActivePixmapReplies()->end())
        return;

    QmlPixmapReply *reply = *iter;
    if (obj)
        QObject::disconnect(reply, 0, obj, 0);
    reply->release();
}

/*!
    This function is mainly for test verification. It returns the number of
    requests that are still unfinished.
*/
int QmlPixmapCache::pendingRequests()
{
    return qmlActivePixmapReplies()->count();
}

#include <qmlpixmapcache.moc>

QT_END_NAMESPACE

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

QT_BEGIN_NAMESPACE

class QmlImageReader : public QThread
{
    Q_OBJECT
public:
    QmlImageReader(QObject *parent=0);
    ~QmlImageReader();

    void read(QmlPixmapReply *rep);
    void cancel(QmlPixmapReply *rep);

protected:
    void run();

private:
    struct Job {
        Job() : reply(0), error(false) {}
        QmlPixmapReply *reply;
        QImage img;
        bool error;
    };

    void loadImage(Job &job);

    QList<Job> jobs;
    QMutex mutex;
    QWaitCondition haveJob;
    bool quit;
};

class QmlImageDecodeEvent : public QEvent
{
public:
    QmlImageDecodeEvent(bool err, QImage &img) : QEvent(QEvent::User), error(err), image(img) {}

    bool error;
    QImage image;
};

Q_GLOBAL_STATIC(QmlImageReader, qmlImageReader)


QmlImageReader::QmlImageReader(QObject *parent) : QThread(parent), quit(false)
{
    start(QThread::LowPriority);
}

QmlImageReader::~QmlImageReader()
{
    quit = true;
    haveJob.wakeOne();
}

void QmlImageReader::read(QmlPixmapReply *reply)
{
    mutex.lock();
    Job job;
    job.reply = reply;
    jobs.append(job);
    if (jobs.count() == 1)
        haveJob.wakeOne();
    mutex.unlock();
}

void QmlImageReader::cancel(QmlPixmapReply *reply)
{
    mutex.lock();
    QList<Job>::iterator it = jobs.begin();
    while (it != jobs.end()) {
        if ((*it).reply == reply) {
            jobs.erase(it);
            break;
        }
        ++it;
    }
    mutex.unlock();
}

void QmlImageReader::loadImage(Job &job)
{
    QImageReader imgio(job.reply->device());
    if (imgio.read(&job.img)) {
        job.error = false;
    } else {
        job.error = true;
        qWarning() << imgio.errorString();
    }
}

void QmlImageReader::run()
{
    while (1) {
        mutex.lock();
        if (!jobs.count())
            haveJob.wait(&mutex);
        if (quit)
            break;
        Job runningJob = jobs.takeFirst();
        runningJob.reply->addRef();
        mutex.unlock();

        loadImage(runningJob);
        QCoreApplication::postEvent(runningJob.reply, new QmlImageDecodeEvent(runningJob.error, runningJob.img));
    }
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

typedef QHash<QString, QmlPixmapReply *> QmlPixmapReplyHash;
static QmlPixmapReplyHash qmlActivePixmapReplies;

class QmlPixmapReplyPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QmlPixmapReply)

public:
    QmlPixmapReplyPrivate(const QString &url, QNetworkReply *r)
        : QObjectPrivate(), refCount(1), urlKey(url), reply(r), status(QmlPixmapReply::Loading) {
    }

    int refCount;
    QString urlKey;
    QNetworkReply *reply;
    QPixmap pixmap; // ensure reference to pixmap so QPixmapCache does not discard
    QImage image;
    QmlPixmapReply::Status status;
};


QmlPixmapReply::QmlPixmapReply(const QString &key, QNetworkReply *reply)
  : QObject(*new QmlPixmapReplyPrivate(key, reply), 0)
{
    Q_D(QmlPixmapReply);

    static int replyDownloadProgress = -1;
    static int replyFinished = -1;
    static int thisDownloadProgress = -1;
    static int thisNetworkRequestDone = -1;

    if (replyDownloadProgress == -1) {
        replyDownloadProgress = QNetworkReply::staticMetaObject.indexOfSignal("downloadProgress(qint64,qint64)");
        replyFinished = QNetworkReply::staticMetaObject.indexOfSignal("finished()");
        thisDownloadProgress = QmlPixmapReply::staticMetaObject.indexOfSignal("downloadProgress(qint64,qint64)");
        thisNetworkRequestDone = QmlPixmapReply::staticMetaObject.indexOfSlot("networkRequestDone()");
    }

    QMetaObject::connect(d->reply, replyDownloadProgress, this, thisDownloadProgress, Qt::DirectConnection);
    QMetaObject::connect(d->reply, replyFinished, this, thisNetworkRequestDone);
}

QmlPixmapReply::~QmlPixmapReply()
{
    Q_D(QmlPixmapReply);
    if (d->status == Decoding)
        qmlImageReader()->cancel(this);
    delete d->reply;
}

void QmlPixmapReply::networkRequestDone()
{
    Q_D(QmlPixmapReply);
    if (d->reply->error()) {
        d->pixmap = QPixmap();
        d->status = Error;
        QPixmapCache::insert(d->urlKey, d->pixmap);
        qWarning() << "Network error loading" << d->urlKey << d->reply->errorString();
        emit finished();
    } else {
        qmlImageReader()->read(this);
        d->status = Decoding;
    }
}

bool QmlPixmapReply::event(QEvent *event)
{
    Q_D(QmlPixmapReply);
    if (event->type() == QEvent::User) {
        if (!release(true)) {
            QmlImageDecodeEvent *de = static_cast<QmlImageDecodeEvent*>(event);
            d->status = de->error ? Error : Ready;
            if (d->status == Ready) {
                d->pixmap = QPixmap::fromImage(de->image);
                d->image = QImage();
            } else {
                qWarning() << "Error decoding" << d->urlKey;
            }
            QPixmapCache::insert(d->urlKey, d->pixmap); // note: may fail (returns false)
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

QIODevice *QmlPixmapReply::device()
{
    Q_D(QmlPixmapReply);
    return d->reply;
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
        qmlActivePixmapReplies.remove(d->urlKey);
        if (defer)
            deleteLater();
        else
            delete this;
        return true;
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

    QString key = url.toString();

    QmlPixmapReplyHash::Iterator iter = qmlActivePixmapReplies.find(key);
    if (iter != qmlActivePixmapReplies.end() && (*iter)->status() == QmlPixmapReply::Ready) {
        // Must check this, since QPixmapCache::insert may have failed.
        *pixmap = (*iter)->d_func()->pixmap;
        status = (*iter)->status();
        (*iter)->release();
    } else if (QPixmapCache::find(key, pixmap)) {
        if (iter != qmlActivePixmapReplies.end()) {
            status = (*iter)->status();
            (*iter)->release();
        } else {
            status = pixmap->isNull() ? QmlPixmapReply::Error : QmlPixmapReply::Ready;
        }
    } else if (iter != qmlActivePixmapReplies.end()) {
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
    QString key = url.toString();
    QmlPixmapReplyHash::Iterator iter = qmlActivePixmapReplies.find(key);
    if (iter == qmlActivePixmapReplies.end()) {
        QNetworkRequest req(url);
        req.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);
        QmlPixmapReply *item = new QmlPixmapReply(key, engine->networkAccessManager()->get(req));
        iter = qmlActivePixmapReplies.insert(key, item);
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
    QString key = url.toString();
    QmlPixmapReplyHash::Iterator iter = qmlActivePixmapReplies.find(key);
    if (iter == qmlActivePixmapReplies.end())
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
    return qmlActivePixmapReplies.count();
}

#include <qmlpixmapcache.moc>

QT_END_NAMESPACE

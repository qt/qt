/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qfxpixmap.h"
#include <QHash>
#include <QNetworkReply>
#include <qfxperf.h>
#include <QtDeclarative/qmlengine.h>
#include <QFile>


QT_BEGIN_NAMESPACE
class QFxPixmapCacheItem;
typedef QHash<QString, QFxPixmapCacheItem *> QFxPixmapCache;
static QFxPixmapCache qfxPixmapCache;

class QFxPixmapCacheItem
{
public:
    QFxPixmapCacheItem() : reply(0), refCount(1) {}
    QString key;
    QNetworkReply *reply;
#if defined(QFX_RENDER_OPENGL)
    QImage image;
#else
    QImage image;
    QImage opaqueImage;
#endif

    int refCount;
    void addRef() { ++refCount; }
    void release() { Q_ASSERT(refCount > 0); --refCount; if(refCount == 0) { qfxPixmapCache.remove(key); delete this; } }
};

static QFxPixmapCacheItem qfxPixmapCacheDummyItem;

class QFxPixmapPrivate
{
public:
    QFxPixmapPrivate()
    : opaque(false), pixmap(&qfxPixmapCacheDummyItem) { pixmap->addRef(); }

    bool opaque;
    QFxPixmapCacheItem *pixmap;
};

/*!
    \internal
    \class QFxPixmap
    \ingroup group_utility
    \brief Enacapsultes a pixmap for QFx items.

    This class is NOT reentrant.
    The pixmap cache will grow indefinately.
 */
QFxPixmap::QFxPixmap()
: d(new QFxPixmapPrivate)
{
}

QFxPixmap::QFxPixmap(const QUrl &url)
: d(new QFxPixmapPrivate)
{
#ifdef Q_ENABLE_PERFORMANCE_LOG
    QFxPerfTimer<QFxPerf::PixmapLoad> perf;
#endif
    QString key = url.toString();
    QFxPixmapCache::Iterator iter = qfxPixmapCache.find(key);
    if(iter == qfxPixmapCache.end()) {
        qWarning() << "QFxPixmap: URL not loaded" << url;
    } else {
        QNetworkReply *reply = (*iter)->reply;
        if (reply) {
            if (reply->error()) {
                qWarning() << "Error loading" << url << reply->errorString();
            } else {
                (*iter)->image.load(reply, 0);
            }
            reply->deleteLater();
            (*iter)->reply = 0;
        }
        (*iter)->addRef();
    }

    d->pixmap = *iter;
}

QFxPixmap::QFxPixmap(const QFxPixmap &o)
: d(new QFxPixmapPrivate)
{
    d->opaque = o.d->opaque;
    o.d->pixmap->addRef();
    d->pixmap->release();
    d->pixmap = o.d->pixmap;
}

QFxPixmap::~QFxPixmap()
{
    d->pixmap->release();
    delete d;
}

QFxPixmap &QFxPixmap::operator=(const QFxPixmap &o)
{
    d->opaque = o.d->opaque;
    o.d->pixmap->addRef();
    d->pixmap->release();
    d->pixmap = o.d->pixmap;
    return *this;
}

bool QFxPixmap::isNull() const
{
    return d->pixmap->image.isNull();
}

bool QFxPixmap::opaque() const
{
    return d->opaque;
}

void QFxPixmap::setOpaque(bool o)
{
    d->opaque = o;
}

int QFxPixmap::width() const
{
    return d->pixmap->image.width();
}

int QFxPixmap::height() const
{
    return d->pixmap->image.height();
}

QPixmap QFxPixmap::pixmap() const
{
    return QPixmap::fromImage(d->pixmap->image);
}

void QFxPixmap::setPixmap(const QPixmap &pix)
{
    QFxPixmapCache::Iterator iter = qfxPixmapCache.find(QString::number(pix.cacheKey()));
    if(iter == qfxPixmapCache.end()) {
        QFxPixmapCacheItem *item = new QFxPixmapCacheItem;
        item->key = QString::number(pix.cacheKey());
        if(d->pixmap)
            d->pixmap->release();
        d->pixmap = item;
        d->pixmap->image = pix.toImage();
        qfxPixmapCache.insert(QString::number(pix.cacheKey()), item);
    } else {
        (*iter)->addRef();
        d->pixmap = *iter;
    }

#if 0
    int size = 0;
    for(QFxPixmapCache::Iterator iter = qfxPixmapCache.begin(); iter != qfxPixmapCache.end(); ++iter) {
        size += (*iter)->image.width() * (*iter)->image.height();
    }
    qWarning() << qfxPixmapCache.count() << size;
#endif
}

QFxPixmap::operator const QSimpleCanvasConfig::Image &() const
{
#if defined(QFX_RENDER_OPENGL)
    return d->pixmap->image;
#else
    if(d->opaque) {
        if(!d->pixmap->image.isNull() && d->pixmap->opaqueImage.isNull()) {
#ifdef Q_ENABLE_PERFORMANCE_LOG
            QFxPerfTimer<QFxPerf::PixmapLoad> perf;
#endif
            d->pixmap->opaqueImage = d->pixmap->image.convertToFormat(QPixmap::defaultDepth() == 16 ?
                                                                      QImage::Format_RGB16 :
                                                                      QImage::Format_RGB32);
        }
        return d->pixmap->opaqueImage;
    } else {
        if(!d->pixmap->image.isNull() && d->pixmap->image.format() != QImage::Format_ARGB32_Premultiplied) {
#ifdef Q_ENABLE_PERFORMANCE_LOG
            QFxPerfTimer<QFxPerf::PixmapLoad> perf;
#endif
            d->pixmap->image = d->pixmap->image.convertToFormat(QImage::Format_ARGB32_Premultiplied);
        }
        return d->pixmap->image;
    }
#endif
}

/*!
    Starts a network request to load \a url. When the URL is loaded,
    the given slot is invoked. Note that if the image is already cached,
    the slot may be invoked immediately.
*/
void QFxPixmap::get(QmlEngine *engine, const QUrl& url, QObject* obj, const char* slot)
{
    QString key = url.toString();
    QFxPixmapCache::Iterator iter = qfxPixmapCache.find(key);
    if(iter == qfxPixmapCache.end()) {
        QFxPixmapCacheItem *item = new QFxPixmapCacheItem;
        item->addRef(); // XXX - will never get deleted.  Need to revisit caching
        item->key = key;
#ifndef QT_NO_LOCALFILE_OPTIMIZED_QML
        if (url.scheme()==QLatin1String("file")) {
            item->image.load(url.toLocalFile(), 0);
        } else
#endif
        {
            QNetworkRequest req(url);
            req.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
            item->reply = engine->networkAccessManager()->get(req);
        }
        iter = qfxPixmapCache.insert(item->key, item);
    } else {
        (*iter)->addRef();
    }
    if ((*iter)->reply) {
        // still loading
        QObject::connect((*iter)->reply, SIGNAL(finished()), obj, slot);
    } else {
        // already loaded
        QObject dummy;
        QObject::connect(&dummy, SIGNAL(destroyed()), obj, slot);
    }
}

/*!
    Stops the given slot being invoked if the given url finishes loading.
    May also cancel loading (eg. if no other pending request).
*/
void QFxPixmap::cancelGet(const QUrl& url, QObject* obj, const char* slot)
{
    QString key = url.toString();
    QFxPixmapCache::Iterator iter = qfxPixmapCache.find(key);
    if(iter == qfxPixmapCache.end())
        return;
    if ((*iter)->reply)
        QObject::disconnect((*iter)->reply, SIGNAL(finished()), obj, slot);
    // XXX - loading not cancelled. Need to revisit caching
}

QT_END_NAMESPACE

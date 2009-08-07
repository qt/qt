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
#include <QImageReader>
#include <QHash>
#include <QNetworkReply>
#include <QPixmapCache>
#include <private/qfxperf_p.h>
#include <QtDeclarative/qmlengine.h>
#include <QFile>
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE
class QSharedNetworkReply;
typedef QHash<QString, QSharedNetworkReply *> QFxSharedNetworkReplyHash;
static QFxSharedNetworkReplyHash qfxActiveNetworkReplies;

class QSharedNetworkReply
{
public:
    QSharedNetworkReply(QNetworkReply *r) : reply(r), refCount(1) {}
    ~QSharedNetworkReply()
    {
        reply->deleteLater();
    }
    QNetworkReply *reply;

    int refCount;
    void addRef()
    {
        ++refCount;
    }
    void release()
    {
        Q_ASSERT(refCount > 0);
        --refCount;
        if (refCount == 0) {
            QString key = reply->url().toString();
            qfxActiveNetworkReplies.remove(key);
            delete this;
        }
    }
};

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
            return false;
        }
    }

/*!
    \internal
    \class QFxPixmap
    \ingroup group_utility
    \brief Enacapsultes a pixmap for QFx items.

    This class is NOT reentrant.
    The pixmap cache will grow indefinately.
 */


bool QFxPixmap::find(const QUrl& url, QPixmap *pixmap)
{
#ifdef Q_ENABLE_PERFORMANCE_LOG
    QFxPerfTimer<QFxPerf::PixmapLoad> perf;
#endif

    QString key = url.toString();
    if (!QPixmapCache::find(key,pixmap)) {
#ifndef QT_NO_LOCALFILE_OPTIMIZED_QML
        if (url.scheme()==QLatin1String("file")) {
            QFile f(url.toLocalFile());
            if (f.open(QIODevice::ReadOnly)) {
                if (!readImage(&f, pixmap)) {
                    qWarning() << "Format error loading" << url;
                    *pixmap = QPixmap();
                }
            } else
                *pixmap = QPixmap();
        } else
#endif
        {
            QFxSharedNetworkReplyHash::Iterator iter = qfxActiveNetworkReplies.find(key);
            if (iter == qfxActiveNetworkReplies.end()) {
                // API usage error
                qWarning() << "QFxPixmap: URL not loaded" << url;
            } else {
                if ((*iter)->reply->error()) {
                    qWarning() << "Network error loading" << url << (*iter)->reply->errorString();
                    *pixmap = QPixmap();
                } else
                    if (!readImage((*iter)->reply, pixmap)) {
                        qWarning() << "Format error loading" << url;
                        *pixmap = QPixmap();
                    }
                (*iter)->release();
            }
        }
        QPixmapCache::insert(key, *pixmap);
    }
}

/*!
    Starts a network request to load \a url. When the URL is loaded,
    the given slot is invoked. Note that if the image is already cached,
    the slot may be invoked immediately.

    Returns a QNetworkReply if the image is not immediately available, otherwise
    returns 0.  The QNetworkReply must not be stored - it may be destroyed at any time.
*/
QNetworkReply *QFxPixmap::get(QmlEngine *engine, const QUrl& url, QPixmap *pixmap)
{
#ifndef QT_NO_LOCALFILE_OPTIMIZED_QML
    if (url.scheme()==QLatin1String("file")) {
        QString key = url.toString();
        if (!QPixmapCache::find(key,pixmap)) {
            QFile f(url.toLocalFile());
            if (f.open(QIODevice::ReadOnly)) {
                if (!readImage(&f, pixmap)) {
                    qWarning() << "Format error loading" << url;
                    *pixmap = QPixmap();
                }
            } else
                *pixmap = QPixmap();
            QPixmapCache::insert(key, *pixmap);
        }
        return 0;
    }
#endif

    QString key = url.toString();
    if (QPixmapCache::find(key,pixmap)) {
        return 0;
    }

    QFxSharedNetworkReplyHash::Iterator iter = qfxActiveNetworkReplies.find(key);
    if (iter == qfxActiveNetworkReplies.end()) {
        QNetworkRequest req(url);
        req.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
        QSharedNetworkReply *item = new QSharedNetworkReply(engine->networkAccessManager()->get(req));
        iter = qfxActiveNetworkReplies.insert(key, item);
    } else {
        (*iter)->addRef();
    }

    return (*iter)->reply;
}

/*!
    Stops the given slot being invoked if the given url finishes loading.
    May also cancel loading (eg. if no other pending request).

    Any connections to the QNetworkReply returned by get() will be
    disconnected.
*/
void QFxPixmap::cancelGet(const QUrl& url, QObject* obj)
{
    QString key = url.toString();
    QFxSharedNetworkReplyHash::Iterator iter = qfxActiveNetworkReplies.find(key);
    if (iter == qfxActiveNetworkReplies.end())
        return;
    QObject::disconnect((*iter)->reply, 0, obj, 0);
    (*iter)->release();
}

QT_END_NAMESPACE

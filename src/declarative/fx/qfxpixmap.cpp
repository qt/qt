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

class QFxPixmapPrivate
{
public:
    QFxPixmapPrivate() {}

    QPixmap pixmap;
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
#ifndef QT_NO_LOCALFILE_OPTIMIZED_QML
    if (url.scheme()==QLatin1String("file")) {
        d->pixmap.load(url.toLocalFile());
    } else
#endif
    {
        QString key = url.toString();
        if (!QPixmapCache::find(key,&d->pixmap)) {
            QFxSharedNetworkReplyHash::Iterator iter = qfxActiveNetworkReplies.find(key);
            if (iter == qfxActiveNetworkReplies.end()) {
                // API usage error
                qWarning() << "QFxPixmap: URL not loaded" << url;
            } else {
                if ((*iter)->reply->error()) {
                    qWarning() << "Network error loading" << url << (*iter)->reply->errorString();
                } else {
                    QImage img;
                    if (img.load((*iter)->reply, 0)) {
                        d->pixmap = QPixmap::fromImage(img);
                        QPixmapCache::insert(key, d->pixmap);
                    } else {
                        qWarning() << "Format error loading" << url;
                    }
                }
                (*iter)->release();
            }
        }
    }
}

QFxPixmap::QFxPixmap(const QFxPixmap &o)
: d(new QFxPixmapPrivate)
{
    d->pixmap = o.d->pixmap;
}

QFxPixmap::~QFxPixmap()
{
    delete d;
}

QFxPixmap &QFxPixmap::operator=(const QFxPixmap &o)
{
    d->pixmap = o.d->pixmap;
    return *this;
}

bool QFxPixmap::isNull() const
{
    return d->pixmap.isNull();
}

int QFxPixmap::width() const
{
    return d->pixmap.width();
}

int QFxPixmap::height() const
{
    return d->pixmap.height();
}

QFxPixmap::operator const QPixmap &() const
{
    return d->pixmap;
}

/*!
    Starts a network request to load \a url. When the URL is loaded,
    the given slot is invoked. Note that if the image is already cached,
    the slot may be invoked immediately.

    Returns a QNetworkReply if the image is not immediately available, otherwise
    returns 0.  The QNetworkReply must not be stored - it may be destroyed at any time.
*/
QNetworkReply *QFxPixmap::get(QmlEngine *engine, const QUrl& url, QObject* obj, const char* slot)
{
#ifndef QT_NO_LOCALFILE_OPTIMIZED_QML
    if (url.scheme()==QLatin1String("file")) {
        QObject dummy;
        QObject::connect(&dummy, SIGNAL(destroyed()), obj, slot);
        return 0;
    }
#endif

    QString key = url.toString();
    if (QPixmapCache::find(key,0)) {
        QObject dummy;
        QObject::connect(&dummy, SIGNAL(destroyed()), obj, slot);
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

    QObject::connect((*iter)->reply, SIGNAL(finished()), obj, slot);
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

// Commit: f4560ff1e42aa5655376490f601ae15a5058c3b9

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


#include "qsgthreadedtexturemanager.h"
#include "qsgtexturemanager_p.h"

#include <QThread>
#include <QMutex>
#include <QWaitCondition>

class QSGThreadedTexture : public QSGTexture
{
    Q_OBJECT
public:
    QSGThreadedTexture(QSGThreadedTextureManager *m)
        : manager(m)
    {
    }
    ~QSGThreadedTexture();

    QSGThreadedTextureManager *manager;
    QSGTextureUploadRequest *request;
};


class QSGThreadedTextureManagerThread : public QThread
{
public:
    QSGThreadedTextureManagerThread(QSGThreadedTextureManager *m)
        : manager(m)
        , currentlyUploading(0)
    {
        manager->createThreadContext();
        start();
    }

    void run() {

        manager->makeThreadContextCurrent();

        while (true) {

            mutex.lock();
            currentlyUploading = 0;
            condition.wakeOne();

            if (requests.isEmpty()) {
                condition.wait(&mutex);
            }

            if (requests.isEmpty()) {
                mutex.unlock();
                continue;
            }

            currentlyUploading = requests.takeFirst();
            mutex.unlock();

            manager->uploadInThread(currentlyUploading, currentlyUploading->request);
        }
    }

    QSGThreadedTextureManager *manager;
    QSGThreadedTexture *currentlyUploading;

    QWaitCondition condition;
    QMutex mutex;

    QList<QSGThreadedTexture *> requests;
};


class QSGThreadedTextureManagerPrivate : public QSGTextureManagerPrivate
{
public:
    QSGThreadedTextureManagerThread *thread;

    QGLWidget *widget;
    QGLContext *context;
};


QSGThreadedTexture::~QSGThreadedTexture()
{
    QSGThreadedTextureManagerPrivate *d = manager->d_func();
    d->thread->mutex.lock();
    d->thread->requests.removeOne(this);
    while (d->thread->currentlyUploading == this)
        d->thread->condition.wait(&d->thread->mutex);
    d->thread->mutex.unlock();

    d->removeTextureFromCache(this);
}


QSGThreadedTextureManager::QSGThreadedTextureManager()
    : QSGTextureManager(*(new QSGThreadedTextureManagerPrivate))
{
    Q_D(QSGThreadedTextureManager);
    d->thread = 0;
    d->widget = 0;
    d->context = 0;
}

QSGTextureRef QSGThreadedTextureManager::upload(const QImage &image)
{
    Q_D(QSGThreadedTextureManager);

    QSGTextureCacheKey key = { image.cacheKey() };
    QSGTexture *texture = d->cache.value(key);

    if (texture) {

        QSGThreadedTexture *ttex = qobject_cast<QSGThreadedTexture *>(texture);
        if ((ttex && ttex->status() == QSGTexture::Ready) || !ttex) {
            // Already fully uploaded... Just return
            return QSGTextureRef(texture);

        } else {

            // Lock and wait for the texture to uploaded...
            d->thread->mutex.lock();
            while (ttex->status() == QSGTexture::InProgress) {
                d->thread->condition.wait(&d->thread->mutex);
            }
            d->thread->mutex.unlock();

            return QSGTextureRef(ttex);
        }
    }

    return d->upload(image);
}

void QSGThreadedTextureManager::requestUpload(QSGTextureUploadRequest *request)
{
    Q_D(QSGThreadedTextureManager);

    const QImage image = request->image();

    QSGTextureCacheKey key = { image.cacheKey() };
    QSGTexture *texture = d->cache.value(key);
    if (texture) {
        request->setTexture(texture);
        request->done();
        return;
    }

    QSGThreadedTexture *ttex = new QSGThreadedTexture(this);
    ttex->request = request;
    ttex->request->setTexture(ttex);
    ttex->setStatus(QSGTexture::InProgress);

    d->cache.insert(key, ttex);

    if (!d->thread) {
        d->thread = new QSGThreadedTextureManagerThread(this);
    }

    d->thread->mutex.lock();
    d->thread->requests << ttex;
    d->thread->condition.wakeOne();
    d->thread->mutex.unlock();
}

/*!
   Called by the rendering thread to create a new context that can be used in the
   upload thread.
 */
void QSGThreadedTextureManager::createThreadContext()
{
    Q_D(QSGThreadedTextureManager);

    QGLContext *ctx = const_cast<QGLContext *>(QGLContext::currentContext());

    // Getting the share widget from the current context is rather nasty and
    // will not work for lighthouse based
    Q_ASSERT(ctx->device() && ctx->device()->devType() == QInternal::Widget);
    QGLWidget *share = static_cast<QGLWidget *>(ctx->device());

    d->widget = new QGLWidget(0, share);
    d->widget->resize(8, 8);

    d->context = const_cast<QGLContext *>(d->widget->context());
    if (!d->context)
        qFatal("QSGThreadedTextureManager: failed to create thread context...");

    d->widget->doneCurrent();

    ctx->makeCurrent();
}


/*!
    Reimplement this function to make the threaded context current.

    This function is called from the background thread. The default
    implementation makes the context created in createThreadContext
    current.
 */
void QSGThreadedTextureManager::makeThreadContextCurrent()
{
    Q_D(QSGThreadedTextureManager);
    d->context->makeCurrent();
}


/*!
    Reimplement this function to upload images in the background thread.

    This function is called from the background thread. The default
    implementation does this using a single glTexImage2D call.
 */

void QSGThreadedTextureManager::uploadInThread(QSGTexture *texture, QSGTextureUploadRequest *request)
{
    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    QImage copy = request->image().convertToFormat(QImage::Format_ARGB32_Premultiplied);
    swizzleBGRAToRGBA(&copy);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, copy.width(), copy.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, copy.constBits());

    bool fail = glGetError() != GL_NO_ERROR;

    glBindTexture(GL_TEXTURE_2D, 0);

    if (fail) {
        texture->setStatus(QSGTexture::Null);
        glDeleteTextures(1, &id);
    } else {
        texture->setTextureId(id);
        texture->setTextureSize(copy.size());
        texture->setAlphaChannel(copy.hasAlphaChannel());
        texture->setStatus(QSGTexture::Ready);
    }

    static_cast<QSGThreadedTexture *>(texture)->request->done();
    static_cast<QSGThreadedTexture *>(texture)->request = 0;
}

#include "qsgthreadedtexturemanager.moc"

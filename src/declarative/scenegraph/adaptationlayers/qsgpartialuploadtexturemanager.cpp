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

#include "qsgpartialuploadtexturemanager.h"
#include "qsgtexturemanager_p.h"

#include <qdatetime.h>


class QSGPartialUploadTexture : public QSGTexture
{
    Q_OBJECT
public:
    QSGPartialUploadTexture(QSGPartialUploadTextureManager *m)
        : manager(m)
    {
    }

    ~QSGPartialUploadTexture();

    int chunkCount;
    int progress;

    QSGPartialUploadTextureManager *manager;
    QSGTextureUploadRequest *request;
};




class QSGPartialUploadTextureManagerPrivate : public QSGTextureManagerPrivate
{
public:

    QSGPartialUploadTextureManagerPrivate()
        : uploadChunkSize(64)
        , maxUploadTime(5)
        , timerId(0)
    {
    }

    int uploadChunkSize;
    int maxUploadTime;

    int timerId;

    QList<QSGPartialUploadTexture *> requests;
};

QSGPartialUploadTexture::~QSGPartialUploadTexture()
{
    manager->d_func()->requests.removeOne(this);
    manager->d_func()->removeTextureFromCache(this);
}


QSGPartialUploadTextureManager::QSGPartialUploadTextureManager()
    : QSGTextureManager(*(new QSGPartialUploadTextureManagerPrivate))
{
}


void QSGPartialUploadTextureManager::setContext(QSGContext *context)
{
    connect(context, SIGNAL(aboutToRenderNextFrame()), this, SLOT(processAsyncTextures()));
    QSGTextureManager::setContext(context);
}


void QSGPartialUploadTextureManager::requestUpload(QSGTextureUploadRequest *request)
{
    Q_D(QSGPartialUploadTextureManager);

    const QImage image = request->image();

    QSGTextureCacheKey key = { image.cacheKey() };
    QSGTexture *texture = d->cache.value(key);
    if (texture) {
        request->setTexture(texture);
        request->done();
        return;
    }

    QSGPartialUploadTexture *ptex = new QSGPartialUploadTexture(this);
    ptex->progress = 0;
    ptex->request = request;
    request->setTexture(ptex);

    int hChunkCount = (image.width() + d->uploadChunkSize - 1) / d->uploadChunkSize;
    int vChunkCount = (image.height() + d->uploadChunkSize - 1) / d->uploadChunkSize;
    ptex->chunkCount = hChunkCount * vChunkCount;
    ptex->setStatus(QSGTexture::InProgress);

    d->cache.insert(key, ptex);

    d->requests << ptex;
    if (d->timerId == 0)
        d->timerId = startTimer(100);
}

QSGTextureRef QSGPartialUploadTextureManager::upload(const QImage &image)
{
    Q_D(QSGPartialUploadTextureManager);

    QSGTextureCacheKey key = { image.cacheKey() };
    QSGTexture *texture = d->cache.value(key);
    if (texture) {

        QSGPartialUploadTexture *ptex = qobject_cast<QSGPartialUploadTexture *>(texture);
        if ((ptex && ptex->progress == ptex->chunkCount) || !ptex) {
            // Already fully uploaded... Just return
            return QSGTextureRef(texture);

        } else {
            // Image is not fully uploaded, force upload and return it.
            GLuint id = d->upload(image, ptex->textureId());
            if (id != 0) {
                ptex->progress = ptex->chunkCount;
                ptex->setTextureId(id);
                ptex->setAlphaChannel(image.hasAlphaChannel());
                ptex->setTextureSize(image.size());
                ptex->setStatus(QSGTexture::Ready);
            } else {
                ptex->setStatus(QSGTexture::Null);
            }
            d->requests.removeOne(ptex);
            ptex->request->done();
            ptex->request = 0;
            return QSGTextureRef(ptex);
        }
    }

    return d->upload(image);
}


void QSGPartialUploadTextureManager::timerEvent(QTimerEvent *event)
{
    Q_D(QSGPartialUploadTextureManager);

     // ### We should be listening for vsync driver to stop animating here...
    if (event->timerId() == d->timerId) {
        int old = d->maxUploadTime;
        d->maxUploadTime = 100;
        processAsyncTextures();
        d->maxUploadTime = old;
    }
}



void QSGPartialUploadTextureManager::processAsyncTextures()
{
    Q_D(QSGPartialUploadTextureManager);

    QTime time;
    time.start();

    while (!d->requests.isEmpty()) {

        QSGPartialUploadTexture *t = d->requests.at(0);
        const QImage image = t->request->image();

        int w = image.width();
        int h = image.height();

//        printf("\nASYNC: texture: %p, id=%d, size=(%dx%d), progress: %d / %d\n",
//               t,
//               t->textureId(),
//               w, h,
//               t->progress, t->chunkCount);

        // Create or bind the texture...
        if (t->textureId() == 0) {
            while (glGetError() != GL_NO_ERROR) {}
            GLuint id;
            glGenTextures(1, &id);
            glBindTexture(GL_TEXTURE_2D, id);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

            // Clean up
            // Gracefully fail in case of an error...
            GLuint error = glGetError();
            if (error != GL_NO_ERROR) {
                glBindTexture(GL_TEXTURE_2D, 0);
                glDeleteTextures(1, &id);
                d->requests.removeAt(0);
                t->setStatus(QSGTexture::Null);
                return;
            }

            t->setTextureId(id);
            t->setTextureSize(QSize(w, h));
            t->setAlphaChannel(image.hasAlphaChannel());

//            printf("ASYNC: created texture %p with id=%d\n", t, id);
        } else {
            glBindTexture(GL_TEXTURE_2D, t->textureId());
        }

        if (time.elapsed() > d->maxUploadTime)
            break;

        int hChunkCount = (image.width() + d->uploadChunkSize - 1) / d->uploadChunkSize;

        while (t->progress < t->chunkCount && time.elapsed() < d->maxUploadTime) {
            int x = (t->progress % hChunkCount) * d->uploadChunkSize;
            int y = (t->progress / hChunkCount) * d->uploadChunkSize;

            QRect area = QRect(x, y, d->uploadChunkSize, d->uploadChunkSize) & image.rect();
            QImage subImage = image.copy(area).convertToFormat(QImage::Format_ARGB32_Premultiplied);
//            printf("ASYNC: - doing another batch: %d (x=%d, y=%d, w=%d, h=%d\n",
//                   request->progress,
//                   x, y, subImage.width(), subImage.height());

            swizzleBGRAToRGBA(&subImage);
            glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, subImage.width(), subImage.height(), GL_RGBA, GL_UNSIGNED_BYTE, subImage.constBits());

            ++t->progress;
        }

        if (t->progress == t->chunkCount) {
            t->setStatus(QSGTexture::Ready);
            t->request->done();
            t->request = 0;
            d->requests.removeAt(0);
            if (d->requests.size() == 0) {
                killTimer(d->timerId);
                d->timerId = 0;
            }
        }
    }

    glBindTexture(GL_TEXTURE_2D, 0);
}

#include "qsgpartialuploadtexturemanager.moc"

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

#include "qsgtexturemanager_p.h"

#include <QImage>

#include <qgl.h>
#include <qhash.h>
#include <qqueue.h>
#include <qdatetime.h>

#ifndef GL_BGRA
#define GL_BGRA 0x80E1
#endif


void QSGTextureManagerPrivate::removeTextureFromCache(QSGTexture *texture)
{
    for (QHash<QSGTextureCacheKey, QSGTexture *>::iterator it = cache.begin();
         it != cache.end(); ++it) {
        if (it.value() == texture) {
            cache.remove(it.key());
            break;
        }
    }
}


QSGTexture::QSGTexture()
    : m_status(Null)
    , m_texture_id(0)
    , m_ref_count(0)
    , m_has_alpha(false)
    , m_owns_texture(true)
    , m_has_mipmaps(false)
{
}



QSGTexture::~QSGTexture()
{
    if (m_owns_texture)
        glDeleteTextures(1, (GLuint *) &m_texture_id);
}



void QSGTexture::setStatus(Status s)
{
    if (m_status == s)
        return;
    Q_ASSERT(s != Ready || (m_texture_id > 0 && !m_texture_size.isEmpty()));
    m_status = s;
}


uint qHash(const QSGTextureCacheKey &key)
{
    return (key.cacheKey >> 32) ^ uint(key.cacheKey);
}


/*!
    The QSGTextureManager class is responsible for converting QImages into texture ids
    inside the QML scene graph.

    The QSGTextureManager is created by the QSGContext class after a GL context is
    available, so the QSGTextureManager and subclasses can safely assume that a
    GL context is bound and make GL calls.
  */
QSGTextureManager::QSGTextureManager()
    : QObject(*new QSGTextureManagerPrivate)
{
    Q_D(QSGTextureManager);
    d->q = this;
}


/*!
   \internal
 */
QSGTextureManager::QSGTextureManager(QSGTextureManagerPrivate &dd)
    : QObject(dd)
{
    dd.q = this;
}


int QSGTextureManager::maxTextureSize() const
{
    Q_D(const QSGTextureManager);
    if (d->maxTextureSize < 0)
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &const_cast<QSGTextureManagerPrivate *>(d)->maxTextureSize);
    return d->maxTextureSize;
}


void QSGTextureManager::setContext(QSGContext *context)
{
    Q_D(QSGTextureManager);
    Q_ASSERT(!d->context);
    d->context = context;
}

QSGContext *QSGTextureManager::context() const
{
    Q_D(const QSGTextureManager);
    return d->context;
}


void QSGTextureManager::textureDestroyed(QObject *destroyed)
{
    Q_D(QSGTextureManager);
    d->removeTextureFromCache(static_cast<QSGTexture *>(destroyed));
}


void QSGTextureManager::swizzleBGRAToRGBA(QImage *image)
{
    const int width = image->width();
    const int height = image->height();
    for (int i = 0; i < height; ++i) {
        uint *p = (uint *) image->scanLine(i);
        for (int x = 0; x < width; ++x)
            p[x] = ((p[x] << 16) & 0xff0000) | ((p[x] >> 16) & 0xff) | (p[x] & 0xff00ff00);
    }
}

uint QSGTextureManagerPrivate::upload(const QImage &image, GLuint id)
{
    // image not already scheduled, upload normally...
    while (glGetError() != GL_NO_ERROR) {}

    if (id == 0)
        glGenTextures(1, &id);

    glBindTexture(GL_TEXTURE_2D, id);

    QImage i = image.convertToFormat(QImage::Format_ARGB32_Premultiplied);

#ifdef QT_OPENGL_ES
    QSGTextureManager::swizzleBGRAToRGBA(&i);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, i.width(), i.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, i.constBits());
#else
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, i.width(), i.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, i.constBits());
#endif

    // Gracefully fail in case of an error...
    GLuint error = glGetError();
    if (error != GL_NO_ERROR) {
        glBindTexture(GL_TEXTURE_2D, 0);
        glDeleteTextures(1, &id);
        return 0;
    }

    return id;
}

QSGTextureRef QSGTextureManagerPrivate::upload(const QImage &image)
{
    Q_ASSERT(!image.isNull());

    // Check if the image is already uploaded and cached
    QSGTextureCacheKey key = { image.cacheKey() };
    QSGTexture *texture = cache.value(key);
    if (texture)
        return QSGTextureRef(texture);

    GLuint id = upload(image, 0);
    if (id == 0)
        return QSGTextureRef();

    texture = new QSGTexture;
    texture->setTextureId(id);
    texture->setTextureSize(image.size());
    texture->setAlphaChannel(image.hasAlphaChannel());
    texture->setStatus(QSGTexture::Ready);

    QObject::connect(texture, SIGNAL(destroyed(QObject*)), q, SLOT(textureDestroyed(QObject*)));

    cache.insert(key, texture);

    QSGTextureRef ref(texture);
    return ref;
}


QSGTextureRef QSGTextureManager::upload(const QImage &image)
{
    Q_D(QSGTextureManager);
    return d->upload(image);
}


/*!
    Schedules the image in \a request to be uploaded.

    In most cases, the texture manager will use some means of delaying
    the upload so that the rendering is interrupted in the least
    possible way.

    When the upload happens is up to the implementation. It may happen
    immediately. The ownership of the \a request is transferred to
    the texture manager.
  */
void QSGTextureManager::requestUpload(QSGTextureUploadRequest *request)
{
    request->setTexture(upload(request->image()));
    request->done();
}


class QSGTextureUploadRequestPrivate : public QObjectPrivate
{
public:
    QImage image;
    QSGTextureRef texture;
};



/*!
    Creates a new texture upload request...
 */
QSGTextureUploadRequest::QSGTextureUploadRequest()
    : QObject(*new QSGTextureUploadRequestPrivate)
{
}


void QSGTextureUploadRequest::setImage(const QImage &image)
{
    Q_D(QSGTextureUploadRequest);
    d->image = image;
}

QImage QSGTextureUploadRequest::image() const
{
    Q_D(const QSGTextureUploadRequest);
    return d->image;
}


void QSGTextureUploadRequest::setTexture(const QSGTextureRef &ref)
{
    Q_D(QSGTextureUploadRequest);
    d->texture = ref;
}

QSGTextureRef QSGTextureUploadRequest::texture() const
{
    Q_D(const QSGTextureUploadRequest);
    return d->texture;
}


/*!
    Called by the texture manager when the request is done.

    The default implementation will emit the requestCompleted() signal
    and then delete the request object.
 */
void QSGTextureUploadRequest::done()
{
    emit requestCompleted(this);
    delete this;
}

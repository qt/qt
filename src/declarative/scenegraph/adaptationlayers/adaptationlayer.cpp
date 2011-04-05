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

#include "adaptationlayer.h"

#include <qdatetime.h>

///*!
//    Constructs a new texture reference with status set to Null
// */
//TextureReference::TextureReference()
//    : m_status(Null)
//    , m_texture_id(0)
//    , m_sub_rect(0, 0, 1, 1)
//    , m_has_alpha(false)
//    , m_mipmap(false)
//{
//}

//TextureReference::~TextureReference()
//{
//    if (m_owns_texture) {
//        glDeleteTextures(1, (GLuint *) &m_texture_id);
//    }
//}

//void TextureReference::setStatus(Status s)
//{
//    m_status = s;

//    Q_ASSERT(s != Uploaded || (m_texture_id > 0 && !m_texture_size.isEmpty()));

//    emit statusChanged(s);
//}

//struct QSGTextureCacheKey {
//    quint64 cacheKey;
//    uint hints;
//};

//struct QSGTextureCacheEntry {
//    int ref;
//    GLuint id;
//};

//class QSGCachedTextureReference : public TextureReference
//{
//public:
//    ~QSGCachedTextureReference();

//    QSGTextureCacheEntry *entry;
//    TextureManagerPrivate *d;
//};


//uint qHash(const QSGTextureCacheKey &key)
//{
//    return (key.cacheKey >> 32) ^ uint(key.cacheKey) ^ uint(key.hints);
//}

//class TextureManagerPrivate
//{
//public:
//    QHash<QSGTextureCacheKey, QSGTextureCacheEntry *> cache;
//};

//QSGCachedTextureReference::~QSGCachedTextureReference()
//{
//    if (!--entry->ref) {
//        d->cache.remove(d->cache.key(entry));
//        glDeleteTextures(1, &entry->id);
//        delete entry;
//    }
//}

//const QSGTextureRef &TextureManager::requestUploadedTexture(const QImage &image,
//                                                               UploadHints hints,
//                                                               QObject *listener,
//                                                               const char *slot)
//{

//    QSGTextureCacheKey key = { image.cacheKey(), uint(hints) };
//    QSGTextureCacheEntry *entry = d->cache.value(key);

//    QSGCachedTextureReference *texture = new QSGCachedTextureReference();
//    QObject::connect(texture, SIGNAL(statusChanged(int)), listener, slot);
//    texture->setTextureSize(image.size());
//    texture->setAlphaChannel(image.hasAlphaChannel());
//    texture->setMipmaps(hints & GenerateMipmapUploadHint);

//    if (!entry) {
//        entry = new QSGTextureCacheEntry;
//        entry->id = uploadTexture(image, hints);
//        entry->ref = 1;
//    } else {
//        ++entry->ref;
//        // ### gunnar: this currently does not support the usecase where
//        // the same image is uploaded both as async and as sync...
//        texture->setStatus(TextureReference::Uploaded);
//    }

//    texture->setTextureId(entry->id);
//    texture->entry = entry;
//    return texture;
//}

///*!
//    Performs a synchronous upload of \a image using the specified \a hints.

//    The upload is done using the QGLContext::bindTexture().
// */
//GLuint TextureManager::uploadTexture(const QImage &image, UploadHints hints)
//{
//}

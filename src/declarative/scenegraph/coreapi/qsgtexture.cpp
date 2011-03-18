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

#include "qsgtexture_p.h"


QSGTexturePrivate::QSGTexturePrivate()
    : wrapChanged(false)
    , filteringChanged(false)
    , horizontalWrap(QSGTexture::ClampToEdge)
    , verticalWrap(QSGTexture::ClampToEdge)
    , mipmapMode(QSGTexture::None)
    , filterMode(QSGTexture::Nearest)
{
}

QSGTexture::QSGTexture()
    : QObject(*(new QSGTexturePrivate))
{

}

QSGTexture::~QSGTexture()
{

}


/*!
    \fn void QSGTexture::setImage(const QImage &image)

    This function may be calld from arbitrary an arbitrary thread and may not
    use GL calls.
 */


/*!
    \fn void QSGTexture::bind()

    Call this function to bind this texture to the current texture
    target.

    Binding a texture may also include uploading the texture data from
    a previously set QImage.
 */

void QSGTexture::removeFromAtlas()
{
    // default textures are not in atlasses, so do nothing...
}

/*!
    Returns weither this texture is part of an atlas or not.

    The default implementation returns false.
 */
bool QSGTexture::isAtlasTexture() const
{
    return false;
}


/*!
    Returns the rectangle inside textureSize() that this texture
    represents in normalized coordinates.

    The default implementation returns a rect at position (0, 0) with
    width and height of 1.
 */
QRectF QSGTexture::textureSubRect() const
{
    return QRectF(0, 0, 1, 1);
}

/*!
    \fn bool QSGTexture::hasMipmaps() const

    Returns true if the texture data contains mipmap levels.
 */


/*!
    Sets wether mipmapping should be used when sampling from this texture.
 */
void QSGTexture::setMipmapFiltering(Filtering filter)
{
    Q_D(QSGTexture);
    if (d->mipmapMode != filter) {
        d->mipmapMode = filter;
        d->filteringChanged = true;
    }
}

/*!
    Returns wether mipmapping should be used when sampling from this texture.
 */
QSGTexture::Filtering QSGTexture::mipmapFiltering() const
{
    return (QSGTexture::Filtering) d_func()->mipmapMode;
}



void QSGTexture::setFiltering(QSGTexture::Filtering filter)
{
    Q_D(QSGTexture);
    if (d->filterMode != filter) {
        d->filterMode = filter;
        d->filteringChanged = true;
    }
}

QSGTexture::Filtering QSGTexture::filtering() const
{
    return (QSGTexture::Filtering) d_func()->filterMode;
}



void QSGTexture::setHorizontalWrapMode(WrapMode hwrap)
{
    Q_D(QSGTexture);
    if (hwrap != d->horizontalWrap) {
        d->horizontalWrap = hwrap;
        d->wrapChanged = true;
    }
}

QSGTexture::WrapMode QSGTexture::horizontalWrapMode() const
{
    return (QSGTexture::WrapMode) d_func()->horizontalWrap;
}



void QSGTexture::setVerticalWrapMode(WrapMode vwrap)
{
    Q_D(QSGTexture);
    if (vwrap != d->verticalWrap) {
        d->verticalWrap = vwrap;
        d->wrapChanged = true;
    }
}

QSGTexture::WrapMode QSGTexture::verticalWrapMode() const
{
    return (QSGTexture::WrapMode) d_func()->verticalWrap;
}


/*!
    Update the texture state to match the filtering, mipmap and wrap options
    currently set.

    If \a force is true, all properties will be updated regardless of weither
    they have changed or not.
 */
void QSGTexture::updateBindOptions(bool force)
{
    Q_D(QSGTexture);
    if (force || d->filteringChanged) {
        bool linear = d->filterMode == Linear;
        GLint minFilter = linear ? GL_LINEAR : GL_NEAREST;
        GLint magFilter = linear ? GL_LINEAR : GL_NEAREST;

        if (hasMipmaps()) {
            if (d->mipmapMode == Nearest)
                minFilter = linear ? GL_LINEAR_MIPMAP_NEAREST : GL_NEAREST_MIPMAP_NEAREST;
            else if (d->mipmapMode == Linear)
                minFilter = linear ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_LINEAR;
        }
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
        d->filteringChanged = false;
    }

    if (force || d->wrapChanged) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, d->horizontalWrap == Repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, d->verticalWrap == Repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE);
        d->wrapChanged = false;
    }
}


void QSGTextureRef::deref()
{
    // ### For multithreaded renderer we need to handle this better... Post something to the renderer thread, for instance
    if (m_texture && !--m_texture->m_ref_count) {
        delete m_texture;
    }
}



QSGPlainTexture::QSGPlainTexture()
    : QSGTexture()
    , m_texture_id(0)
    , m_has_alpha(false)
    , m_has_mipmaps(false)
    , m_owns_texture(true)
{
}


QSGPlainTexture::~QSGPlainTexture()
{
    if (m_texture_id && m_owns_texture)
        glDeleteTextures(1, &m_texture_id);
}


void QSGPlainTexture::setImage(const QImage &image)
{
    m_image = image.convertToFormat(QImage::Format_ARGB32_Premultiplied);
#ifdef QT_OPENGL_ES
    QSGTextureManager::swizzleBGRAToRGBA(&m_image);
#endif

    m_texture_size = image.size();
    m_has_alpha = image.hasAlphaChannel();
    m_dirty_texture = true;
 }

void QSGPlainTexture::setTextureId(int id)
{
    m_texture_id = id;
    m_dirty_texture = false;
}


void QSGPlainTexture::bind()
{
    if (!m_dirty_texture) {
        glBindTexture(GL_TEXTURE_2D, m_texture_id);
        updateBindOptions();
        return;
    }

    m_dirty_texture = false;

    if (m_texture_id)
        glDeleteTextures(1, &m_texture_id);

    if (m_image.isNull()) {
        m_texture_id == 0;
        m_texture_size = QSize();
        m_has_mipmaps = false;
        m_has_alpha = false;
        return;
    }

    glGenTextures(1, &m_texture_id);
    glBindTexture(GL_TEXTURE_2D, m_texture_id);

    // ### TODO: check for out-of-memory situations...
    int w = m_image.width();
    int h = m_image.height();

#ifdef QT_OPENGL_ES
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_image.constBits());
#else
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, m_image.constBits());
#endif

    if (m_has_mipmaps)
        glGenerateMipmap(GL_TEXTURE_2D);

    m_texture_size = QSize(w, h);
    m_texture_rect = QRectF(0, 0, 1, 1);

    updateBindOptions(true);
 }

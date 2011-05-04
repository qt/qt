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

#define GL_GLEXT_PROTOTYPES

#include <private/qsgtexture_p.h>
#include <qglfunctions.h>
#include <private/qsgcontext_p.h>
#include <qthread.h>

QT_BEGIN_NAMESPACE

QSGTexturePrivate::QSGTexturePrivate()
    : wrapChanged(false)
    , filteringChanged(false)
    , horizontalWrap(QSGTexture::ClampToEdge)
    , verticalWrap(QSGTexture::ClampToEdge)
    , mipmapMode(QSGTexture::None)
    , filterMode(QSGTexture::Nearest)
{
}

#ifndef QT_NO_DEBUG
static int qt_texture_count = 0;

static void qt_print_texture_count()
{
    qDebug("Number of leaked textures: %i", qt_texture_count);
    qt_texture_count = -1;
}
#endif



QSGTexture::QSGTexture()
    : QObject(*(new QSGTexturePrivate))
{
#ifndef QT_NO_DEBUG
    ++qt_texture_count;
    static bool atexit_registered = false;
    if (!atexit_registered) {
        atexit(qt_print_texture_count);
        atexit_registered = true;
    }
#endif
}

QSGTexture::~QSGTexture()
{
#ifndef QT_NO_DEBUG
    --qt_texture_count;
    if (qt_texture_count < 0)
        qDebug("Material destroyed after qt_print_texture_count() was called.");
#endif
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
    Sets the mipmap sampling mode to be used for the upcoming bind() call to \a filter.

    Setting the mipmap filtering has no effect it the texture does not have mipmaps.

    \sa hasMipmaps()
 */
void QSGTexture::setMipmapFiltering(Filtering filter)
{
    Q_D(QSGTexture);
    if (d->mipmapMode != (uint) filter) {
        d->mipmapMode = filter;
        d->filteringChanged = true;
    }
}

/*!
    Returns whether mipmapping should be used when sampling from this texture.
 */
QSGTexture::Filtering QSGTexture::mipmapFiltering() const
{
    return (QSGTexture::Filtering) d_func()->mipmapMode;
}


/*!
    Sets the sampling mode to be used for the upcoming bind() call to \a filter.
 */
void QSGTexture::setFiltering(QSGTexture::Filtering filter)
{
    Q_D(QSGTexture);
    if (d->filterMode != (uint) filter) {
        d->filterMode = filter;
        d->filteringChanged = true;
    }
}

QSGTexture::Filtering QSGTexture::filtering() const
{
    return (QSGTexture::Filtering) d_func()->filterMode;
}



/*!
    Sets the horizontal wrap mode to be used for the upcoming bind() call to \a hwrap
 */

void QSGTexture::setHorizontalWrapMode(WrapMode hwrap)
{
    Q_D(QSGTexture);
    if ((uint) hwrap != d->horizontalWrap) {
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
    if ((uint) vwrap != d->verticalWrap) {
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

QSGPlainTexture::QSGPlainTexture()
    : QSGTexture()
    , m_texture_id(0)
    , m_has_alpha(false)
    , m_has_mipmaps(false)
    , m_dirty_bind_options(false)
    , m_owns_texture(true)
    , m_mipmaps_generated(false)
{
}


QSGPlainTexture::~QSGPlainTexture()
{
    if (m_texture_id && m_owns_texture)
        glDeleteTextures(1, &m_texture_id);
}

#ifdef QT_OPENGL_ES
static void swizzleBGRAToRGBA(QImage *image)
{
    const int width = image->width();
    const int height = image->height();
    for (int i = 0; i < height; ++i) {
        uint *p = (uint *) image->scanLine(i);
        for (int x = 0; x < width; ++x)
            p[x] = ((p[x] << 16) & 0xff0000) | ((p[x] >> 16) & 0xff) | (p[x] & 0xff00ff00);
    }
}
#endif

void QSGPlainTexture::setImage(const QImage &image)
{
    m_image = image.convertToFormat(QImage::Format_ARGB32_Premultiplied);
#ifdef QT_OPENGL_ES
    swizzleBGRAToRGBA(&m_image);
#endif

    m_texture_size = image.size();
    m_has_alpha = image.hasAlphaChannel();
    m_dirty_texture = true;
    m_dirty_bind_options = true;
 }

void QSGPlainTexture::setTextureId(int id)
{
    if (m_texture_id && m_owns_texture)
        glDeleteTextures(1, &m_texture_id);

    m_texture_id = id;
    m_dirty_texture = false;
    m_dirty_bind_options = true;
    m_image = QImage();
    m_mipmaps_generated = false;
}

void QSGPlainTexture::setHasMipmaps(bool mm)
{
    m_has_mipmaps = mm;
    m_mipmaps_generated = false;
}


void QSGPlainTexture::bind()
{
    if (!m_dirty_texture) {
        glBindTexture(GL_TEXTURE_2D, m_texture_id);
        if (m_has_mipmaps && !m_mipmaps_generated) {
            const QGLContext *ctx = QGLContext::currentContext();
            ctx->functions()->glGenerateMipmap(GL_TEXTURE_2D);
            m_mipmaps_generated = true;
        }
        updateBindOptions(m_dirty_bind_options);
        m_dirty_bind_options = false;
        return;
    }

    m_dirty_texture = false;

    if (m_texture_id && m_owns_texture)
        glDeleteTextures(1, &m_texture_id);

    if (m_image.isNull()) {
        m_texture_id = 0;
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    for (int i = 0; i < m_image.height(); ++i)
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, i, w, 1, GL_RGBA, GL_UNSIGNED_BYTE, m_image.constScanLine(h - 1 - i));
#else
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, 0);
    for (int i = 0; i < m_image.height(); ++i)
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, i, w, 1, GL_BGRA, GL_UNSIGNED_BYTE, m_image.constScanLine(h - 1 - i));
#endif

    if (m_has_mipmaps) {
        const QGLContext *ctx = QGLContext::currentContext();
        ctx->functions()->glGenerateMipmap(GL_TEXTURE_2D);
        m_mipmaps_generated = true;
    }

    m_texture_size = QSize(w, h);

    updateBindOptions(m_dirty_bind_options);
    m_dirty_bind_options = false;
}


/*!
    \class QSGDynamicTexture
    \brief The QSGDynamicTexture class serves as a baseclass for dynamically changing textures,
    such as content that is rendered to FBO's.

    To update the content of the texture, call updateTexture() explicitly. Simply calling bind()
    will not update the texture.
 */


/*!
    \fn bool QSGDynamicTexture::updateTexture()

    Call this function to explicitely update the dynamic texture. Calling bind() will bind
    the content that was previously updated.

    The function returns true if the texture was changed as a resul of the update; otherwise
    returns false.
 */



QT_END_NAMESPACE

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

#include "qsgpainternode_p.h"

#include "qsgpainteditem_p.h"
#include "private/qsgcontext_p.h"
#include <qglframebufferobject.h>

QT_BEGIN_NAMESPACE

QSGPainterTexture::QSGPainterTexture()
    : QSGPlainTexture()
{

}

void QSGPainterTexture::bind()
{
    if (m_dirty_rect.isNull() || m_texture_id == 0) {
        QSGPlainTexture::bind();
    } else {
        glBindTexture(GL_TEXTURE_2D, m_texture_id);

        QImage subImage = m_image.copy(m_dirty_rect);

#ifdef QT_OPENGL_ES
        glTexSubImage2D(GL_TEXTURE_2D, 0,
                        m_dirty_rect.x(), m_dirty_rect.y(), m_dirty_rect.width(), m_dirty_rect.height(),
                        GL_RGBA, GL_UNSIGNED_BYTE, subImage.constBits());
#else
        glTexSubImage2D(GL_TEXTURE_2D, 0,
                        m_dirty_rect.x(), m_dirty_rect.y(), m_dirty_rect.width(), m_dirty_rect.height(),
                        GL_BGRA, GL_UNSIGNED_BYTE, subImage.constBits());
#endif

        m_dirty_texture = false;
        m_dirty_bind_options = false;
    }
    m_dirty_rect = QRect();
}

QSGPainterNode::QSGPainterNode()
    : QSGGeometryNode()
    , m_preferredPaintSurface(Image)
    , m_actualPaintSurface(Image)
    , m_fbo(0)
    , m_multisampledFbo(0)
    , m_geometry(QSGGeometry::defaultAttributes_TexturedPoint2D(), 4)
    , m_size(1, 1)
    , m_opaquePainting(false)
    , m_linear_filtering(false)
    , m_smoothPainting(false)
    , m_extensionsChecked(false)
    , m_multisamplingSupported(false)
    , m_dirtyGeometry(false)
    , m_dirtySurface(false)
    , m_dirtyTexture(false)
{
    setMaterial(&m_materialO);
    setOpaqueMaterial(&m_material);
    setGeometry(&m_geometry);
}

QSGPainterNode::~QSGPainterNode()
{
    delete m_texture;
    delete m_fbo;
    delete m_multisampledFbo;
}

void QSGPainterNode::setPreferredPaintSurface(PaintSurface surface)
{
    m_preferredPaintSurface = surface;
}

void QSGPainterNode::paint(QSGPaintedItem *item, const QRect &clipRect)
{
    if (!item)
        return;

    QRect dirtyRect = clipRect.isNull() ? QRect(0, 0, m_size.width(), m_size.height()) : clipRect;

    QPainter painter;
    if (m_actualPaintSurface == Image)
        painter.begin(&m_image);
    else if (m_multisampledFbo)
        painter.begin(m_multisampledFbo);
    else
        painter.begin(m_fbo);

    if (m_smoothPainting) {
        painter.setRenderHints(QPainter::Antialiasing | QPainter::HighQualityAntialiasing
                               | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    }
    if (!m_opaquePainting) {
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        painter.fillRect(dirtyRect, Qt::transparent);
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    }
    if (!clipRect.isNull())
        painter.setClipRect(dirtyRect);
    item->paint(&painter);
    painter.end();

    if (m_actualPaintSurface == Image) {
        QSGPainterTexture *tex = m_texture;
        tex->setImage(m_image);
        tex->setDirtyRect(dirtyRect);
    } else if (m_multisampledFbo) {
        QGLFramebufferObject::blitFramebuffer(m_fbo, dirtyRect, m_multisampledFbo, dirtyRect);
    }

    markDirty(DirtyMaterial);
}

void QSGPainterNode::update()
{
    if (!m_extensionsChecked) {
        QList<QByteArray> extensions = QByteArray((const char *)glGetString(GL_EXTENSIONS)).split(' ');
        m_multisamplingSupported = extensions.contains("GL_EXT_framebuffer_multisample")
                && extensions.contains("GL_EXT_framebuffer_blit");
        m_extensionsChecked = true;
    }
    if (m_preferredPaintSurface == Image) {
        m_actualPaintSurface = Image;
    } else {
        if (!m_multisamplingSupported && m_smoothPainting)
            m_actualPaintSurface = Image;
        else
            m_actualPaintSurface = FramebufferObject;
    }

    if (m_dirtyGeometry)
        updateGeometry();
    if (m_dirtySurface || m_dirtyGeometry)
        updateSurface();
    if (m_dirtySurface || m_dirtyTexture)
        updateTexture();

    m_dirtyGeometry = false;
    m_dirtySurface = false;
    m_dirtyTexture = false;
}

void QSGPainterNode::updateTexture()
{
    m_texture->setHasAlphaChannel(!m_opaquePainting);
    m_material.setTexture(m_texture);
    m_materialO.setTexture(m_texture);

    markDirty(DirtyMaterial);
}

void QSGPainterNode::updateGeometry()
{
    QRectF source;
    if (m_actualPaintSurface == Image)
        source = QRectF(0, 0, 1, 1);
    else
        source = QRectF(0, 1, 1, -1);
    QSGGeometry::updateTexturedRectGeometry(&m_geometry,
                                            QRectF(0, 0, m_size.width(), m_size.height()),
                                            source);
    markDirty(DirtyGeometry);
}

void QSGPainterNode::updateSurface()
{
    if (m_actualPaintSurface == FramebufferObject) {
        const QGLContext *ctx = QGLContext::currentContext();
        if (m_fbo && !m_dirtyGeometry && (!ctx->format().sampleBuffers() || !m_multisamplingSupported))
            return;

        delete m_fbo;
        delete m_multisampledFbo;
        m_fbo = m_multisampledFbo = 0;

        if (m_smoothPainting && ctx->format().sampleBuffers() && m_multisamplingSupported) {
            if (!m_multisampledFbo) {
                QGLFramebufferObjectFormat format;
                format.setAttachment(QGLFramebufferObject::CombinedDepthStencil);
                format.setSamples(ctx->format().samples());
                m_multisampledFbo = new QGLFramebufferObject(m_size, format);
            }
            {
                QGLFramebufferObjectFormat format;
                format.setAttachment(QGLFramebufferObject::NoAttachment);
                m_fbo = new QGLFramebufferObject(m_size, format);
            }
        } else {
            QGLFramebufferObjectFormat format;
            format.setAttachment(QGLFramebufferObject::CombinedDepthStencil);
            m_fbo = new QGLFramebufferObject(m_size, format);
        }
    } else {
        m_image = QImage(m_size, QImage::Format_ARGB32_Premultiplied);
    }

    QSGPainterTexture *texture = new QSGPainterTexture;
    if (m_actualPaintSurface == Image) {
        texture->setOwnsTexture(true);
    } else {
        texture->setTextureId(m_fbo->texture());
        texture->setOwnsTexture(false);
    }

    if (m_texture)
        delete m_texture;

    texture->setTextureSize(m_size);
    m_texture = texture;
    m_material.setFiltering(m_linear_filtering ? QSGTexture::Linear : QSGTexture::Nearest);
    m_materialO.setFiltering(m_linear_filtering ? QSGTexture::Linear : QSGTexture::Nearest);
}

void QSGPainterNode::setSize(const QSize &size)
{
    if (size == m_size)
        return;

    m_size = size;
    m_dirtyGeometry = true;
    m_dirtySurface = true;
}

void QSGPainterNode::setOpaquePainting(bool opaque)
{
    if (opaque == m_opaquePainting)
        return;

    m_opaquePainting = opaque;
    m_dirtyTexture = true;
}

void QSGPainterNode::setLinearFiltering(bool linearFiltering)
{
    if (linearFiltering == m_linear_filtering)
        return;

    m_linear_filtering = linearFiltering;

    m_material.setFiltering(linearFiltering ? QSGTexture::Linear : QSGTexture::Nearest);
    m_materialO.setFiltering(linearFiltering ? QSGTexture::Linear : QSGTexture::Nearest);
    markDirty(DirtyMaterial);
}

void QSGPainterNode::setSmoothPainting(bool s)
{
    if (s == m_smoothPainting)
        return;

    m_smoothPainting = s;
    m_dirtySurface = true;
}


QT_END_NAMESPACE

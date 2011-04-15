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

#include "qsgpainteditem.h"
#include <private/qsgcontext_p.h>
#include <qglframebufferobject.h>
#include <qglfunctions.h>

QT_BEGIN_NAMESPACE

#define QT_MINIMUM_FBO_SIZE 64

static inline int qt_next_power_of_two(int v)
{
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    ++v;
    return v;
}

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

        int w = m_dirty_rect.width();
        int h = m_dirty_rect.height();
        int y = m_image.height() - m_dirty_rect.y() - h;

#ifdef QT_OPENGL_ES
        for (int i = 0; i < h; ++i) {
            glTexSubImage2D(GL_TEXTURE_2D, 0, m_dirty_rect.x(), y + i, w, 1,
                            GL_RGBA, GL_UNSIGNED_BYTE, subImage.constScanLine(h - 1 - i));
        }
#else
        for (int i = 0; i < h; ++i) {
            glTexSubImage2D(GL_TEXTURE_2D, 0, m_dirty_rect.x(), y + i, w, 1,
                            GL_BGRA, GL_UNSIGNED_BYTE, subImage.constScanLine(h - 1 - i));
        }

#endif

        m_dirty_texture = false;
        m_dirty_bind_options = false;
    }
    m_dirty_rect = QRect();
}

QSGPainterNode::QSGPainterNode(QSGPaintedItem *item)
    : QSGGeometryNode()
    , m_preferredRenderTarget(QSGPaintedItem::Image)
    , m_actualRenderTarget(QSGPaintedItem::Image)
    , m_item(item)
    , m_fbo(0)
    , m_multisampledFbo(0)
    , m_geometry(QSGGeometry::defaultAttributes_TexturedPoint2D(), 4)
    , m_texture(0)
    , m_size(1, 1)
    , m_dirtyContents(false)
    , m_opaquePainting(false)
    , m_linear_filtering(false)
    , m_smoothPainting(false)
    , m_extensionsChecked(false)
    , m_multisamplingSupported(false)
    , m_fillColor(Qt::transparent)
    , m_dirtyGeometry(false)
    , m_dirtyRenderTarget(false)
    , m_dirtyTexture(false)
{
    setMaterial(&m_materialO);
    setOpaqueMaterial(&m_material);
    setGeometry(&m_geometry);
    setFlag(UsePreprocess);
}

QSGPainterNode::~QSGPainterNode()
{
    delete m_texture;
    delete m_fbo;
    delete m_multisampledFbo;
}

void QSGPainterNode::preprocess()
{
    if (!m_dirtyContents)
        return;

    QRect dirtyRect = m_dirtyRect.isNull() ? QRect(0, 0, m_size.width(), m_size.height()) : m_dirtyRect;

    QPainter painter;
    if (m_actualRenderTarget == QSGPaintedItem::Image)
        painter.begin(&m_image);
    else if (m_multisampledFbo)
        painter.begin(m_multisampledFbo);
    else
        painter.begin(m_fbo);

    if (m_smoothPainting) {
        painter.setRenderHints(QPainter::Antialiasing | QPainter::HighQualityAntialiasing
                               | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    }

    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(dirtyRect, m_fillColor);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    if (!m_dirtyRect.isNull())
        painter.setClipRect(dirtyRect);
    m_item->paint(&painter);
    painter.end();

    if (m_actualRenderTarget == QSGPaintedItem::Image) {
        m_texture->setImage(m_image);
        m_texture->setDirtyRect(dirtyRect);
    } else if (m_multisampledFbo) {
        QGLFramebufferObject::blitFramebuffer(m_fbo, dirtyRect, m_multisampledFbo, dirtyRect);
    }

    m_dirtyContents = false;
    m_dirtyRect = QRect();
}

void QSGPainterNode::update()
{
    if (m_dirtyRenderTarget)
        updateRenderTarget();
    if (m_dirtyGeometry)
        updateGeometry();
    if (m_dirtyTexture)
        updateTexture();

    m_dirtyGeometry = false;
    m_dirtyRenderTarget = false;
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
    if (m_actualRenderTarget == QSGPaintedItem::Image)
        source = QRectF(0, 1, 1, -1);
    else
        source = QRectF(0, 1, qreal(m_size.width()) / m_fboSize.width(), qreal(-m_size.height()) / m_fboSize.height());
    QSGGeometry::updateTexturedRectGeometry(&m_geometry,
                                            QRectF(0, 0, m_size.width(), m_size.height()),
                                            source);
    markDirty(DirtyGeometry);
}

void QSGPainterNode::updateRenderTarget()
{
    if (!m_extensionsChecked) {
        QList<QByteArray> extensions = QByteArray((const char *)glGetString(GL_EXTENSIONS)).split(' ');
        m_multisamplingSupported = extensions.contains("GL_EXT_framebuffer_multisample")
                && extensions.contains("GL_EXT_framebuffer_blit");
        m_extensionsChecked = true;
    }

    m_dirtyContents = true;

    QSGPaintedItem::RenderTarget oldTarget = m_actualRenderTarget;
    if (m_preferredRenderTarget == QSGPaintedItem::Image) {
        m_actualRenderTarget = QSGPaintedItem::Image;
    } else {
        if (!m_multisamplingSupported && m_smoothPainting)
            m_actualRenderTarget = QSGPaintedItem::Image;
        else
            m_actualRenderTarget = QSGPaintedItem::FramebufferObject;
    }
    if (oldTarget != m_actualRenderTarget) {
        m_image = QImage();
        delete m_fbo;
        delete m_multisampledFbo;
        m_fbo = m_multisampledFbo = 0;
    }

    if (m_actualRenderTarget == QSGPaintedItem::FramebufferObject) {
        const QGLContext *ctx = QGLContext::currentContext();
        if (m_fbo && !m_dirtyGeometry && (!ctx->format().sampleBuffers() || !m_multisamplingSupported))
            return;

        if (m_fboSize.isEmpty())
            updateFBOSize();

        delete m_fbo;
        delete m_multisampledFbo;
        m_fbo = m_multisampledFbo = 0;

        if (m_smoothPainting && ctx->format().sampleBuffers() && m_multisamplingSupported) {
            {
                QGLFramebufferObjectFormat format;
                format.setAttachment(QGLFramebufferObject::CombinedDepthStencil);
                format.setSamples(ctx->format().samples());
                m_multisampledFbo = new QGLFramebufferObject(m_fboSize, format);
            }
            {
                QGLFramebufferObjectFormat format;
                format.setAttachment(QGLFramebufferObject::NoAttachment);
                m_fbo = new QGLFramebufferObject(m_fboSize, format);
            }
        } else {
            QGLFramebufferObjectFormat format;
            format.setAttachment(QGLFramebufferObject::CombinedDepthStencil);
            m_fbo = new QGLFramebufferObject(m_fboSize, format);
        }
    } else {
        if (!m_image.isNull() && !m_dirtyGeometry)
            return;

        m_image = QImage(m_size, QImage::Format_ARGB32_Premultiplied);
        m_image.fill(Qt::transparent);
    }

    QSGPainterTexture *texture = new QSGPainterTexture;
    if (m_actualRenderTarget == QSGPaintedItem::Image) {
        texture->setOwnsTexture(true);
        texture->setTextureSize(m_size);
    } else {
        texture->setTextureId(m_fbo->texture());
        texture->setOwnsTexture(false);
        texture->setTextureSize(m_fboSize);
    }

    if (m_texture)
        delete m_texture;

    texture->setTextureSize(m_size);
    m_texture = texture;
    m_material.setFiltering(m_linear_filtering ? QSGTexture::Linear : QSGTexture::Nearest);
    m_materialO.setFiltering(m_linear_filtering ? QSGTexture::Linear : QSGTexture::Nearest);
}

void QSGPainterNode::updateFBOSize()
{
    int fboWidth = qMax(QT_MINIMUM_FBO_SIZE, qt_next_power_of_two(m_size.width()));
    int fboHeight = qMax(QT_MINIMUM_FBO_SIZE, qt_next_power_of_two(m_size.height()));
    m_fboSize = QSize(fboWidth, fboHeight);
}

void QSGPainterNode::setPreferredRenderTarget(QSGPaintedItem::RenderTarget target)
{
    if (m_preferredRenderTarget == target)
        return;

    m_preferredRenderTarget = target;

    m_dirtyRenderTarget = true;
    m_dirtyGeometry = true;
    m_dirtyTexture = true;
}

void QSGPainterNode::setSize(const QSize &size)
{
    if (size == m_size)
        return;

    m_size = size;
    updateFBOSize();

    if (m_fbo)
        m_dirtyRenderTarget = m_fbo->size() != m_fboSize || m_dirtyRenderTarget;
    else
        m_dirtyRenderTarget = true;
    m_dirtyGeometry = true;
    m_dirtyTexture = true;
}

void QSGPainterNode::setDirty(bool d, const QRect &dirtyRect)
{
    m_dirtyContents = d;
    m_dirtyRect = dirtyRect;

    markDirty(DirtyMaterial);
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
    m_dirtyRenderTarget = true;
}

void QSGPainterNode::setFillColor(const QColor &c)
{
    if (c == m_fillColor)
        return;

    m_fillColor = c;
    markDirty(DirtyMaterial);
}


QT_END_NAMESPACE

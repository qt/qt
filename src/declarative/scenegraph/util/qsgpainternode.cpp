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
#include "private/qsgtexture_p.h"
#include "private/qsgcontext_p.h"
#include <qglframebufferobject.h>

QT_BEGIN_NAMESPACE

QSGPainterNode::QSGPainterNode()
    : QSGGeometryNode()
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
    , m_dirtyFBO(false)
    , m_dirtyTexture(false)
{
    setMaterial(&m_materialO);
    setOpaqueMaterial(&m_material);
    setGeometry(&m_geometry);
}

QSGPainterNode::~QSGPainterNode()
{
    delete m_fbo;
    delete m_multisampledFbo;
}

void QSGPainterNode::paint(QSGPaintedItem *item)
{
    if (!item)
        return;

    Q_ASSERT(m_fbo != 0);

    QPainter fbo_painter;
    if (m_multisampledFbo)
        fbo_painter.begin(m_multisampledFbo);
    else
        fbo_painter.begin(m_fbo);

    if (m_smoothPainting)
        fbo_painter.setRenderHints(QPainter::HighQualityAntialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    fbo_painter.setCompositionMode(QPainter::CompositionMode_Source);
    fbo_painter.fillRect(QRectF(QPointF(0, 0), m_fbo->size()), Qt::transparent);
    fbo_painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    item->paint(&fbo_painter);
    fbo_painter.end();

    if (m_multisampledFbo) {
        QRect r(0, 0, m_size.width(), m_size.height());
        QGLFramebufferObject::blitFramebuffer(m_fbo, r, m_multisampledFbo, r);
    }

    markDirty(DirtyMaterial);
}

bool QSGPainterNode::update()
{
    bool fboChanged = false;

    if (m_dirtyGeometry)
        updateGeometry();
    if (m_dirtyFBO || m_dirtyGeometry)
        fboChanged = updateFBO();
    if (m_dirtyFBO || m_dirtyTexture)
        updateTexture();

    m_dirtyGeometry = false;
    m_dirtyFBO = false;
    m_dirtyTexture = false;

    return fboChanged;
}

void QSGPainterNode::updateTexture()
{
    QSGPlainTexture *texture = new QSGPlainTexture;
    texture->setTextureId(m_fbo->texture());
    texture->setTextureSize(m_fbo->size());
    texture->setHasAlphaChannel(!m_opaquePainting);
    texture->setOwnsTexture(false);
    m_texture = QSGTextureRef(texture);

    m_material.setTexture(m_texture, m_opaquePainting);
    m_material.setLinearFiltering(m_linear_filtering);
    m_materialO.setTexture(m_texture, m_opaquePainting);
    m_materialO.setLinearFiltering(m_linear_filtering);

    markDirty(DirtyMaterial);
}

void QSGPainterNode::updateGeometry()
{
    QSGGeometry::updateTexturedRectGeometry(&m_geometry,
                                            QRectF(0, 0, m_size.width(), m_size.height()),
                                            QRectF(0, 1, 1, -1));
    markDirty(DirtyGeometry);
}

bool QSGPainterNode::updateFBO()
{
    const QGLContext *ctx = QSGContext::current->glContext();
    if (!m_extensionsChecked) {
        QList<QByteArray> extensions = QByteArray((const char *)glGetString(GL_EXTENSIONS)).split(' ');
        m_multisamplingSupported = extensions.contains("GL_EXT_framebuffer_multisample")
                && extensions.contains("GL_EXT_framebuffer_blit");
        m_extensionsChecked = true;
    }

    if (m_fbo && !m_dirtyGeometry && (!ctx->format().sampleBuffers() || !m_multisamplingSupported))
        return false;

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

    return true;
}

void QSGPainterNode::setSize(const QSize &size)
{
    if (size == m_size)
        return;

    m_size = size;
    m_dirtyGeometry = true;
    m_dirtyFBO = true;
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

    m_material.setLinearFiltering(linearFiltering);
    m_materialO.setLinearFiltering(linearFiltering);
    markDirty(DirtyMaterial);
}

void QSGPainterNode::setSmoothPainting(bool s)
{
    if (s == m_smoothPainting)
        return;

    m_smoothPainting = s;
    m_dirtyFBO = true;
}


QT_END_NAMESPACE

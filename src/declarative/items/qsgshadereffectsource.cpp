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

#include "qsgshadereffectsource_p.h"

#include "qsgitem_p.h"
#include "qsgcanvas_p.h"
#include <private/qsgadaptationlayer_p.h>
#include <private/qsgrenderer_p.h>

#include "qglframebufferobject.h"
#include "qmath.h"
#include <private/qsgtexture_p.h>

QT_BEGIN_NAMESPACE

DEFINE_BOOL_CONFIG_OPTION(qmlFboOverlay, QML_FBO_OVERLAY)

QSGShaderEffectTextureProvider::QSGShaderEffectTextureProvider(QObject *parent)
    : QSGTextureProvider(parent)
    , m_item(0)
    , m_format(GL_RGBA)
    , m_renderer(0)
    , m_fbo(0)
    , m_multisampledFbo(0)
#ifdef QSG_DEBUG_FBO_OVERLAY
    , m_debugOverlay(0)
#endif
    , m_live(true)
    , m_dirtyTexture(true)
    , m_multisamplingSupportChecked(false)
    , m_multisampling(false)
{
}

QSGShaderEffectTextureProvider::~QSGShaderEffectTextureProvider()
{
    delete m_renderer;
    delete m_fbo;
    delete m_multisampledFbo;
#ifdef QSG_DEBUG_FBO_OVERLAY
    delete m_debugOverlay;
#endif
}

void QSGShaderEffectTextureProvider::updateTexture()
{
    if (m_dirtyTexture)
        grab();
}

QSGTextureRef QSGShaderEffectTextureProvider::texture()
{
    return m_texture;
}

QSGTextureProvider::WrapMode QSGShaderEffectTextureProvider::horizontalWrapMode() const
{
    return WrapMode(m_hWrapMode);
}

QSGTextureProvider::WrapMode QSGShaderEffectTextureProvider::verticalWrapMode() const
{
    return WrapMode(m_vWrapMode);
}

QSGTextureProvider::Filtering QSGShaderEffectTextureProvider::filtering() const
{
    return Filtering(m_filtering);
}

QSGTextureProvider::Filtering QSGShaderEffectTextureProvider::mipmapFiltering() const
{
    return Filtering(m_mipmapFiltering);
}

void QSGShaderEffectTextureProvider::setHorizontalWrapMode(QSGTextureProvider::WrapMode mode)
{
    m_hWrapMode = mode;
}

void QSGShaderEffectTextureProvider::setVerticalWrapMode(QSGTextureProvider::WrapMode mode)
{
    m_vWrapMode = mode;
}

void QSGShaderEffectTextureProvider::setFiltering(QSGTextureProvider::Filtering filtering)
{
    m_filtering = filtering;
}

void QSGShaderEffectTextureProvider::setMipmapFiltering(QSGTextureProvider::Filtering filtering)
{
    if (filtering == m_mipmapFiltering)
        return;
    m_mipmapFiltering = filtering;
    if (filtering != None && m_fbo && !m_fbo->format().mipmap())
        markDirtyTexture();
}


void QSGShaderEffectTextureProvider::setItem(QSGNode *item)
{
    if (item == m_item)
        return;
    m_item = item;
    markDirtyTexture();
}

void QSGShaderEffectTextureProvider::setRect(const QRectF &rect)
{
    if (rect == m_rect)
        return;
    m_rect = rect;
    markDirtyTexture();
}

void QSGShaderEffectTextureProvider::setSize(const QSize &size)
{
    if (size == m_size)
        return;
    m_size = size;
    markDirtyTexture();
}

void QSGShaderEffectTextureProvider::setFormat(GLenum format)
{
    if (format == m_format)
        return;
    m_format = format;
    markDirtyTexture();
}

void QSGShaderEffectTextureProvider::setLive(bool live)
{
    if (live == m_live)
        return;
    m_live = live;
    markDirtyTexture();
}

void QSGShaderEffectTextureProvider::markDirtyTexture()
{
    if (m_live) {
        m_dirtyTexture = true;
        emit textureChanged();
    }
}

void QSGShaderEffectTextureProvider::grab()
{
    Q_ASSERT(m_item);
    QSGNode *root = m_item;
    while (root->childCount() && root->type() != QSGNode::RootNodeType)
        root = root->childAtIndex(0);
    if (root->type() != QSGNode::RootNodeType)
        return;

    if (m_size.isEmpty()) {
        m_texture = QSGTextureRef();
        delete m_fbo;
        delete m_multisampledFbo;
        m_multisampledFbo = m_fbo = 0;
        return;
    }

    if (!m_renderer) {
        m_renderer = QSGContext::current->createRenderer();
        connect(m_renderer, SIGNAL(sceneGraphChanged()), this, SLOT(markDirtyTexture()));
    }
    m_renderer->setRootNode(static_cast<QSGRootNode *>(root));

    bool mipmap = m_mipmapFiltering != None;
    if (!m_fbo || m_fbo->size() != m_size || m_fbo->format().internalTextureFormat() != m_format
        || (!m_fbo->format().mipmap() && mipmap))
    {
        if (!m_multisamplingSupportChecked) {
            QList<QByteArray> extensions = QByteArray((const char *)glGetString(GL_EXTENSIONS)).split(' ');
            m_multisampling = extensions.contains("GL_EXT_framebuffer_multisample")
                            && extensions.contains("GL_EXT_framebuffer_blit");
            m_multisamplingSupportChecked = true;
        }
        if (m_multisampling) {
            delete m_fbo;
            delete m_multisampledFbo;
            QGLFramebufferObjectFormat format;

            format.setAttachment(QGLFramebufferObject::CombinedDepthStencil);
            format.setInternalTextureFormat(m_format);
            format.setSamples(8);
            m_multisampledFbo = new QGLFramebufferObject(m_size, format);

            format.setAttachment(QGLFramebufferObject::NoAttachment);
            format.setMipmap(m_mipmapFiltering);
            format.setSamples(0);
            m_fbo = new QGLFramebufferObject(m_size, format);

            QSGPlainTexture *tex = new QSGPlainTexture;
            tex->setTextureSize(QSize(m_fbo->size()));
            tex->setTextureId(m_fbo->texture());
            tex->setOwnsTexture(false);
            tex->setHasMipmaps(mipmap);
            tex->setHasAlphaChannel(m_format != GL_RGB);
            m_texture = QSGTextureRef(tex);
        } else {
            delete m_fbo;
            QGLFramebufferObjectFormat format;
            format.setAttachment(QGLFramebufferObject::CombinedDepthStencil);
            format.setInternalTextureFormat(m_format);
            format.setMipmap(m_mipmapFiltering);
            m_fbo = new QGLFramebufferObject(m_size, format);
            QSGPlainTexture *tex = new QSGPlainTexture;
            tex->setTextureSize(QSize(m_fbo->size()));
            tex->setTextureId(m_fbo->texture());
            tex->setOwnsTexture(false);
            tex->setHasMipmaps(mipmap);
            tex->setHasAlphaChannel(m_format != GL_RGB);
            m_texture = QSGTextureRef(tex);
        }
    }

    // Render texture.
    QSGNode::DirtyFlags dirty = root->dirtyFlags();
    root->markDirty(QSGNode::DirtyNodeAdded); // Force matrix and clip update.
    m_renderer->nodeChanged(root, QSGNode::DirtyNodeAdded); // Force render list update.

#ifdef QSG_DEBUG_FBO_OVERLAY
    if (qmlFboOverlay()) {
        if (!m_debugOverlay)
            m_debugOverlay = QSGContext::current->createRectangleNode();
        m_debugOverlay->setRect(QRectF(0, 0, m_size.width(), m_size.height()));
        m_debugOverlay->setColor(QColor(0xff, 0x00, 0x80, 0x40));
        m_debugOverlay->setPenColor(QColor());
        m_debugOverlay->setPenWidth(0);
        m_debugOverlay->setRadius(0);
        m_debugOverlay->update();
        root->appendChildNode(m_debugOverlay);
    }
#endif

    m_dirtyTexture = false;

    const QGLContext *ctx = QSGContext::current->glContext();
    m_renderer->setDeviceRect(m_size);
    m_renderer->setViewportRect(m_size);
    QRectF mirrored(m_rect.left(), m_rect.bottom(), m_rect.width(), -m_rect.height());
    m_renderer->setProjectMatrixToRect(mirrored);
    m_renderer->setClearColor(Qt::transparent);

    if (m_multisampling) {
        m_renderer->renderScene(BindableFbo(const_cast<QGLContext *>(ctx), m_multisampledFbo));
        QRect r(0, 0, m_fbo->width(), m_fbo->height());
        QGLFramebufferObject::blitFramebuffer(m_fbo, r, m_multisampledFbo, r);
    } else {
        m_renderer->renderScene(BindableFbo(const_cast<QGLContext *>(ctx), m_fbo));
    }

    if (mipmap) {
        glBindTexture(GL_TEXTURE_2D, m_texture->textureId());
        ctx->functions()->glGenerateMipmap(GL_TEXTURE_2D);
    }

    root->markDirty(dirty | QSGNode::DirtyNodeAdded); // Force matrix, clip and render list update.

#ifdef QSG_DEBUG_FBO_OVERLAY
    if (qmlFboOverlay())
        root->removeChildNode(m_debugOverlay);
#endif
}


QSGShaderEffectSource::QSGShaderEffectSource(QSGItem *parent)
    : QSGItem(parent)
    , m_textureProvider(0)
    , m_wrapMode(ClampToEdge)
    , m_sourceItem(0)
    , m_textureSize(0, 0)
    , m_format(RGBA)
    , m_live(true)
    , m_hideSource(false)
    , m_mipmap(false)
{
    setFlag(ItemHasContents);
    m_textureProvider = new QSGShaderEffectTextureProvider(this);
}

QSGShaderEffectSource::~QSGShaderEffectSource()
{
    if (m_sourceItem)
        QSGItemPrivate::get(m_sourceItem)->derefFromEffectItem(m_hideSource);
}

QSGTextureProvider *QSGShaderEffectSource::textureProvider() const
{
    return m_textureProvider;
}

QSGShaderEffectSource::WrapMode QSGShaderEffectSource::wrapMode() const
{
    return m_wrapMode;
}

void QSGShaderEffectSource::setWrapMode(WrapMode mode)
{
    if (mode == m_wrapMode)
        return;
    m_wrapMode = mode;
    update();
    emit wrapModeChanged();
}

QSGItem *QSGShaderEffectSource::sourceItem() const
{
    return m_sourceItem;
}

void QSGShaderEffectSource::setSourceItem(QSGItem *item)
{
    if (item == m_sourceItem)
        return;
    if (m_sourceItem)
        QSGItemPrivate::get(m_sourceItem)->derefFromEffectItem(m_hideSource);
    m_sourceItem = item;
    if (m_sourceItem) {
        // TODO: Find better solution.
        // 'm_sourceItem' needs a canvas to get a scenegraph node.
        // The easiest way to make sure it gets a canvas is to
        // make it a part of the same item tree as 'this'.
        if (m_sourceItem->parentItem() == 0) {
            m_sourceItem->setParentItem(this);
            m_sourceItem->setVisible(false);
        }
        QSGItemPrivate::get(m_sourceItem)->refFromEffectItem(m_hideSource);
    }
    update();
    emit sourceItemChanged();
}

QRectF QSGShaderEffectSource::sourceRect() const
{
    return m_sourceRect;
}

void QSGShaderEffectSource::setSourceRect(const QRectF &rect)
{
    if (rect == m_sourceRect)
        return;
    m_sourceRect = rect;
    update();
    emit sourceRectChanged();
}

QSize QSGShaderEffectSource::textureSize() const
{
    return m_textureSize;
}

void QSGShaderEffectSource::setTextureSize(const QSize &size)
{
    if (size == m_textureSize)
        return;
    m_textureSize = size;
    update();
    emit textureSizeChanged();
}

QSGShaderEffectSource::Format QSGShaderEffectSource::format() const
{
    return m_format;
}

void QSGShaderEffectSource::setFormat(QSGShaderEffectSource::Format format)
{
    if (format == m_format)
        return;
    m_format = format;
    update();
    emit formatChanged();
}

bool QSGShaderEffectSource::live() const
{
    return m_live;
}

void QSGShaderEffectSource::setLive(bool live)
{
    if (live == m_live)
        return;
    m_live = live;
    update();
    emit liveChanged();
}

bool QSGShaderEffectSource::hideSource() const
{
    return m_hideSource;
}

void QSGShaderEffectSource::setHideSource(bool hide)
{
    if (hide == m_hideSource)
        return;
    if (m_sourceItem) {
        QSGItemPrivate::get(m_sourceItem)->refFromEffectItem(hide);
        QSGItemPrivate::get(m_sourceItem)->derefFromEffectItem(m_hideSource);
    }
    m_hideSource = hide;
    update();
    emit hideSourceChanged();
}

bool QSGShaderEffectSource::mipmap() const
{
    return m_mipmap;
}

void QSGShaderEffectSource::setMipmap(bool enabled)
{
    if (enabled == m_mipmap)
        return;
    m_mipmap = enabled;
    update();
    emit mipmapChanged();
}

void QSGShaderEffectSource::grab()
{
    if (!m_sourceItem)
        return;
    QSGCanvas *canvas = m_sourceItem->canvas();
    if (!canvas)
        return;
    QSGCanvasPrivate::get(canvas)->updateDirtyNodes();
    QGLContext *glctx = const_cast<QGLContext *>(canvas->context());
    glctx->makeCurrent();
    static_cast<QSGShaderEffectTextureProvider *>(textureProvider())->grab();
}

QSGNode *QSGShaderEffectSource::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
    if (!m_sourceItem) {
        delete oldNode;
        return 0;
    }

    QSGImageNode *node = static_cast<QSGImageNode *>(oldNode);
    if (!node) {
        node = QSGContext::current->createImageNode();
        node->setFlag(QSGNode::UsePreprocess, true);
        node->setTexture(m_textureProvider);
    }

    m_textureProvider->setItem(QSGItemPrivate::get(m_sourceItem)->itemNode());
    QRectF sourceRect = m_sourceRect.isEmpty() 
                      ? QRectF(0, 0, m_sourceItem->width(), m_sourceItem->height())
                      : m_sourceRect;
    m_textureProvider->setRect(sourceRect);
    QSize textureSize = m_textureSize.isEmpty()
                      ? QSize(qCeil(sourceRect.width()), qCeil(sourceRect.height()))
                      : m_textureSize;
    m_textureProvider->setSize(textureSize);
    m_textureProvider->setLive(m_live);
    m_textureProvider->setFormat(GLenum(m_format));

    QSGTextureProvider::Filtering filtering = QSGItemPrivate::get(this)->smooth
                                            ? QSGTextureProvider::Linear
                                            : QSGTextureProvider::Nearest;
    m_textureProvider->setMipmapFiltering(m_mipmap ? filtering : QSGTextureProvider::None);

    QSGTextureProvider::WrapMode hWrap = QSGTextureProvider::ClampToEdge;
    QSGTextureProvider::WrapMode vWrap = QSGTextureProvider::ClampToEdge;
    switch (m_wrapMode) {
    case RepeatHorizontally:
        hWrap = QSGTextureProvider::Repeat;
        break;
    case RepeatVertically:
        vWrap = QSGTextureProvider::Repeat;
        break;
    case Repeat:
        hWrap = vWrap = QSGTextureProvider::Repeat;
        break;
    default:
        break;
    }

    m_textureProvider->setHorizontalWrapMode(hWrap);
    m_textureProvider->setVerticalWrapMode(vWrap);
    m_textureProvider->setFiltering(QSGItemPrivate::get(this)->smooth
                                    ? QSGTextureProvider::Linear : QSGTextureProvider::Nearest);

    node->setTargetRect(QRectF(0, 0, width(), height()));
    node->setSourceRect(QRectF(0, 0, 1, 1));
    node->update();

    return node;
}

QT_END_NAMESPACE

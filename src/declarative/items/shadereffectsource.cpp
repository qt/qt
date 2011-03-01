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

#include "shadereffectsource.h"

#include "qsgitem_p.h"
#include "qsgcanvas_p.h"
#include "adaptationlayer.h"

#include "qglframebufferobject.h"
#include "qmath.h"

QT_BEGIN_NAMESPACE

DEFINE_BOOL_CONFIG_OPTION(qmlFboOverlay, QML_FBO_OVERLAY)

ShaderEffectTextureProvider::ShaderEffectTextureProvider(QObject *parent)
    : QSGTextureProvider(parent)
    , m_item(0)
    , m_format(GL_RGBA)
    , m_renderer(0)
    , m_fbo(0)
#ifdef QSG_DEBUG_FBO_OVERLAY
    , m_debugOverlay(0)
#endif
    , m_live(true)
    , m_dirtyTexture(true)
{
}

ShaderEffectTextureProvider::~ShaderEffectTextureProvider()
{
    delete m_renderer;
    delete m_fbo;
#ifdef QSG_DEBUG_FBO_OVERLAY
    delete m_debugOverlay;
#endif
}

void ShaderEffectTextureProvider::updateTexture()
{
    if (m_dirtyTexture)
        grab();
}

QSGTextureRef ShaderEffectTextureProvider::texture()
{
    return m_texture;
}

void ShaderEffectTextureProvider::setItem(Node *item)
{
    if (item == m_item)
        return;
    m_item = item;
    markDirtyTexture();
}

void ShaderEffectTextureProvider::setRect(const QRectF &rect)
{
    if (rect == m_rect)
        return;
    m_rect = rect;
    markDirtyTexture();
}

void ShaderEffectTextureProvider::setSize(const QSize &size)
{
    if (size == m_size)
        return;
    m_size = size;
    markDirtyTexture();
}

void ShaderEffectTextureProvider::setFormat(GLenum format)
{
    if (format == m_format)
        return;
    m_format = format;
    markDirtyTexture();
}

void ShaderEffectTextureProvider::setLive(bool live)
{
    if (live == m_live)
        return;
    m_live = live;
    markDirtyTexture();
}

void ShaderEffectTextureProvider::markDirtyTexture()
{
    if (m_live) {
        m_dirtyTexture = true;
        emit textureChanged();
    }
}

void ShaderEffectTextureProvider::grab()
{
    Q_ASSERT(m_item);
    Node *root = m_item;
    while (root->childCount() && root->type() != Node::RootNodeType)
        root = root->childAtIndex(0);
    if (root->type() != Node::RootNodeType)
        return;

    if (m_size.isEmpty()) {
        m_texture = QSGTextureRef();
        delete m_fbo;
        m_fbo = 0;
        return;
    }

    if (!m_renderer) {
        m_renderer = QSGContext::current->createRenderer();
        connect(m_renderer, SIGNAL(sceneGraphChanged()), this, SLOT(markDirtyTexture()));
    }
    m_renderer->setRootNode(static_cast<RootNode *>(root));

    if (!m_fbo || m_fbo->size() != m_size || m_fbo->format().internalTextureFormat() != m_format) {
        delete m_fbo;
        QGLFramebufferObjectFormat format;
        format.setAttachment(QGLFramebufferObject::CombinedDepthStencil);
        format.setInternalTextureFormat(m_format);
        m_fbo = new QGLFramebufferObject(m_size, format);
        QSGTexture *tex = new QSGTexture;
        tex->setTextureId(m_fbo->texture());
        tex->setOwnsTexture(false);
        m_texture = QSGTextureRef(tex);
    }

    // Render texture.
    Node::DirtyFlags dirty = root->dirtyFlags();
    root->markDirty(Node::DirtyNodeAdded); // Force matrix and clip update.
    m_renderer->nodeChanged(root, Node::DirtyNodeAdded); // Force render list update.

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
    m_renderer->renderScene(BindableFbo(const_cast<QGLContext *>(ctx), m_fbo));

    root->markDirty(dirty | Node::DirtyNodeAdded); // Force matrix, clip and render list update.

#ifdef QSG_DEBUG_FBO_OVERLAY
    if (qmlFboOverlay())
        root->removeChildNode(m_debugOverlay);
#endif
}


ShaderEffectSource::ShaderEffectSource(QSGItem *parent)
    : TextureItem(parent)
    , m_sourceItem(0)
    , m_textureSize(0, 0)
    , m_format(RGBA)
    , m_live(true)
    , m_hideSource(false)
{
    setTextureProvider(new ShaderEffectTextureProvider(this), true);
}

ShaderEffectSource::~ShaderEffectSource()
{
    if (m_sourceItem)
        QSGItemPrivate::get(m_sourceItem)->derefFromEffectItem(m_hideSource);
}

QSGItem *ShaderEffectSource::sourceItem() const
{
    return m_sourceItem;
}

void ShaderEffectSource::setSourceItem(QSGItem *item)
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

QRectF ShaderEffectSource::sourceRect() const
{
    return m_sourceRect;
}

void ShaderEffectSource::setSourceRect(const QRectF &rect)
{
    if (rect == m_sourceRect)
        return;
    m_sourceRect = rect;
    update();
    emit sourceRectChanged();
}

QSize ShaderEffectSource::textureSize() const
{
    return m_textureSize;
}

void ShaderEffectSource::setTextureSize(const QSize &size)
{
    if (size == m_textureSize)
        return;
    m_textureSize = size;
    update();
    emit textureSizeChanged();
}

ShaderEffectSource::Format ShaderEffectSource::format() const
{
    return m_format;
}

void ShaderEffectSource::setFormat(ShaderEffectSource::Format format)
{
    if (format == m_format)
        return;
    m_format = format;
    update();
    emit formatChanged();
}

bool ShaderEffectSource::live() const
{
    return m_live;
}

void ShaderEffectSource::setLive(bool live)
{
    if (live == m_live)
        return;
    m_live = live;
    update();
    emit liveChanged();
}

bool ShaderEffectSource::hideSource() const
{
    return m_hideSource;
}

void ShaderEffectSource::setHideSource(bool hide)
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

void ShaderEffectSource::grab()
{
    if (!m_sourceItem)
        return;
    QSGCanvas *canvas = m_sourceItem->canvas();
    if (!canvas)
        return;
    QSGCanvasPrivate::get(canvas)->updateDirtyNodes();
    QGLContext *glctx = const_cast<QGLContext *>(canvas->context());
    glctx->makeCurrent();
    static_cast<ShaderEffectTextureProvider *>(textureProvider())->grab();
}

Node *ShaderEffectSource::updatePaintNode(Node *oldNode, UpdatePaintNodeData *data)
{
    if (!m_sourceItem) {
        delete oldNode;
        return 0;
    }

    ShaderEffectTextureProvider *tp = static_cast<ShaderEffectTextureProvider *>(textureProvider());
    tp->setItem(QSGItemPrivate::get(m_sourceItem)->itemNode());
    QRectF sourceRect = m_sourceRect.isEmpty() 
                      ? QRectF(0, 0, m_sourceItem->width(), m_sourceItem->height())
                      : m_sourceRect;
    tp->setRect(sourceRect);
    QSize textureSize = m_textureSize.isEmpty()
                      ? QSize(qCeil(sourceRect.width()), qCeil(sourceRect.height()))
                      : m_textureSize;
    tp->setSize(textureSize);
    tp->setLive(m_live);
    tp->setFormat(GLenum(m_format));

    return TextureItem::updatePaintNode(oldNode, data);
}

QT_END_NAMESPACE

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

#include "qglframebufferobject.h"
#include "qmath.h"

QT_BEGIN_NAMESPACE

ShaderEffectTextureProvider::ShaderEffectTextureProvider(QObject *parent)
    : QSGTextureProvider(parent)
    , m_item(0)
    , m_renderer(0)
    , m_fbo(0)
#ifdef QML_SUBTREE_DEBUG
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
#ifdef QML_SUBTREE_DEBUG
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

    qreal w = m_rect.width();
    qreal h = m_rect.height();

    if (w <= 0 || h <= 0) {
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

    if (!m_fbo || m_fbo->width() != qCeil(w) || m_fbo->height() != qCeil(h)) {
        delete m_fbo;
        QGLFramebufferObjectFormat format;
        format.setAttachment(QGLFramebufferObject::CombinedDepthStencil);
        m_fbo = new QGLFramebufferObject(qCeil(w), qCeil(h), format);
        QSGTexture *tex = new QSGTexture;
        tex->setTextureId(m_fbo->texture());
        tex->setOwnsTexture(false);
        m_texture = QSGTextureRef(tex);
    }

    // Render texture.
    Node::DirtyFlags dirty = root->dirtyFlags();
    root->markDirty(Node::DirtyNodeAdded); // Force matrix and clip update.
    m_renderer->nodeChanged(root, Node::DirtyNodeAdded); // Force render list update.

#ifdef QML_SUBTREE_DEBUG
    if (!m_debugOverlay)
        m_debugOverlay = QSGContext::current->createRectangleNode();
    m_debugOverlay->setRect(QRectF(0, 0, m_fbo->width(), m_fbo->height()));
    m_debugOverlay->setColor(QColor(0xff, 0x00, 0x80, 0x40));
    m_debugOverlay->setPenColor(QColor());
    m_debugOverlay->setPenWidth(0);
    m_debugOverlay->setRadius(0);
    m_debugOverlay->update();
    root->appendChildNode(m_debugOverlay);
#endif

    m_dirtyTexture = false;

    const QGLContext *ctx = QSGContext::current->glContext();
    m_renderer->setDeviceRect(m_fbo->size());
    m_renderer->setProjectMatrixToRect(m_rect);
    m_renderer->setClearColor(Qt::transparent);
    m_renderer->renderScene(BindableFbo(const_cast<QGLContext *>(ctx), m_fbo));

    root->markDirty(dirty | Node::DirtyNodeAdded); // Force matrix, clip and render list update.

#ifdef QML_SUBTREE_DEBUG
    root->removeChildNode(m_debugOverlay);
#endif
}


ShaderEffectSource::ShaderEffectSource(QSGItem *parent)
    : TextureItem(parent)
    , m_item(0)
    , m_margins(0, 0)
    , m_live(true)
{
    setTextureProvider(new ShaderEffectTextureProvider(this), true);
}

ShaderEffectSource::~ShaderEffectSource()
{
    if (m_item)
        QSGItemPrivate::get(m_item)->derefFromEffectItem();
}

QSGItem *ShaderEffectSource::item() const
{
    return m_item;
}

void ShaderEffectSource::setItem(QSGItem *item)
{
    if (item == m_item)
        return;
    if (m_item) {
        QSGItemPrivate::get(m_item)->derefFromEffectItem();
        if (m_item->parentItem() == this)
            m_item->setParentItem(0);
    }
    m_item = item;
    if (m_item) {
        // TODO: Find better solution.
        // 'm_item' needs a canvas to get a scenegraph node.
        // The easiest way to make sure it gets a canvas is to
        // make it a part of the same item tree as 'this'.
        if (m_item->parentItem() == 0) {
            m_item->setParentItem(this);
            m_item->setVisible(false);
        }
        QSGItemPrivate::get(m_item)->refFromEffectItem();
    }
    update();
    emit itemChanged();
}

QSizeF ShaderEffectSource::margins() const
{
    return m_margins;
}

void ShaderEffectSource::setMargins(const QSizeF &margins)
{
    if (margins == m_margins)
        return;
    m_margins = margins;
    update();
    emit marginsChanged();
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

void ShaderEffectSource::grab()
{
    if (!m_item)
        return;
    QSGCanvas *canvas = m_item->canvas();
    QSGCanvasPrivate::get(canvas)->updateDirtyNodes();
    QGLContext *glctx = const_cast<QGLContext *>(canvas->context());
    glctx->makeCurrent();
    static_cast<ShaderEffectTextureProvider *>(textureProvider())->grab();
}

Node *ShaderEffectSource::updatePaintNode(Node *oldNode, UpdatePaintNodeData *data)
{
    if (!m_item) {
        delete oldNode;
        return 0;
    }

    ShaderEffectTextureProvider *tp = static_cast<ShaderEffectTextureProvider *>(textureProvider());
    tp->setItem(QSGItemPrivate::get(m_item)->itemNode());
    QRectF rect(0, 0, m_item->width(), m_item->height());
    rect.adjust(-m_margins.width(), -m_margins.height(), m_margins.width(), m_margins.height());
    tp->setRect(rect);
    tp->setLive(m_live);

    return TextureItem::updatePaintNode(oldNode, data);
}

QT_END_NAMESPACE

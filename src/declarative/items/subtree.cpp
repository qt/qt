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

#include "subtree.h"
#include "qsgtexturemanager.h"
#include "qsgitem_p.h"
#include "adaptationlayer.h"
#include "renderer.h"

#include "qglframebufferobject.h"
#include "qmath.h"

QT_BEGIN_NAMESPACE

TextureNode::TextureNode()
    : m_texture(0)
    , m_sourceRect(0, 0, 1, 1)
    , m_opacity(1)
    , m_dirtyTexture(false)
    , m_dirtyGeometry(false)
{
    setFlag(UsePreprocess, true);

    m_material.setClampToEdge(true);
    m_material.setLinearFiltering(false);
    m_materialO.setClampToEdge(true);
    m_materialO.setLinearFiltering(false);
    m_materialO.setOpacity(m_opacity);
    setMaterial(&m_material);

    QVector<QSGAttributeDescription> desc = QVector<QSGAttributeDescription>()
        << QSGAttributeDescription(0, 2, GL_FLOAT, 4 * sizeof(float))
        << QSGAttributeDescription(1, 2, GL_FLOAT, 4 * sizeof(float));
    updateGeometryDescription(desc, GL_UNSIGNED_SHORT);
}

void TextureNode::setTargetRect(const QRectF &rect)
{
    if (rect == m_targetRect)
        return;
    m_targetRect = rect;
    m_dirtyGeometry = true;
    markDirty(DirtyGeometry);
}

void TextureNode::setSourceRect(const QRectF &rect)
{
    if (rect == m_sourceRect)
        return;
    m_sourceRect = rect;
    m_dirtyGeometry = true;
    markDirty(DirtyGeometry);
}

void TextureNode::setOpacity(qreal opacity)
{
    if (m_opacity == opacity)
        return;
    m_opacity = opacity;
    m_materialO.setOpacity(m_opacity);
    setMaterial(m_opacity < 1 ? (AbstractMaterial *)&m_materialO : (AbstractMaterial *)&m_material);
}

void TextureNode::setTexture(QSGTextureProvider *texture)
{
    if (texture == m_texture)
        return;
    if (m_texture)
        disconnect(m_texture, SIGNAL(textureChanged()), this, SLOT(markDirtyTexture()));
    m_texture = texture;
    if (m_texture)
        connect(m_texture, SIGNAL(textureChanged()), this, SLOT(markDirtyTexture()));
    markDirtyTexture();
}

void TextureNode::preprocess()
{
    if (m_dirtyGeometry)
        updateGeometry();
    if (m_dirtyTexture)
        updateTexture();
}

void TextureNode::markDirtyTexture()
{
    m_dirtyTexture = true;
    markDirty(DirtyMaterial);
}

void TextureNode::updateGeometry()
{
    Geometry *g = geometry();
    g->setVertexCount(4);
    g->setIndexCount(0);
    g->setDrawingMode(QSG::TriangleStrip);
    struct V { float x, y, u, v; };
    V *v = static_cast<V *>(g->vertexData());

    for (int i = 0; i < 4; ++i) {
        v[i].x = (i & 2 ? m_targetRect.right() : m_targetRect.left());
        v[i].y = (i & 1 ? m_targetRect.bottom() : m_targetRect.top());
        v[i].u = (i & 2 ? m_sourceRect.right() : m_sourceRect.left());
        v[i].v = (i & 1 ? m_sourceRect.bottom() : m_sourceRect.top());
    }
    m_dirtyGeometry = false;
}

void TextureNode::updateTexture()
{
    if (m_texture) {
        QSGTextureRef tex = m_texture->texture();
        m_material.setTexture(tex, m_texture->opaque());
        m_material.setClampToEdge(m_texture->clampToEdge());
        m_material.setLinearFiltering(m_texture->linearFiltering());
        m_materialO.setTexture(tex, m_texture->opaque());
        m_materialO.setClampToEdge(m_texture->clampToEdge());
        m_materialO.setLinearFiltering(m_texture->linearFiltering());
    } else {
        m_material.setTexture(QSGTextureRef());
        m_materialO.setTexture(QSGTextureRef());
    }
    m_dirtyTexture = false;
}


SubTreeTextureProvider::SubTreeTextureProvider(QObject *parent)
    : QSGTextureProvider(parent)
    , m_item(0)
    , m_margins(0, 0)
    , m_renderer(0)
    , m_fbo(0)
    , m_live(true)
    , m_dirtyTexture(true)
{
    connect(this, SIGNAL(itemChanged()), this, SLOT(markDirtyTexture()));
    connect(this, SIGNAL(marginsChanged()), this, SLOT(markDirtyTexture()));
    connect(this, SIGNAL(liveChanged()), this, SLOT(markDirtyTexture()));
}

SubTreeTextureProvider::~SubTreeTextureProvider()
{
    if (m_item)
        QSGItemPrivate::get(m_item)->derefFromEffectItem();
    delete m_renderer;
    delete m_fbo;
}

QSGTextureRef SubTreeTextureProvider::texture()
{
    if (m_dirtyTexture)
        grab();
    return m_texture;
}

void SubTreeTextureProvider::setItem(QSGItem *item)
{
    if (item == m_item)
        return;
    if (m_item) {
        if (m_renderer)
            m_renderer->setRootNode(0);
        QSGItemPrivate::get(m_item)->derefFromEffectItem();
    }
    m_item = item;
    if (m_item) {
        QSGItemPrivate::get(m_item)->refFromEffectItem();
        if (m_renderer)
            m_renderer->setRootNode(QSGItemPrivate::get(m_item)->rootNode);
    }

    emit itemChanged();
}

void SubTreeTextureProvider::setMargins(const QSizeF &margins)
{
    if (margins == m_margins)
        return;
    m_margins = margins;
    emit marginsChanged();
}

void SubTreeTextureProvider::setLive(bool live)
{
    if (live == m_live)
        return;
    m_live = live;
    emit liveChanged();
}

void SubTreeTextureProvider::markDirtyTexture()
{
    if (m_live) {
        m_dirtyTexture = true;
        emit textureChanged();
    }
}

void SubTreeTextureProvider::grab()
{
    qreal w = 2 * m_margins.width();
    qreal h = 2 * m_margins.height();
    if (m_item) {
        w += m_item->width();
        h += m_item->height();
    }
    if (!m_item || w <= 0 || h <= 0) {
        m_texture = QSGTextureRef();
        delete m_fbo;
        m_fbo = 0;
        return;
    }

    if (!m_renderer) {
        m_renderer = QSGContext::current->createRenderer();
        connect(m_renderer, SIGNAL(sceneGraphChanged()), this, SLOT(markDirtyTexture()));
        m_renderer->setRootNode(QSGItemPrivate::get(m_item)->rootNode);
    }

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
    Node::DirtyFlags dirty = m_renderer->rootNode()->dirtyFlags();
    const QGLContext *ctx = QSGContext::current->glContext();
    QRectF r(0, 0, m_item->width(), m_item->height());
    r.adjust(-m_margins.width(), -m_margins.height(), m_margins.width(), m_margins.height());
    m_renderer->setDeviceRect(m_fbo->size());
    m_renderer->setProjectMatrixToRect(r);
    m_renderer->setClearColor(Qt::transparent);
    m_renderer->renderScene(BindableFbo(const_cast<QGLContext *>(ctx), m_fbo));
    m_dirtyTexture = false;
    m_renderer->rootNode()->markDirty(dirty);
}


TextureItem::TextureItem(QSGItem *parent)
    : QSGItem(parent)
{
    setFlag(ItemHasContents);
}

QSGTextureProvider *TextureItem::textureProvider() const
{
    return m_textureProvider;
}

void TextureItem::setTextureProvider(QSGTextureProvider *provider)
{
    if (provider == m_textureProvider)
        return;
    m_textureProvider = provider;
    emit textureProviderChanged();
}

Node *TextureItem::updatePaintNode(Node *oldNode, UpdatePaintNodeData *data)
{
    TextureNode *node = static_cast<TextureNode *>(oldNode);
    if (!node)
        node = new TextureNode;

    node->setTargetRect(QRectF(0, 0, width(), height()));
    node->setSourceRect(QRectF(0, 0, 1, 1));
    node->setOpacity(data->opacity);
    node->setTexture(m_textureProvider);

    return node;
}


SubTree::SubTree(QSGItem *parent)
    : QSGItem(parent)
    , m_textureProvider(new SubTreeTextureProvider(this))
{
    setFlag(ItemHasContents);
}

QSGItem *SubTree::item() const
{
    return m_textureProvider->item();
}

void SubTree::setItem(QSGItem *item)
{
    if (item == m_textureProvider->item())
        return;
    m_textureProvider->setItem(item);
    update();
    emit itemChanged();
}

QSizeF SubTree::margins() const
{
    return m_textureProvider->margins();
}

void SubTree::setMargins(const QSizeF &margins)
{
    if (margins == m_textureProvider->margins())
        return;
    m_textureProvider->setMargins(margins);
    update();
    emit marginsChanged();
}

bool SubTree::live() const
{
    return m_textureProvider->live();
}

void SubTree::setLive(bool live)
{
    if (live == m_textureProvider->live())
        return;
    m_textureProvider->setLive(live);
    update();
    emit liveChanged();
}

QSGTextureProvider *SubTree::textureProvider() const
{
    return m_textureProvider;
}

Node *SubTree::updatePaintNode(Node *oldNode, UpdatePaintNodeData *data)
{
    TextureNode *node = static_cast<TextureNode *>(oldNode);
    if (!node)
        node = new TextureNode;

    m_textureProvider->setClampToEdge(true);
    m_textureProvider->setLinearFiltering(QSGItemPrivate::get(this)->smooth);

    node->setTargetRect(QRectF(0, 0, width(), height()));
    node->setSourceRect(QRectF(0, 0, 1, 1));
    node->setOpacity(data->opacity);
    node->setTexture(m_textureProvider);

    return node;
}

QT_END_NAMESPACE

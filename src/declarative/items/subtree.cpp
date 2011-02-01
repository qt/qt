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

class SubTreeTextureProvider : public QSGTextureProvider
{
public:
    SubTreeTextureProvider(SubTree *parent);
    virtual QSGTextureRef texture() const;

private:
    SubTree *subtree;
};

SubTreeTextureProvider::SubTreeTextureProvider(SubTree *parent)
    : QSGTextureProvider(parent), subtree(parent)
{
    Q_ASSERT(parent);
    connect(parent->renderer(), SIGNAL(sceneGraphChanged()), this, SIGNAL(textureChanged()));
    connect(parent, SIGNAL(itemChanged()), this, SIGNAL(textureChanged()));
    connect(parent, SIGNAL(marginsChanged()), this, SIGNAL(textureChanged()));
}

QSGTextureRef SubTreeTextureProvider::texture() const
{
    const QSGItemPrivate *d = QSGItemPrivate::get(subtree);
    TextureNodeInterface *node = static_cast<TextureNodeInterface *>(d->paintNode);
    return node ? node->texture() : QSGTextureRef();
}


SubTree::SubTree(QSGItem *parent)
    : QSGItem(parent)
    , m_item(0)
    , m_margins(0, 0)
    , m_textureProvider(new SubTreeTextureProvider(this))
    , m_renderer(0)
    , m_fbo(0)
    , m_live(true)
    , m_dirtyTexture(true)
{
}

SubTree::~SubTree()
{
    delete m_renderer;
    delete m_fbo;
}

QSGItem *SubTree::item() const
{
    return m_item;
}

void SubTree::setItem(QSGItem *item)
{
    if (item == m_item)
        return;
    m_item = item;
    if (m_live) {
        m_dirtyTexture = true;
        update();
    }
    emit itemChanged();
}

QSizeF SubTree::margins() const
{
    return m_margins;
}

void SubTree::setMargins(const QSizeF &margins)
{
    if (margins == m_margins)
        return;
    m_margins = margins;
    if (m_live) {
        m_dirtyTexture = true;
        update();
    }
    emit marginsChanged();
}

bool SubTree::live() const
{
    return m_live;
}

void SubTree::setLive(bool live)
{
    if (live == bool(m_live))
        return;
    m_live = live;
    if (m_live) {
        m_dirtyTexture = true;
        update();
    }
    emit liveChanged();
}

QSGTextureProvider *SubTree::textureProvider() const
{
    return m_textureProvider;
}

Renderer *SubTree::renderer() const
{
    return m_renderer;
}

void SubTree::markDirtyTexture()
{
    if (m_live)
        update();
    m_dirtyTexture = true;
}


Node *SubTree::updatePaintNode(Node *oldNode, UpdatePaintNodeData *data)
{
    qreal w = (m_item ? m_item->width() : qreal(0)) + 2 * m_margins.width();
    qreal h = (m_item ? m_item->height() : qreal(0)) + 2 * m_margins.height();
    TextureNodeInterface *node = static_cast<TextureNodeInterface *>(oldNode);

    if (m_live) {
        if (!m_item || w == 0 || h == 0) {
            delete node;
            return 0;
        }

        bool newTextureOrNode = false;
        if (!node) {
            node = QSGContext::current->createTextureNode();
            newTextureOrNode = true;
        }

        if (!m_fbo || m_fbo->width() != qCeil(w) || m_fbo->height() != qCeil(h)) {
            delete m_fbo;
            m_fbo = new QGLFramebufferObject(qCeil(w), qCeil(h),
                                             QGLFramebufferObject::CombinedDepthStencil);
            QSGTexture *tex = new QSGTexture;
            tex->setTextureId(m_fbo->texture());
            tex->setOwnsTexture(false);
            m_texture = QSGTextureRef(tex);
            newTextureOrNode = true;
        }

        if (newTextureOrNode)
            node->setTexture(m_texture);

        // Render texture.
        // TODO.
    } else if (!node) {
        node = QSGContext::current->createTextureNode();
        node->setTexture(m_texture);
    }

    node->setRect(QRectF(0, 0, width(), height()));
    node->setSourceRect(QRectF(0, 0, 1, 1));
    node->setClampToEdge(true);
    node->setLinearFiltering(QSGItemPrivate::get(this)->smooth);
    node->setOpacity(data->opacity);
    node->update();

    return node;
}

QT_END_NAMESPACE

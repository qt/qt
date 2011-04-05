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

#include "effectnode.h"
#include "renderer.h"
#include "adaptationlayer.h"

#include <qgltexture2d.h>
#include <qglframebufferobject.h>

QT_BEGIN_NAMESPACE

EffectSubtreeNode::EffectSubtreeNode()
    : m_margins(0, 0)
    , m_dirty(false)
{
    setFlag(Node::UsePreprocess, true);
}

EffectSubtreeNode::~EffectSubtreeNode()
{
    for (int i = 0; i < m_subtrees.size(); ++i) {
        delete m_subtrees.at(i).renderer;
        delete m_subtrees.at(i).fbo;
    }
}

void EffectSubtreeNode::setSize(const QSize &size)
{
    if (m_size == size)
        return;

    for (int i = 0; i < m_subtrees.size(); ++i) {
        delete m_subtrees.at(i).fbo;
        m_subtrees[i].fbo = 0;
    }

    m_dirty = true;
    m_size = size;
    markDirty(DirtyMaterial);
}

void EffectSubtreeNode::setMargins(const QSize &margins)
{
    if (m_margins == margins)
        return;

    for (int i = 0; i < m_subtrees.size(); ++i) {
        delete m_subtrees.at(i).fbo;
        m_subtrees[i].fbo = 0;
    }

    m_dirty = true;
    m_margins = margins;
    markDirty(DirtyMaterial);
}

QRectF EffectSubtreeNode::sourceRect() const
{
    QSizeF s = m_size + 2 * m_margins;
    return QRectF(m_margins.width() / s.width(), 1 - m_margins.height() / s.height(), m_size.width() / s.width(), -m_size.height() / s.height());
}

void EffectSubtreeNode::setSubtreeCount(int count)
{
    int oldSize = m_subtrees.size();
    for (int i = count; i < oldSize; ++i) {
        delete m_subtrees.at(i).renderer;
        delete m_subtrees.at(i).fbo;
    }
    m_subtrees.resize(count);
    for (int i = oldSize; i < count; ++i) {
        SourceData &subtree = m_subtrees[i];
        subtree.renderer = 0;
        subtree.fbo = 0;
        subtree.node = 0;
    }
}

void EffectSubtreeNode::setSubtree(RootNode *node, int index)
{
    Q_ASSERT(index >= 0 && index < m_subtrees.size());
    SourceData &subtree = m_subtrees[index];
    subtree.node = node;

    if (subtree.renderer)
        subtree.renderer->setRootNode(subtree.node);
    m_dirty = true;
    markDirty(DirtyMaterial);
}

void EffectSubtreeNode::setSubtreeMatrix(const QMatrix4x4 &matrix, int index)
{
    Q_ASSERT(index >= 0 && index < m_subtrees.size());
    m_subtrees[index].matrix = matrix;
    markDirty(DirtyMaterial);
}

void EffectSubtreeNode::sceneGraphChanged()
{
    m_dirty = true;
    markDirty(DirtyMaterial);
}


void EffectSubtreeNode::preprocess()
{
    if (!m_dirty) {
        return;
    }

    emit aboutToRender();

    bool hasSubtree = false;
    for (int i = 0; i < m_subtrees.size(); ++i)
        hasSubtree |= m_subtrees.at(i).node != 0;
    if (!hasSubtree) {
        qWarning("EffectSubtreeNode::preprocess: aborting because there are no root nodes\n");
        return;
    }

    QGLContext *ctx = const_cast<QGLContext *>(QGLContext::currentContext());

    QSize texSize = m_size + 2 * m_margins;
    for (int i = 0; i < m_subtrees.size(); ++i) {
        SourceData &subtree = m_subtrees[i];
        if (!subtree.node)
            continue;

        if (!subtree.renderer) {
            subtree.renderer = qt_adaptation_layer()->createRenderer();
            subtree.renderer->setRootNode(subtree.node);
            connect(subtree.renderer, SIGNAL(sceneGraphChanged()), this, SLOT(sceneGraphChanged()));
            connect(subtree.renderer, SIGNAL(sceneGraphChanged()), this, SIGNAL(repaintRequired()));
        }

        if (!subtree.fbo) {
            subtree.fbo = new QGLFramebufferObject(texSize, QGLFramebufferObject::CombinedDepthStencil);
            emit textureCreated(subtree.fbo->texture(), texSize, i);
        }

        subtree.renderer->setDeviceRect(texSize);
        subtree.renderer->setViewportRect(texSize);
        subtree.renderer->setProjectMatrixToDeviceRect();
        QMatrix4x4 m = subtree.renderer->projectMatrix();
        m.translate(m_margins.width(), m_margins.height());
        subtree.renderer->setProjectMatrix(m * subtree.matrix);
        subtree.renderer->setClearColor(Qt::transparent);

        subtree.renderer->renderScene(BindableFbo(ctx, subtree.fbo));
    }

    m_dirty = false;
}

QT_END_NAMESPACE

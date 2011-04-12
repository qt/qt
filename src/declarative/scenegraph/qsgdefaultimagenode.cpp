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

#include "qsgdefaultimagenode_p.h"

#include <private/qsgtextureprovider_p.h>

QT_BEGIN_NAMESPACE

QSGDefaultImageNode::QSGDefaultImageNode()
    : m_sourceRect(0, 0, 1, 1)
    , m_dirtyGeometry(false)
    , m_geometry(QSGGeometry::defaultAttributes_TexturedPoint2D(), 4)
{
    setMaterial(&m_materialO);
    setOpaqueMaterial(&m_material);
    setGeometry(&m_geometry);

#ifdef QML_RUNTIME_TESTING
    description = QLatin1String("image");
#endif
}

void QSGDefaultImageNode::setTargetRect(const QRectF &rect)
{
    if (rect == m_targetRect)
        return;
    m_targetRect = rect;
    m_dirtyGeometry = true;
}

void QSGDefaultImageNode::setSourceRect(const QRectF &rect)
{
    if (rect == m_sourceRect)
        return;
    m_sourceRect = rect;
    m_dirtyGeometry = true;
}


void QSGDefaultImageNode::setFiltering(QSGTexture::Filtering filtering)
{
    if (m_material.filtering() == filtering)
        return;

    m_material.setFiltering(filtering);
    m_materialO.setFiltering(filtering);
    markDirty(DirtyMaterial);
}


void QSGDefaultImageNode::setMipmapFiltering(QSGTexture::Filtering filtering)
{
    if (m_material.mipmapFiltering() == filtering)
        return;

    m_material.setMipmapFiltering(filtering);
    m_materialO.setMipmapFiltering(filtering);
    markDirty(DirtyMaterial);
}


void QSGDefaultImageNode::setVerticalWrapMode(QSGTexture::WrapMode wrapMode)
{
    if (m_material.verticalWrapMode() == wrapMode)
        return;

    m_material.setVerticalWrapMode(wrapMode);
    m_materialO.setVerticalWrapMode(wrapMode);
    markDirty(DirtyMaterial);
}

void QSGDefaultImageNode::setHorizontalWrapMode(QSGTexture::WrapMode wrapMode)
{
    if (m_material.horizontalWrapMode() == wrapMode)
        return;

    m_material.setHorizontalWrapMode(wrapMode);
    m_materialO.setHorizontalWrapMode(wrapMode);
    markDirty(DirtyMaterial);
}


void QSGDefaultImageNode::setTexture(QSGTexture *texture)
{
    if (texture == m_material.texture())
        return;

    m_material.setTexture(texture);
    m_materialO.setTexture(texture);
    // Texture cleanup
//    if (!texture.isNull())
//        m_material.setBlending(texture->hasAlphaChannel());
    markDirty(DirtyMaterial);

    // Because the texture can be a different part of the atlas, we need to update it...
    m_dirtyGeometry = true;
}

void QSGDefaultImageNode::update()
{
    if (m_dirtyGeometry)
        updateGeometry();
}

void QSGDefaultImageNode::preprocess()
{
    bool doDirty = false;
    QSGDynamicTexture *t = qobject_cast<QSGDynamicTexture *>(m_material.texture());
    if (t) {
        doDirty = t->updateTexture();
        updateGeometry();
    }
// ### texture cleanup
//    bool alpha = m_material.blending();
//    if (!m_material->texture().isNull() && alpha != m_material.texture()->hasAlphaChannel()) {
//        m_material.setBlending(!alpha);
//        doDirty = true;
//    }

    if (doDirty)
        markDirty(DirtyMaterial);
}

void QSGDefaultImageNode::updateGeometry()
{
    const QSGTexture *t = m_material.texture();
    if (!t) {
        QSGGeometry::updateTexturedRectGeometry(&m_geometry, QRectF(), QRectF());
    } else {
        QRectF textureRect = t->textureSubRect();
        QRectF sr(textureRect.x() + m_sourceRect.x() * textureRect.width(),
                  textureRect.y() + m_sourceRect.y() * textureRect.height(),
                  m_sourceRect.width() * textureRect.width(),
                  m_sourceRect.height() * textureRect.height());

        QSGGeometry::updateTexturedRectGeometry(&m_geometry, m_targetRect, sr);
    }
    markDirty(DirtyGeometry);
    m_dirtyGeometry = false;
}

QT_END_NAMESPACE

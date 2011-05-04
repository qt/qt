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


#include "qsgsimpletexturenode.h"

QT_BEGIN_NAMESPACE

static void qsgsimpletexturenode_update(QSGGeometry *g,
                                        QSGTexture *texture,
                                        const QRectF &rect)
{
    if (!texture)
        return;

    QSize ts = texture->textureSize();
    QRectF sourceRect(0, ts.height(), ts.width(), -ts.height());
    QSGGeometry::updateTexturedRectGeometry(g, rect, texture->convertToNormalizedSourceRect(sourceRect));
}

/*!
  \class QSGSimpleTextureNode
  \brief The QSGSimpleTextureNode provided for convenience to easily draw
  textured content using the QML scene graph.

  \warning The simple texture node class must have texture before being
  added to the scene graph to be rendered.
*/

/*!
    Constructs a new simple texture node
 */
QSGSimpleTextureNode::QSGSimpleTextureNode()
    : m_geometry(QSGGeometry::defaultAttributes_TexturedPoint2D(), 4)
{
    setGeometry(&m_geometry);
    setMaterial(&m_opaque_material);
    setOpaqueMaterial(&m_material);
}

/*!
    Sets the filtering to be used for this texture node to \a filtering.

    For smooth scaling, use QSGTexture::Linear; for normal scaling, use
    QSGTexture::Nearest.
 */
void QSGSimpleTextureNode::setFiltering(QSGTexture::Filtering filtering)
{
    if (m_material.filtering() == filtering)
        return;

    m_material.setFiltering(filtering);
    m_opaque_material.setFiltering(filtering);
    markDirty(DirtyMaterial);
}


/*!
    Returns the filtering currently set on this texture node
 */
QSGTexture::Filtering QSGSimpleTextureNode::filtering() const
{
    return m_material.filtering();
}


/*!
    Sets the target rect of this texture node to \a r
 */
void QSGSimpleTextureNode::setRect(const QRectF &r)
{
    if (m_rect == r)
        return;
    m_rect = r;
    qsgsimpletexturenode_update(&m_geometry, texture(), m_rect);
    markDirty(DirtyGeometry);
}


/*!
    Returns the target rect of this texture node.
 */
QRectF QSGSimpleTextureNode::rect() const
{
    return m_rect;
}

/*!
    Sets the texture of this texture node to \a texture.

    \warning A texture node must have a texture before being added
    to the scenegraph to be rendered.
 */
void QSGSimpleTextureNode::setTexture(QSGTexture *texture)
{
    if (m_material.texture() == texture)
        return;
    m_material.setTexture(texture);
    m_opaque_material.setTexture(texture);
    qsgsimpletexturenode_update(&m_geometry, texture, m_rect);
    markDirty(DirtyMaterial);
}



/*!
    Returns the texture for this texture node
 */
QSGTexture *QSGSimpleTextureNode::texture() const
{
    return m_material.texture();
}

QT_END_NAMESPACE

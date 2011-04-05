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

#include "qsgninepatchnode_p.h"
#include <private/qsgadaptationlayer_p.h>

QSGNinePatchNode::QSGNinePatchNode()
    : m_geometry(QSGGeometry::defaultAttributes_TexturedPoint2D(), 4)
{
    setOpaqueMaterial(&m_material);
    setMaterial(&m_materialO);
    setGeometry(&m_geometry);
}

void QSGNinePatchNode::setInnerRect(const QRectF &rect)
{
    if (m_innerRect == rect)
        return;
    m_innerRect = rect;
    m_dirtyGeometry = true;
}

void QSGNinePatchNode::setRect(const QRectF &rect)
{
    if (m_targetRect == rect)
        return;
    m_targetRect = rect;
    m_dirtyGeometry = true;
}

void QSGNinePatchNode::setHorzontalTileMode(QSGBorderImage::TileMode mode)
{
    if (mode == QSGBorderImage::TileMode(m_horizontalTileMode))
        return;
    m_horizontalTileMode = mode;
    m_dirtyGeometry = true;
}


void QSGNinePatchNode::setVerticalTileMode(QSGBorderImage::TileMode mode)
{
    if (mode == QSGBorderImage::TileMode(m_verticalTileMode))
        return;
    m_verticalTileMode = mode;
    m_dirtyGeometry = true;
}


void QSGNinePatchNode::setLinearFiltering(bool linearFiltering)
{
    if (m_material.linearFiltering() == linearFiltering)
        return;

    m_material.setLinearFiltering(linearFiltering);
    m_materialO.setLinearFiltering(linearFiltering);
    markDirty(DirtyMaterial);
}

bool QSGNinePatchNode::linearFiltering() const
{
    return m_material.linearFiltering();
}

void QSGNinePatchNode::setTexture(const QSGTextureRef &texture)
{
    if (texture == m_material.texture())
        return;
    m_material.setTexture(texture);
    m_materialO.setTexture(texture);
    markDirty(DirtyMaterial);
}

QSGTextureRef QSGNinePatchNode::texture() const
{
    return m_material.texture();
}



void QSGNinePatchNode::update()
{
    QSGGeometry::updateTexturedRectGeometry(&m_geometry, m_targetRect, QRectF(0, 0, 1, 1));
}


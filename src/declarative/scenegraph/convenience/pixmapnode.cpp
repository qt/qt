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

#include "pixmapnode.h"
#include "utilities.h"
#include "texturematerial.h"

QT_BEGIN_NAMESPACE

PixmapNode::PixmapNode(const QRectF &t, const QPixmap &pixmap, const QRectF &sourceRect)
    : m_target_rect(t)
    , m_pixmap(pixmap)
    , m_source_rect(sourceRect)
{
    m_texture = Utilities::getTextureForPixmap(pixmap, true);
    m_material.setTexture(m_texture->texture(), !pixmap.hasAlphaChannel());
    m_material.setLinearFiltering(true);
    setMaterial(&m_material);
    updateGeometry();
}

void PixmapNode::setRect(const QRectF &rect)
{
    m_target_rect = rect;
    updateGeometry();
}

void PixmapNode::setSourceRect(const QRectF &rect)
{
    m_source_rect = rect;
    updateGeometry();
}

void PixmapNode::setRects(const QRectF &targetRect, const QRectF &sourceRect)
{
    m_target_rect = targetRect;
    m_source_rect = sourceRect;
    updateGeometry();
}

void PixmapNode::updateGeometry()
{
    if (m_source_rect.isEmpty())
        m_source_rect = m_pixmap.rect();
    Geometry *g = geometry();

    QRect src(m_texture->sourceRect().x() + m_source_rect.left() / m_pixmap.width() * m_texture->sourceRect().width(),
        m_texture->sourceRect().y() + m_source_rect.top() / m_pixmap.height() * m_texture->sourceRect().height(),
        m_source_rect.width() / m_pixmap.width() * m_texture->sourceRect().width(),
        m_source_rect.height() / m_pixmap.height() * m_texture->sourceRect().height());

    if (g->isNull())
        updateGeometryDescription(Utilities::getTexturedRectGeometryDescription(), GL_UNSIGNED_SHORT);
    Utilities::setupRectGeometry(g, m_target_rect, QSize(1, 1), src);
    markDirty(Node::DirtyGeometry);

    setBoundingRect(m_target_rect);
}

QT_END_NAMESPACE

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

#include "default_texturenode.h"

#include <qmath.h>
#include <qvarlengtharray.h>

DefaultTextureNode::DefaultTextureNode()
    : m_texture(0)
    , m_dirty_texture(false)
    , m_dirty_geometry(false)
{
    m_material.setLinearFiltering(m_linear_filtering);
    m_materialO.setLinearFiltering(m_linear_filtering);
    m_materialO.setOpacity(m_opacity);
    setMaterial(&m_material);

    updateGeometryDescription(Utilities::getTexturedRectGeometryDescription(), GL_UNSIGNED_SHORT);

#ifdef QML_RUNTIME_TESTING
    description = "pixmap";
#endif
}

void DefaultTextureNode::setRect(const QRectF &rect)
{
    if (m_rect == rect)
        return;
    m_rect = rect;
    setBoundingRect(rect);
    m_dirty_geometry = true;
}

void DefaultTextureNode::setSourceRect(const QRectF &rect)
{
    if (m_source_rect == rect)
        return;
    m_source_rect = rect;
    m_dirty_geometry = true;
}

void DefaultTextureNode::setOpacity(qreal opacity)
{
    if (m_opacity == opacity)
        return;
    m_materialO.setOpacity(opacity);
    m_opacity = opacity;
    setMaterial(opacity == 1 ? &m_material : &m_materialO); // Indicate that the material state has changed.
}

void DefaultTextureNode::setTexture(const QSGTextureRef &texture)
{
    if (texture.texture() == m_texture.texture()
            && texture.subRect() == m_texture.subRect())
        return;

    m_texture = texture;
    m_dirty_texture = true;
}

void DefaultTextureNode::setClampToEdge(bool clampToEdge)
{
    if (m_clamp_to_edge == clampToEdge)
        return;
    m_clamp_to_edge = clampToEdge;
    m_dirty_texture = true;
}

void DefaultTextureNode::setLinearFiltering(bool linearFiltering)
{
    if (m_linear_filtering == linearFiltering)
        return;
    m_material.setLinearFiltering(linearFiltering);
    m_materialO.setLinearFiltering(linearFiltering);
    m_linear_filtering = linearFiltering;
    setMaterial(m_opacity == 1 ? &m_material : &m_materialO); // Indicate that the material state has changed.
}

void DefaultTextureNode::update()
{
    if (m_dirty_texture)
        updateTexture();

    // A texture update causes the source rectangle to change, so update geometry too.
    if (m_dirty_geometry || m_dirty_texture)
        updateGeometry();

    m_dirty_geometry = false;
    m_dirty_texture = false;
}

void DefaultTextureNode::updateGeometry()
{
    if (m_texture.isNull())
        return;

    Geometry *g = geometry();

    QRectF normalizedSource = m_source_rect;

    QVarLengthArray<qreal> xSrc, xDst;
    xSrc.append(normalizedSource.left());
    xDst.append(m_rect.left());
    // ### gunnar: needs porting
//    if (!m_texture.isNull() && !m_texture->hasOwnTexture()) {
//        for (int i = qFloor(normalizedSource.left()) + 1; i <= qCeil(normalizedSource.right()) - 1; ++i) {
//            xSrc.append(i);
//            xDst.append(m_rect.left() + m_rect.width() * (i - normalizedSource.left()) / normalizedSource.width());
//        }
//    }
    xSrc.append(normalizedSource.right());
    xDst.append(m_rect.right());

    QVarLengthArray<qreal> ySrc, yDst;
    ySrc.append(normalizedSource.top());
    yDst.append(m_rect.top());
    // ### gunnar: needs porting
//    if (!m_texture.isNull() && !m_texture->hasOwnTexture()) {
//        for (int i = qFloor(normalizedSource.top()) + 1; i <= qCeil(normalizedSource.bottom()) - 1; ++i) {
//            ySrc.append(i);
//            yDst.append(m_rect.top() + m_rect.height() * (i - normalizedSource.top()) / normalizedSource.height());
//        }
//    }
    ySrc.append(normalizedSource.bottom());
    yDst.append(m_rect.bottom());

    if (xSrc.size() == 2 && ySrc.size() == 2) {
        g->setDrawingMode(QSG::TriangleStrip);
        g->setVertexCount(4);
        if (!m_clamp_to_edge) {
            g->setIndexCount(4);
            for (int i=0; i<4; ++i)
                g->ushortIndexData()[i] = i;
        }
    } else {
        g->setDrawingMode(QSG::Triangles);
        int cellCount = (xSrc.size() - 1) * (ySrc.size() - 1);
        int vertexCount = cellCount * 4; // Four vertices per grid cell.
        if (vertexCount > 0x10000)
            qWarning("QxPixmapNode::updateGeometry: Number of vertices exceeds 65536.");
        g->setVertexCount(vertexCount);

        if (!m_clamp_to_edge) {
            int indexCount = cellCount * 6; // Six indices per grid cell.
            g->setIndexCount(indexCount);

            for (int i = 0; i < cellCount; ++i) {
                g->ushortIndexData()[6 * i + 0] = i * 4 + 0;
                g->ushortIndexData()[6 * i + 1] = i * 4 + 1;
                g->ushortIndexData()[6 * i + 2] = i * 4 + 3;
                g->ushortIndexData()[6 * i + 3] = i * 4 + 3;
                g->ushortIndexData()[6 * i + 4] = i * 4 + 2;
                g->ushortIndexData()[6 * i + 5] = i * 4 + 0;
            }
        }
    }

    struct V {
        float x, y, tx, ty;
    };

    V *v = (V *) g->vertexData();

    qreal xSrcOffset = qFloor(normalizedSource.left());
    qreal ySrcOffset = qFloor(normalizedSource.top());
    QRectF texSrcRect = m_texture.subRect();

    for (int j = 0; j < ySrc.size() - 1; ++j) {
        for (int i = 0; i < xSrc.size() - 1; ++i) {
            v[0].x = v[1].x = xDst.at(i);
            v[2].x = v[3].x = xDst.at(i + 1);
            v[0].y = v[2].y = yDst.at(j);
            v[1].y = v[3].y = yDst.at(j + 1);

            QRectF src(texSrcRect.x() + (xSrc.at(i) - i - xSrcOffset) * texSrcRect.width(),
                texSrcRect.y() + (ySrc.at(j) - j - ySrcOffset) * texSrcRect.height(),
                (xSrc.at(i + 1) - xSrc.at(i)) * texSrcRect.width(),
                (ySrc.at(j + 1) - ySrc.at(j)) * texSrcRect.height());

            v[0].tx = v[1].tx = src.left();
            v[2].tx = v[3].tx = src.right();
            v[0].ty = v[2].ty = src.top();
            v[1].ty = v[3].ty = src.bottom();            
            v += 4;
        }
    }

    markDirty(DirtyGeometry);
}

void DefaultTextureNode::updateTexture()
{
    bool opaque = !m_texture->hasAlphaChannel();
    m_material.setTexture(m_texture, opaque);
    m_materialO.setTexture(m_texture, opaque);
    setMaterial(m_opacity == 1 ? &m_material : &m_materialO); // Indicate that the material state has changed.

    m_material.setClampToEdge(m_clamp_to_edge);
    m_materialO.setClampToEdge(m_clamp_to_edge);
}

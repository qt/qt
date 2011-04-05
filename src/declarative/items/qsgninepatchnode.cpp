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
#include <private/qmath_p.h>

QSGNinePatchNode::QSGNinePatchNode()
    : m_geometry(QSGGeometry::defaultAttributes_TexturedPoint2D(), 0)
    , m_horizontalTileMode(QSGBorderImage::Stretch)
    , m_verticalTileMode(QSGBorderImage::Stretch)
    , m_dirtyGeometry(false)
{
    setOpaqueMaterial(&m_material);
    setMaterial(&m_materialO);
    setGeometry(&m_geometry);
    m_geometry.setDrawingMode(GL_TRIANGLES);
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
    if (!m_dirtyGeometry)
        return;

    Q_ASSERT(!m_material.texture().isNull());

    float tw = m_material.texture()->textureSize().width();
    float th = m_material.texture()->textureSize().height();

    float rightBorder = tw - m_innerRect.right();

//    qDebug() << m_innerRect << m_targetRect << m_horizontalTileMode << m_verticalTileMode;

    int xChunkCount = 0;
    float xChunkSize = 0; // Relative to target rect.
    float xTexSize = m_innerRect.width(); // Size of the texture to stretch/tile
    float xSize = m_targetRect.width() - m_innerRect.left() - rightBorder;

    if (m_horizontalTileMode == QSGBorderImage::Repeat) {
        xChunkCount = qCeil(xSize / xTexSize);
        xChunkSize = xTexSize;
    } else if (m_horizontalTileMode == QSGBorderImage::Round) {
        xChunkCount = qCeil(xSize / xTexSize);
        qreal fullWidth = xChunkCount * xTexSize;
        xChunkSize = xTexSize * xSize / fullWidth;
    } else {
        xChunkCount = 1;
        xChunkSize = xSize;
    }

    int xTotalChunkCount = xChunkCount + 2;

    int vertexCount = xTotalChunkCount * 2 * 2;
    int indexCount = xTotalChunkCount * 6;

//    qDebug() << " - xChunkCount" << xChunkCount << endl
//             << " - xChunkSize" << xChunkSize << endl
//             << " - xTexSize" << xTexSize << endl
//             << " - xSize" << xSize << endl
//             << " - xTotalChunkCount" << xTotalChunkCount << endl;

//    qDebug() << " - vertexCount" << vertexCount << endl
//             << " - indexCount" << indexCount;

    if (vertexCount != m_geometry.vertexCount() || indexCount != m_geometry.indexCount())
        m_geometry.allocate(vertexCount, indexCount);


    QSGGeometry::TexturedPoint2D *v = m_geometry.vertexDataAsTexturedPoint2D();

    float xTexChunk1 = m_innerRect.left() / tw;
    float xTexChunk2 = m_innerRect.right() / tw;

    for (int yy=0; yy<2; ++yy) {
        float y = yy * m_targetRect.height();
        float ty = yy;

        v++->set(0, y, 0, ty);
        v++->set(m_innerRect.x(), y, xTexChunk1, ty);

        for (int xc=0; xc<xChunkCount; ++xc) {
            float xx = m_innerRect.x() + xChunkSize * xc;
            v++->set(xx, y, xTexChunk1, ty);

            // Special case the last one
            if (xc == xChunkCount - 1) {
                float t = m_horizontalTileMode == QSGBorderImage::Repeat
                        ? xTexChunk1 + (xTexChunk2 - xTexChunk1) * (m_targetRect.width() - rightBorder - xx) / xChunkSize
                        : xTexChunk2;
                v->set(m_targetRect.width() - rightBorder, y, t, ty);
            } else {
                v->set(xx + xChunkSize, y, xTexChunk2, ty);
            }
            ++v;
        }

        v++->set(m_targetRect.width() - rightBorder, y, xTexChunk2, ty);
        v++->set(m_targetRect.width(), y, 1, ty);
    }

//    v = m_geometry.vertexDataAsTexturedPoint2D();
//    for (int i=0; i<m_geometry.vertexCount(); ++i) {
//        printf("Vertex: %d:  (%.3f, %.3f) - (%.3f, %.3f)\n",
//               i,
//               v->x, v->y, v->tx, v->ty);
//        ++v;
//    }

    quint16 *i = m_geometry.indexDataAsUShort();
    int row = xTotalChunkCount * 2;
    for (int c=0; c<xTotalChunkCount; ++c) {
        i[c*6    ] = c * 2;
        i[c*6 + 1] = c * 2 + 1;
        i[c*6 + 2] = c * 2 + row;
        i[c*6 + 3] = c * 2 + 1;
        i[c*6 + 4] = c * 2 + row + 1;
        i[c*6 + 5] = c * 2 + row;
    }

//    for (int idx=0; idx<m_geometry.indexCount(); idx+=6) {
//        printf("%2d: ", idx);
//        for (int s=0; s<6; ++s)
//            printf(" %d", i[idx + s]);
//        printf("\n");
//    }

    markDirty(QSGNode::DirtyGeometry);
}


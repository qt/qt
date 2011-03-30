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

QSGNinePatchNode::QSGNinePatchNode(const QRectF &targetRect, const QSGTextureRef &texture,
                                 const QRect &innerRect, bool linearFiltering)
    : m_targetRect(targetRect)
    , m_innerRect(innerRect)
    , m_linearFiltering(linearFiltering)
    , m_geometry(QSGGeometry::defaultAttributes_TexturedPoint2D(), 6 * 6, 5 * 5 * 6)
{
    m_texture = texture;
    bool alpha = texture->hasAlphaChannel();
    m_material.setTexture(m_texture, !alpha);
    m_material.setLinearFiltering(linearFiltering);
    m_materialO.setTexture(m_texture, !alpha);
    m_materialO.setLinearFiltering(linearFiltering);

    setOpaqueMaterial(&m_material);
    setMaterial(&m_materialO);
    setGeometry(&m_geometry);
    m_geometry.setDrawingMode(GL_TRIANGLES);

    updateGeometry();
}

void QSGNinePatchNode::setRect(const QRectF &rect)
{
    if (m_targetRect == rect)
        return;

    m_targetRect = rect;
    updateGeometry();
}

void QSGNinePatchNode::setLinearFiltering(bool linearFiltering)
{
    if (m_linearFiltering == linearFiltering)
        return;

    m_material.setLinearFiltering(linearFiltering);
    m_materialO.setLinearFiltering(linearFiltering);
    markDirty(DirtyMaterial);
}

void QSGNinePatchNode::updateGeometry()
{
    QSGGeometry *g = geometry();

    ushort *indices = g->indexDataAsUShort();
    int count = 0;
    for (int i = 0; i < 5; ++i) {
        int i6 = i * 6;
        for (int j = 0; j < 5; ++j) {
            indices[count++] = i6 + j + 0;
            indices[count++] = i6 + j + 6;
            indices[count++] = i6 + j + 7;
            indices[count++] = i6 + j + 7;
            indices[count++] = i6 + j + 1;
            indices[count++] = i6 + j + 0;
        }
    }

    struct V
    {
        V(float x, float y, float u, float v) : x(x), y(y), u(u), v(v) { }
        float x, y, u, v;
    };

    V *vertices = (V *)g->vertexData();
    Q_ASSERT(sizeof(V) == g->stride());

    qreal x[6], y[6], u[6], v[6];

    QSizeF pixSize = m_texture->textureSize();

    x[0] = m_targetRect.x();
    x[1] = m_targetRect.x() + m_innerRect.x();
    x[2] = x[1] + 0.5;
    x[3] = m_targetRect.x() + m_targetRect.width() - (pixSize.width() - (m_innerRect.x() + m_innerRect.width()));
    x[4] = x[3] + 0.5;
    x[5] = m_targetRect.x() + m_targetRect.width();

    y[0] = m_targetRect.y();
    y[1] = m_targetRect.y() + m_innerRect.y();
    y[2] = y[1] + 0.5;
    y[3] = m_targetRect.y() + m_targetRect.height() - (pixSize.height() - (m_innerRect.y() + m_innerRect.height()));
    y[4] = y[3] + 0.5;
    y[5] = m_targetRect.y() + m_targetRect.height();

    QRectF src = m_texture->textureSubRect();
    qreal pw = src.width() / m_texture->textureSize().width();
    qreal ph = src.height() / m_texture->textureSize().height();

    u[0] = src.left();
    u[1] = src.left() + m_innerRect.x() * pw;
    u[2] = u[1];
    u[3] = src.left() + (m_innerRect.x() + m_innerRect.width()) * pw;
    u[4] = u[3];
    u[5] = src.right();

    v[0] = src.top();
    v[1] = src.top() + m_innerRect.y() * ph;
    v[2] = v[1];
    v[3] = src.top() + (m_innerRect.y() + m_innerRect.height()) * ph;
    v[4] = v[3];
    v[5] = src.bottom();

    for (int i = 0; i < 6; ++i) {
        for (int j = 0; j < 6; ++j)
            vertices[i * 6 + j] = V(x[j], y[i], u[j], v[i]);
    }

    markDirty(QSGNode::DirtyGeometry);
}


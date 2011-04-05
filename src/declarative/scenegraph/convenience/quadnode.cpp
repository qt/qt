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

#include "quadnode.h"
#include "utilities.h"
#include "adaptationlayer.h"

QT_BEGIN_NAMESPACE

QuadNode::QuadNode(const QRectF &t, const QRectF &sourceRect)
{
    updateGeometryDescription(Utilities::getTexturedRectGeometryDescription(), GL_UNSIGNED_SHORT);
    Utilities::setupRectGeometry(geometry(),t, QSize(1, 1), sourceRect);
}

QuadNode::QuadNode(const QRectF &t, const QRectF &sourceRect, qreal z, int meshSize)
{
    if (meshSize <= 1 && z == 0) {
        updateGeometryDescription(Utilities::getTexturedRectGeometryDescription(), GL_UNSIGNED_SHORT);
        Utilities::setupRectGeometry(geometry(),t, QSize(1, 1), sourceRect);
    } else {
        QVector<QSGAttributeDescription> desc;
        if (z == 0) {
            desc << QSGAttributeDescription(QSG::Position, 2, GL_FLOAT, 4 * sizeof(float));
            desc << QSGAttributeDescription(QSG::TextureCoord0, 2, GL_FLOAT, 4 * sizeof(float));
        } else {
            desc << QSGAttributeDescription(QSG::Position, 3, GL_FLOAT, 5 * sizeof(float));
            desc << QSGAttributeDescription(QSG::TextureCoord0, 2, GL_FLOAT, 5 * sizeof(float));
        }

        updateGeometryDescription(desc, GL_UNSIGNED_SHORT);
        Geometry *g = geometry();
        g->setDrawingMode(QSG::Triangles);
        g->setVertexCount((meshSize + 1) * (meshSize + 1));
        g->setIndexCount(meshSize * meshSize * 6);

        struct V4 {
            V4(float x, float y, float u, float v) : x(x), y(y), u(u), v(v) { }
            float x, y, u, v;
        };
        struct V5 {
            V5(float x, float y, float z, float u, float v) : x(x), y(y), z(z), u(u), v(v) { }
            float x, y, z, u, v;
        };

        qreal dx = (t.right() - t.left()) / meshSize;
        qreal du = (sourceRect.right() - sourceRect.left()) / meshSize;
        qreal dy = (t.bottom() - t.top()) / meshSize;
        qreal dv = (sourceRect.bottom() - sourceRect.top()) / meshSize;

        qreal y = t.top();
        qreal v = sourceRect.top();

        V4 *v4p = (V4 *)g->vertexData();
        V5 *v5p = (V5 *)g->vertexData();
        ushort *indices = g->ushortIndexData();

        for (int j = 0; j <= meshSize; ++j, y += dy, v += dv) {
            if (j == meshSize) {
                y = t.bottom();
                v = sourceRect.bottom();
            }
            qreal x = t.left();
            qreal u = sourceRect.left();
            for (int i = 0; i <= meshSize; ++i, x += dx, u += du) {
                if (i == meshSize) {
                    x = t.right();
                    u = sourceRect.right();
                }
                if (z == 0)
                    v4p[i + j * (meshSize + 1)] = V4(x, y, u, v);
                else
                    v5p[i + j * (meshSize + 1)] = V5(x, y, z, u, v);
            }
        }

        int index = 0;
        for (int j = 0; j < meshSize; ++j, ++index) {
            for (int i = 0; i < meshSize; ++i, ++index) {
                indices[6 * (j * meshSize + i) + 0] = index;
                indices[6 * (j * meshSize + i) + 1] = index + meshSize + 1;
                indices[6 * (j * meshSize + i) + 2] = index + meshSize + 2;
                indices[6 * (j * meshSize + i) + 3] = index + meshSize + 2;
                indices[6 * (j * meshSize + i) + 4] = index + 1;
                indices[6 * (j * meshSize + i) + 5] = index;
            }
        }

        markDirty(DirtyGeometry);
    }
}

QT_END_NAMESPACE

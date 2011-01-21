
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


#include "qsgclipnode_p.h"
#include "utilities.h"

#include <QtGui/qvector2d.h>
#include <QtCore/qmath.h>

QSGClipNode::QSGClipNode(const QRectF &rect)
    : m_rect(rect)
    , m_radius(0)
    , m_dirty_geometry(true)
{
    updateGeometryDescription(Utilities::getRectGeometryDescription(), GL_UNSIGNED_SHORT);
    Utilities::setupRectGeometry(geometry(), m_rect);

    // ### gunnar: Clip nodes should not register for preprocess
    // but rather have an update mechanism similar to updatePaintNode();
    setFlag(ClipIsRectangular, true);
}

void QSGClipNode::setRect(const QRectF &rect)
{
    m_rect = rect;
    m_dirty_geometry = true;
}

void QSGClipNode::setRadius(qreal radius)
{
    m_radius = radius;
    m_dirty_geometry = true;
    setFlag(ClipIsRectangular, radius == 0);
}

void QSGClipNode::update()
{
    if (m_dirty_geometry) {
        updateGeometry();
        m_dirty_geometry = false;
    }
}

void QSGClipNode::updateGeometry()
{
    if (qFuzzyIsNull(m_radius)) {
        updateGeometryDescription(Utilities::getRectGeometryDescription(), GL_UNSIGNED_SHORT);
        Utilities::setupRectGeometry(geometry(), m_rect);
    } else {
        struct Vertex
        {
            QVector2D position;
        };

        Geometry *g = geometry();
        int vertexCount = 0;

        // Radius should never exceeds half of the width or half of the height
        qreal radius = qMin(qMin(m_rect.width() / 2, m_rect.height() / 2), m_radius);
        QRectF rect = m_rect;
        rect.adjust(radius, radius, -radius, -radius);

        int segments = qMin(30, qCeil(radius)); // Number of segments per corner.

        // Overestimate the number of vertices and indices, reduce afterwards when the actual numbers are known.
        g->setVertexCount((segments + 1) * 4);
        g->setIndexCount(0);

        Vertex *vertices = (Vertex *)g->vertexData();

        for (int part = 0; part < 2; ++part) {
            for (int i = 0; i <= segments; ++i) {
                //### Should change to calculate sin/cos only once.
                qreal angle = qreal(0.5 * M_PI) * (part + i / qreal(segments));
                qreal s = qFastSin(angle);
                qreal c = qFastCos(angle);
                qreal y = (part ? rect.bottom() : rect.top()) - radius * c; // current inner y-coordinate.
                qreal lx = rect.left() - radius * s; // current inner left x-coordinate.
                qreal rx = rect.right() + radius * s; // current inner right x-coordinate.

                vertices[vertexCount++].position = QVector2D(rx, y);
                vertices[vertexCount++].position = QVector2D(lx, y);
            }
        }

        g->setDrawingMode(QGL::TriangleStrip);
        g->setVertexCount(vertexCount);

        markDirty(DirtyGeometry);
    }
    setBoundingRect(m_rect);
}


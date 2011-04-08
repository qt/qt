
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



#include "qsgdefaultrectanglenode_p.h"

#include <private/qsgvertexcolormaterial_p.h>
#include "qsgtexturematerial.h"

#include <private/qsgcontext_p.h>

#include <QtCore/qmath.h>

QT_BEGIN_NAMESPACE

QSGDefaultRectangleNode::QSGDefaultRectangleNode(QSGContext *context)
    : m_border(0)
    , m_radius(0)
    , m_pen_width(0)
    , m_gradient_is_opaque(true)
    , m_dirty_geometry(false)
    , m_default_geometry(QSGGeometry::defaultAttributes_Point2D(), 4)
    , m_context(context)
{
    setGeometry(&m_default_geometry);
    setMaterial(&m_fill_material);
    m_border_material.setColor(QColor(0, 0, 0));

    m_material_type = TypeFlat;

#ifdef QML_RUNTIME_TESTING
    description = QLatin1String("rectangle");
#endif
}

QSGDefaultRectangleNode::~QSGDefaultRectangleNode()
{
    switch (m_material_type) {
    case TypeFlat:
        break;
    case TypeVertexGradient:
        delete material();
        break;
    }
    delete m_border;
}

QSGGeometryNode *QSGDefaultRectangleNode::border()
{
    if (!m_border) {
        m_border = new QSGGeometryNode;
        m_border->setMaterial(&m_border_material);
        QSGGeometry *geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 0);
        m_border->setGeometry(geometry);
        m_border->setFlag(QSGNode::OwnsGeometry);
    }
    return m_border;
}

void QSGDefaultRectangleNode::setRect(const QRectF &rect)
{
    if (rect == m_rect)
        return;
    m_rect = rect;
    m_dirty_geometry = true;
}

void QSGDefaultRectangleNode::setColor(const QColor &color)
{
    if (color == m_fill_material.color())
        return;
    if (m_gradient_stops.isEmpty()) {
        Q_ASSERT(m_material_type == TypeFlat);
        m_fill_material.setColor(color);
        setMaterial(&m_fill_material); // Indicate that the material state has changed.
    }
}

void QSGDefaultRectangleNode::setPenColor(const QColor &color)
{
    if (color == m_border_material.color())
        return;
    m_border_material.setColor(color);
    border()->setMaterial(&m_border_material); // Indicate that the material state has changed.
}

void QSGDefaultRectangleNode::setPenWidth(qreal width)
{
    if (width == m_pen_width)
        return;
    m_pen_width = width;
    QSGNode *b = border();
    if (m_pen_width <= 0 && b->parent())
        removeChildNode(b);
    else if (m_pen_width > 0 && !b->parent())
        appendChildNode(b);
    m_dirty_geometry = true;
}


void QSGDefaultRectangleNode::setGradientStops(const QGradientStops &stops)
{
    if (stops.constData() == m_gradient_stops.constData())
        return;

    m_gradient_stops = stops;

    m_gradient_is_opaque = true;
    for (int i = 0; i < stops.size(); ++i)
        m_gradient_is_opaque &= stops.at(i).second.alpha() == 0xff;

    if (stops.isEmpty()) {
        // No gradient specified, use flat color.
        if (m_material_type != TypeFlat) {

            delete material();
            delete opaqueMaterial();
            setOpaqueMaterial(0);

            setMaterial(&m_fill_material);
            m_material_type = TypeFlat;

            setGeometry(&m_default_geometry);
            setFlag(OwnsGeometry, false);
        }
    } else {
        if (m_material_type == TypeFlat) {
            QSGVertexColorMaterial *material = new QSGVertexColorMaterial;
            setMaterial(material);
            m_material_type = TypeVertexGradient;
            QSGGeometry *g = new QSGGeometry(QSGGeometry::defaultAttributes_ColoredPoint2D(), 0);
            setGeometry(g);
            setFlag(OwnsGeometry);
        }
        static_cast<QSGVertexColorMaterial *>(material())->setOpaque(m_gradient_is_opaque);
    }

    m_dirty_geometry = true;
}

void QSGDefaultRectangleNode::setRadius(qreal radius)
{
    if (radius == m_radius)
        return;
    m_radius = radius;
    m_dirty_geometry = true;
}

void QSGDefaultRectangleNode::update()
{
    if (m_dirty_geometry) {
        updateGeometry();
        m_dirty_geometry = false;
    }
}

struct Color4ub
{
    unsigned char r, g, b, a;
};

Color4ub operator *(Color4ub c, float t) { c.a *= t; c.r *= t; c.g *= t; c.b *= t; return c; }
Color4ub operator +(Color4ub a, Color4ub b) {  a.a += b.a; a.r += b.r; a.g += b.g; a.b += b.b; return a; }

static inline Color4ub colorToColor4ub(const QColor &c)
{
    Color4ub color = { c.redF() * c.alphaF() * 255,
                       c.greenF() * c.alphaF() * 255,
                       c.blueF() * c.alphaF() * 255,
                       c.alphaF() * 255
                     };
    return color;
}

struct Vertex
{
    QVector2D position;
};

struct ColorVertex
{
    QVector2D position;
    Color4ub color;
};

void QSGDefaultRectangleNode::updateGeometry()
{
    // fast path for the simple case...
    if ((m_pen_width == 0 || m_border_material.color().alpha() == 0)
            && m_radius == 0
            && m_material_type == TypeFlat) {
        QSGGeometry::updateRectGeometry(&m_default_geometry, m_rect);
        return;
    }


    // ### This part down below is not optimal, using QVectors and reallocation for
    // every change, but its all going to be fixed up in rewrite...

    QSGGeometry *fill = geometry();

    // Check that the vertex type matches the material.
    Q_ASSERT(m_material_type != TypeFlat || fill->stride() == sizeof(Vertex));
    Q_ASSERT(m_material_type != TypeVertexGradient || fill->stride() == sizeof(ColorVertex));

    QSGGeometry *borderGeometry = 0;
    if (m_border) {
        borderGeometry = border()->geometry();
        Q_ASSERT(borderGeometry->stride() == sizeof(QSGGeometry::Point2D));
    }

    int fillVertexCount = 0;
    int borderVertexCount = 0;
    int borderIndexCount = 0;

    QVector<uchar> fillVertexData;
    QVector<Vertex> borderVertexData;
    QVector<ushort> borderIndexData;

    Color4ub fillColor = colorToColor4ub(m_fill_material.color());
    const QGradientStops &stops = m_gradient_stops;

    if (m_radius > 0) {
        // Rounded corners.

        // Radius should never exceeds half of the width or half of the height
        qreal radius = qMin(qMin(m_rect.width() / 2, m_rect.height() / 2), m_radius);
        QRectF innerRect = m_rect;
        innerRect.adjust(radius, radius, -radius, -radius);
        if (m_pen_width & 1) {
            // Pen width is odd, so add the offset as documented.
            innerRect.moveLeft(innerRect.left() + qreal(0.5));
            innerRect.moveTop(innerRect.top() + qreal(0.5));
        }

        qreal innerRadius = radius - m_pen_width * qreal(0.5);
        qreal outerRadius = radius + m_pen_width * qreal(0.5);

        int segments = qMin(30, qCeil(outerRadius)); // Number of segments per corner.

        /*

        --+-__
          | segment
          |       _+
        --+-__  _-   \
              -+  segment
        --------+      \        <- gradient line
                 +-----+
                 |     |

        */

        // Overestimate the number of vertices and indices, reduce afterwards when the actual numbers are known.
        if (m_pen_width) {
            // The reason I add extra vertices where the gradient lines intersect the border is
            // to avoid pixel sized gaps between the fill and the border caused by floating point
            // inaccuracies.
            borderVertexData.resize((segments + 1) * 2 * 4 + m_gradient_stops.size() * 2);
        }
        fillVertexData.resize(((segments + 1) * 4 + m_gradient_stops.size() * 2) * fill->stride());

        Vertex *borderVertices = borderVertexData.data();
        void *fillVertices = fillVertexData.data(); // Can be Vertex, ColorVertex or TextureVertex.


        int nextGradientStop = 0;
        qreal gradientPos = (radius - innerRadius) / (innerRect.height() + 2 * radius);
        while (nextGradientStop < stops.size() && stops.at(nextGradientStop).first <= gradientPos)
            ++nextGradientStop;

        qreal py = 0; // previous inner y-coordinate.
        qreal plx = 0; // previous inner left x-coordinate.
        qreal prx = 0; // previous inner right x-coordinate.

        for (int part = 0; part < 2; ++part) {
            for (int i = 0; i <= segments; ++i) {
                //### Should change to calculate sin/cos only once.
                qreal angle = qreal(0.5 * M_PI) * (part + i / qreal(segments));
                qreal s = qFastSin(angle);
                qreal c = qFastCos(angle);
                qreal y = (part ? innerRect.bottom() : innerRect.top()) - innerRadius * c; // current inner y-coordinate.
                qreal lx = innerRect.left() - innerRadius * s; // current inner left x-coordinate.
                qreal rx = innerRect.right() + innerRadius * s; // current inner right x-coordinate.
                qreal Y = (part ? innerRect.bottom() : innerRect.top()) - outerRadius * c; // current outer y-coordinate.
                qreal lX = innerRect.left() - outerRadius * s; // current outer left x-coordinate.
                qreal rX = innerRect.right() + outerRadius * s; // current outer right x-coordinate.

                gradientPos = ((part ? innerRect.height() : 0) + radius - innerRadius * c) / (innerRect.height() + 2 * radius);
                while (nextGradientStop < stops.size() && stops.at(nextGradientStop).first <= gradientPos) {
                    // Insert vertices at gradient stops.
                    qreal gy = (innerRect.top() - radius) + stops.at(nextGradientStop).first * (innerRect.height() + 2 * radius);
                    Q_ASSERT(fillVertexCount >= 2);
                    qreal t = (gy - py) / (y - py);
                    qreal glx = plx * (1 - t) + t * lx;
                    qreal grx = prx * (1 - t) + t * rx;

                    if (m_pen_width) {
                        borderVertices[borderVertexCount++].position = QVector2D(grx, gy);
                        borderVertices[borderVertexCount++].position = QVector2D(glx, gy);

                        int first = borderIndexData.first();
                        borderIndexData.prepend(borderVertexCount - 1);
                        borderIndexData.prepend(first);

                        int last = borderIndexData.at(borderIndexData.size() - 2);
                        borderIndexData.append(last);
                        borderIndexData.append(borderVertexCount - 2);
                    }

                    Q_ASSERT(QSGVertexColorMaterial::is(material()));
                    ColorVertex *vertices = (ColorVertex *)fillVertices;

                    fillColor = colorToColor4ub(stops.at(nextGradientStop).second);
                    vertices[fillVertexCount].position = QVector2D(grx, gy);
                    vertices[fillVertexCount].color = fillColor;
                    ++fillVertexCount;
                    vertices[fillVertexCount].position = QVector2D(glx, gy);
                    vertices[fillVertexCount].color = fillColor;
                    ++fillVertexCount;

                    ++nextGradientStop;
                }

                if (m_pen_width) {
                    borderVertices[borderVertexCount++].position = QVector2D(rX, Y);
                    borderVertices[borderVertexCount++].position = QVector2D(lX, Y);
                    borderVertices[borderVertexCount++].position = QVector2D(rx, y);
                    borderVertices[borderVertexCount++].position = QVector2D(lx, y);

                    borderIndexData.prepend(borderVertexCount - 1);
                    borderIndexData.prepend(borderVertexCount - 3);
                    borderIndexData.append(borderVertexCount - 4);
                    borderIndexData.append(borderVertexCount - 2);
                }

                if (stops.isEmpty()) {
                    Q_ASSERT(m_material_type == TypeFlat);
                    Vertex *vertices = (Vertex *)fillVertices;
                    vertices[fillVertexCount++].position = QVector2D(rx, y);
                    vertices[fillVertexCount++].position = QVector2D(lx, y);
                } else {
                    if (nextGradientStop == 0) {
                        fillColor = colorToColor4ub(stops.at(0).second);
                    } else if (nextGradientStop == stops.size()) {
                        fillColor = colorToColor4ub(stops.last().second);
                    } else {
                        const QGradientStop &prev = stops.at(nextGradientStop - 1);
                        const QGradientStop &next = stops.at(nextGradientStop);
                        qreal t = (gradientPos - prev.first) / (next.first - prev.first);
                        fillColor = (colorToColor4ub(prev.second) * (1 - t) + colorToColor4ub(next.second) * t);
                    }

                    ColorVertex *vertices = (ColorVertex *)fillVertices;
                    vertices[fillVertexCount].position = QVector2D(rx, y);
                    vertices[fillVertexCount].color = fillColor;
                    ++fillVertexCount;
                    vertices[fillVertexCount].position = QVector2D(lx, y);
                    vertices[fillVertexCount].color = fillColor;
                    ++fillVertexCount;
                }

                py = y;
                plx = lx;
                prx = rx;
            }
        }


        if (m_pen_width) {
            // Close border.
            ushort first = borderIndexData.at(0);
            ushort second = borderIndexData.at(1);
            borderIndexData.append(first);
            borderIndexData.append(second);

            borderIndexCount = borderIndexData.size();
        }

    } else {

        // Straight corners.
        QRectF innerRect = m_rect;
        QRectF outerRect = m_rect;

        qreal halfPenWidth = 0;
        if (m_pen_width) {
            if (m_pen_width & 1) {
                // Pen width is odd, so add the offset as documented.
                innerRect.moveLeft(innerRect.left() + qreal(0.5));
                innerRect.moveTop(innerRect.top() + qreal(0.5));
                outerRect = innerRect;
            }
            halfPenWidth = m_pen_width * qreal(0.5);
            innerRect.adjust(halfPenWidth, halfPenWidth, -halfPenWidth, -halfPenWidth);
            outerRect.adjust(-halfPenWidth, -halfPenWidth, halfPenWidth, halfPenWidth);
        }

        if (m_pen_width) {
            borderVertexData.resize((2 + stops.size()) * 2 + 4);
            borderIndexData.resize((2 + stops.size()) * 2 * 2 + 4);
        }
        fillVertexData.resize((2 + stops.size()) * 2 * fill->stride());

        void *fillVertices = fillVertexData.data();
        Vertex *borderVertices = (Vertex *) borderVertexData.data();
        ushort *borderIndices = borderIndexData.data();

        int nextGradientStop = 0;
        qreal gradientPos = halfPenWidth / m_rect.height();
        while (nextGradientStop < stops.size() && stops.at(nextGradientStop).first <= gradientPos)
            ++nextGradientStop;

        for (int part = 0; part < 2; ++part) {
            qreal y = (part ? innerRect.bottom() : innerRect.top());
            gradientPos = (y - innerRect.top() + halfPenWidth) / m_rect.height();

            while (nextGradientStop < stops.size() && stops.at(nextGradientStop).first <= gradientPos) {
                // Insert vertices at gradient stops.
                qreal gy = (innerRect.top() - halfPenWidth) + stops.at(nextGradientStop).first * m_rect.height();
                Q_ASSERT(fillVertexCount >= 2);

                Q_ASSERT(QSGVertexColorMaterial::is(material()));
                ColorVertex *vertices = (ColorVertex *)fillVertices;

                fillColor = colorToColor4ub(stops.at(nextGradientStop).second);
                vertices[fillVertexCount].position = QVector2D(innerRect.right(), gy);
                vertices[fillVertexCount].color = fillColor;
                ++fillVertexCount;
                vertices[fillVertexCount].position = QVector2D(innerRect.left(), gy);
                vertices[fillVertexCount].color = fillColor;
                ++fillVertexCount;

                if (m_pen_width) {
                    borderVertices[borderVertexCount++].position = QVector2D(innerRect.right(), gy);
                    borderVertices[borderVertexCount++].position = QVector2D(innerRect.left(), gy);
                }

                ++nextGradientStop;
            }

            if (stops.isEmpty()) {
                Q_ASSERT(m_material_type == TypeFlat);
                Vertex *vertices = (Vertex *)fillVertices;
                vertices[fillVertexCount++].position = QVector2D(innerRect.right(), y);
                vertices[fillVertexCount++].position = QVector2D(innerRect.left(), y);
            } else {
                if (nextGradientStop == 0) {
                    fillColor = colorToColor4ub(stops.at(0).second);
                } else if (nextGradientStop == stops.size()) {
                    fillColor = colorToColor4ub(stops.last().second);
                } else {
                    const QGradientStop &prev = stops.at(nextGradientStop - 1);
                    const QGradientStop &next = stops.at(nextGradientStop);
                    qreal t = (gradientPos - prev.first) / (next.first - prev.first);
                    fillColor = (colorToColor4ub(prev.second) * (1 - t) + colorToColor4ub(next.second) * t);
                }

                ColorVertex *vertices = (ColorVertex *)fillVertices;
                vertices[fillVertexCount].position = QVector2D(innerRect.right(), y);
                vertices[fillVertexCount].color = fillColor;
                ++fillVertexCount;
                vertices[fillVertexCount].position = QVector2D(innerRect.left(), y);
                vertices[fillVertexCount].color = fillColor;
                ++fillVertexCount;
            }

            if (m_pen_width) {
                borderVertices[borderVertexCount++].position = QVector2D(innerRect.right(), y);
                borderVertices[borderVertexCount++].position = QVector2D(innerRect.left(), y);
            }
        }

        if (m_pen_width) {
            // Add four corners.
            borderVertices[borderVertexCount++].position = QVector2D(outerRect.right(), outerRect.top());
            borderVertices[borderVertexCount++].position = QVector2D(outerRect.left(), outerRect.top());
            borderVertices[borderVertexCount++].position = QVector2D(outerRect.right(), outerRect.bottom());
            borderVertices[borderVertexCount++].position = QVector2D(outerRect.left(), outerRect.bottom());

            for (int i = 0; i < fillVertexCount / 2; ++i) {
                borderIndices[borderIndexCount++] = borderVertexCount - (i == 0 ? 4 : 2); // Upper or lower right corner.
                borderIndices[borderIndexCount++] = 2 * i + 0;
            }
            for (int i = 0; i < fillVertexCount / 2; ++i) {
                borderIndices[borderIndexCount++] = borderVertexCount - (i == 0 ? 1 : 3); // Lower or upper left corner.
                borderIndices[borderIndexCount++] = fillVertexCount - 2 * i - 1;
            }
            borderIndices[borderIndexCount++] = fillVertexCount; // Upper right corner.
            borderIndices[borderIndexCount++] = 0;
            Q_ASSERT(fillVertexCount + 4 == borderVertexCount);
        }
    }

    // Copy from temporary datastructures to geometry...
    if (m_pen_width) {
        borderGeometry->allocate(borderVertexCount, borderIndexCount);
        memcpy(borderGeometry->indexData(), borderIndexData.constData(), borderIndexCount * sizeof(quint16));
        memcpy(borderGeometry->vertexData(), borderVertexData.constData(), borderVertexCount * sizeof(Vertex));
        m_border->markDirty(DirtyGeometry);
    }

    fill->allocate(fillVertexCount);
    memcpy(fill->vertexData(), fillVertexData.constData(), fillVertexCount * fill->stride());

    markDirty(DirtyGeometry);
}


QT_END_NAMESPACE

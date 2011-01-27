
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



#include "default_rectanglenode.h"

#include "vertexcolormaterial.h"
#include "texturematerial.h"

#include "qsgcontext.h"

#include <QtCore/qmath.h>

DefaultRectangleNode::DefaultRectangleNode(MaterialPreference preference, QSGContext *context)
    : m_material_preference(preference)
    , m_gradient_texture(0)
    , m_gradient_is_opaque(true)
    , m_dirty_geometry(false)
    , m_dirty_gradienttexture(false)
    , m_context(context)
{
    m_border_material.setColor(m_pen_color);
    m_border_material.setOpacity(m_opacity);
    m_border.setFlag(OwnedByParent, false);
    m_border.setMaterial(&m_border_material);

    QVector<QSGAttributeDescription> desc = QVector<QSGAttributeDescription>()
        << QSGAttributeDescription(0, 2, GL_FLOAT, 2 * sizeof(float));
    m_border.updateGeometryDescription(desc, GL_UNSIGNED_SHORT);
    updateGeometryDescription(desc, GL_UNSIGNED_SHORT);

    // The scene-graph requires that there is a material and a geometry on the node.
    FlatColorMaterial *material = new FlatColorMaterial;
    material->setColor(m_color);
    material->setOpacity(m_opacity);
    setMaterial(m_fill_material = material);

#ifdef QML_RUNTIME_TESTING
    description = "rectangle";
#endif
}

DefaultRectangleNode::~DefaultRectangleNode()
{
    delete m_fill_material;
}

void DefaultRectangleNode::setRect(const QRectF &rect)
{
    if (rect == m_rect)
        return;
    m_rect = rect;
    QRectF bounds = calculateBoundingRect();
    setBoundingRect(bounds);
    m_border.setBoundingRect(bounds);
    m_dirty_geometry = true;
}

void DefaultRectangleNode::setColor(const QColor &color)
{
    if (color == m_color)
        return;
    m_color = color;
    if (m_gradient_stops.isEmpty()) {
        Q_ASSERT(FlatColorMaterial::is(m_fill_material));
        static_cast<FlatColorMaterial *>(m_fill_material)->setColor(color);
        setMaterial(m_fill_material); // Indicate that the material state has changed.
    }
}

void DefaultRectangleNode::setPenColor(const QColor &color)
{
    if (color == m_pen_color)
        return;
    m_pen_color = color;
    m_border_material.setColor(color);
    m_border.setMaterial(&m_border_material); // Indicate that the material state has changed.
}

void DefaultRectangleNode::setPenWidth(int width)
{
    if (width == m_pen_width)
        return;
    m_pen_width = width;

    if (m_pen_width <= 0 && m_border.parent())
        removeChildNode(&m_border);
    else if (m_pen_width > 0 && !m_border.parent())
        appendChildNode(&m_border);

    QRectF bounds = calculateBoundingRect();
    setBoundingRect(bounds);
    m_border.setBoundingRect(bounds);

    m_dirty_geometry = true;
}

void DefaultRectangleNode::setOpacity(qreal opacity)
{
    if (opacity == m_opacity)
        return;
    m_opacity = opacity;

    m_border_material.setOpacity(opacity);
    m_border.setMaterial(&m_border_material); // Indicate that the material state has changed.

    if (FlatColorMaterial::is(m_fill_material)) {
        static_cast<FlatColorMaterial *>(m_fill_material)->setOpacity(m_opacity);
    } else if (VertexColorMaterial::is(m_fill_material)) {
        static_cast<VertexColorMaterial *>(m_fill_material)->setOpacity(opacity);
    } else if (TextureMaterial::is(m_fill_material)) {
        if (opacity < 1) {
            delete m_fill_material;
            TextureMaterialWithOpacity *material = new TextureMaterialWithOpacity;
            if (!m_gradient_texture.isNull())
                material->setTexture(m_gradient_texture, m_gradient_is_opaque);
            material->setLinearFiltering(true);
            material->setOpacity(opacity);
            m_fill_material = material;
        }
    } else {
        Q_ASSERT(TextureMaterialWithOpacity::is(m_fill_material));
        if (opacity >= 1) {
            delete m_fill_material;
            TextureMaterial *material = new TextureMaterial;
            if (!m_gradient_texture.isNull())
                material->setTexture(m_gradient_texture, m_gradient_is_opaque);
            material->setLinearFiltering(true);
            m_fill_material = material;
        } else {
            static_cast<TextureMaterialWithOpacity *>(m_fill_material)->setOpacity(opacity);
        }
    }
    setMaterial(m_fill_material);
}

void DefaultRectangleNode::setGradientStops(const QGradientStops &stops)
{
    if (stops.constData() == m_gradient_stops.constData())
        return;

    m_gradient_stops = stops;

    m_gradient_is_opaque = true;
    for (int i = 0; i < stops.size(); ++i)
        m_gradient_is_opaque &= stops.at(i).second.alpha() == 0xff;

    if (stops.isEmpty()) {
        // No gradient specified, use flat color.
        if (!FlatColorMaterial::is(m_fill_material)) {
            delete m_fill_material;
            FlatColorMaterial *material = new FlatColorMaterial;
            material->setColor(m_color);
            material->setOpacity(m_opacity);
            m_fill_material = material;
            QVector<QSGAttributeDescription> desc = QVector<QSGAttributeDescription>()
                << QSGAttributeDescription(0, 2, GL_FLOAT, 2 * sizeof(float));
            updateGeometryDescription(desc, GL_UNSIGNED_SHORT);
        }
    } else if (m_material_preference == PreferTextureMaterial) {
        if (FlatColorMaterial::is(m_fill_material)) {
            delete m_fill_material;
            if (m_opacity < 1) {
                TextureMaterialWithOpacity *material = new TextureMaterialWithOpacity;
                material->setLinearFiltering(true);
                material->setOpacity(m_opacity);
                m_fill_material = material;
            } else {
                TextureMaterial *material = new TextureMaterial;
                material->setLinearFiltering(true);
                m_fill_material = material;
            }
            QVector<QSGAttributeDescription> desc = QVector<QSGAttributeDescription>()
                << QSGAttributeDescription(0, 2, GL_FLOAT, 4 * sizeof(float))
                << QSGAttributeDescription(1, 2, GL_FLOAT, 4 * sizeof(float));
            updateGeometryDescription(desc, GL_UNSIGNED_SHORT);
        }
        m_dirty_gradienttexture = true;
    } else {
        Q_ASSERT(m_material_preference == PreferVertexColorMaterial);
        if (!VertexColorMaterial::is(m_fill_material)) {
            delete m_fill_material;
            VertexColorMaterial *material = new VertexColorMaterial;
            material->setOpacity(m_opacity);
            m_fill_material = material;
            QVector<QSGAttributeDescription> desc = QVector<QSGAttributeDescription>()
                << QSGAttributeDescription(0, 2, GL_FLOAT, 6 * sizeof(float))
                << QSGAttributeDescription(1, 4, GL_FLOAT, 6 * sizeof(float));
            updateGeometryDescription(desc, GL_UNSIGNED_SHORT);
        }

        Q_ASSERT(VertexColorMaterial::is(m_fill_material));
        static_cast<VertexColorMaterial *>(m_fill_material)->setOpaque(m_gradient_is_opaque);
    }

    setMaterial(m_fill_material);
    m_dirty_geometry = true;
}

void DefaultRectangleNode::setRadius(qreal radius)
{
    if (radius == m_radius)
        return;
    m_radius = radius;
    m_dirty_geometry = true;
}

void DefaultRectangleNode::update()
{
    if (m_dirty_gradienttexture) {
        updateGradientTexture();
        m_dirty_gradienttexture = false;
    }

    if (m_dirty_geometry) {
        updateGeometry();
        m_dirty_geometry = false;
    }
}

static inline QVector4D colorToVector(const QColor &c)
{
    return QVector4D(c.redF() * c.alphaF(), c.greenF() * c.alphaF(), c.blueF() * c.alphaF(), c.alphaF());
}

void DefaultRectangleNode::updateGeometry()
{
    struct Vertex
    {
        QVector2D position;
    };

    struct ColorVertex
    {
        QVector2D position;
        QVector4D color;
    };

    struct TextureVertex
    {
        QVector2D position;
        QVector2D texCoord;
    };

    Geometry *fill = geometry();
    Geometry *border = m_border.geometry();

    // Check that the vertex type matches the material.
    Q_ASSERT((fill->stride() == sizeof(Vertex)) == FlatColorMaterial::is(m_fill_material));
    Q_ASSERT((fill->stride() == sizeof(ColorVertex)) == VertexColorMaterial::is(m_fill_material));
    Q_ASSERT((fill->stride() == sizeof(TextureVertex)) == (TextureMaterial::is(m_fill_material) || TextureMaterialWithOpacity::is(m_fill_material)));
    Q_ASSERT(border->stride() == sizeof(Vertex));

    int fillIndexCount = 0;
    int fillVertexCount = 0;
    int borderVertexCount = 0;

    QVector4D fillColor = colorToVector(m_color);
    const QGradientStops &stops = m_gradient_stops;

    // Calculate from where in the texture to sample gradient colours.
    qreal gradientSourceX0 = 0, gradientSourceDX = 0, gradientSourceY = 0;
    if (m_gradient_texture.isReady()) {
        QRectF src = m_gradient_texture.subRect();
        gradientSourceY = qreal(0.5) * (src.top() + src.bottom());
        gradientSourceDX = src.width() / stops.size();
        gradientSourceX0 = src.left() + qreal(0.5) * gradientSourceDX;
    }

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
            border->setVertexCount((segments + 1) * 2 * 4 + m_gradient_stops.size() * 2);
            border->setIndexCount((segments + 1) * 2 * 4 + m_gradient_stops.size() * 2 * 2 + 2); // +2 to close the border.
        }
        fill->setVertexCount((segments + 1) * 4 + m_gradient_stops.size() * 2);
        fill->setIndexCount((segments + 1) * 4 + m_gradient_stops.size() * 2);

        Vertex *borderVertices = (Vertex *)border->vertexData();
        void *fillVertices = fill->vertexData(); // Can be Vertex, ColorVertex or TextureVertex.

        QList<ushort> borderIndices;

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

                        int first = borderIndices.first();
                        borderIndices.prepend(borderVertexCount - 1);
                        borderIndices.prepend(first);

                        int last = borderIndices.at(borderIndices.size() - 2);
                        borderIndices.append(last);
                        borderIndices.append(borderVertexCount - 2);
                    }

                    if (m_material_preference == PreferVertexColorMaterial) {
                        Q_ASSERT(VertexColorMaterial::is(m_fill_material));
                        ColorVertex *vertices = (ColorVertex *)fillVertices;

                        fillColor = colorToVector(stops.at(nextGradientStop).second);
                        vertices[fillVertexCount].position = QVector2D(grx, gy);
                        vertices[fillVertexCount].color = fillColor;
                        ++fillVertexCount;
                        vertices[fillVertexCount].position = QVector2D(glx, gy);
                        vertices[fillVertexCount].color = fillColor;
                        ++fillVertexCount;
                    } else {
                        Q_ASSERT(m_material_preference == PreferTextureMaterial);
                        Q_ASSERT(TextureMaterial::is(m_fill_material) || TextureMaterialWithOpacity::is(m_fill_material));

                        TextureVertex *vertices = (TextureVertex *)fillVertices;
                        QVector2D texCoord(gradientSourceX0 + gradientSourceDX * nextGradientStop, gradientSourceY);

                        vertices[fillVertexCount].position = QVector2D(grx, gy);
                        vertices[fillVertexCount].texCoord = texCoord;
                        ++fillVertexCount;
                        vertices[fillVertexCount].position = QVector2D(glx, gy);
                        vertices[fillVertexCount].texCoord = texCoord;
                        ++fillVertexCount;
                    }

                    ++nextGradientStop;
                }

                if (m_pen_width) {
                    borderVertices[borderVertexCount++].position = QVector2D(rX, Y);
                    borderVertices[borderVertexCount++].position = QVector2D(lX, Y);
                    borderVertices[borderVertexCount++].position = QVector2D(rx, y);
                    borderVertices[borderVertexCount++].position = QVector2D(lx, y);

                    borderIndices.prepend(borderVertexCount - 1);
                    borderIndices.prepend(borderVertexCount - 3);
                    borderIndices.append(borderVertexCount - 4);
                    borderIndices.append(borderVertexCount - 2);
                }

                if (stops.isEmpty()) {
                    Q_ASSERT(FlatColorMaterial::is(m_fill_material));
                    Vertex *vertices = (Vertex *)fillVertices;
                    vertices[fillVertexCount++].position = QVector2D(rx, y);
                    vertices[fillVertexCount++].position = QVector2D(lx, y);
                } else if (m_material_preference == PreferVertexColorMaterial) {
                    Q_ASSERT(VertexColorMaterial::is(m_fill_material));

                    if (nextGradientStop == 0) {
                        fillColor = colorToVector(stops.at(0).second);
                    } else if (nextGradientStop == stops.size()) {
                        fillColor = colorToVector(stops.last().second);
                    } else {
                        const QGradientStop &prev = stops.at(nextGradientStop - 1);
                        const QGradientStop &next = stops.at(nextGradientStop);
                        qreal t = (gradientPos - prev.first) / (next.first - prev.first);
                        fillColor = (colorToVector(prev.second) * (1 - t) + colorToVector(next.second) * t);
                    }

                    ColorVertex *vertices = (ColorVertex *)fillVertices;
                    vertices[fillVertexCount].position = QVector2D(rx, y);
                    vertices[fillVertexCount].color = fillColor;
                    ++fillVertexCount;
                    vertices[fillVertexCount].position = QVector2D(lx, y);
                    vertices[fillVertexCount].color = fillColor;
                    ++fillVertexCount;
                } else {
                    Q_ASSERT(m_material_preference == PreferTextureMaterial);
                    Q_ASSERT(TextureMaterial::is(m_fill_material) || TextureMaterialWithOpacity::is(m_fill_material));

                    qreal x = gradientSourceX0;
                    if (nextGradientStop == 0) {
                        // x already has the correct value.
                    } else if (nextGradientStop == stops.size()) {
                        x += gradientSourceDX * (nextGradientStop - 1);
                    } else {
                        const QGradientStop &prev = stops.at(nextGradientStop - 1);
                        const QGradientStop &next = stops.at(nextGradientStop);
                        qreal t = (gradientPos - prev.first) / (next.first - prev.first);
                        x += gradientSourceDX * (nextGradientStop - 1 + t);
                    }

                    TextureVertex *vertices = (TextureVertex *)fillVertices;
                    QVector2D texCoord(x, gradientSourceY);
                    vertices[fillVertexCount].position = QVector2D(rx, y);
                    vertices[fillVertexCount].texCoord = texCoord;
                    ++fillVertexCount;
                    vertices[fillVertexCount].position = QVector2D(lx, y);
                    vertices[fillVertexCount].texCoord = texCoord;
                    ++fillVertexCount;
                }

                py = y;
                plx = lx;
                prx = rx;
            }
        }

        if (m_pen_width) {
            // Close border.
            ushort first = borderIndices.at(0);
            ushort second = borderIndices.at(1);
            borderIndices.append(first);
            borderIndices.append(second);

            Q_ASSERT(borderIndices.size() <= border->indexCount());
            ushort *indices = border->ushortIndexData();
            for (int i = 0; i < borderIndices.size(); ++i)
                indices[i] = borderIndices.at(i);
            border->setIndexCount(borderIndices.size());
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
            border->setVertexCount((2 + stops.size()) * 2 + 4);
            border->setIndexCount((2 + stops.size()) * 2 * 2 + 4);
        }
        fill->setVertexCount((2 + stops.size()) * 2);

        void *fillVertices = fill->vertexData();
        Vertex *borderVertices = (Vertex *)border->vertexData();
        ushort *borderIndices = border->ushortIndexData();
        int borderIndexCount = 0;

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

                if (m_material_preference == PreferVertexColorMaterial) {
                    Q_ASSERT(VertexColorMaterial::is(m_fill_material));
                    ColorVertex *vertices = (ColorVertex *)fillVertices;

                    fillColor = colorToVector(stops.at(nextGradientStop).second);
                    vertices[fillVertexCount].position = QVector2D(innerRect.right(), gy);
                    vertices[fillVertexCount].color = fillColor;
                    ++fillVertexCount;
                    vertices[fillVertexCount].position = QVector2D(innerRect.left(), gy);
                    vertices[fillVertexCount].color = fillColor;
                    ++fillVertexCount;
                } else {
                    Q_ASSERT(m_material_preference == PreferTextureMaterial);
                    Q_ASSERT(TextureMaterial::is(m_fill_material) || TextureMaterialWithOpacity::is(m_fill_material));

                    TextureVertex *vertices = (TextureVertex *)fillVertices;
                    QVector2D texCoord(gradientSourceX0 + gradientSourceDX * nextGradientStop, gradientSourceY);

                    vertices[fillVertexCount].position = QVector2D(innerRect.right(), gy);
                    vertices[fillVertexCount].texCoord = texCoord;
                    ++fillVertexCount;
                    vertices[fillVertexCount].position = QVector2D(innerRect.left(), gy);
                    vertices[fillVertexCount].texCoord = texCoord;
                    ++fillVertexCount;
                }

                if (m_pen_width) {
                    borderVertices[borderVertexCount++].position = QVector2D(innerRect.right(), gy);
                    borderVertices[borderVertexCount++].position = QVector2D(innerRect.left(), gy);
                }

                ++nextGradientStop;
            }

            if (stops.isEmpty()) {
                Q_ASSERT(FlatColorMaterial::is(m_fill_material));
                Vertex *vertices = (Vertex *)fillVertices;
                vertices[fillVertexCount++].position = QVector2D(innerRect.right(), y);
                vertices[fillVertexCount++].position = QVector2D(innerRect.left(), y);
            } else if (m_material_preference == PreferVertexColorMaterial) {
                Q_ASSERT(VertexColorMaterial::is(m_fill_material));

                if (nextGradientStop == 0) {
                    fillColor = colorToVector(stops.at(0).second);
                } else if (nextGradientStop == stops.size()) {
                    fillColor = colorToVector(stops.last().second);
                } else {
                    const QGradientStop &prev = stops.at(nextGradientStop - 1);
                    const QGradientStop &next = stops.at(nextGradientStop);
                    qreal t = (gradientPos - prev.first) / (next.first - prev.first);
                    fillColor = (colorToVector(prev.second) * (1 - t) + colorToVector(next.second) * t);
                }

                ColorVertex *vertices = (ColorVertex *)fillVertices;
                vertices[fillVertexCount].position = QVector2D(innerRect.right(), y);
                vertices[fillVertexCount].color = fillColor;
                ++fillVertexCount;
                vertices[fillVertexCount].position = QVector2D(innerRect.left(), y);
                vertices[fillVertexCount].color = fillColor;
                ++fillVertexCount;
            } else {
                Q_ASSERT(m_material_preference == PreferTextureMaterial);
                Q_ASSERT(TextureMaterial::is(m_fill_material) || TextureMaterialWithOpacity::is(m_fill_material));

                qreal x = gradientSourceX0;
                if (nextGradientStop == 0) {
                    // x already has the correct value.
                } else if (nextGradientStop == stops.size()) {
                    x += gradientSourceDX * (nextGradientStop - 1);
                } else {
                    const QGradientStop &prev = stops.at(nextGradientStop - 1);
                    const QGradientStop &next = stops.at(nextGradientStop);
                    qreal t = (gradientPos - prev.first) / (next.first - prev.first);
                    x += gradientSourceDX * (nextGradientStop - 1 + t);
                }

                TextureVertex *vertices = (TextureVertex *)fillVertices;
                QVector2D texCoord(x, gradientSourceY);
                vertices[fillVertexCount].position = QVector2D(innerRect.right(), y);
                vertices[fillVertexCount].texCoord = texCoord;
                ++fillVertexCount;
                vertices[fillVertexCount].position = QVector2D(innerRect.left(), y);
                vertices[fillVertexCount].texCoord = texCoord;
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
            Q_ASSERT(borderIndexCount < border->indexCount());
            border->setIndexCount(borderIndexCount);
        }
    }

    Q_ASSERT(borderVertexCount <= border->vertexCount());
    Q_ASSERT(fillVertexCount <= fill->vertexCount());
    Q_ASSERT(fillIndexCount <= fill->indexCount());

    fill->setDrawingMode(QSG::TriangleStrip);
    fill->setVertexCount(fillVertexCount);
    fill->setIndexCount(fillIndexCount);

    border->setDrawingMode(QSG::TriangleStrip);
    border->setVertexCount(borderVertexCount);

    markDirty(DirtyGeometry);
    m_border.markDirty(DirtyGeometry);
}

void DefaultRectangleNode::updateGradientTexture()
{
    if (m_gradient_stops.isEmpty())
        return;

    QImage image(m_gradient_stops.size(), 1, QImage::Format_ARGB32_Premultiplied);
    QRgb *line = (QRgb *)image.scanLine(0);

    for (int i = 0; i < m_gradient_stops.size(); ++i) {
        const QColor &c = m_gradient_stops.at(i).second;
        line[i] = QColor::fromRgbF(c.redF() * c.alphaF(), c.greenF() * c.alphaF(), c.blueF() * c.alphaF(), c.alphaF()).rgba();
    }

    m_gradient_texture = m_context->textureManager()->upload(image);

    Q_ASSERT(TextureMaterial::is(m_fill_material) || TextureMaterialWithOpacity::is(m_fill_material));

    // TextureMaterial and TextureMaterialWithOpacity have different (non-virtual) setTexture() implementations.
    if (TextureMaterial::is(m_fill_material))
        static_cast<TextureMaterial *>(m_fill_material)->setTexture(m_gradient_texture, m_gradient_is_opaque);
    else
        static_cast<TextureMaterialWithOpacity *>(m_fill_material)->setTexture(m_gradient_texture, m_gradient_is_opaque);
}

QRectF DefaultRectangleNode::calculateBoundingRect()
{
    if (m_pen_width == 0)
        return m_rect;

    QRectF b = m_rect;
    if (m_pen_width & 1) {
        // Pen width is odd, so add the offset as documented.
        b.moveLeft(b.left() + qreal(0.5));
        b.moveTop(b.top() + qreal(0.5));
    }
    qreal halfPenWidth = m_pen_width * qreal(0.5);
    b.adjust(-halfPenWidth, -halfPenWidth, halfPenWidth, halfPenWidth);
    return b;
}

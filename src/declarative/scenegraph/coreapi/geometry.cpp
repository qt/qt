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

#include "geometry.h"

#include "qsgattributevalue.h"
#include <QApplication>

Geometry::Geometry()
    : m_mode(QSG::Triangles)
    , m_vertex_stride(0)
    , m_index_stride(0)
{
    setIndexType(GL_UNSIGNED_SHORT);
    GeometryDataUploader::registerGeometry(this);
}

Geometry::Geometry(const QVector<QSGAttributeDescription> &description, GLenum indexType)
    : m_mode(QSG::Triangles)
{
    setIndexType(indexType);
    setVertexDescription(description);
    GeometryDataUploader::registerGeometry(this);
}

Geometry::~Geometry()
{
    GeometryDataUploader::unregisterGeometry(this);
}

void *Geometry::vertexData()
{
    GeometryDataUploader::markVertexDirty();
    return m_vertex_data.data();
}

void Geometry::setVertexDescription(const QVector<QSGAttributeDescription> &description)
{
    m_vertex_data.clear();
    m_vertex_stride = 0;
    m_vertex_description = description;
    for (int i = 0; i < description.size(); ++i)
        m_vertex_stride += description.at(i).tupleSize() * description.at(i).sizeOfType();
}

void *Geometry::indexData()
{
    GeometryDataUploader::markIndexDirty();
    return m_index_data.data();
}

void Geometry::setIndexType(GLenum indexType)
{
    Q_ASSERT(indexType == GL_UNSIGNED_SHORT || indexType == GL_UNSIGNED_INT);
    m_index_data.clear();
    m_index_type = indexType;
    m_index_stride = (m_index_type == GL_UNSIGNED_SHORT ? sizeof(ushort) : sizeof(uint));
}

ushort *Geometry::ushortIndexData()
{
    if (m_index_type == GL_UNSIGNED_SHORT) {
        GeometryDataUploader::markIndexDirty();
        return reinterpret_cast<ushort *>(indexData());
    }
    return 0;
}

uint *Geometry::uintIndexData()
{
    if (m_index_type == GL_UNSIGNED_INT) {
        GeometryDataUploader::markIndexDirty();
        return reinterpret_cast<uint *>(indexData());
    }
    return 0;
}

const ushort *Geometry::constUshortIndexData() const
{
    if (m_index_type == GL_UNSIGNED_SHORT)
        return reinterpret_cast<const ushort *>(constIndexData());
    return 0;
}

const uint *Geometry::constUintIndexData() const
{
    if (m_index_type == GL_UNSIGNED_INT)
        return reinterpret_cast<const uint *>(constIndexData());
    return 0;
}

QSGAttributeValue Geometry::attributeValue(int attribute) const
{
    int offset = 0;
    for (int i = 0; i < m_vertex_description.size(); ++i) {
        const QSGAttributeDescription &desc = m_vertex_description.at(i);
        if (desc.attribute() == attribute) {
            return QSGAttributeValue(desc.tupleSize(), desc.type(), m_vertex_stride,
                                     m_vertex_data.constData() + offset, vertexCount());
        }
        offset += desc.tupleSize() * desc.sizeOfType();
    }
    return QSGAttributeValue();
}


// Copy bigger memory blocks at once
static inline void arraycpy(QSGArray<uchar> &dest, const QSGArray<uchar> &src)
{
    int extendSize = src.size();
    int size = dest.size();
    dest.extend(extendSize);
    memcpy(dest.data() + size, src.data(), extendSize * sizeof(uchar));
}

bool GeometryDataUploader::m_use_buffers = true;
QSet<const Geometry *> GeometryDataUploader::m_geometries;
QGLBuffer GeometryDataUploader::m_vertex_buffer(QGLBuffer::VertexBuffer);
QGLBuffer GeometryDataUploader::m_index_buffer(QGLBuffer::IndexBuffer);
QSGArray<uchar> GeometryDataUploader::m_vertex_data;
QSGArray<uchar> GeometryDataUploader::m_index_data;
QHash<const Geometry *, int> GeometryDataUploader::m_vertex_offsets;
QHash<const Geometry *, int> GeometryDataUploader::m_index_offsets;
bool GeometryDataUploader::m_vertex_bound = false;
bool GeometryDataUploader::m_index_bound = false;
bool GeometryDataUploader::m_vertex_dirty = false;
bool GeometryDataUploader::m_index_dirty = false;

void GeometryDataUploader::registerGeometry(const Geometry *g)
{
    if (!m_use_buffers)
        return;

    m_geometries.insert(g);
}

void GeometryDataUploader::unregisterGeometry(const Geometry *g)
{
    if (!m_use_buffers)
        return;

    m_geometries.remove(g);
}

void GeometryDataUploader::addGeometryVertex(const Geometry *g)
{
    if (!m_use_buffers || g->vertexCount() == 0)
        return;

    const QSGArray<uchar> &vertexData = g->vertexArray();
    m_vertex_offsets.insert(g, m_vertex_data.count());
    arraycpy(m_vertex_data, vertexData);
}

void GeometryDataUploader::addGeometryIndex(const Geometry *g)
{
    if (!m_use_buffers || g->indexCount() == 0)
        return;

    if (g->indexCount()) {
        const QSGArray<uchar> &indexData = g->indexArray();
        m_index_offsets.insert(g, m_index_data.count());
        arraycpy(m_index_data, indexData);
    }
}

void GeometryDataUploader::bind()
{
    if (!m_use_buffers)
        return;

    if (!m_vertex_buffer.isCreated())
        m_vertex_buffer.create();
    if (!m_index_buffer.isCreated())
        m_index_buffer.create();

    if (!m_vertex_bound)
        m_vertex_bound = m_vertex_buffer.bind();
    if (!m_index_bound)
        m_index_bound = m_index_buffer.bind();
}

void GeometryDataUploader::release()
{
    if (!m_use_buffers)
        return;

    if (m_vertex_bound)
        m_vertex_buffer.release();
    if (m_index_bound)
        m_index_buffer.release();
    m_vertex_bound = false;
    m_index_bound = false;
}

void GeometryDataUploader::upload()
{
    if (!m_use_buffers || (!m_vertex_dirty && !m_index_dirty))
        return;

    bind();

    if (m_vertex_dirty)
        clearVertexData();
    if (m_index_dirty)
        clearIndexData();

    QSet<const Geometry *>::const_iterator i;
    for (i = m_geometries.begin(); i != m_geometries.end(); ++i) {
        if (m_vertex_dirty)
            addGeometryVertex(*i);
        if (m_index_dirty)
            addGeometryIndex(*i);
    }

    if (!m_vertex_data.isEmpty() && m_vertex_dirty)
        m_vertex_buffer.allocate(m_vertex_data.data(), m_vertex_data.size());
    if (!m_index_data.isEmpty() && m_index_dirty)
        m_index_buffer.allocate(m_index_data.data(), m_index_data.size());

    m_vertex_dirty = false;
    m_index_dirty = false;
}

const void *GeometryDataUploader::vertexData(const Geometry *g, int offset)
{
    if (m_use_buffers)
        return reinterpret_cast<const void *>(m_vertex_offsets.value(g) + offset);
    else
        return reinterpret_cast<const void *>(g->vertexArray().constData() + offset);
}

const void *GeometryDataUploader::indexData(const Geometry *g)
{
    if (m_use_buffers)
        return reinterpret_cast<const void *>(m_index_offsets.value(g));
    else
        return g->constIndexData();
}


/*!
    Creates a geometry object with coordinates from \a rect.

    The returned geometry is a triangle strip of 4 vertices with 2
    floats for each vertex for attribute position 0.

    \sa updateRectGeometry
 */
Geometry *GeometryHelper::createRectGeometry(const QRectF &rect)
{
    QVector<QSGAttributeDescription> desc;
    desc << QSGAttributeDescription(0, 2, GL_FLOAT, 2 * sizeof(float));
    Geometry *g = new Geometry(desc);
    g->setDrawingMode(QSG::TriangleStrip);
    g->setVertexCount(4);

    updateRectGeometry(g, rect);

    return g;
}

/*!
    Updates the rectangle geometry \a g with the coordinates in \a rect.

    The function assumes the geometry object contains a single triangle strip
    of four vertices of 2 floats each.

    \sa createRectGeometry
 */
void GeometryHelper::updateRectGeometry(Geometry *g, const QRectF &rect)
{
    Q_ASSERT(!g->isNull());
    Q_ASSERT(g->vertexCount() == 4);
    Q_ASSERT(g->stride() == 2 * sizeof(float));
    Q_ASSERT(!g->attributeValue(0).isNull());

    float *v = (float *) g->vertexData();
    v[0] = rect.left();
    v[1] = rect.top();

    v[2] = rect.right();
    v[3] = rect.top();

    v[4] = rect.left();
    v[5] = rect.bottom();

    v[6] = rect.right();
    v[7] = rect.bottom();
}

/*!
    Creates a geometry object with coordinates from \a rect and texture coordinates
    from \a source.

    \a textureRect should be in normalized coordinates.

    The returned geometry is a triangle strip of four vertices with four floats for
    each vertex, two floats for x and y for attribute position 0 and two floats
    for tx and ty for attribute position 1.

    \sa updateTexturedRectGeometry
 */
Geometry *GeometryHelper::createTexturedRectGeometry(const QRectF &rect, const QRectF &textureRect)
{
    QVector<QSGAttributeDescription> desc;
    desc << QSGAttributeDescription(0, 2, GL_FLOAT, 2 * sizeof(float));
    desc << QSGAttributeDescription(1, 2, GL_FLOAT, 2 * sizeof(float));

    Geometry *g = new Geometry(desc);
    g->setDrawingMode(QSG::TriangleStrip);
    g->setVertexCount(4);

    updateTexturedRectGeometry(g, rect, textureRect);

    return g;
}

/*!
    Updates the rectangle geometry \a g with the coordinates in \a rect and texture
    coordinates from \a textureRect.

    \a textureRect should be in normalized coordinates.

    \a g is assumed to be a triangle strip of four vertices with four floats for
    each vertex, two floats for x and y for attribute position 0 and two floats
    for tx and ty for attribute position 1.

    \sa createTexturedRectGeometry
 */
void GeometryHelper::updateTexturedRectGeometry(Geometry *g, const QRectF &rect, const QRectF &textureRect)
{
    Q_ASSERT(!g->isNull());
    Q_ASSERT(g->vertexCount() == 4);
    Q_ASSERT(g->stride() == 4 * sizeof(float));
    Q_ASSERT(!g->attributeValue(0).isNull());
    Q_ASSERT(!g->attributeValue(1).isNull());

    float *v = (float *) g->vertexData();
    v[0] = rect.left();
    v[1] = rect.top();
    v[2] = textureRect.left();
    v[3] = textureRect.top();

    v[4] = rect.right();
    v[5] = rect.top();
    v[6] = textureRect.right();
    v[7] = textureRect.top();

    v[8] = rect.left();
    v[9] = rect.bottom();
    v[10] = textureRect.left();
    v[11] = textureRect.bottom();

    v[12] = rect.right();
    v[13] = rect.bottom();
    v[14] = textureRect.right();
    v[15] = textureRect.bottom();
}

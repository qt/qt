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
#include "utilities.h"

#include "qglattributevalue.h"
#include <QApplication>

Geometry::Geometry()
    : m_mode(QGL::Triangles)
    , m_vertex_stride(0)
    , m_index_stride(0)
{
    setIndexType(GL_UNSIGNED_SHORT);
    GeometryDataUploader::registerGeometry(this);
}

Geometry::Geometry(const QVector<QGLAttributeDescription> &description, GLenum indexType)
    : m_mode(QGL::Triangles)
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

void Geometry::setVertexDescription(const QVector<QGLAttributeDescription> &description)
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

QGLAttributeValue Geometry::attributeValue(QGL::VertexAttribute attribute) const
{
    int offset = 0;
    for (int i = 0; i < m_vertex_description.size(); ++i) {
        const QGLAttributeDescription &desc = m_vertex_description.at(i);
        if (desc.attribute() == attribute) {
            return QGLAttributeValue(desc.tupleSize(), desc.type(), m_vertex_stride,
                                     m_vertex_data.constData() + offset, vertexCount());
        }
        offset += desc.tupleSize() * desc.sizeOfType();
    }
    return QGLAttributeValue();
}


// Copy bigger memory blocks at once
static inline void arraycpy(QArray<uchar> &dest, const QArray<uchar> &src)
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
QArray<uchar> GeometryDataUploader::m_vertex_data;
QArray<uchar> GeometryDataUploader::m_index_data;
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

    const QArray<uchar> &vertexData = g->vertexArray();
    m_vertex_offsets.insert(g, m_vertex_data.count());
    arraycpy(m_vertex_data, vertexData);
}

void GeometryDataUploader::addGeometryIndex(const Geometry *g)
{
    if (!m_use_buffers || g->indexCount() == 0)
        return;

    if (g->indexCount()) {
        const QArray<uchar> &indexData = g->indexArray();
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

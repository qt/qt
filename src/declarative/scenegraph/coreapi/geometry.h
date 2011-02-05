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

#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <qsgattributedescription.h>
#include <qsgarray.h>
#include <QtCore/qvector.h>
#include <qglfunctions.h>
#include <QGLBuffer>

class QSGAttributeValue;

class Q_DECLARATIVE_EXPORT Geometry
{
public:
    Geometry(); // Creates a null geometry.
    Geometry(const QVector<QSGAttributeDescription> &description, GLenum indexType = GL_UNSIGNED_SHORT);
    ~Geometry();

    void *vertexData();
    const void *constVertexData() const { return m_vertex_data.constData(); }
    const QSGArray<uchar> &vertexArray() const { return m_vertex_data; }

    void setVertexDescription(const QVector<QSGAttributeDescription> &description);
    const QVector<QSGAttributeDescription> &vertexDescription() const { return m_vertex_description; }

    int vertexCount() const { return m_vertex_data.size() / m_vertex_stride; }
    void setVertexCount(int count) {
        int currSize = m_vertex_data.size();
        int newSize = count * m_vertex_stride;
        if (newSize > currSize)
            m_vertex_data.extend(newSize - currSize); // Do not initialize newly allocated memory
        else if (newSize < currSize)
            m_vertex_data.resize(newSize);
    }

    void *indexData();
    ushort *ushortIndexData();
    uint *uintIndexData();
    const void *constIndexData() const { return m_index_data.constData(); }
    const ushort *constUshortIndexData() const;
    const uint *constUintIndexData() const;
    const QSGArray<uchar> &indexArray() const { return m_index_data; }

    void setIndexType(GLenum indexType);
    GLenum indexType() const { return m_index_type; }

    int indexCount() const { return m_index_data.size() / m_index_stride; }
    void setIndexCount(int count) {
        int currSize = m_index_data.size();
        int newSize = count * m_index_stride;
        if (newSize > currSize)
            m_index_data.extend(newSize - currSize); // Do not initialize newly allocated memory
        else if (newSize < currSize)
            m_index_data.resize(newSize);
    }

    QSG::DrawingMode drawingMode() const { return m_mode; }
    void setDrawingMode(QSG::DrawingMode mode) { m_mode = mode; }

    int stride() const { return m_vertex_stride; }

    QSGAttributeValue attributeValue(int attribute) const;

    bool isNull() const { return m_vertex_description.isEmpty(); }

protected:
    QSGArray<uchar> m_vertex_data;
    QSGArray<uchar> m_index_data;

    QVector<QSGAttributeDescription> m_vertex_description; // QSGAttributeDescription should have offset and normalized, must assume tightly packed data for now.
    GLenum m_index_type;
    QSG::DrawingMode m_mode;
    int m_vertex_stride;
    int m_index_stride;
};


class Q_DECLARATIVE_EXPORT GeometryHelper
{
public:
    static Geometry *createRectGeometry(const QRectF &rect);
    static void updateRectGeometry(Geometry *g, const QRectF &rect);

    static Geometry *createTexturedRectGeometry(const QRectF &rect, const QRectF &sourceRect);
    static void updateTexturedRectGeometry(Geometry *g, const QRectF &rect, const QRectF &sourceRect);
};


class GeometryDataUploader
{
public:
    static void registerGeometry(const Geometry *);
    static void unregisterGeometry(const Geometry *);

    static void bind();
    static void release();
    static void upload();

    static const void *vertexData(const Geometry *g, int offset = 0);
    static const void *indexData(const Geometry *g);

    static void setUseBuffers(bool b) { m_use_buffers = b; }
    static bool useBuffers() { return m_use_buffers; }

    static void markVertexDirty() { m_vertex_dirty = true; }
    static void markIndexDirty() { m_index_dirty = true; }

private:
    static void addGeometryVertex(const Geometry *);
    static void addGeometryIndex(const Geometry *);

    static void clearVertexData() { m_vertex_offsets.clear(); m_vertex_data.clear(); }
    static void clearIndexData() { m_index_offsets.clear(); m_index_data.clear(); }

    static bool m_use_buffers;
    static QSet<const Geometry *> m_geometries;
    static QGLBuffer m_vertex_buffer;
    static QGLBuffer m_index_buffer;
    static QSGArray<uchar> m_vertex_data;
    static QSGArray<uchar> m_index_data;
    static QHash<const Geometry *, int> m_vertex_offsets;
    static QHash<const Geometry *, int> m_index_offsets;
    static bool m_vertex_bound;
    static bool m_index_bound;
    static bool m_vertex_dirty;
    static bool m_index_dirty;
};


#endif

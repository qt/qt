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

#ifndef QSGAttributeDescription_H
#define QSGAttributeDescription_H

#include <QtOpenGL/qgl.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QSG
{
    enum VertexAttribute {
        Position,
        Normal,
        Color,
        TextureCoord0,
        TextureCoord1,
        TextureCoord2,
        CustomVertex0,
        CustomVertex1,
        UserVertex
    };

    enum DrawingMode
    {
        Points                      = 0x0000, // GL_POINTS
        Lines                       = 0x0001, // GL_LINES
        LineLoop                    = 0x0002, // GL_LINE_LOOP
        LineStrip                   = 0x0003, // GL_LINE_STRIP
        Triangles                   = 0x0004, // GL_TRIANGLES
        TriangleStrip               = 0x0005, // GL_TRIANGLE_STRIP
        TriangleFan                 = 0x0006, // GL_TRIANGLE_FAN
        LinesAdjacency              = 0x000A, // GL_LINES_ADJACENCY
        LineStripAdjacency          = 0x000B, // GL_LINE_STRIP_ADJACENCY
        TrianglesAdjacency          = 0x000C, // GL_TRIANGLES_ADJACENCY
        TriangleStripAdjacency      = 0x000D  // GL_TRIANGLE_STRIP_ADJACENCY
    };
};

class Q_DECLARATIVE_EXPORT QSGAttributeDescription
{
public:
    QSGAttributeDescription();
    QSGAttributeDescription(QSG::VertexAttribute attribute,
                            int tupleSize, GLenum type, int stride);

    bool isNull() const;

    QSG::VertexAttribute attribute() const;
    void setAttribute(QSG::VertexAttribute attribute);

    GLenum type() const;
    void setType(GLenum type);

    int sizeOfType() const;

    int tupleSize() const;
    void setTupleSize(int tupleSize);

    int stride() const;
    void setStride(int stride);

private:
    QSG::VertexAttribute m_attribute;
    GLenum m_type;
    int m_tupleSize;
    int m_stride;
};

inline QSGAttributeDescription::QSGAttributeDescription()
    : m_attribute(QSG::Position), m_type(GL_FLOAT),
      m_tupleSize(0), m_stride(0)
{
}

inline QSGAttributeDescription::QSGAttributeDescription
        (QSG::VertexAttribute attribute, int tupleSize, GLenum type, int stride)
    : m_attribute(attribute), m_type(type),
      m_tupleSize(tupleSize), m_stride(stride)
{
    Q_ASSERT(tupleSize >= 1 && tupleSize <= 4);
}

inline bool QSGAttributeDescription::isNull() const
{
    return m_tupleSize == 0;
}

inline QSG::VertexAttribute QSGAttributeDescription::attribute() const
{
    return m_attribute;
}

inline void QSGAttributeDescription::setAttribute(QSG::VertexAttribute attribute)
{
    m_attribute = attribute;
}

inline GLenum QSGAttributeDescription::type() const
{
    return m_type;
}

inline void QSGAttributeDescription::setType(GLenum type)
{
    m_type = type;
}

inline int QSGAttributeDescription::tupleSize() const
{
    return m_tupleSize;
}

inline void QSGAttributeDescription::setTupleSize(int tupleSize)
{
    Q_ASSERT(tupleSize >= 1 && tupleSize <= 4);
    m_tupleSize = tupleSize;
}

inline int QSGAttributeDescription::stride() const
{
    return m_stride;
}

inline void QSGAttributeDescription::setStride(int stride)
{
    m_stride = stride;
}

QT_END_NAMESPACE

QT_END_HEADER

#endif

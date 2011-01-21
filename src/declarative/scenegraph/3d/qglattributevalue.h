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

#ifndef QGLATTRIBUTEVALUE_H
#define QGLATTRIBUTEVALUE_H

#include "qglattributedescription.h"
#include "qcustomdataarray.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QGLVertexBuffer;

class Q_QT3D_EXPORT QGLAttributeValue
{
public:
    QGLAttributeValue();
    QGLAttributeValue(const QArray<float>& array);
    QGLAttributeValue(const QArray<QVector2D>& array);
    QGLAttributeValue(const QArray<QVector3D>& array);
    QGLAttributeValue(const QArray<QVector4D>& array);
    QGLAttributeValue(const QArray<QColor4ub>& array);
    QGLAttributeValue(const QCustomDataArray& array);
    QGLAttributeValue(int tupleSize, GLenum type, int stride, const void *data, int count = 0);
    QGLAttributeValue(int tupleSize, GLenum type, int stride, int offset, int count = 0);
    QGLAttributeValue(const QGLAttributeDescription& description, const void *data, int count = 0);
    QGLAttributeValue(const QGLAttributeDescription& description, int offset, int count = 0);

    bool isNull() const;

    const QGLAttributeDescription &description() const;
    GLenum type() const;
    int sizeOfType() const;
    int tupleSize() const;
    int stride() const;
    int offset() const;
    const void *data() const;
    const float *floatData() const;
    int count() const;

private:
    QGLAttributeDescription m_description;
    const void *m_data;
    int m_count;

    void setStride(int stride) { m_description.setStride(stride); }
    void setOffset(int offset)
        { m_data = reinterpret_cast<const void *>(offset); }

    friend class QGLVertexBuffer;
};

inline QGLAttributeValue::QGLAttributeValue()
    : m_data(0), m_count(0)
{
}

inline QGLAttributeValue::QGLAttributeValue(const QArray<float>& array)
    : m_description(QGL::Position, 1, GL_FLOAT, 0), m_data(array.constData()), m_count(array.count())
{
}

inline QGLAttributeValue::QGLAttributeValue(const QArray<QVector2D>& array)
    : m_description(QGL::Position, 2, GL_FLOAT, 0), m_data(array.constData()), m_count(array.count())
{
}

inline QGLAttributeValue::QGLAttributeValue(const QArray<QVector3D>& array)
    : m_description(QGL::Position, 3, GL_FLOAT, 0), m_data(array.constData()), m_count(array.count())
{
}

inline QGLAttributeValue::QGLAttributeValue(const QArray<QVector4D>& array)
    : m_description(QGL::Position, 4, GL_FLOAT, 0), m_data(array.constData()), m_count(array.count())
{
}

inline QGLAttributeValue::QGLAttributeValue(const QArray<QColor4ub>& array)
    : m_description(QGL::Position, 4, GL_UNSIGNED_BYTE, 0), m_data(array.constData()), m_count(array.count())
{
}

inline QGLAttributeValue::QGLAttributeValue
        (int tupleSize, GLenum type, int stride, const void *data, int count)
    : m_description(QGL::Position, tupleSize, type, stride), m_data(data), m_count(count)
{
}

inline QGLAttributeValue::QGLAttributeValue
        (int tupleSize, GLenum type, int stride, int offset, int count)
    : m_description(QGL::Position, tupleSize, type, stride),
      m_data(reinterpret_cast<const void *>(offset)), m_count(count)
{
}

inline QGLAttributeValue::QGLAttributeValue
        (const QGLAttributeDescription& description, const void *data, int count)
    : m_description(description), m_data(data), m_count(count)
{
}

inline QGLAttributeValue::QGLAttributeValue
        (const QGLAttributeDescription& description, int offset, int count)
    : m_description(description),
      m_data(reinterpret_cast<const void *>(offset)), m_count(count)
{
}

inline bool QGLAttributeValue::isNull() const
{
    return m_description.isNull();
}

inline const QGLAttributeDescription &QGLAttributeValue::description() const
{
    return m_description;
}

inline GLenum QGLAttributeValue::type() const
{
    return m_description.type();
}

inline int QGLAttributeValue::tupleSize() const
{
    return m_description.tupleSize();
}

inline int QGLAttributeValue::stride() const
{
    return m_description.stride();
}

inline int QGLAttributeValue::offset() const
{
    return int(reinterpret_cast<size_t>(m_data));
}

inline const void *QGLAttributeValue::data() const
{
    return m_data;
}

inline const float *QGLAttributeValue::floatData() const
{
    return reinterpret_cast<const float *>(m_data);
}

inline int QGLAttributeValue::count() const
{
    return m_count;
}

QT_END_NAMESPACE

QT_END_HEADER

#endif

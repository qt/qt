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

#ifndef QSGATTRIBUTEVALUE_H
#define QSGATTRIBUTEVALUE_H

#include "qsgattributedescription.h"
#include "qsgarray.h"
#include <QtGui/qvector2d.h>
#include <QtGui/qvector3d.h>
#include <QtGui/qvector4d.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class Q_DECLARATIVE_EXPORT QSGAttributeValue
{
public:
    QSGAttributeValue();
    QSGAttributeValue(const QSGArray<float>& array);
    QSGAttributeValue(const QSGArray<QVector2D>& array);
    QSGAttributeValue(const QSGArray<QVector3D>& array);
    QSGAttributeValue(const QSGArray<QVector4D>& array);
    QSGAttributeValue(int tupleSize, GLenum type, int stride, const void *data, int count = 0);
    QSGAttributeValue(int tupleSize, GLenum type, int stride, int offset, int count = 0);
    QSGAttributeValue(const QSGAttributeDescription& description, const void *data, int count = 0);
    QSGAttributeValue(const QSGAttributeDescription& description, int offset, int count = 0);

    bool isNull() const;

    const QSGAttributeDescription &description() const;
    GLenum type() const;
    int sizeOfType() const;
    int tupleSize() const;
    int stride() const;
    int offset() const;
    const void *data() const;
    const float *floatData() const;
    int count() const;

private:
    QSGAttributeDescription m_description;
    const void *m_data;
    int m_count;

    void setStride(int stride) { m_description.setStride(stride); }
    void setOffset(int offset)
        { m_data = reinterpret_cast<const void *>(offset); }
};

inline QSGAttributeValue::QSGAttributeValue()
    : m_data(0), m_count(0)
{
}

inline QSGAttributeValue::QSGAttributeValue(const QSGArray<float>& array)
    : m_description(QSG::Position, 1, GL_FLOAT, 0), m_data(array.constData()), m_count(array.count())
{
}

inline QSGAttributeValue::QSGAttributeValue(const QSGArray<QVector2D>& array)
    : m_description(QSG::Position, 2, GL_FLOAT, 0), m_data(array.constData()), m_count(array.count())
{
}

inline QSGAttributeValue::QSGAttributeValue(const QSGArray<QVector3D>& array)
    : m_description(QSG::Position, 3, GL_FLOAT, 0), m_data(array.constData()), m_count(array.count())
{
}

inline QSGAttributeValue::QSGAttributeValue(const QSGArray<QVector4D>& array)
    : m_description(QSG::Position, 4, GL_FLOAT, 0), m_data(array.constData()), m_count(array.count())
{
}

inline QSGAttributeValue::QSGAttributeValue
        (int tupleSize, GLenum type, int stride, const void *data, int count)
    : m_description(QSG::Position, tupleSize, type, stride), m_data(data), m_count(count)
{
}

inline QSGAttributeValue::QSGAttributeValue
        (int tupleSize, GLenum type, int stride, int offset, int count)
    : m_description(QSG::Position, tupleSize, type, stride),
      m_data(reinterpret_cast<const void *>(offset)), m_count(count)
{
}

inline QSGAttributeValue::QSGAttributeValue
        (const QSGAttributeDescription& description, const void *data, int count)
    : m_description(description), m_data(data), m_count(count)
{
}

inline QSGAttributeValue::QSGAttributeValue
        (const QSGAttributeDescription& description, int offset, int count)
    : m_description(description),
      m_data(reinterpret_cast<const void *>(offset)), m_count(count)
{
}

inline bool QSGAttributeValue::isNull() const
{
    return m_description.isNull();
}

inline const QSGAttributeDescription &QSGAttributeValue::description() const
{
    return m_description;
}

inline GLenum QSGAttributeValue::type() const
{
    return m_description.type();
}

inline int QSGAttributeValue::tupleSize() const
{
    return m_description.tupleSize();
}

inline int QSGAttributeValue::stride() const
{
    return m_description.stride();
}

inline int QSGAttributeValue::offset() const
{
    return int(reinterpret_cast<size_t>(m_data));
}

inline const void *QSGAttributeValue::data() const
{
    return m_data;
}

inline const float *QSGAttributeValue::floatData() const
{
    return reinterpret_cast<const float *>(m_data);
}

inline int QSGAttributeValue::count() const
{
    return m_count;
}

QT_END_NAMESPACE

QT_END_HEADER

#endif

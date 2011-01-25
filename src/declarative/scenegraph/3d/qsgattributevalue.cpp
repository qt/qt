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

#include "qsgattributevalue.h"

QT_BEGIN_NAMESPACE

/*!
    \class QSGAttributeValue
    \brief The QSGAttributeValue class encapsulates information about an OpenGL attribute value.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::enablers

    OpenGL has many functions that take a pointer to vertex attribute
    values: \c{glVertexPointer()}, \c{glNormalPointer()},
    \c{glVertexAttribPointer()}, etc.  These functions typically
    take four arguments: tuple size (1, 2, 3, or 4), component type
    (e.g. GL_FLOAT), stride, and data pointer (\c{glNormalPointer()}
    does not use tuple size, assuming that it is 3).  When used with
    vertex buffers, the data pointer may be an offset into the vertex
    buffer instead.

    QSGAttributeValue encapsulates these four values so that they can
    be easily manipulated as a set during OpenGL painting operations.
    Constructors are provided for converting QSGArray and
    QCustomDataArray objects into an attribute value.

    Because the data() value is a raw pointer to arbitrary memory,
    care should be taken that the memory remains valid until the
    QSGAttributeValue is no longer required.

    \sa QSGAttributeDescription, QSGArray, QCustomDataArray
*/

/*!
    \fn QSGAttributeValue::QSGAttributeValue()

    Constructs a null attribute value with default parameters of
    tupleSize(), stride(), and offset() set to zero, type() set to
    GL_FLOAT, and data() set to null.

    \sa isNull()
*/

/*!
    \fn QSGAttributeValue::QSGAttributeValue(const QSGArray<float>& array)

    Constructs an attribute value that refers to the contents of \a array,
    setting tupleSize() to 1, type() to GL_FLOAT, and stride() to zero.

    The \a array must not be destroyed or modified until the attribute
    value is no longer required.
*/

/*!
    \fn QSGAttributeValue::QSGAttributeValue(const QSGArray<QVector2D>& array)

    Constructs an attribute value that refers to the contents of \a array,
    setting tupleSize() to 2, type() to GL_FLOAT, and stride() to zero.

    The \a array must not be destroyed or modified until the attribute
    value is no longer required.
*/

/*!
    \fn QSGAttributeValue::QSGAttributeValue(const QSGArray<QVector3D>& array)

    Constructs an attribute value that refers to the contents of \a array,
    setting tupleSize() to 3, type() to GL_FLOAT, and stride() to zero.

    The \a array must not be destroyed or modified until the attribute
    value is no longer required.
*/

/*!
    \fn QSGAttributeValue::QSGAttributeValue(const QSGArray<QVector4D>& array)

    Constructs an attribute value that refers to the contents of \a array,
    setting tupleSize() to 4, type() to GL_FLOAT, and stride() to zero.

    The \a array must not be destroyed or modified until the attribute
    value is no longer required.
*/

/*!
    \fn QSGAttributeValue::QSGAttributeValue(int tupleSize, GLenum type, int stride, const void *data, int count)

    Constructs an attribute value with the fields \a tupleSize, \a type,
    \a stride, \a data, and \a count.
*/

/*!
    \fn QSGAttributeValue::QSGAttributeValue(int tupleSize, GLenum type, int stride, int offset, int count)

    Constructs an attribute value with the fields \a tupleSize, \a type,
    \a stride, \a offset, and \a count.
*/

/*!
    \fn QSGAttributeValue::QSGAttributeValue(const QSGAttributeDescription& description, const void *data, int count)

    Constructs an attribute value with the supplied \a description,
    \a data, and \a count.
*/

/*!
    \fn QSGAttributeValue::QSGAttributeValue(const QSGAttributeDescription& description, int offset, int count)

    Constructs an attribute value with the supplied \a description,
    \a offset, and \a count.
*/

/*!
    \fn bool QSGAttributeValue::isNull() const

    Returns true if tupleSize() is zero, which indicates an unset
    attribute value; false otherwise.

    Note: it is possible for data() to be null, but isNull() returns true.
    This can happen when data() is actually a zero offset() into a
    vertex buffer.

    \sa offset()
*/

/*!
    \fn const QSGAttributeDescription &QSGAttributeValue::description() const

    Returns a reference to the description of this attribute.

    \sa type()
*/

/*!
    \fn GLenum QSGAttributeValue::type() const

    Returns the component type for this attribute value.  The default
    value is GL_FLOAT.

    \sa sizeOfType(), description()
*/

/*!
    Returns the size in bytes of type().

    \sa type(), tupleSize()
*/
int QSGAttributeValue::sizeOfType() const
{
    switch (m_description.type()) {
    case GL_BYTE:           return int(sizeof(GLbyte));
    case GL_UNSIGNED_BYTE:  return int(sizeof(GLubyte));
    case GL_SHORT:          return int(sizeof(GLshort));
    case GL_UNSIGNED_SHORT: return int(sizeof(GLushort));
    case GL_INT:            return int(sizeof(GLint));
    case GL_UNSIGNED_INT:   return int(sizeof(GLuint));
    case GL_FLOAT:          return int(sizeof(GLfloat));
#if defined(GL_DOUBLE) && !defined(QT_OPENGL_ES)
    case GL_DOUBLE:         return int(sizeof(GLdouble));
#endif
    default:                return 0;
    }
}

/*!
    \fn int QSGAttributeValue::tupleSize() const

    Returns the tuple size of this attribute in components.  For example,
    a return value of 3 indicates a vector of 3-dimensional values.
    If tupleSize() is zero, then this attribute value is null.

    \sa isNull(), sizeOfType()
*/

/*!
    \fn int QSGAttributeValue::stride() const

    Returns the stride in bytes from one vertex element to the
    next for this attribute value.  The default value of 0 indicates
    that the elements are tightly packed within the data() array.
*/

/*!
    \fn int QSGAttributeValue::offset() const

    Returns the vertex buffer offset for this attribute value.

    This function is a convenience that returns data() cast
    to an integer offset value.

    \sa data()
*/

/*!
    \fn const void *QSGAttributeValue::data() const

    Returns the data pointer for the elements in this attribute value.

    \sa offset(), floatData()
*/

/*!
    \fn const float *QSGAttributeValue::floatData() const

    Returns the data pointer for the elements in this attribute value
    as a pointer to float.

    This is a convenience for the common case of passing data() to a
    function that expects a GLfloat * argument.

    \sa data()
*/

/*!
    \fn int QSGAttributeValue::count() const

    Returns the count of vertex elements in this attribute value;
    zero if the count is unknown.
*/

QT_END_NAMESPACE

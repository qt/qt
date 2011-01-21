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

#include "qcustomdataarray.h"
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

/*!
    \class QCustomDataArray
    \brief The QCustomDataArray class is a polymorphic array of data values suitable for use in 3D applications.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::enablers

    QArray is an efficient storage mechanism for vertex attributes.
    However, there are some situations where the element type of a custom
    vertex attribute is not known until runtime.  QCustomDataArray is
    intended for use in those situations.  It has a small performance
    penalty compared to QArray to achieve polymorphism.

    The elements that may be stored in a QCustomDataArray are limited
    to a few types: float, QVector2D, QVector3D, QVector4D, and
    QColor4ub.  This provides a reasonable range of efficient use
    cases without overloading the API.  QArray can be used on
    any type, but is restricted to types that are known at compile time.

    Like QArray, QCustomDataArray uses implicit sharing and
    copy-on-write semantics to support passing large arrays around
    an application with little overhead.

    \sa QArray
*/

/*!
    \enum QCustomDataArray::ElementType
    This enum defines the element type within a QCustomDataArray.

    \value Float The elements are of type float.
    \value Vector2D The elements are of type QVector2D.
    \value Vector3D The elements are of type QVector3D.
    \value Vector4D The elements are of type QVector4D.
    \value Color The elements are of type QColor4ub, which consists of
        four unsigned bytes.  To represent colors as four floating-point
        values, use Vector4D as the element type.
*/

/*!
    \fn QCustomDataArray::QCustomDataArray()

    Constructs an empty custom data array.  The elementType() will
    initially be QCustomDataArray::Float, which can be changed with a
    call to setElementType() before the elements are appended.

    \sa setElementType(), append()
*/

/*!
    Constructs an empty custom data array with elements represented
    by the specified \a type.

    \sa setElementType()
*/
QCustomDataArray::QCustomDataArray(QCustomDataArray::ElementType type)
{
    setElementType(type);
}

/*!
    Constructs an empty custom data array with elements represented
    by the specified \a type.  The array is initially resized to \a size;
    filling all elements with zeroes.

    \sa setElementType(), resize()
*/
QCustomDataArray::QCustomDataArray(QCustomDataArray::ElementType type, int size)
{
    setElementType(type);
    resize(size);
}

/*!
    \fn QCustomDataArray::QCustomDataArray(const QCustomDataArray& other)

    Constructs a copy of \a other.
*/

/*!
    Constructs a copy of the floating-point QArray \a other.

    The elementType() will be set to QCustomDataArray::Float.

    \sa toFloatArray()
*/
QCustomDataArray::QCustomDataArray(const QArray<float>& other)
    : m_array(other),
      m_elementType(QCustomDataArray::Float),
      m_elementComponents(1)
{
}

/*!
    Constructs a copy of the 2D vector QArray \a other.

    The elementType() will be set to QCustomDataArray::Vector2D.

    This constructor needs to make a complete copy of the data
    in \a other so it may be expensive performance-wise.

    \sa toVector2DArray()
*/
QCustomDataArray::QCustomDataArray(const QArray<QVector2D>& other)
    : m_elementType(QCustomDataArray::Vector2D),
      m_elementComponents(2)
{
    int size = other.size();
    if (size > 0) {
        const QVector2D *src = other.constData();
        float *dst = m_array.extend(size * 2);
        qMemCopy(dst, src, size * sizeof(QVector2D));
    }
}

/*!
    Constructs a copy of the 3D vector QArray \a other.

    The elementType() will be set to QCustomDataArray::Vector3D.

    This constructor needs to make a complete copy of the data
    in \a other so it may be expensive performance-wise.

    \sa toVector3DArray()
*/
QCustomDataArray::QCustomDataArray(const QArray<QVector3D>& other)
    : m_elementType(QCustomDataArray::Vector3D),
      m_elementComponents(3)
{
    int size = other.size();
    if (size > 0) {
        const QVector3D *src = other.constData();
        float *dst = m_array.extend(size * 3);
        qMemCopy(dst, src, size * sizeof(QVector3D));
    }
}

/*!
    Constructs a copy of the 4D vector QArray \a other.

    The elementType() will be set to QCustomDataArray::Vector4D.

    This constructor needs to make a complete copy of the data
    in \a other so it may be expensive performance-wise.

    \sa toVector3DArray()
*/
QCustomDataArray::QCustomDataArray(const QArray<QVector4D>& other)
    : m_elementType(QCustomDataArray::Vector4D),
      m_elementComponents(4)
{
    int size = other.size();
    if (size > 0) {
        const QVector4D *src = other.constData();
        float *dst = m_array.extend(size * 4);
        qMemCopy(dst, src, size * sizeof(QVector4D));
    }
}

/*!
    Constructs a copy of the color QArray \a other.

    The elementType() will be set to QCustomDataArray::Color.

    This constructor needs to make a complete copy of the data
    in \a other so it may be expensive performance-wise.

    \sa toColorArray()
*/
QCustomDataArray::QCustomDataArray(const QArray<QColor4ub>& other)
    : m_elementType(QCustomDataArray::Color),
      m_elementComponents(1)
{
    int size = other.size();
    qMemCopy(m_array.extend(size), other.constData(), sizeof(QColor4ub) * size);
}

/*!
    \fn QCustomDataArray::~QCustomDataArray()

    Destroys this custom data array.
*/

/*!
    \fn QCustomDataArray& QCustomDataArray::operator=(const QCustomDataArray& other)

    Assigns \a other to this custom data array and returns a reference
    to this custom data array.

    The previous elementType() for this custom data array will be
    replaced with the type from \a other.  The element data is assigned
    directly without conversion.
*/

/*!
    \fn QCustomDataArray::ElementType QCustomDataArray::elementType() const

    Returns the representation type of elements in this custom data array.

    \sa setElementType()
*/

/*!
    Sets the representation \a type of elements in this custom data array.
    The array must be empty to change the element type.

    \sa elementType(), append()
*/
void QCustomDataArray::setElementType(QCustomDataArray::ElementType type)
{
    Q_ASSERT(m_array.isEmpty());
    m_elementType = type;
    switch (type) {
    case QCustomDataArray::Float:
        m_elementComponents = 1;
        break;
    case QCustomDataArray::Vector2D:
        m_elementComponents = 2;
        break;
    case QCustomDataArray::Vector3D:
        m_elementComponents = 3;
        break;
    case QCustomDataArray::Vector4D:
        m_elementComponents = 4;
        break;
    case QCustomDataArray::Color:
        m_elementComponents = 1;    // 4 bytes packed into a float.
        break;
    default:
        Q_ASSERT_X(false, "QCustomDataArray::setElementType",
                   "unknown element type");
        m_elementComponents = 1;
        break;
    }
}

/*!
    \fn int QCustomDataArray::size() const

    Returns the number of elements in this custom data array.

    \sa resize(), capacity(), isEmpty()
*/

/*!
    \fn int QCustomDataArray::count() const

    Same as size(); provided for convenience.
*/

/*!
    \fn int QCustomDataArray::capacity() const

    Returns the number of elements that can be stored in this
    custom data array before reallocation.

    \sa reserve(), size()
*/

/*!
    \fn bool QCustomDataArray::isEmpty() const

    Returns true if this data array is empty; false otherwise.

    \sa size(), clear()
*/

/*!
    \fn int QCustomDataArray::elementSize() const

    Returns the size of individual elements in this custom data
    array, in bytes.  For example, the element size of an array
    containing QVector3D values will be 3 * sizeof(float),
    normally 12.

    \sa setElementType()
*/

/*!
    \fn void QCustomDataArray::clear()

    Clears all elements from this custom data array and sets the size to zero.

    This function will deallocate any memory that is used on the heap
    to store the custom data array's elements.  To reuse the same memory
    as before, call resize() with an argument of zero.

    \sa resize(), isEmpty()
*/

/*!
    \fn void QCustomDataArray::resize(int size)

    Sets the size of the custom data array to \a size.  If \a size is greater
    than the current size, elements are added to the end; the new elements
    are initialized with all-zeroes.  If \a size is less than the current
    size, elements are removed from the end.

    \sa size(), reserve(), squeeze()
*/

/*!
    \fn void QCustomDataArray::reserve(int size)

    Increases the capacity of this custom data array to reserve space for
    at least \a size elements.  If the capacity is already larger
    than \a size, this function does nothing; in particular, it does
    not remove elements from the array like resize() does.

    This function can be useful when you know how roughly many elements
    will be appended ahead of time.  Reserving the space once can avoid
    unnecessary realloc operations later.

    \sa capacity(), resize(), squeeze()
*/

/*!
    \fn void QCustomDataArray::squeeze()

    Releases any memory not required to store the custom data array's
    elements by reducing its capacity() to size().

    This function is intended for reclaiming memory in a custom data
    array that is being used over and over with different contents.
    As elements are added to a custom data array, it will be constantly
    expanded in size.  This function can realloc the custom data array
    to a smaller size to reclaim unused memory.

    \sa reserve(), capacity()
*/

/*!
    Returns the value of the element at \a index in this custom
    data array as a QVariant.

    Color elements are returned as a QVariant containing a
    QColor4ub, not a QColor.

    \sa setAt(), append(), floatAt(), vector2DAt(), vector3DAt()
    \sa vector4DAt(), colorAt()
*/
QVariant QCustomDataArray::at(int index) const
{
    Q_ASSERT(index >= 0 && index < size());

    const float *data;
    switch (m_elementType) {

    case QCustomDataArray::Float:
        return QVariant(m_array.at(index));

    case QCustomDataArray::Vector2D:
        data = m_array.constData() + index * 2;
        return qVariantFromValue(QVector2D(data[0], data[1]));

    case QCustomDataArray::Vector3D:
        data = m_array.constData() + index * 3;
        return qVariantFromValue(QVector3D(data[0], data[1], data[2]));

    case QCustomDataArray::Vector4D:
        data = m_array.constData() + index * 4;
        return qVariantFromValue
            (QVector4D(data[0], data[1], data[2], data[3]));

    case QCustomDataArray::Color:
        data = m_array.constData() + index;
        return qVariantFromValue
            (QColor4ub::fromRaw(reinterpret_cast<const uchar *>(data)));

    default: break;
    }
    return QVariant();
}

/*!
    Sets the element at \a index in this custom data array to \a value.

    The type of \a value must be consistent with elementType().
    The two exceptions to this are that a Float value can be
    specified by either a float or double QVariant, and a Color
    value can be specified as either a QColor4ub or QColor QVariant.

    \sa at(), elementType()
*/
void QCustomDataArray::setAt(int index, const QVariant& value)
{
    Q_ASSERT(index >= 0 && index < size());

    switch (value.type()) {

    case (QVariant::Type)QMetaType::Float:
        Q_ASSERT(m_elementType == QCustomDataArray::Float);
        m_array[index] = value.toFloat();
        break;

    case QVariant::Double:
        // Convert Double into Float.
        Q_ASSERT(m_elementType == QCustomDataArray::Float);
        m_array[index] = float(value.toDouble());
        break;

    case QVariant::Vector2D:
        Q_ASSERT(m_elementType == QCustomDataArray::Vector2D);
        *(reinterpret_cast<QVector2D *>(m_array.data() + index * 2))
            = qVariantValue<QVector2D>(value);
        break;

    case QVariant::Vector3D:
        Q_ASSERT(m_elementType == QCustomDataArray::Vector3D);
        *(reinterpret_cast<QVector3D *>(m_array.data() + index * 3))
            = qVariantValue<QVector3D>(value);
        break;

    case QVariant::Vector4D:
        Q_ASSERT(m_elementType == QCustomDataArray::Vector4D);
        *(reinterpret_cast<QVector4D *>(m_array.data() + index * 4))
            = qVariantValue<QVector4D>(value);
        break;

    case QVariant::Color:
        // Convert QColor into QColor4ub.
        Q_ASSERT(m_elementType == QCustomDataArray::Color);
        *(reinterpret_cast<QColor4ub *>(m_array.data() + index))
            = QColor4ub(qVariantValue<QColor>(value));
        break;

    case QVariant::UserType:
        if (value.userType() == qMetaTypeId<QColor4ub>()) {
            Q_ASSERT(m_elementType == QCustomDataArray::Color);
            *(reinterpret_cast<QColor4ub *>(m_array.data() + index))
                = qVariantValue<QColor4ub>(value);
            break;
        }
        // Fall through.

    default:
        Q_ASSERT_X(false, "QCustomDataArray::setAt",
                   "QVariant type not supported for elements");
        break;
    }
}

/*!
    \fn void QCustomDataArray::setAt(int index, qreal x)
    \overload

    Sets the floating-point element at \a index in this custom data
    array to \a x.  The elementType() must be QCustomDataArray::Float.

    \sa at(), elementType(), floatAt()
*/

/*!
    \fn void QCustomDataArray::setAt(int index, qreal x, qreal y)
    \overload

    Sets the 2D vector element at \a index in this custom
    data array to (\a x, \a y).  The elementType() must be
    QCustomDataArray::Vector2D.

    \sa at(), elementType(), vector2DAt()
*/

/*!
    \fn void QCustomDataArray::setAt(int index, qreal x, qreal y, qreal z)
    \overload

    Sets the 3D vector element at \a index in this custom
    data array to (\a x, \a y, \a z).  The elementType() must be
    QCustomDataArray::Vector3D.

    \sa at(), elementType(), vector3DAt()
*/

/*!
    \fn void QCustomDataArray::setAt(int index, qreal x, qreal y, qreal z, qreal w)
    \overload

    Sets the 4D vector element at \a index in this custom
    data array to (\a x, \a y, \a z, \a w).  The elementType() must be
    QCustomDataArray::Vector4D.

    \sa at(), elementType(), vector4DAt()
*/

/*!
    \fn void QCustomDataArray::setAt(int index, const QVector2D& value)
    \overload

    Sets the 2D vector element at \a index in this custom
    data array to \a value.  The elementType() must be
    QCustomDataArray::Vector2D.

    \sa at(), elementType(), vector2DAt()
*/

/*!
    \fn void QCustomDataArray::setAt(int index, const QVector3D& value)
    \overload

    Sets the 3D vector element at \a index in this custom
    data array to \a value.  The elementType() must be
    QCustomDataArray::Vector3D.

    \sa at(), elementType(), vector3DAt()
*/

/*!
    \fn void QCustomDataArray::setAt(int index, const QVector4D& value)
    \overload

    Sets the 4D vector element at \a index in this custom
    data array to \a value.  The elementType() must be
    QCustomDataArray::Vector4D.

    \sa at(), elementType(), vector4DAt()
*/

/*!
    \fn void QCustomDataArray::setAt(int index, const QColor4ub& value)
    \overload

    Sets the color element at \a index in this custom data array to \a value.
    The elementType() must be QCustomDataArray::Color.

    \sa at(), elementType(), colorAt()
*/

/*!
    \fn void QCustomDataArray::setAt(int index, Qt::GlobalColor value)
    \overload

    Sets the color element at \a index in this custom data array to \a value.
    The elementType() must be QCustomDataArray::Color.

    \sa at(), elementType(), colorAt()
*/

/*!
    \fn qreal QCustomDataArray::floatAt(int index) const

    Returns the floating-point element at \a index in this custom data array.
    The elementType() must be QCustomDataArray::Float.

    \sa at(), setAt(), elementType()
*/

/*!
    \fn QVector2D QCustomDataArray::vector2DAt(int index) const

    Returns the 2D vector element at \a index in this custom data array.
    The elementType() must be QCustomDataArray::Vector2D.

    \sa at(), setAt(), elementType()
*/

/*!
    \fn QVector3D QCustomDataArray::vector3DAt(int index) const

    Returns the 3D vector element at \a index in this custom data array.
    The elementType() must be QCustomDataArray::Vector3D.

    \sa at(), setAt(), elementType()
*/

/*!
    \fn QVector4D QCustomDataArray::vector4DAt(int index) const

    Returns the 4D vector element at \a index in this custom data array.
    The elementType() must be QCustomDataArray::Vector4D.

    \sa at(), setAt(), elementType()
*/

/*!
    \fn QColor4ub QCustomDataArray::colorAt(int index) const

    Returns the color element at \a index in this custom data array.
    The elementType() must be QCustomDataArray::Color.

    \sa at(), setAt(), elementType()
*/

/*!
    \fn void QCustomDataArray::append(qreal x)
    \overload

    Appends the floating-point value \a x to this custom data array.
    The elementType() must be QCustomDataArray::Float.

    \sa setAt(), floatAt()
*/

/*!
    \fn void QCustomDataArray::append(qreal x, qreal y)
    \overload

    Appends the 2D vector value (\a x, \a y) to this custom data array.
    The elementType() must be QCustomDataArray::Vector2D.

    \sa setAt(), vector2DAt()
*/

/*!
    \fn void QCustomDataArray::append(qreal x, qreal y, qreal z)
    \overload

    Appends the 3D vector value (\a x, \a y, \a z) to this custom
    data array.  The elementType() must be QCustomDataArray::Vector3D.

    \sa setAt(), vector3DAt()
*/

/*!
    \fn void QCustomDataArray::append(qreal x, qreal y, qreal z, qreal w)
    \overload

    Appends the 4D vector value (\a x, \a y, \a z, \a w) to this custom
    data array.  The elementType() must be QCustomDataArray::Vector4D.

    \sa setAt(), vector4DAt()
*/

/*!
    \fn void QCustomDataArray::append(const QVector2D& value)
    \overload

    Appends the 2D vector \a value to this custom data array.
    The elementType() must be QCustomDataArray::Vector2D.

    \sa setAt(), vector2DAt()
*/

/*!
    \fn void QCustomDataArray::append(const QVector3D& value)
    \overload

    Appends the 3D vector \a value to this custom data array.
    The elementType() must be QCustomDataArray::Vector3D.

    \sa setAt(), vector3DAt()
*/

/*!
    \fn void QCustomDataArray::append(const QVector4D& value)
    \overload

    Appends the 4D vector \a value to this custom data array.
    The elementType() must be QCustomDataArray::Vector4D.

    \sa setAt(), vector4DAt()
*/

/*!
    \fn void QCustomDataArray::append(const QColor4ub& value)
    \overload

    Appends the color \a value to this custom data array.
    The elementType() must be QCustomDataArray::Color.

    \sa setAt(), colorAt()
*/

/*!
    \fn void QCustomDataArray::append(Qt::GlobalColor value)
    \overload

    Appends the color \a value to this custom data array.
    The elementType() must be QCustomDataArray::Color.

    \sa setAt(), colorAt()
*/

/*!
    \fn void QCustomDataArray::append(const QCustomDataArray &array)
    \overload

    Appends the values in \a array to this custom data array.  This
    custom data array must have the same element type as \a array,
    unless this custom data array is empty - in which case the
    element type and data of \a array will be assigned to this.
*/

/*!
    Appends \a value to this custom data array.

    The type of \a value must be consistent with elementType().
    The two exceptions to this are that a Float value can be
    specified by either a float or double QVariant, and a Color
    value can be specified as either a QColor4ub or QColor QVariant.

    \sa at(), setAt(), elementType()
*/
void QCustomDataArray::append(const QVariant& value)
{
    switch (value.type()) {

    case (QVariant::Type)QMetaType::Float:
        Q_ASSERT(m_elementType == QCustomDataArray::Float);
        m_array.append(value.toFloat());
        break;

    case QVariant::Double:
        // Convert Double into Float.
        Q_ASSERT(m_elementType == QCustomDataArray::Float);
        m_array.append(float(value.toDouble()));
        break;

    case QVariant::Vector2D:
        append(qVariantValue<QVector2D>(value));
        break;

    case QVariant::Vector3D:
        append(qVariantValue<QVector3D>(value));
        break;

    case QVariant::Vector4D:
        append(qVariantValue<QVector4D>(value));
        break;

    case QVariant::Color:
        // Convert QColor into QColor4ub.
        append(QColor4ub(qVariantValue<QColor>(value)));
        break;

    case QVariant::UserType:
        if (value.userType() == qMetaTypeId<QColor4ub>()) {
            append(qVariantValue<QColor4ub>(value));
            break;
        }
        // Fall through.

    default:
        Q_ASSERT_X(false, "QCustomDataArray::append",
                   "QVariant type not supported for elements");
        break;
    }
}

/*!
    Returns the contents of this custom data array as a QArray
    of float values.

    The elementType() must be QCustomDataArray::Float.
*/
QArray<float> QCustomDataArray::toFloatArray() const
{
    Q_ASSERT(m_elementType == QCustomDataArray::Float);
    return m_array;
}

/*!
    Returns the contents of this custom data array as a QArray
    of QVector2D values.

    The elementType() must be QCustomDataArray::Vector2D.

    This function needs to make a complete copy of the data
    in this array so it may be expensive performance-wise.
*/
QArray<QVector2D> QCustomDataArray::toVector2DArray() const
{
    Q_ASSERT(m_elementType == QCustomDataArray::Vector2D);
    int size = m_array.size() / 2;
    QArray<QVector2D> result;
    if (size > 0) {
        QVector2D *dst = result.extend(size);
        const float *src = m_array.constData();
        qMemCopy(dst, src, size * sizeof(QVector2D));
    }
    return result;
}

/*!
    Returns the contents of this custom data array as a QArray
    of QVector3D values.

    The elementType() must be QCustomDataArray::Vector3D.

    This function needs to make a complete copy of the data
    in this array so it may be expensive performance-wise.
*/
QArray<QVector3D> QCustomDataArray::toVector3DArray() const
{
    Q_ASSERT(m_elementType == QCustomDataArray::Vector3D);
    int size = m_array.size() / 3;
    QArray<QVector3D> result;
    if (size > 0) {
        QVector3D *dst = result.extend(size);
        const float *src = m_array.constData();
        qMemCopy(dst, src, size * sizeof(QVector3D));
    }
    return result;
}

/*!
    Returns the contents of this custom data array as a QArray
    of QVector4D values.

    The elementType() must be QCustomDataArray::Vector4D.

    This function needs to make a complete copy of the data
    in this array so it may be expensive performance-wise.
*/
QArray<QVector4D> QCustomDataArray::toVector4DArray() const
{
    Q_ASSERT(m_elementType == QCustomDataArray::Vector4D);
    int size = m_array.size() / 4;
    QArray<QVector4D> result;
    if (size > 0) {
        QVector4D *dst = result.extend(size);
        const float *src = m_array.constData();
        qMemCopy(dst, src, size * sizeof(QVector4D));
    }
    return result;
}

/*!
    Returns the contents of this custom data array as a QArray
    of QColor4ub values.

    The elementType() must be QCustomDataArray::Color.

    This function needs to make a complete copy of the data
    in this array so it may be expensive performance-wise.
*/
QArray<QColor4ub> QCustomDataArray::toColorArray() const
{
    Q_ASSERT(m_elementType == QCustomDataArray::Color);
    int size = m_array.size();
    QArray<QColor4ub> result;
    result.reserve(size);
    const QColor4ub *data =
        reinterpret_cast<const QColor4ub *>(m_array.constData());
    for (int index = 0; index < size; ++index)
        result.append(*data++);
    return result;
}

/*!
    \fn const void *QCustomDataArray::data() const

    Returns a const pointer to the data stored in the custom data array.
    The pointer can be used to access the items in the custom data array.
    The pointer remains valid as long as the custom data array isn't
    reallocated.

    This function is mostly useful to pass a custom data array to a function
    that accepts a plain C++ array.
*/

#ifndef QT_NO_DEBUG_STREAM
Q_QT3D_EXPORT QDebug operator<<(QDebug dbg, const QCustomDataArray &array)
{
    dbg << "QCustomDataArray" << &array << " -- count:" << array.count();
    for (int i = 0; i < array.count(); ++i)
        dbg << array.at(i);
    return dbg;
}
#endif


QT_END_NAMESPACE

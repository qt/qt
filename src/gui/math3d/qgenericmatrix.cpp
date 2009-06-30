/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgenericmatrix.h"

QT_BEGIN_NAMESPACE

/*!
    \class QGenericMatrix
    \brief The QGenericMatrix class is a template class that represents a NxM transformation matrix with N columns and M rows.
    \since 4.6

    The QGenericMatrix template has four parameters:

    \table
    \row \i N \i Number of columns.
    \row \i M \i Number of rows.
    \row \i T \i Element type that is visible to users of the class.
    \row \i InnerT \i Element type that is used inside the class.
    \endtable

    Normally T and InnerT are the same type; e.g. float or double.
    But they can be different if the user wants to store elements
    internally in a fixed-point format for the underlying hardware.

    \sa QMatrix4x4
*/

/*!
    \fn QGenericMatrix::QGenericMatrix()

    Constructs a NxM identity matrix.
*/

/*!
    \fn QGenericMatrix::QGenericMatrix(const QGenericMatrix<N, M, T, InnerT>& other)

    Constructs a copy of \a other.
*/

/*!
    \fn QGenericMatrix::QGenericMatrix(const T *values)

    Constructs a matrix from the given N * M floating-point \a values.
    The contents of the array \a values is assumed to be in
    row-major order.

    \sa toValueArray()
*/

/*!
    \fn T QGenericMatrix::operator()(int row, int column) const

    Returns the element at position (\a row, \a column) in this matrix.
*/

/*!
    \fn InnerT& QGenericMatrix::operator()(int row, int column)

    Returns a reference to the element at position (\a row, \a column)
    in this matrix so that the element can be assigned to.
*/

/*!
    \fn bool QGenericMatrix::isIdentity() const

    Returns true if this matrix is the identity; false otherwise.

    \sa setIdentity()
*/

/*!
    \fn void QGenericMatrix::setIdentity()

    Sets this matrix to the identity.

    \sa isIdentity()
*/

/*!
    \fn void QGenericMatrix::fill(T value)

    Fills all elements of this matrix with \a value.
*/

/*!
    \fn QGenericMatrix<M, N> QGenericMatrix::transposed() const

    Returns this matrix, transposed about its diagonal.
*/

/*!
    \fn QGenericMatrix<N, M, T, InnerT>& QGenericMatrix::operator+=(const QGenericMatrix<N, M, T, InnerT>& other)

    Adds the contents of \a other to this matrix.
*/

/*!
    \fn QGenericMatrix<N, M, T, InnerT>& QGenericMatrix::operator-=(const QGenericMatrix<N, M, T, InnerT>& other)

    Subtracts the contents of \a other from this matrix.
*/

/*!
    \fn QGenericMatrix<N, M, T, InnerT>& QGenericMatrix::operator*=(T factor)

    Multiplies all elements of this matrix by \a factor.
*/

/*!
    \fn QGenericMatrix<N, M, T, InnerT>& QGenericMatrix::operator/=(T divisor)

    Divides all elements of this matrix by \a divisor.
*/

/*!
    \fn bool QGenericMatrix::operator==(const QGenericMatrix<N, M, T, InnerT>& other) const

    Returns true if this matrix is identical to \a other; false otherwise.
*/

/*!
    \fn bool QGenericMatrix::operator!=(const QGenericMatrix<N, M, T, InnerT>& other) const

    Returns true if this matrix is not identical to \a other; false otherwise.
*/

/*!
    \fn QGenericMatrix<N, M, T, InnerT> operator+(const QGenericMatrix<N, M, T, InnerT>& m1, const QGenericMatrix<N, M, T, InnerT>& m2)
    \relates QGenericMatrix

    Returns the sum of \a m1 and \a m2.
*/

/*!
    \fn QGenericMatrix<N, M, T, InnerT> operator-(const QGenericMatrix<N, M, T, InnerT>& m1, const QGenericMatrix<N, M, T, InnerT>& m2)
    \relates QGenericMatrix

    Returns the difference of \a m1 and \a m2.
*/

/*!
    \fn QGenericMatrix<M1, M2, T, InnerT> operator*(const QGenericMatrix<N, M2, T, InnerT>& m1, const QGenericMatrix<M1, N, T, InnerT>& m2)
    \relates QGenericMatrix

    Returns the product of the NxM2 matrix \a m1 and the M1xN matrix \a m2
    to produce a M1xM2 matrix result.
*/

/*!
    \fn QGenericMatrix<N, M, T, InnerT> operator-(const QGenericMatrix<N, M, T, InnerT>& matrix)
    \overload
    \relates QGenericMatrix

    Returns the negation of \a matrix.
*/

/*!
    \fn QGenericMatrix<N, M, T, InnerT> operator*(T factor, const QGenericMatrix<N, M, T, InnerT>& matrix)
    \relates QGenericMatrix

    Returns the result of multiplying all elements of \a matrix by \a factor.
*/

/*!
    \fn QGenericMatrix<N, M, T, InnerT> operator*(const QGenericMatrix<N, M, T, InnerT>& matrix, T factor)
    \relates QGenericMatrix

    Returns the result of multiplying all elements of \a matrix by \a factor.
*/

/*!
    \fn QGenericMatrix<N, M, T, InnerT> operator/(const QGenericMatrix<N, M, T, InnerT>& matrix, T divisor)
    \relates QGenericMatrix

    Returns the result of dividing all elements of \a matrix by \a divisor.
*/

/*!
    \fn void QGenericMatrix::toValueArray(T *values)

    Retrieves the N * M items in this matrix and writes them to \a values
    in row-major order.
*/

/*!
    \fn InnerT *QGenericMatrix::data()

    Returns a pointer to the raw data of this matrix.  This is intended
    for use with raw GL functions.

    \sa constData()
*/

/*!
    \fn const InnerT *QGenericMatrix::data() const

    Returns a constant pointer to the raw data of this matrix.
    This is intended for use with raw GL functions.

    \sa constData()
*/

/*!
    \fn const InnerT *QGenericMatrix::constData() const

    Returns a constant pointer to the raw data of this matrix.
    This is intended for use with raw GL functions.

    \sa data()
*/

QT_END_NAMESPACE

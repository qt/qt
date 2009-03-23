/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
****************************************************************************/

#include "qfixedpt.h"

QT_BEGIN_NAMESPACE

/*!
    \internal
    Returns the fixed-point square root of \a value.
*/
qint64 qt_math3d_fixed_sqrt(qint64 value)
{
    qint64 result = 0;
    qint64 bit = ((qint64)1) << 62;
    while (bit > value)
        bit >>= 2;
    while (bit != 0) {
        if (value >= (result + bit)) {
            value -= result + bit;
            result += (bit << 1);
        }
        result >>= 1;
        bit >>= 2;
    }
    return result;
}

/*!
    \class QFixedPt
    \brief The QFixedPt class represents fixed-point numbers within a 32-bit integer with a configurable precision.

    The template parameter is the number of bits of precision after
    the decimal point.  For example, QFixedPt<5> indicates that there
    are 27 bits before the decimal point, and 5 bits of precision after
    the decimal point.
*/

/*!
    \fn QFixedPt::QFixedPt()

    Constructs a default fixed-point number.  The initial value
    is undefined.
*/

/*!
    \fn QFixedPt::QFixedPt(int value)

    Constructs a fixed-point number from the integer \a value.
*/

/*!
    \fn QFixedPt::QFixedPt(qreal value)

    Constructs a fixed-point number from the floating-point \a value.
*/

/*!
    \fn QFixedPt<PrecBits>& QFixedPt::operator=(int value)

    Assigns the integer \a value to this fixed-point variable.
*/

/*!
    \fn QFixedPt<PrecBits>& QFixedPt::operator=(qreal value)

    Assigns the floating-point \a value to this fixed-point variable.
*/

/*!
    \fn int QFixedPt::bits() const

    Returns the raw bits that represent the fixed-point value of this object.

    \sa setBits()
*/

/*!
    \fn void QFixedPt::setBits(int value)

    Sets the raw bits that represent the fixed-point value of
    this object to \a value.

    \sa bits()
*/

#if !defined(QT_NO_MEMBER_TEMPLATES) || defined(Q_QDOC)

/*!
    \fn QFixedPt<Prec> QFixedPt::toPrecision() const

    Returns this fixed-point number, converted to the new fixed-point
    precision Prec.

    \sa qFixedPtToPrecision()
*/

#endif

/*!
    \fn QFixedPt<Prec> qFixedPtToPrecision(const QFixedPt<PrecBits>& value)

    Returns the fixed-point number \a value, converted to the new fixed-point
    precision Prec.

    \sa QFixedPt::toPrecision()
*/

/*!
    \fn QFixedPt<PrecBits>& QFixedPt::operator+=(const QFixedPt<PrecBits>& value)

    Adds \a value to this fixed-point number.
*/

/*!
    \fn QFixedPt<PrecBits>& QFixedPt::operator+=(int value)

    Adds an integer \a value to this fixed-point number.
*/

/*!
    \fn QFixedPt<PrecBits>& QFixedPt::operator+=(qreal value)

    Adds a floating-point \a value to this fixed-point number.
*/

/*!
    \fn QFixedPt<PrecBits>& QFixedPt::operator-=(const QFixedPt<PrecBits>& value)

    Subtracts \a value from this fixed-point number.
*/

/*!
    \fn QFixedPt<PrecBits>& QFixedPt::operator-=(int value)

    Subtracts an integer \a value from this fixed-point number.
*/

/*!
    \fn QFixedPt<PrecBits>& QFixedPt::operator-=(qreal value)

    Subtracts a floating-point \a value from this fixed-point number.
*/

/*!
    \fn QFixedPt<PrecBits>& QFixedPt::operator*=(const QFixedPt<PrecBits>& value)

    Multiplies this fixed-point number by \a value.
*/

/*!
    \fn QFixedPt<PrecBits>& QFixedPt::operator*=(int value)

    Multiplies this fixed-point number by an integer \a value.
*/

/*!
    \fn QFixedPt<PrecBits>& QFixedPt::operator*=(qreal value)

    Multiplies this fixed-point number by a floating-point \a value.
*/

/*!
    \fn QFixedPt<PrecBits>& QFixedPt::operator/=(const QFixedPt<PrecBits>& value)

    Divides this fixed-point number by \a value.  Division by zero
    will result in zero.
*/

/*!
    \fn QFixedPt<PrecBits>& QFixedPt::operator/=(int value)

    Divides this fixed-point number by an integer \a value.  Division
    by zero will result in zero.
*/

/*!
    \fn QFixedPt<PrecBits>& QFixedPt::operator/=(qreal value)

    Divides this fixed-point number by a floating-point \a value.  Division
    by zero will result in zero.
*/

/*!
    \fn QFixedPt<PrecBits>& QFixedPt::operator<<=(int value)

    Shifts this fixed-point number left by \a value bits.
*/

/*!
    \fn QFixedPt<PrecBits>& QFixedPt::operator>>=(int value)

    Shifts this fixed-point number right by \a value bits.
*/

/*!
    \fn QFixedPt<PrecBits> QFixedPt::operator<<(int value) const

    Returns the result of shifting this fixed-point number
    left by \a value bits.
*/

/*!
    \fn QFixedPt<PrecBits> QFixedPt::operator>>(int value) const

    Returns the result of shifting this fixed-point number
    right by \a value bits.
*/

/*!
    \fn bool QFixedPt::operator==(const QFixedPt<PrecBits>& value) const

    Returns true if this fixed-point number is equal to \a value;
    false otherwise.
*/

/*!
    \fn bool operator==(const QFixedPt<PrecBits>& v1, int v2)
    \relates QFixedPt

    Returns true if \a v1 is equal to \a v2; false otherwise.
*/

/*!
    \fn bool operator==(int v1, const QFixedPt<PrecBits>& v2)
    \relates QFixedPt

    Returns true if \a v1 is equal to \a v2; false otherwise.
*/

/*!
    \fn bool operator==(const QFixedPt<PrecBits>& v1, qreal v2)
    \relates QFixedPt

    Returns true if \a v1 is equal to \a v2; false otherwise.
*/

/*!
    \fn bool operator==(qreal v1, const QFixedPt<PrecBits>& v2)
    \relates QFixedPt

    Returns true if \a v1 is equal to \a v2; false otherwise.
*/

/*!
    \fn bool QFixedPt::operator!=(const QFixedPt<PrecBits>& value) const

    Returns true if this fixed-point number is not equal to \a value;
    false otherwise.
*/

/*!
    \fn bool operator!=(const QFixedPt<PrecBits>& v1, int v2)
    \relates QFixedPt

    Returns true if \a v1 is not equal to \a v2; false otherwise.
*/

/*!
    \fn bool operator!=(int v1, const QFixedPt<PrecBits>& v2)
    \relates QFixedPt

    Returns true if \a v1 is not equal to \a v2; false otherwise.
*/

/*!
    \fn bool operator!=(const QFixedPt<PrecBits>& v1, qreal v2)
    \relates QFixedPt

    Returns true if \a v1 is not equal to \a v2; false otherwise.
*/

/*!
    \fn bool operator!=(qreal v1, const QFixedPt<PrecBits>& v2)
    \relates QFixedPt

    Returns true if \a v1 is not equal to \a v2; false otherwise.
*/

/*!
    \fn bool QFixedPt::operator<=(const QFixedPt<PrecBits>& value) const

    Returns true if this fixed-point number is less than or equal to
    \a value; false otherwise.
*/

/*!
    \fn bool operator<=(const QFixedPt<PrecBits>& v1, int v2)
    \relates QFixedPt

    Returns true if \a v1 is less than or equal to \a v2; false otherwise.
*/

/*!
    \fn bool operator<=(int v1, const QFixedPt<PrecBits>& v2)
    \relates QFixedPt

    Returns true if \a v1 is less than or equal to \a v2; false otherwise.
*/

/*!
    \fn bool operator<=(const QFixedPt<PrecBits>& v1, qreal v2)
    \relates QFixedPt

    Returns true if \a v1 is less than or equal to \a v2; false otherwise.
*/

/*!
    \fn bool operator<=(qreal v1, const QFixedPt<PrecBits>& v2)
    \relates QFixedPt

    Returns true if \a v1 is less than or equal to \a v2; false otherwise.
*/

/*!
    \fn bool QFixedPt::operator<(const QFixedPt<PrecBits>& value) const

    Returns true if this fixed-point number is less than \a value;
    false otherwise.
*/

/*!
    \fn bool operator<(const QFixedPt<PrecBits>& v1, int v2)
    \relates QFixedPt

    Returns true if \a v1 is less than \a v2; false otherwise.
*/

/*!
    \fn bool operator<(int v1, const QFixedPt<PrecBits>& v2)
    \relates QFixedPt

    Returns true if \a v1 is less than \a v2; false otherwise.
*/

/*!
    \fn bool operator<(const QFixedPt<PrecBits>& v1, qreal v2)
    \relates QFixedPt

    Returns true if \a v1 is less than \a v2; false otherwise.
*/

/*!
    \fn bool operator<(qreal v1, const QFixedPt<PrecBits>& v2)
    \relates QFixedPt

    Returns true if \a v1 is less than \a v2; false otherwise.
*/

/*!
    \fn bool QFixedPt::operator>=(const QFixedPt<PrecBits>& value) const

    Returns true if this fixed-point number is greater than or equal to
    \a value; false otherwise.
*/

/*!
    \fn bool operator>=(const QFixedPt<PrecBits>& v1, int v2)
    \relates QFixedPt

    Returns true if \a v1 is greater than or equal to \a v2; false otherwise.
*/

/*!
    \fn bool operator>=(int v1, const QFixedPt<PrecBits>& v2)
    \relates QFixedPt

    Returns true if \a v1 is greater than or equal to \a v2; false otherwise.
*/

/*!
    \fn bool operator>=(const QFixedPt<PrecBits>& v1, qreal v2)
    \relates QFixedPt

    Returns true if \a v1 is greater than or equal to \a v2; false otherwise.
*/

/*!
    \fn bool operator>=(qreal v1, const QFixedPt<PrecBits>& v2)
    \relates QFixedPt

    Returns true if \a v1 is greater than or equal to \a v2; false otherwise.
*/

/*!
    \fn bool QFixedPt::operator>(const QFixedPt<PrecBits>& value) const

    Returns true if this fixed-point number is greater than \a value;
    false otherwise.
*/

/*!
    \fn bool operator>(const QFixedPt<PrecBits>& v1, int v2)
    \relates QFixedPt

    Returns true if \a v1 is greater than \a v2; false otherwise.
*/

/*!
    \fn bool operator>(int v1, const QFixedPt<PrecBits>& v2)
    \relates QFixedPt

    Returns true if \a v1 is greater than \a v2; false otherwise.
*/

/*!
    \fn bool operator>(const QFixedPt<PrecBits>& v1, qreal v2)
    \relates QFixedPt

    Returns true if \a v1 is greater than \a v2; false otherwise.
*/

/*!
    \fn bool operator>(qreal v1, const QFixedPt<PrecBits>& v2)
    \relates QFixedPt

    Returns true if \a v1 is greater than \a v2; false otherwise.
*/

/*!
    \fn QFixedPt<PrecBits> QFixedPt::operator+(const QFixedPt<PrecBits>& value) const

    Returns the result of adding this fixed-point number and \a value.
*/

/*!
    \fn QFixedPt<PrecBits> QFixedPt::operator+(int value) const

    Returns the result of adding this fixed-point number and \a value.
*/

/*!
    \fn QFixedPt<PrecBits> QFixedPt::operator+(qreal value) const

    Returns the result of adding this fixed-point number and \a value.
*/

/*!
    \fn QFixedPt<PrecBits> operator+(int v1, const QFixedPt<PrecBits>& v2)
    \relates QFixedPt

    Returns the result of adding \a v1 and \a v2.
*/

/*!
    \fn QFixedPt<PrecBits> operator+(qreal v1, const QFixedPt<PrecBits>& v2)
    \relates QFixedPt

    Returns the result of adding \a v1 and \a v2.
*/

/*!
    \fn QFixedPt<PrecBits> QFixedPt::operator-(const QFixedPt<PrecBits>& value) const

    Returns the result of subtracting \a value from this fixed-point number.
*/

/*!
    \fn QFixedPt<PrecBits> QFixedPt::operator-(int value) const

    Returns the result of subtracting \a value from this fixed-point number.
*/

/*!
    \fn QFixedPt<PrecBits> QFixedPt::operator-(qreal value) const

    Returns the result of subtracting \a value from this fixed-point number.
*/

/*!
    \fn QFixedPt<PrecBits> operator-(int v1, const QFixedPt<PrecBits>& v2)
    \relates QFixedPt

    Returns the result of subtracting \a v2 from \a v1.
*/

/*!
    \fn QFixedPt<PrecBits> operator-(qreal v1, const QFixedPt<PrecBits>& v2)
    \relates QFixedPt

    Returns the result of subtracting \a v2 from \a v1.
*/

/*!
    \fn QFixedPt<PrecBits> QFixedPt::operator*(const QFixedPt<PrecBits>& value) const

    Returns the result of multiplying this fixed-point number by \a value.
*/

/*!
    \fn QFixedPt<PrecBits> QFixedPt::operator*(int value) const

    Returns the result of multiplying this fixed-point number by \a value.
*/

/*!
    \fn QFixedPt<PrecBits> QFixedPt::operator*(qreal value) const

    Returns the result of multiplying this fixed-point number by \a value.
*/

/*!
    \fn QFixedPt<PrecBits> operator*(int v1, const QFixedPt<PrecBits>& v2)
    \relates QFixedPt

    Returns the result of multiplying \a v1 by \a v2.
*/

/*!
    \fn QFixedPt<PrecBits> operator*(qreal v1, const QFixedPt<PrecBits>& v2)
    \relates QFixedPt

    Returns the result of multiplying \a v1 by \a v2.
*/

/*!
    \fn QFixedPt<PrecBits> QFixedPt::operator/(const QFixedPt<PrecBits>& value) const

    Returns the result of dividing this fixed-point number by \a value.
    Division by zero will result in zero.
*/

/*!
    \fn QFixedPt<PrecBits> QFixedPt::operator/(int value) const

    Returns the result of dividing this fixed-point number by \a value.
    Division by zero will result in zero.
*/

/*!
    \fn QFixedPt<PrecBits> QFixedPt::operator/(qreal value) const

    Returns the result of dividing this fixed-point number by \a value.
    Division by zero will result in zero.
*/

/*!
    \fn QFixedPt<PrecBits> operator/(int v1, const QFixedPt<PrecBits>& v2)
    \relates QFixedPt

    Returns the result of dividing \a v1 by \a v2.  Division by zero will
    result in zero.
*/

/*!
    \fn QFixedPt<PrecBits> operator/(qreal v1, const QFixedPt<PrecBits>& v2)
    \relates QFixedPt

    Returns the result of dividing \a v1 by \a v2.  Division by zero will
    result in zero.
*/

/*!
    \fn QFixedPt<PrecBits> QFixedPt::operator-() const

    Returns the negation of this fixed-point number.
*/

/*!
    \fn QFixedPt<PrecBits> QFixedPt::sqrt() const

    Returns the square root of this fixed-point number.

    \sa sqrtF()
*/

/*!
    \fn qreal QFixedPt::sqrtF() const

    Return the square root of this fixed-point number as a
    floating-point value.

    \sa sqrt()
*/

/*!
    \fn QFixedPt<PrecBits> QFixedPt::round() const

    Returns this fixed-point number, rounded to the nearest integer.

    \sa floor(), ceil(), truncate()
*/

/*!
    \fn QFixedPt<PrecBits> QFixedPt::floor() const;

    Returns the largest integer that is less than or equal to
    this fixed-point number.

    \sa round(), ceil(), truncate()
*/

/*!
    \fn QFixedPt<PrecBits> QFixedPt::ceil() const

    Returns the smallest integer that is greater than or equal to
    this fixed-point number.

    \sa round(), floor(), truncate()
*/

/*!
    \fn int QFixedPt::truncate() const

    Returns this fixed-point number with the bits after the
    decimal point truncated.

    \sa round(), floor(), ceil()
*/

/*!
    \fn int QFixedPt::toInt() const

    Returns this fixed-point number, rounded to the nearest integer.

    \sa toReal()
*/

/*!
    \fn qreal QFixedPt::toReal() const

    Returns this fixed-point number as a floating-point value.

    \sa toInt()
*/

/*!
    \fn int qCeil(const QFixedPt<PrecBits>& value)
    \relates QFixedPt

    Returns the smallest integer that is greater than or equal to
    \a value.

    \sa qFloor(), qRound(), QFixedPt::ceil()
*/

/*!
    \fn int qFloor(const QFixedPt<PrecBits>& value)
    \relates QFixedPt

    Returns the largest integer that is less than or equal to
    \a value.

    \sa qCeil(), qRound(), QFixedPt::floor()
*/

/*!
    \fn int qRound(const QFixedPt<PrecBits>& value)
    \relates QFixedPt

    Returns \a value, rounded to the nearest integer.

    \sa qCeil(), qFloor(), QFixedPt::round()
*/

/*!
    \fn bool qFuzzyCompare(const QFixedPt<PrecBits>& v1, const QFixedPt<PrecBits>& v2, int compareBits)
    \relates QFixedPt

    Returns true if \a v1 is almost equal to \a v2; false otherwise.
    The \a compareBits parameter specifies the number of bits of precision
    that should be considered relevant when performing the comparison.
    By default, \a compareBits is PrecBits / 4.
*/

/*!
    \fn bool qIsNull(const QFixedPt<PrecBits>& v)
    \relates QFixedPt

    Returns true if \a v is zero; false otherwise.
*/

QT_END_NAMESPACE

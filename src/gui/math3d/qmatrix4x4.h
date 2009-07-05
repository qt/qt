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

#ifndef QMATRIX4X4_H
#define QMATRIX4X4_H

#include <QtGui/qvector3d.h>
#include <QtGui/qvector4d.h>
#include <QtGui/qquaternion.h>
#include <QtGui/qgenericmatrix.h>
#include <QtCore/qrect.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

#ifndef QT_NO_MATRIX4X4

class QMatrix;
class QTransform;

class Q_GUI_EXPORT QMatrix4x4
{
public:
    inline QMatrix4x4() { setIdentity(); }
    explicit QMatrix4x4(const qreal *values);
    inline QMatrix4x4(qreal m11, qreal m12, qreal m13, qreal m14,
                      qreal m21, qreal m22, qreal m23, qreal m24,
                      qreal m31, qreal m32, qreal m33, qreal m34,
                      qreal m41, qreal m42, qreal m43, qreal m44);
#if !defined(QT_NO_MEMBER_TEMPLATES) || defined(Q_QDOC)
    template <int N, int M>
    explicit QMatrix4x4(const QGenericMatrix<N, M, qreal, float>& matrix);
#endif
    QMatrix4x4(const float *values, int cols, int rows);
    QMatrix4x4(const QTransform& transform);
    QMatrix4x4(const QMatrix& matrix);

    inline qreal operator()(int row, int column) const;
    inline float& operator()(int row, int column);

    inline QVector4D column(int index) const;
    inline void setColumn(int index, const QVector4D& value);

    inline QVector4D row(int index) const;
    inline void setRow(int index, const QVector4D& value);

    inline bool isIdentity() const;
    inline void setIdentity();

    inline void fill(qreal value);

    qreal determinant() const;
    QMatrix4x4 inverted(bool *invertible = 0) const;
    QMatrix4x4 transposed() const;
    QMatrix3x3 normalMatrix() const;

    inline QMatrix4x4& operator+=(const QMatrix4x4& other);
    inline QMatrix4x4& operator-=(const QMatrix4x4& other);
    inline QMatrix4x4& operator*=(const QMatrix4x4& other);
    inline QMatrix4x4& operator*=(qreal factor);
    QMatrix4x4& operator/=(qreal divisor);
    inline bool operator==(const QMatrix4x4& other) const;
    inline bool operator!=(const QMatrix4x4& other) const;

    friend QMatrix4x4 operator+(const QMatrix4x4& m1, const QMatrix4x4& m2);
    friend QMatrix4x4 operator-(const QMatrix4x4& m1, const QMatrix4x4& m2);
    friend QMatrix4x4 operator*(const QMatrix4x4& m1, const QMatrix4x4& m2);
#ifndef QT_NO_VECTOR3D
    friend QVector3D operator*(const QMatrix4x4& matrix, const QVector3D& vector);
    friend QVector3D operator*(const QVector3D& vector, const QMatrix4x4& matrix);
#endif
#ifndef QT_NO_VECTOR4D
    friend QVector4D operator*(const QVector4D& vector, const QMatrix4x4& matrix);
    friend QVector4D operator*(const QMatrix4x4& matrix, const QVector4D& vector);
#endif
    friend QPoint operator*(const QPoint& point, const QMatrix4x4& matrix);
    friend QPointF operator*(const QPointF& point, const QMatrix4x4& matrix);
    friend QMatrix4x4 operator-(const QMatrix4x4& matrix);
    friend QPoint operator*(const QMatrix4x4& matrix, const QPoint& point);
    friend QPointF operator*(const QMatrix4x4& matrix, const QPointF& point);
    friend QMatrix4x4 operator*(qreal factor, const QMatrix4x4& matrix);
    friend QMatrix4x4 operator*(const QMatrix4x4& matrix, qreal factor);
    friend Q_GUI_EXPORT QMatrix4x4 operator/(const QMatrix4x4& matrix, qreal divisor);

    friend inline bool qFuzzyCompare(const QMatrix4x4& m1, const QMatrix4x4& m2);

#ifndef QT_NO_VECTOR3D
    QMatrix4x4& scale(const QVector3D& vector);
    QMatrix4x4& translate(const QVector3D& vector);
    QMatrix4x4& rotate(qreal angle, const QVector3D& vector);
#endif
    QMatrix4x4& scale(qreal x, qreal y);
    QMatrix4x4& scale(qreal x, qreal y, qreal z);
    QMatrix4x4& scale(qreal factor);
    QMatrix4x4& translate(qreal x, qreal y);
    QMatrix4x4& translate(qreal x, qreal y, qreal z);
    QMatrix4x4& rotate(qreal angle, qreal x, qreal y, qreal z = 0.0f);
#ifndef QT_NO_QUATERNION
    QMatrix4x4& rotate(const QQuaternion& quaternion);
#endif

#ifndef QT_NO_VECTOR3D
    void extractAxisRotation(qreal &angle, QVector3D &axis) const;
    QVector3D extractTranslation() const;
#endif

    QMatrix4x4& ortho(const QRect& rect);
    QMatrix4x4& ortho(const QRectF& rect);
    QMatrix4x4& ortho(qreal left, qreal right, qreal bottom, qreal top, qreal nearPlane, qreal farPlane);
    QMatrix4x4& frustum(qreal left, qreal right, qreal bottom, qreal top, qreal nearPlane, qreal farPlane);
    QMatrix4x4& perspective(qreal angle, qreal aspect, qreal nearPlane, qreal farPlane);
#ifndef QT_NO_VECTOR3D
    QMatrix4x4& lookAt(const QVector3D& eye, const QVector3D& center, const QVector3D& up);
#endif
    QMatrix4x4& flipCoordinates();

    void toValueArray(qreal *values) const;

    QMatrix toAffine() const;
    QTransform toTransform() const;

    QPoint map(const QPoint& point) const;
    QPointF map(const QPointF& point) const;
#ifndef QT_NO_VECTOR3D
    QVector3D map(const QVector3D& point) const;
#endif
#ifndef QT_NO_VECTOR4D
    QVector4D map(const QVector4D& point) const;
#endif
    QRect mapRect(const QRect& rect) const;
    QRectF mapRect(const QRectF& rect) const;

#if !defined(QT_NO_MEMBER_TEMPLATES) || defined(Q_QDOC)
    template <int N, int M>
    QGenericMatrix<N, M, qreal, float> toGenericMatrix() const;
#endif

    inline float *data();
    inline const float *data() const { return m[0]; }
    inline const float *constData() const { return m[0]; }

    void inferSpecialType();

#ifndef QT_NO_DEBUG_STREAM
    friend Q_GUI_EXPORT QDebug operator<<(QDebug dbg, const QMatrix4x4 &m);
#endif

private:
    float m[4][4];          // Column-major order to match OpenGL.
    int flagBits;           // Flag bits from the enum below.

    enum {
        Identity        = 0x0001,   // Identity matrix
        General         = 0x0002,   // General matrix, unknown contents
        Translation     = 0x0004,   // Contains a simple translation
        Scale           = 0x0008,   // Contains a simple scale
        Rotation        = 0x0010    // Contains a simple rotation
    };

    // Construct without initializing identity matrix.
    QMatrix4x4(int) { flagBits = General; }

    QMatrix4x4 orthonormalInverse() const;
};

inline QMatrix4x4::QMatrix4x4
        (qreal m11, qreal m12, qreal m13, qreal m14,
         qreal m21, qreal m22, qreal m23, qreal m24,
         qreal m31, qreal m32, qreal m33, qreal m34,
         qreal m41, qreal m42, qreal m43, qreal m44)
{
    m[0][0] = m11; m[0][1] = m21; m[0][2] = m31; m[0][3] = m41;
    m[1][0] = m12; m[1][1] = m22; m[1][2] = m32; m[1][3] = m42;
    m[2][0] = m13; m[2][1] = m23; m[2][2] = m33; m[2][3] = m43;
    m[3][0] = m14; m[3][1] = m24; m[3][2] = m34; m[3][3] = m44;
    flagBits = General;
}

#if !defined(QT_NO_MEMBER_TEMPLATES)

template <int N, int M>
Q_INLINE_TEMPLATE QMatrix4x4::QMatrix4x4
    (const QGenericMatrix<N, M, qreal, float>& matrix)
{
    const float *values = matrix.constData();
    for (int col = 0; col < 4; ++col) {
        for (int row = 0; row < 4; ++row) {
            if (col < N && row < M)
                m[col][row] = values[col * M + row];
            else if (col == row)
                m[col][row] = 1.0f;
            else
                m[col][row] = 0.0f;
        }
    }
    flagBits = General;
}

template <int N, int M>
QGenericMatrix<N, M, qreal, float> QMatrix4x4::toGenericMatrix() const
{
    QGenericMatrix<N, M, qreal, float> result;
    float *values = result.data();
    for (int col = 0; col < N; ++col) {
        for (int row = 0; row < M; ++row) {
            if (col < 4 && row < 4)
                values[col * M + row] = m[col][row];
            else if (col == row)
                values[col * M + row] = 1.0f;
            else
                values[col * M + row] = 0.0f;
        }
    }
    return result;
}

#endif

inline qreal QMatrix4x4::operator()(int row, int column) const
{
    Q_ASSERT(row >= 0 && row < 4 && column >= 0 && column < 4);
    return qreal(m[column][row]);
}

inline float& QMatrix4x4::operator()(int row, int column)
{
    Q_ASSERT(row >= 0 && row < 4 && column >= 0 && column < 4);
    flagBits = General;
    return m[column][row];
}

inline QVector4D QMatrix4x4::column(int index) const
{
    Q_ASSERT(index >= 0 && index < 4);
    return QVector4D(m[index][0], m[index][1], m[index][2], m[index][3], 1);
}

inline void QMatrix4x4::setColumn(int index, const QVector4D& value)
{
    Q_ASSERT(index >= 0 && index < 4);
    m[index][0] = value.xp;
    m[index][1] = value.yp;
    m[index][2] = value.zp;
    m[index][3] = value.wp;
    flagBits = General;
}

inline QVector4D QMatrix4x4::row(int index) const
{
    Q_ASSERT(index >= 0 && index < 4);
    return QVector4D(m[0][index], m[1][index], m[2][index], m[3][index], 1);
}

inline void QMatrix4x4::setRow(int index, const QVector4D& value)
{
    Q_ASSERT(index >= 0 && index < 4);
    m[0][index] = value.xp;
    m[1][index] = value.yp;
    m[2][index] = value.zp;
    m[3][index] = value.wp;
    flagBits = General;
}

Q_GUI_EXPORT QMatrix4x4 operator/(const QMatrix4x4& matrix, qreal divisor);

inline bool QMatrix4x4::isIdentity() const
{
    if (flagBits == Identity)
        return true;
    if (m[0][0] != 1.0f || m[0][1] != 0.0f || m[0][2] != 0.0f)
        return false;
    if (m[0][3] != 0.0f || m[1][0] != 0.0f || m[1][1] != 1.0f)
        return false;
    if (m[1][2] != 0.0f || m[1][3] != 0.0f || m[2][0] != 0.0f)
        return false;
    if (m[2][1] != 0.0f || m[2][2] != 1.0f || m[2][3] != 0.0f)
        return false;
    if (m[3][0] != 0.0f || m[3][1] != 0.0f || m[3][2] != 0.0f)
        return false;
    return (m[3][3] == 1.0f);
}

inline void QMatrix4x4::setIdentity()
{
    m[0][0] = 1.0f;
    m[0][1] = 0.0f;
    m[0][2] = 0.0f;
    m[0][3] = 0.0f;
    m[1][0] = 0.0f;
    m[1][1] = 1.0f;
    m[1][2] = 0.0f;
    m[1][3] = 0.0f;
    m[2][0] = 0.0f;
    m[2][1] = 0.0f;
    m[2][2] = 1.0f;
    m[2][3] = 0.0f;
    m[3][0] = 0.0f;
    m[3][1] = 0.0f;
    m[3][2] = 0.0f;
    m[3][3] = 1.0f;
    flagBits = Identity;
}

inline void QMatrix4x4::fill(qreal value)
{
    m[0][0] = value;
    m[0][1] = value;
    m[0][2] = value;
    m[0][3] = value;
    m[1][0] = value;
    m[1][1] = value;
    m[1][2] = value;
    m[1][3] = value;
    m[2][0] = value;
    m[2][1] = value;
    m[2][2] = value;
    m[2][3] = value;
    m[3][0] = value;
    m[3][1] = value;
    m[3][2] = value;
    m[3][3] = value;
    flagBits = General;
}

inline QMatrix4x4& QMatrix4x4::operator+=(const QMatrix4x4& other)
{
    m[0][0] += other.m[0][0];
    m[0][1] += other.m[0][1];
    m[0][2] += other.m[0][2];
    m[0][3] += other.m[0][3];
    m[1][0] += other.m[1][0];
    m[1][1] += other.m[1][1];
    m[1][2] += other.m[1][2];
    m[1][3] += other.m[1][3];
    m[2][0] += other.m[2][0];
    m[2][1] += other.m[2][1];
    m[2][2] += other.m[2][2];
    m[2][3] += other.m[2][3];
    m[3][0] += other.m[3][0];
    m[3][1] += other.m[3][1];
    m[3][2] += other.m[3][2];
    m[3][3] += other.m[3][3];
    flagBits = General;
    return *this;
}

inline QMatrix4x4& QMatrix4x4::operator-=(const QMatrix4x4& other)
{
    m[0][0] -= other.m[0][0];
    m[0][1] -= other.m[0][1];
    m[0][2] -= other.m[0][2];
    m[0][3] -= other.m[0][3];
    m[1][0] -= other.m[1][0];
    m[1][1] -= other.m[1][1];
    m[1][2] -= other.m[1][2];
    m[1][3] -= other.m[1][3];
    m[2][0] -= other.m[2][0];
    m[2][1] -= other.m[2][1];
    m[2][2] -= other.m[2][2];
    m[2][3] -= other.m[2][3];
    m[3][0] -= other.m[3][0];
    m[3][1] -= other.m[3][1];
    m[3][2] -= other.m[3][2];
    m[3][3] -= other.m[3][3];
    flagBits = General;
    return *this;
}

inline QMatrix4x4& QMatrix4x4::operator*=(const QMatrix4x4& other)
{
    if (flagBits == Identity) {
        *this = other;
        return *this;
    } else if (other.flagBits == Identity) {
        return *this;
    } else {
        *this = *this * other;
        return *this;
    }
}

inline QMatrix4x4& QMatrix4x4::operator*=(qreal factor)
{
    m[0][0] *= factor;
    m[0][1] *= factor;
    m[0][2] *= factor;
    m[0][3] *= factor;
    m[1][0] *= factor;
    m[1][1] *= factor;
    m[1][2] *= factor;
    m[1][3] *= factor;
    m[2][0] *= factor;
    m[2][1] *= factor;
    m[2][2] *= factor;
    m[2][3] *= factor;
    m[3][0] *= factor;
    m[3][1] *= factor;
    m[3][2] *= factor;
    m[3][3] *= factor;
    flagBits = General;
    return *this;
}

inline bool QMatrix4x4::operator==(const QMatrix4x4& other) const
{
    return m[0][0] == other.m[0][0] &&
           m[0][1] == other.m[0][1] &&
           m[0][2] == other.m[0][2] &&
           m[0][3] == other.m[0][3] &&
           m[1][0] == other.m[1][0] &&
           m[1][1] == other.m[1][1] &&
           m[1][2] == other.m[1][2] &&
           m[1][3] == other.m[1][3] &&
           m[2][0] == other.m[2][0] &&
           m[2][1] == other.m[2][1] &&
           m[2][2] == other.m[2][2] &&
           m[2][3] == other.m[2][3] &&
           m[3][0] == other.m[3][0] &&
           m[3][1] == other.m[3][1] &&
           m[3][2] == other.m[3][2] &&
           m[3][3] == other.m[3][3];
}

inline bool QMatrix4x4::operator!=(const QMatrix4x4& other) const
{
    return m[0][0] != other.m[0][0] ||
           m[0][1] != other.m[0][1] ||
           m[0][2] != other.m[0][2] ||
           m[0][3] != other.m[0][3] ||
           m[1][0] != other.m[1][0] ||
           m[1][1] != other.m[1][1] ||
           m[1][2] != other.m[1][2] ||
           m[1][3] != other.m[1][3] ||
           m[2][0] != other.m[2][0] ||
           m[2][1] != other.m[2][1] ||
           m[2][2] != other.m[2][2] ||
           m[2][3] != other.m[2][3] ||
           m[3][0] != other.m[3][0] ||
           m[3][1] != other.m[3][1] ||
           m[3][2] != other.m[3][2] ||
           m[3][3] != other.m[3][3];
}

inline QMatrix4x4 operator+(const QMatrix4x4& m1, const QMatrix4x4& m2)
{
    QMatrix4x4 m(1);
    m.m[0][0] = m1.m[0][0] + m2.m[0][0];
    m.m[0][1] = m1.m[0][1] + m2.m[0][1];
    m.m[0][2] = m1.m[0][2] + m2.m[0][2];
    m.m[0][3] = m1.m[0][3] + m2.m[0][3];
    m.m[1][0] = m1.m[1][0] + m2.m[1][0];
    m.m[1][1] = m1.m[1][1] + m2.m[1][1];
    m.m[1][2] = m1.m[1][2] + m2.m[1][2];
    m.m[1][3] = m1.m[1][3] + m2.m[1][3];
    m.m[2][0] = m1.m[2][0] + m2.m[2][0];
    m.m[2][1] = m1.m[2][1] + m2.m[2][1];
    m.m[2][2] = m1.m[2][2] + m2.m[2][2];
    m.m[2][3] = m1.m[2][3] + m2.m[2][3];
    m.m[3][0] = m1.m[3][0] + m2.m[3][0];
    m.m[3][1] = m1.m[3][1] + m2.m[3][1];
    m.m[3][2] = m1.m[3][2] + m2.m[3][2];
    m.m[3][3] = m1.m[3][3] + m2.m[3][3];
    return m;
}

inline QMatrix4x4 operator-(const QMatrix4x4& m1, const QMatrix4x4& m2)
{
    QMatrix4x4 m(1);
    m.m[0][0] = m1.m[0][0] - m2.m[0][0];
    m.m[0][1] = m1.m[0][1] - m2.m[0][1];
    m.m[0][2] = m1.m[0][2] - m2.m[0][2];
    m.m[0][3] = m1.m[0][3] - m2.m[0][3];
    m.m[1][0] = m1.m[1][0] - m2.m[1][0];
    m.m[1][1] = m1.m[1][1] - m2.m[1][1];
    m.m[1][2] = m1.m[1][2] - m2.m[1][2];
    m.m[1][3] = m1.m[1][3] - m2.m[1][3];
    m.m[2][0] = m1.m[2][0] - m2.m[2][0];
    m.m[2][1] = m1.m[2][1] - m2.m[2][1];
    m.m[2][2] = m1.m[2][2] - m2.m[2][2];
    m.m[2][3] = m1.m[2][3] - m2.m[2][3];
    m.m[3][0] = m1.m[3][0] - m2.m[3][0];
    m.m[3][1] = m1.m[3][1] - m2.m[3][1];
    m.m[3][2] = m1.m[3][2] - m2.m[3][2];
    m.m[3][3] = m1.m[3][3] - m2.m[3][3];
    return m;
}

inline QMatrix4x4 operator*(const QMatrix4x4& m1, const QMatrix4x4& m2)
{
    if (m1.flagBits == QMatrix4x4::Identity)
        return m2;
    else if (m2.flagBits == QMatrix4x4::Identity)
        return m1;

    QMatrix4x4 m(1);
    m.m[0][0] = m1.m[0][0] * m2.m[0][0] +
                m1.m[1][0] * m2.m[0][1] +
                m1.m[2][0] * m2.m[0][2] +
                m1.m[3][0] * m2.m[0][3];
    m.m[0][1] = m1.m[0][1] * m2.m[0][0] +
                m1.m[1][1] * m2.m[0][1] +
                m1.m[2][1] * m2.m[0][2] +
                m1.m[3][1] * m2.m[0][3];
    m.m[0][2] = m1.m[0][2] * m2.m[0][0] +
                m1.m[1][2] * m2.m[0][1] +
                m1.m[2][2] * m2.m[0][2] +
                m1.m[3][2] * m2.m[0][3];
    m.m[0][3] = m1.m[0][3] * m2.m[0][0] +
                m1.m[1][3] * m2.m[0][1] +
                m1.m[2][3] * m2.m[0][2] +
                m1.m[3][3] * m2.m[0][3];
    m.m[1][0] = m1.m[0][0] * m2.m[1][0] +
                m1.m[1][0] * m2.m[1][1] +
                m1.m[2][0] * m2.m[1][2] +
                m1.m[3][0] * m2.m[1][3];
    m.m[1][1] = m1.m[0][1] * m2.m[1][0] +
                m1.m[1][1] * m2.m[1][1] +
                m1.m[2][1] * m2.m[1][2] +
                m1.m[3][1] * m2.m[1][3];
    m.m[1][2] = m1.m[0][2] * m2.m[1][0] +
                m1.m[1][2] * m2.m[1][1] +
                m1.m[2][2] * m2.m[1][2] +
                m1.m[3][2] * m2.m[1][3];
    m.m[1][3] = m1.m[0][3] * m2.m[1][0] +
                m1.m[1][3] * m2.m[1][1] +
                m1.m[2][3] * m2.m[1][2] +
                m1.m[3][3] * m2.m[1][3];
    m.m[2][0] = m1.m[0][0] * m2.m[2][0] +
                m1.m[1][0] * m2.m[2][1] +
                m1.m[2][0] * m2.m[2][2] +
                m1.m[3][0] * m2.m[2][3];
    m.m[2][1] = m1.m[0][1] * m2.m[2][0] +
                m1.m[1][1] * m2.m[2][1] +
                m1.m[2][1] * m2.m[2][2] +
                m1.m[3][1] * m2.m[2][3];
    m.m[2][2] = m1.m[0][2] * m2.m[2][0] +
                m1.m[1][2] * m2.m[2][1] +
                m1.m[2][2] * m2.m[2][2] +
                m1.m[3][2] * m2.m[2][3];
    m.m[2][3] = m1.m[0][3] * m2.m[2][0] +
                m1.m[1][3] * m2.m[2][1] +
                m1.m[2][3] * m2.m[2][2] +
                m1.m[3][3] * m2.m[2][3];
    m.m[3][0] = m1.m[0][0] * m2.m[3][0] +
                m1.m[1][0] * m2.m[3][1] +
                m1.m[2][0] * m2.m[3][2] +
                m1.m[3][0] * m2.m[3][3];
    m.m[3][1] = m1.m[0][1] * m2.m[3][0] +
                m1.m[1][1] * m2.m[3][1] +
                m1.m[2][1] * m2.m[3][2] +
                m1.m[3][1] * m2.m[3][3];
    m.m[3][2] = m1.m[0][2] * m2.m[3][0] +
                m1.m[1][2] * m2.m[3][1] +
                m1.m[2][2] * m2.m[3][2] +
                m1.m[3][2] * m2.m[3][3];
    m.m[3][3] = m1.m[0][3] * m2.m[3][0] +
                m1.m[1][3] * m2.m[3][1] +
                m1.m[2][3] * m2.m[3][2] +
                m1.m[3][3] * m2.m[3][3];
    return m;
}

#ifndef QT_NO_VECTOR3D

inline QVector3D operator*(const QVector3D& vector, const QMatrix4x4& matrix)
{
    float x, y, z, w;
    x = vector.xp * matrix.m[0][0] +
        vector.yp * matrix.m[0][1] +
        vector.zp * matrix.m[0][2] +
        matrix.m[0][3];
    y = vector.xp * matrix.m[1][0] +
        vector.yp * matrix.m[1][1] +
        vector.zp * matrix.m[1][2] +
        matrix.m[1][3];
    z = vector.xp * matrix.m[2][0] +
        vector.yp * matrix.m[2][1] +
        vector.zp * matrix.m[2][2] +
        matrix.m[2][3];
    w = vector.xp * matrix.m[3][0] +
        vector.yp * matrix.m[3][1] +
        vector.zp * matrix.m[3][2] +
        matrix.m[3][3];
    if (w == 1.0f)
        return QVector3D(x, y, z, 1);
    else
        return QVector3D(x / w, y / w, z / w, 1);
}

inline QVector3D operator*(const QMatrix4x4& matrix, const QVector3D& vector)
{
    float x, y, z, w;
    if (matrix.flagBits == QMatrix4x4::Identity) {
        return vector;
    } else if (matrix.flagBits == QMatrix4x4::Translation) {
        return QVector3D(vector.xp + matrix.m[3][0],
                         vector.yp + matrix.m[3][1],
                         vector.zp + matrix.m[3][2], 1);
    } else if (matrix.flagBits ==
                    (QMatrix4x4::Translation | QMatrix4x4::Scale)) {
        return QVector3D(vector.xp * matrix.m[0][0] + matrix.m[3][0],
                         vector.yp * matrix.m[1][1] + matrix.m[3][1],
                         vector.zp * matrix.m[2][2] + matrix.m[3][2], 1);
    } else if (matrix.flagBits == QMatrix4x4::Scale) {
        return QVector3D(vector.xp * matrix.m[0][0],
                         vector.yp * matrix.m[1][1],
                         vector.zp * matrix.m[2][2], 1);
    } else {
        x = vector.xp * matrix.m[0][0] +
            vector.yp * matrix.m[1][0] +
            vector.zp * matrix.m[2][0] +
            matrix.m[3][0];
        y = vector.xp * matrix.m[0][1] +
            vector.yp * matrix.m[1][1] +
            vector.zp * matrix.m[2][1] +
            matrix.m[3][1];
        z = vector.xp * matrix.m[0][2] +
            vector.yp * matrix.m[1][2] +
            vector.zp * matrix.m[2][2] +
            matrix.m[3][2];
        w = vector.xp * matrix.m[0][3] +
            vector.yp * matrix.m[1][3] +
            vector.zp * matrix.m[2][3] +
            matrix.m[3][3];
        if (w == 1.0f)
            return QVector3D(x, y, z, 1);
        else
            return QVector3D(x / w, y / w, z / w, 1);
    }
}

#endif

#ifndef QT_NO_VECTOR4D

inline QVector4D operator*(const QVector4D& vector, const QMatrix4x4& matrix)
{
    float x, y, z, w;
    x = vector.xp * matrix.m[0][0] +
        vector.yp * matrix.m[0][1] +
        vector.zp * matrix.m[0][2] +
        vector.wp * matrix.m[0][3];
    y = vector.xp * matrix.m[1][0] +
        vector.yp * matrix.m[1][1] +
        vector.zp * matrix.m[1][2] +
        vector.wp * matrix.m[1][3];
    z = vector.xp * matrix.m[2][0] +
        vector.yp * matrix.m[2][1] +
        vector.zp * matrix.m[2][2] +
        vector.wp * matrix.m[2][3];
    w = vector.xp * matrix.m[3][0] +
        vector.yp * matrix.m[3][1] +
        vector.zp * matrix.m[3][2] +
        vector.wp * matrix.m[3][3];
    return QVector4D(x, y, z, w, 1);
}

inline QVector4D operator*(const QMatrix4x4& matrix, const QVector4D& vector)
{
    float x, y, z, w;
    x = vector.xp * matrix.m[0][0] +
        vector.yp * matrix.m[1][0] +
        vector.zp * matrix.m[2][0] +
        vector.wp * matrix.m[3][0];
    y = vector.xp * matrix.m[0][1] +
        vector.yp * matrix.m[1][1] +
        vector.zp * matrix.m[2][1] +
        vector.wp * matrix.m[3][1];
    z = vector.xp * matrix.m[0][2] +
        vector.yp * matrix.m[1][2] +
        vector.zp * matrix.m[2][2] +
        vector.wp * matrix.m[3][2];
    w = vector.xp * matrix.m[0][3] +
        vector.yp * matrix.m[1][3] +
        vector.zp * matrix.m[2][3] +
        vector.wp * matrix.m[3][3];
    return QVector4D(x, y, z, w, 1);
}

#endif

inline QPoint operator*(const QPoint& point, const QMatrix4x4& matrix)
{
    float xin, yin;
    float x, y, w;
    xin = point.x();
    yin = point.y();
    x = xin * matrix.m[0][0] +
        yin * matrix.m[0][1] +
        matrix.m[0][3];
    y = xin * matrix.m[1][0] +
        yin * matrix.m[1][1] +
        matrix.m[1][3];
    w = xin * matrix.m[3][0] +
        yin * matrix.m[3][1] +
        matrix.m[3][3];
    if (w == 1.0f)
        return QPoint(qRound(x), qRound(y));
    else
        return QPoint(qRound(x / w), qRound(y / w));
}

inline QPointF operator*(const QPointF& point, const QMatrix4x4& matrix)
{
    float xin, yin;
    float x, y, w;
    xin = point.x();
    yin = point.y();
    x = xin * matrix.m[0][0] +
        yin * matrix.m[0][1] +
        matrix.m[0][3];
    y = xin * matrix.m[1][0] +
        yin * matrix.m[1][1] +
        matrix.m[1][3];
    w = xin * matrix.m[3][0] +
        yin * matrix.m[3][1] +
        matrix.m[3][3];
    if (w == 1.0f) {
        return QPointF(qreal(x), qreal(y));
    } else {
        return QPointF(qreal(x / w), qreal(y / w));
    }
}

inline QPoint operator*(const QMatrix4x4& matrix, const QPoint& point)
{
    float xin, yin;
    float x, y, w;
    xin = point.x();
    yin = point.y();
    if (matrix.flagBits == QMatrix4x4::Identity) {
        return point;
    } else if (matrix.flagBits == QMatrix4x4::Translation) {
        return QPoint(qRound(xin + matrix.m[3][0]),
                      qRound(yin + matrix.m[3][1]));
    } else if (matrix.flagBits ==
                    (QMatrix4x4::Translation | QMatrix4x4::Scale)) {
        return QPoint(qRound(xin * matrix.m[0][0] + matrix.m[3][0]),
                      qRound(yin * matrix.m[1][1] + matrix.m[3][1]));
    } else if (matrix.flagBits == QMatrix4x4::Scale) {
        return QPoint(qRound(xin * matrix.m[0][0]),
                      qRound(yin * matrix.m[1][1]));
    } else {
        x = xin * matrix.m[0][0] +
            yin * matrix.m[1][0] +
            matrix.m[3][0];
        y = xin * matrix.m[0][1] +
            yin * matrix.m[1][1] +
            matrix.m[3][1];
        w = xin * matrix.m[0][3] +
            yin * matrix.m[1][3] +
            matrix.m[3][3];
        if (w == 1.0f)
            return QPoint(qRound(x), qRound(y));
        else
            return QPoint(qRound(x / w), qRound(y / w));
    }
}

inline QPointF operator*(const QMatrix4x4& matrix, const QPointF& point)
{
    float xin, yin;
    float x, y, w;
    xin = point.x();
    yin = point.y();
    if (matrix.flagBits == QMatrix4x4::Identity) {
        return point;
    } else if (matrix.flagBits == QMatrix4x4::Translation) {
        return QPointF(xin + matrix.m[3][0],
                       yin + matrix.m[3][1]);
    } else if (matrix.flagBits ==
                    (QMatrix4x4::Translation | QMatrix4x4::Scale)) {
        return QPointF(xin * matrix.m[0][0] + matrix.m[3][0],
                       yin * matrix.m[1][1] + matrix.m[3][1]);
    } else if (matrix.flagBits == QMatrix4x4::Scale) {
        return QPointF(xin * matrix.m[0][0],
                       yin * matrix.m[1][1]);
    } else {
        x = xin * matrix.m[0][0] +
            yin * matrix.m[1][0] +
            matrix.m[3][0];
        y = xin * matrix.m[0][1] +
            yin * matrix.m[1][1] +
            matrix.m[3][1];
        w = xin * matrix.m[0][3] +
            yin * matrix.m[1][3] +
            matrix.m[3][3];
        if (w == 1.0f) {
            return QPointF(qreal(x), qreal(y));
        } else {
            return QPointF(qreal(x / w), qreal(y / w));
        }
    }
}

inline QMatrix4x4 operator-(const QMatrix4x4& matrix)
{
    QMatrix4x4 m(1);
    m.m[0][0] = -matrix.m[0][0];
    m.m[0][1] = -matrix.m[0][1];
    m.m[0][2] = -matrix.m[0][2];
    m.m[0][3] = -matrix.m[0][3];
    m.m[1][0] = -matrix.m[1][0];
    m.m[1][1] = -matrix.m[1][1];
    m.m[1][2] = -matrix.m[1][2];
    m.m[1][3] = -matrix.m[1][3];
    m.m[2][0] = -matrix.m[2][0];
    m.m[2][1] = -matrix.m[2][1];
    m.m[2][2] = -matrix.m[2][2];
    m.m[2][3] = -matrix.m[2][3];
    m.m[3][0] = -matrix.m[3][0];
    m.m[3][1] = -matrix.m[3][1];
    m.m[3][2] = -matrix.m[3][2];
    m.m[3][3] = -matrix.m[3][3];
    return m;
}

inline QMatrix4x4 operator*(qreal factor, const QMatrix4x4& matrix)
{
    QMatrix4x4 m(1);
    m.m[0][0] = matrix.m[0][0] * factor;
    m.m[0][1] = matrix.m[0][1] * factor;
    m.m[0][2] = matrix.m[0][2] * factor;
    m.m[0][3] = matrix.m[0][3] * factor;
    m.m[1][0] = matrix.m[1][0] * factor;
    m.m[1][1] = matrix.m[1][1] * factor;
    m.m[1][2] = matrix.m[1][2] * factor;
    m.m[1][3] = matrix.m[1][3] * factor;
    m.m[2][0] = matrix.m[2][0] * factor;
    m.m[2][1] = matrix.m[2][1] * factor;
    m.m[2][2] = matrix.m[2][2] * factor;
    m.m[2][3] = matrix.m[2][3] * factor;
    m.m[3][0] = matrix.m[3][0] * factor;
    m.m[3][1] = matrix.m[3][1] * factor;
    m.m[3][2] = matrix.m[3][2] * factor;
    m.m[3][3] = matrix.m[3][3] * factor;
    return m;
}

inline QMatrix4x4 operator*(const QMatrix4x4& matrix, qreal factor)
{
    QMatrix4x4 m(1);
    m.m[0][0] = matrix.m[0][0] * factor;
    m.m[0][1] = matrix.m[0][1] * factor;
    m.m[0][2] = matrix.m[0][2] * factor;
    m.m[0][3] = matrix.m[0][3] * factor;
    m.m[1][0] = matrix.m[1][0] * factor;
    m.m[1][1] = matrix.m[1][1] * factor;
    m.m[1][2] = matrix.m[1][2] * factor;
    m.m[1][3] = matrix.m[1][3] * factor;
    m.m[2][0] = matrix.m[2][0] * factor;
    m.m[2][1] = matrix.m[2][1] * factor;
    m.m[2][2] = matrix.m[2][2] * factor;
    m.m[2][3] = matrix.m[2][3] * factor;
    m.m[3][0] = matrix.m[3][0] * factor;
    m.m[3][1] = matrix.m[3][1] * factor;
    m.m[3][2] = matrix.m[3][2] * factor;
    m.m[3][3] = matrix.m[3][3] * factor;
    return m;
}

inline bool qFuzzyCompare(const QMatrix4x4& m1, const QMatrix4x4& m2)
{
    return qFuzzyCompare(m1.m[0][0], m2.m[0][0]) &&
           qFuzzyCompare(m1.m[0][1], m2.m[0][1]) &&
           qFuzzyCompare(m1.m[0][2], m2.m[0][2]) &&
           qFuzzyCompare(m1.m[0][3], m2.m[0][3]) &&
           qFuzzyCompare(m1.m[1][0], m2.m[1][0]) &&
           qFuzzyCompare(m1.m[1][1], m2.m[1][1]) &&
           qFuzzyCompare(m1.m[1][2], m2.m[1][2]) &&
           qFuzzyCompare(m1.m[1][3], m2.m[1][3]) &&
           qFuzzyCompare(m1.m[2][0], m2.m[2][0]) &&
           qFuzzyCompare(m1.m[2][1], m2.m[2][1]) &&
           qFuzzyCompare(m1.m[2][2], m2.m[2][2]) &&
           qFuzzyCompare(m1.m[2][3], m2.m[2][3]) &&
           qFuzzyCompare(m1.m[3][0], m2.m[3][0]) &&
           qFuzzyCompare(m1.m[3][1], m2.m[3][1]) &&
           qFuzzyCompare(m1.m[3][2], m2.m[3][2]) &&
           qFuzzyCompare(m1.m[3][3], m2.m[3][3]);
}

inline QPoint QMatrix4x4::map(const QPoint& point) const
{
    return *this * point;
}

inline QPointF QMatrix4x4::map(const QPointF& point) const
{
    return *this * point;
}

#ifndef QT_NO_VECTOR3D

inline QVector3D QMatrix4x4::map(const QVector3D& point) const
{
    return *this * point;
}

#endif

#ifndef QT_NO_VECTOR4D

inline QVector4D QMatrix4x4::map(const QVector4D& point) const
{
    return *this * point;
}

#endif

inline float *QMatrix4x4::data()
{
    // We have to assume that the caller will modify the matrix elements,
    // so we flip it over to "General" mode.
    flagBits = General;
    return m[0];
}

#ifndef QT_NO_DEBUG_STREAM
Q_GUI_EXPORT QDebug operator<<(QDebug dbg, const QMatrix4x4 &m);
#endif

template <int N, int M>
QMatrix4x4 qGenericMatrixToMatrix4x4(const QGenericMatrix<N, M, qreal, float>& matrix)
{
    return QMatrix4x4(matrix.constData(), N, M);
}

template <int N, int M>
QGenericMatrix<N, M, qreal, float> qGenericMatrixFromMatrix4x4(const QMatrix4x4& matrix)
{
    QGenericMatrix<N, M, qreal, float> result;
    const float *m = matrix.constData();
    float *values = result.data();
    for (int col = 0; col < N; ++col) {
        for (int row = 0; row < M; ++row) {
            if (col < 4 && row < 4)
                values[col * M + row] = m[col * 4 + row];
            else if (col == row)
                values[col * M + row] = 1.0f;
            else
                values[col * M + row] = 0.0f;
        }
    }
    return result;
}

#endif

QT_END_NAMESPACE

#ifndef QT_NO_MATRIX4X4
Q_DECLARE_METATYPE(QMatrix4x4)
#endif

QT_END_HEADER

#endif

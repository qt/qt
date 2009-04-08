/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef VECTOR_H
#define VECTOR_H

#include <cassert>
#include <cmath>
#include <iostream>

namespace gfx
{

template<class T, int n>
struct Vector
{
    // Keep the Vector struct a plain old data (POD) struct by avoiding constructors

    static Vector vector(T x)
    {
        Vector result;
        for (int i = 0; i < n; ++i)
            result.v[i] = x;
        return result;
    }

    // Use only for 2D vectors
    static Vector vector(T x, T y)
    {
        assert(n == 2);
        Vector result;
        result.v[0] = x;
        result.v[1] = y;
        return result;
    }

    // Use only for 3D vectors
    static Vector vector(T x, T y, T z)
    {
        assert(n == 3);
        Vector result;
        result.v[0] = x;
        result.v[1] = y;
        result.v[2] = z;
        return result;
    }

    // Use only for 4D vectors
    static Vector vector(T x, T y, T z, T w)
    {
        assert(n == 4);
        Vector result;
        result.v[0] = x;
        result.v[1] = y;
        result.v[2] = z;
        result.v[3] = w;
        return result;
    }

    // Pass 'n' arguments to this function.
    static Vector vector(T *v)
    {
        Vector result;
        for (int i = 0; i < n; ++i)
            result.v[i] = v[i];
        return result;
    }

    T &operator [] (int i) {return v[i];}
    T operator [] (int i) const {return v[i];}

#define VECTOR_BINARY_OP(op, arg, rhs)      \
    Vector operator op (arg) const          \
    {                                       \
        Vector result;                      \
        for (int i = 0; i < n; ++i)         \
            result.v[i] = v[i] op rhs;      \
        return result;                      \
    }

    VECTOR_BINARY_OP(+, const Vector &u, u.v[i])
    VECTOR_BINARY_OP(-, const Vector &u, u.v[i])
    VECTOR_BINARY_OP(*, const Vector &u, u.v[i])
    VECTOR_BINARY_OP(/, const Vector &u, u.v[i])
    VECTOR_BINARY_OP(+, T s, s)
    VECTOR_BINARY_OP(-, T s, s)
    VECTOR_BINARY_OP(*, T s, s)
    VECTOR_BINARY_OP(/, T s, s)
#undef VECTOR_BINARY_OP

    Vector operator - () const
    {
        Vector result;
        for (int i = 0; i < n; ++i)
            result.v[i] = -v[i];
        return result;
    }

#define VECTOR_ASSIGN_OP(op, arg, rhs)      \
    Vector &operator op (arg)               \
    {                                       \
        for (int i = 0; i < n; ++i)         \
            v[i] op rhs;                    \
        return *this;                       \
    }

    VECTOR_ASSIGN_OP(+=, const Vector &u, u.v[i])
    VECTOR_ASSIGN_OP(-=, const Vector &u, u.v[i])
    VECTOR_ASSIGN_OP(=, T s, s)
    VECTOR_ASSIGN_OP(*=, T s, s)
    VECTOR_ASSIGN_OP(/=, T s, s)
#undef VECTOR_ASSIGN_OP

    static T dot(const Vector &u, const Vector &v)
    {
        T sum(0);
        for (int i = 0; i < n; ++i)
            sum += u.v[i] * v.v[i];
        return sum;
    }

    static Vector cross(const Vector &u, const Vector &v)
    {
        assert(n == 3);
        return vector(u.v[1] * v.v[2] - u.v[2] * v.v[1],
                      u.v[2] * v.v[0] - u.v[0] * v.v[2],
                      u.v[0] * v.v[1] - u.v[1] * v.v[0]);
    }

    T sqrNorm() const
    {
        return dot(*this, *this);
    }

    // requires floating point type T
    void normalize()
    {
        T s = sqrNorm();
        if (s != 0)
            *this /= sqrt(s);
    }

    // requires floating point type T
    Vector normalized() const
    {
        T s = sqrNorm();
        if (s == 0)
            return *this;
        return *this / sqrt(s);
    }

    T *bits() {return v;}
    const T *bits() const {return v;}

    T v[n];
};

#define SCALAR_VECTOR_BINARY_OP(op)                     \
template<class T, int n>                                \
Vector<T, n> operator op (T s, const Vector<T, n>& u)   \
{                                                       \
    Vector<T, n> result;                                \
    for (int i = 0; i < n; ++i)                         \
        result[i] = s op u[i];                          \
    return result;                                      \
}

SCALAR_VECTOR_BINARY_OP(+)
SCALAR_VECTOR_BINARY_OP(-)
SCALAR_VECTOR_BINARY_OP(*)
SCALAR_VECTOR_BINARY_OP(/)
#undef SCALAR_VECTOR_BINARY_OP

template<class T, int n>
std::ostream &operator << (std::ostream &os, const Vector<T, n> &v)
{
    assert(n > 0);
    os << "[" << v[0];
    for (int i = 1; i < n; ++i)
        os << ", " << v[i];
    os << "]";
    return os;
}

typedef Vector<float, 2> Vector2f;
typedef Vector<float, 3> Vector3f;
typedef Vector<float, 4> Vector4f;

template<class T, int rows, int cols>
struct Matrix
{
    // Keep the Matrix struct a plain old data (POD) struct by avoiding constructors

    static Matrix matrix(T x)
    {
        Matrix result;
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j)
                result.v[i][j] = x;
        }
        return result;
    }

    static Matrix matrix(T *m)
    {
        Matrix result;
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                result.v[i][j] = *m;
                ++m;
            }
        }
        return result;
    }

    T &operator () (int i, int j) {return v[i][j];}
    T operator () (int i, int j) const {return v[i][j];}
    Vector<T, cols> &operator [] (int i) {return v[i];}
    const Vector<T, cols> &operator [] (int i) const {return v[i];}

    // TODO: operators, methods

    Vector<T, rows> operator * (const Vector<T, cols> &u) const
    {
        Vector<T, rows> result;
        for (int i = 0; i < rows; ++i)
            result[i] = Vector<T, cols>::dot(v[i], u);
        return result;
    }

    template<int k>
    Matrix<T, rows, k> operator * (const Matrix<T, cols, k> &m)
    {
        Matrix<T, rows, k> result;
        for (int i = 0; i < rows; ++i)
            result[i] = v[i] * m;
        return result;
    }

    T* bits() {return reinterpret_cast<T *>(this);}
    const T* bits() const {return reinterpret_cast<const T *>(this);}

    // Simple Gauss elimination.
    // TODO: Optimize and improve stability.
    Matrix inverse(bool *ok = 0) const
    {
        assert(rows == cols);
        Matrix rhs = identity();
        Matrix lhs(*this);
        T temp;
        // Down
        for (int i = 0; i < rows; ++i) {
            // Pivoting
            int pivot = i;
            for (int j = i; j < rows; ++j) {
                if (qAbs(lhs(j, i)) > lhs(pivot, i))
                    pivot = j;
            }
			// TODO: fuzzy compare.
            if (lhs(pivot, i) == T(0)) {
                if (ok)
                    *ok = false;
                return rhs;
            }
            if (pivot != i) {
                for (int j = i; j < cols; ++j) {
                    temp = lhs(pivot, j);
                    lhs(pivot, j) = lhs(i, j);
                    lhs(i, j) = temp;
                }
                for (int j = 0; j < cols; ++j) {
                    temp = rhs(pivot, j);
                    rhs(pivot, j) = rhs(i, j);
                    rhs(i, j) = temp;
                }
            }

            // Normalize i-th row
            rhs[i] /= lhs(i, i);
            for (int j = cols - 1; j > i; --j)
                lhs(i, j) /= lhs(i, i);

			// Eliminate non-zeros in i-th column below the i-th row.
            for (int j = i + 1; j < rows; ++j) {
                rhs[j] -= lhs(j, i) * rhs[i];
                for (int k = i + 1; k < cols; ++k)
                    lhs(j, k) -= lhs(j, i) * lhs(i, k);
            }
        }
        // Up
        for (int i = rows - 1; i > 0; --i) {
            for (int j = i - 1; j >= 0; --j)
                rhs[j] -= lhs(j, i) * rhs[i];
        }
        if (ok)
            *ok = true;
        return rhs;
    }

    Matrix<T, cols, rows> transpose() const
    {
        Matrix<T, cols, rows> result;
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j)
                result.v[j][i] = v[i][j];
        }
        return result;
    }

    static Matrix identity()
    {
        Matrix result = matrix(T(0));
        for (int i = 0; i < rows && i < cols; ++i)
            result.v[i][i] = T(1);
        return result;
    }

    Vector<T, cols> v[rows];
};

template<class T, int rows, int cols>
Vector<T, cols> operator * (const Vector<T, rows> &u, const Matrix<T, rows, cols> &m)
{
    Vector<T, cols> result = Vector<T, cols>::vector(T(0));
    for (int i = 0; i < rows; ++i)
        result += m[i] * u[i];
    return result;
}

template<class T, int rows, int cols>
std::ostream &operator << (std::ostream &os, const Matrix<T, rows, cols> &m)
{
    assert(rows > 0);
    os << "[" << m[0];
    for (int i = 1; i < rows; ++i)
        os << ", " << m[i];
    os << "]";
    return os;
}


typedef Matrix<float, 2, 2> Matrix2x2f;
typedef Matrix<float, 3, 3> Matrix3x3f;
typedef Matrix<float, 4, 4> Matrix4x4f;

template<class T>
struct Quaternion
{
    // Keep the Quaternion struct a plain old data (POD) struct by avoiding constructors

    static Quaternion quaternion(T s, T x, T y, T z)
    {
        Quaternion result;
        result.scalar = s;
        result.vector[0] = x;
        result.vector[1] = y;
        result.vector[2] = z;
        return result;
    }

    static Quaternion quaternion(T s, const Vector<T, 3> &v)
    {
        Quaternion result;
        result.scalar = s;
        result.vector = v;
        return result;
    }

    static Quaternion identity()
    {
        return quaternion(T(1), T(0), T(0), T(0));
    }

    // assumes that all the elements are packed tightly
    T& operator [] (int i) {return reinterpret_cast<T *>(this)[i];}
    T operator [] (int i) const {return reinterpret_cast<const T *>(this)[i];}

#define QUATERNION_BINARY_OP(op, arg, rhs)  \
    Quaternion operator op (arg) const      \
    {                                       \
        Quaternion result;                  \
        for (int i = 0; i < 4; ++i)         \
            result[i] = (*this)[i] op rhs;  \
        return result;                      \
    }

    QUATERNION_BINARY_OP(+, const Quaternion &q, q[i])
    QUATERNION_BINARY_OP(-, const Quaternion &q, q[i])
    QUATERNION_BINARY_OP(*, T s, s)
    QUATERNION_BINARY_OP(/, T s, s)
#undef QUATERNION_BINARY_OP

    Quaternion operator - () const
    {
        return Quaternion(-scalar, -vector);
    }

    Quaternion operator * (const Quaternion &q) const
    {
        Quaternion result;
        result.scalar = scalar * q.scalar - Vector<T, 3>::dot(vector, q.vector);
        result.vector = scalar * q.vector + vector * q.scalar + Vector<T, 3>::cross(vector, q.vector);
        return result;
    }

    Quaternion operator * (const Vector<T, 3> &v) const
    {
        Quaternion result;
        result.scalar = -Vector<T, 3>::dot(vector, v);
        result.vector = scalar * v + Vector<T, 3>::cross(vector, v);
        return result;
    }

    friend Quaternion operator * (const Vector<T, 3> &v, const Quaternion &q)
    {
        Quaternion result;
        result.scalar = -Vector<T, 3>::dot(v, q.vector);
        result.vector = v * q.scalar + Vector<T, 3>::cross(v, q.vector);
        return result;
    }

#define QUATERNION_ASSIGN_OP(op, arg, rhs)  \
    Quaternion &operator op (arg)           \
    {                                       \
        for (int i = 0; i < 4; ++i)         \
            (*this)[i] op rhs;              \
        return *this;                       \
    }

    QUATERNION_ASSIGN_OP(+=, const Quaternion &q, q[i])
    QUATERNION_ASSIGN_OP(-=, const Quaternion &q, q[i])
    QUATERNION_ASSIGN_OP(=, T s, s)
    QUATERNION_ASSIGN_OP(*=, T s, s)
    QUATERNION_ASSIGN_OP(/=, T s, s)
#undef QUATERNION_ASSIGN_OP

    Quaternion& operator *= (const Quaternion &q)
    {
        Quaternion result;
        result.scalar = scalar * q.scalar - Vector<T, 3>::dot(vector, q.vector);
        result.vector = scalar * q.vector + vector * q.scalar + Vector<T, 3>::cross(vector, q.vector);
        return (*this = result);
    }

    Quaternion& operator *= (const Vector<T, 3> &v)
    {
        Quaternion result;
        result.scalar = -Vector<T, 3>::dot(vector, v);
        result.vector = scalar * v + Vector<T, 3>::cross(vector, v);
        return (*this = result);
    }

    Quaternion conjugate() const
    {
        return quaternion(scalar, -vector);
    }

    T sqrNorm() const
    {
        return scalar * scalar + vector.sqrNorm();
    }

    Quaternion inverse() const
    {
        return conjugate() / sqrNorm();
    }

    // requires floating point type T
    Quaternion normalized() const
    {
        T s = sqrNorm();
        if (s == 0)
            return *this;
        return *this / sqrt(s);
    }

    void matrix(Matrix<T, 3, 3>& m) const
    {
        T bb = vector[0] * vector[0];
        T cc = vector[1] * vector[1];
        T dd = vector[2] * vector[2];
        T diag = scalar * scalar - bb - cc - dd;
        T ab = scalar * vector[0];
        T ac = scalar * vector[1];
        T ad = scalar * vector[2];
        T bc = vector[0] * vector[1];
        T cd = vector[1] * vector[2];
        T bd = vector[2] * vector[0];
        m(0, 0) = diag + 2 * bb;
        m(0, 1) = 2 * (bc - ad);
        m(0, 2) = 2 * (ac + bd);
        m(1, 0) = 2 * (ad + bc);
        m(1, 1) = diag + 2 * cc;
        m(1, 2) = 2 * (cd - ab);
        m(2, 0) = 2 * (bd - ac);
        m(2, 1) = 2 * (ab + cd);
        m(2, 2) = diag + 2 * dd;
    }

    void matrix(Matrix<T, 4, 4>& m) const
    {
        T bb = vector[0] * vector[0];
        T cc = vector[1] * vector[1];
        T dd = vector[2] * vector[2];
        T diag = scalar * scalar - bb - cc - dd;
        T ab = scalar * vector[0];
        T ac = scalar * vector[1];
        T ad = scalar * vector[2];
        T bc = vector[0] * vector[1];
        T cd = vector[1] * vector[2];
        T bd = vector[2] * vector[0];
        m(0, 0) = diag + 2 * bb;
        m(0, 1) = 2 * (bc - ad);
        m(0, 2) = 2 * (ac + bd);
        m(0, 3) = 0;
        m(1, 0) = 2 * (ad + bc);
        m(1, 1) = diag + 2 * cc;
        m(1, 2) = 2 * (cd - ab);
        m(1, 3) = 0;
        m(2, 0) = 2 * (bd - ac);
        m(2, 1) = 2 * (ab + cd);
        m(2, 2) = diag + 2 * dd;
        m(2, 3) = 0;
        m(3, 0) = 0;
        m(3, 1) = 0;
        m(3, 2) = 0;
        m(3, 3) = 1;
    }

    // assumes that 'this' is normalized
    Vector<T, 3> transform(const Vector<T, 3> &v) const
    {
        Matrix<T, 3, 3> m;
        matrix(m);
        return v * m;
    }

    // assumes that all the elements are packed tightly
    T* bits() {return reinterpret_cast<T *>(this);}
    const T* bits() const {return reinterpret_cast<const T *>(this);}

    // requires floating point type T
    static Quaternion rotation(T angle, const Vector<T, 3> &unitAxis)
    {
        T s = sin(angle / 2);
        T c = cos(angle / 2);
        return quaternion(c, unitAxis * s);
    }

    T scalar;
    Vector<T, 3> vector;
};

template<class T>
Quaternion<T> operator * (T s, const Quaternion<T>& q)
{
    return Quaternion<T>::quaternion(s * q.scalar, s * q.vector);
}

typedef Quaternion<float> Quaternionf;

} // end namespace gfx

#endif

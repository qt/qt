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

#ifndef QGENERICMATRIX_H
#define QGENERICMATRIX_H

#include <QtCore/qmetatype.h>
#include <QtCore/qdebug.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

template <int N, int M, typename T, typename InnerT = T>
class QGenericMatrix
{
public:
    QGenericMatrix();
    QGenericMatrix(const QGenericMatrix<N, M, T, InnerT>& other);
    explicit QGenericMatrix(const T *values);

    T operator()(int row, int column) const;
    InnerT& operator()(int row, int column);

    bool isIdentity() const;
    void setIdentity();

    void fill(T value);

    QGenericMatrix<M, N, T, InnerT> transposed() const;

    QGenericMatrix<N, M, T, InnerT>& operator+=(const QGenericMatrix<N, M, T, InnerT>& other);
    QGenericMatrix<N, M, T, InnerT>& operator-=(const QGenericMatrix<N, M, T, InnerT>& other);
    QGenericMatrix<N, M, T, InnerT>& operator*=(T factor);
    QGenericMatrix<N, M, T, InnerT>& operator/=(T divisor);
    bool operator==(const QGenericMatrix<N, M, T, InnerT>& other) const;
    bool operator!=(const QGenericMatrix<N, M, T, InnerT>& other) const;

    void toValueArray(T *values);

    InnerT *data() { return m[0]; }
    const InnerT *data() const { return m[0]; }
    const InnerT *constData() const { return m[0]; }

#if !defined(Q_NO_TEMPLATE_FRIENDS)
    template<int NN, int MM, typename TT, typename ITT>
    friend QGenericMatrix<NN, MM, TT, ITT> operator+(const QGenericMatrix<NN, MM, TT, ITT>& m1, const QGenericMatrix<NN, MM, TT, ITT>& m2);
    template<int NN, int MM, typename TT, typename ITT>
    friend QGenericMatrix<NN, MM, TT, ITT> operator-(const QGenericMatrix<NN, MM, TT, ITT>& m1, const QGenericMatrix<NN, MM, TT, ITT>& m2);
    template<int NN, int M1, int M2, typename TT, typename ITT>
    friend QGenericMatrix<M1, M2, TT, ITT> operator*(const QGenericMatrix<NN, M2, TT, ITT>& m1, const QGenericMatrix<M1, NN, TT, ITT>& m2);
    template<int NN, int MM, typename TT, typename ITT>
    friend QGenericMatrix<NN, MM, TT, ITT> operator-(const QGenericMatrix<NN, MM, TT, ITT>& matrix);
    template<int NN, int MM, typename TT, typename ITT>
    friend QGenericMatrix<NN, MM, TT, ITT> operator*(TT factor, const QGenericMatrix<NN, MM, TT, ITT>& matrix);
    template<int NN, int MM, typename TT, typename ITT>
    friend QGenericMatrix<NN, MM, TT, ITT> operator*(const QGenericMatrix<NN, MM, TT, ITT>& matrix, TT factor);
    template<int NN, int MM, typename TT, typename ITT>
    friend QGenericMatrix<NN, MM, TT, ITT> operator/(const QGenericMatrix<NN, MM, TT, ITT>& matrix, TT divisor);

private:
#endif
    InnerT m[N][M];    // Column-major order to match OpenGL.

    QGenericMatrix(int) {}       // Construct without initializing identity matrix.

#if !defined(Q_NO_TEMPLATE_FRIENDS)
    template <int NN, int MM, typename TT, typename ITT>
    friend class QGenericMatrix;
#endif
};

template <int N, int M, typename T, typename InnerT>
Q_INLINE_TEMPLATE QGenericMatrix<N, M, T, InnerT>::QGenericMatrix()
{
    setIdentity();
}

template <int N, int M, typename T, typename InnerT>
Q_INLINE_TEMPLATE QGenericMatrix<N, M, T, InnerT>::QGenericMatrix(const QGenericMatrix<N, M, T, InnerT>& other)
{
    qMemCopy(m, other.m, sizeof(m));
}

template <int N, int M, typename T, typename InnerT>
Q_OUTOFLINE_TEMPLATE QGenericMatrix<N, M, T, InnerT>::QGenericMatrix(const T *values)
{
    for (int col = 0; col < N; ++col)
        for (int row = 0; row < M; ++row)
            m[col][row] = values[row * N + col];
}

template <int N, int M, typename T, typename InnerT>
Q_INLINE_TEMPLATE T QGenericMatrix<N, M, T, InnerT>::operator()(int row, int column) const
{
    Q_ASSERT(row >= 0 && row < M && column >= 0 && column < N);
    return T(m[column][row]);
}

template <int N, int M, typename T, typename InnerT>
Q_INLINE_TEMPLATE InnerT& QGenericMatrix<N, M, T, InnerT>::operator()(int row, int column)
{
    Q_ASSERT(row >= 0 && row < M && column >= 0 && column < N);
    return m[column][row];
}

template <int N, int M, typename T, typename InnerT>
Q_OUTOFLINE_TEMPLATE bool QGenericMatrix<N, M, T, InnerT>::isIdentity() const
{
    for (int col = 0; col < N; ++col) {
        for (int row = 0; row < M; ++row) {
            if (row == col) {
                if (m[col][row] != 1.0f)
                    return false;
            } else {
                if (m[col][row] != 0.0f)
                    return false;
            }
        }
    }
    return true;
}

template <int N, int M, typename T, typename InnerT>
Q_OUTOFLINE_TEMPLATE void QGenericMatrix<N, M, T, InnerT>::setIdentity()
{
    for (int col = 0; col < N; ++col) {
        for (int row = 0; row < M; ++row) {
            if (row == col)
                m[col][row] = 1.0f;
            else
                m[col][row] = 0.0f;
        }
    }
}

template <int N, int M, typename T, typename InnerT>
Q_OUTOFLINE_TEMPLATE void QGenericMatrix<N, M, T, InnerT>::fill(T value)
{
    for (int col = 0; col < N; ++col)
        for (int row = 0; row < M; ++row)
            m[col][row] = value;
}

template <int N, int M, typename T, typename InnerT>
Q_OUTOFLINE_TEMPLATE QGenericMatrix<M, N, T, InnerT> QGenericMatrix<N, M, T, InnerT>::transposed() const
{
    QGenericMatrix<M, N, T, InnerT> result(1);
    for (int row = 0; row < M; ++row)
        for (int col = 0; col < N; ++col)
            result.m[row][col] = m[col][row];
    return result;
}

template <int N, int M, typename T, typename InnerT>
Q_OUTOFLINE_TEMPLATE QGenericMatrix<N, M, T, InnerT>& QGenericMatrix<N, M, T, InnerT>::operator+=(const QGenericMatrix<N, M, T, InnerT>& other)
{
    for (int index = 0; index < N * M; ++index)
        m[0][index] += other.m[0][index];
    return *this;
}

template <int N, int M, typename T, typename InnerT>
Q_OUTOFLINE_TEMPLATE QGenericMatrix<N, M, T, InnerT>& QGenericMatrix<N, M, T, InnerT>::operator-=(const QGenericMatrix<N, M, T, InnerT>& other)
{
    for (int index = 0; index < N * M; ++index)
        m[0][index] -= other.m[0][index];
    return *this;
}

template <int N, int M, typename T, typename InnerT>
Q_OUTOFLINE_TEMPLATE QGenericMatrix<N, M, T, InnerT>& QGenericMatrix<N, M, T, InnerT>::operator*=(T factor)
{
    InnerT f(factor);
    for (int index = 0; index < N * M; ++index)
        m[0][index] *= f;
    return *this;
}

template <int N, int M, typename T, typename InnerT>
Q_OUTOFLINE_TEMPLATE bool QGenericMatrix<N, M, T, InnerT>::operator==(const QGenericMatrix<N, M, T, InnerT>& other) const
{
    for (int index = 0; index < N * M; ++index) {
        if (m[0][index] != other.m[0][index])
            return false;
    }
    return true;
}

template <int N, int M, typename T, typename InnerT>
Q_OUTOFLINE_TEMPLATE bool QGenericMatrix<N, M, T, InnerT>::operator!=(const QGenericMatrix<N, M, T, InnerT>& other) const
{
    for (int index = 0; index < N * M; ++index) {
        if (m[0][index] != other.m[0][index])
            return true;
    }
    return false;
}

template <int N, int M, typename T, typename InnerT>
Q_OUTOFLINE_TEMPLATE QGenericMatrix<N, M, T, InnerT>& QGenericMatrix<N, M, T, InnerT>::operator/=(T divisor)
{
    InnerT d(divisor);
    for (int index = 0; index < N * M; ++index)
        m[0][index] /= d;
    return *this;
}

template <int N, int M, typename T, typename InnerT>
Q_OUTOFLINE_TEMPLATE QGenericMatrix<N, M, T, InnerT> operator+(const QGenericMatrix<N, M, T, InnerT>& m1, const QGenericMatrix<N, M, T, InnerT>& m2)
{
    QGenericMatrix<N, M, T, InnerT> result(1);
    for (int index = 0; index < N * M; ++index)
        result.m[0][index] = m1.m[0][index] + m2.m[0][index];
    return result;
}

template <int N, int M, typename T, typename InnerT>
Q_OUTOFLINE_TEMPLATE QGenericMatrix<N, M, T, InnerT> operator-(const QGenericMatrix<N, M, T, InnerT>& m1, const QGenericMatrix<N, M, T, InnerT>& m2)
{
    QGenericMatrix<N, M, T, InnerT> result(1);
    for (int index = 0; index < N * M; ++index)
        result.m[0][index] = m1.m[0][index] - m2.m[0][index];
    return result;
}

template <int N, int M1, int M2, typename T, typename InnerT>
Q_OUTOFLINE_TEMPLATE QGenericMatrix<M1, M2, T, InnerT> operator*(const QGenericMatrix<N, M2, T, InnerT>& m1, const QGenericMatrix<M1, N, T, InnerT>& m2)
{
    QGenericMatrix<M1, M2, T, InnerT> result(1);
    for (int row = 0; row < M2; ++row) {
        for (int col = 0; col < M1; ++col) {
            InnerT sum(0.0f);
            for (int j = 0; j < N; ++j)
                sum += m1.m[j][row] * m2.m[col][j];
            result.m[col][row] = sum;
        }
    }
    return result;
}

template <int N, int M, typename T, typename InnerT>
Q_OUTOFLINE_TEMPLATE QGenericMatrix<N, M, T, InnerT> operator-(const QGenericMatrix<N, M, T, InnerT>& matrix)
{
    QGenericMatrix<N, M, T, InnerT> result(1);
    for (int index = 0; index < N * M; ++index)
        result.m[0][index] = -matrix.m[0][index];
    return result;
}

template <int N, int M, typename T, typename InnerT>
Q_OUTOFLINE_TEMPLATE QGenericMatrix<N, M, T, InnerT> operator*(T factor, const QGenericMatrix<N, M, T, InnerT>& matrix)
{
    InnerT f(factor);
    QGenericMatrix<N, M, T, InnerT> result(1);
    for (int index = 0; index < N * M; ++index)
        result.m[0][index] = matrix.m[0][index] * f;
    return result;
}

template <int N, int M, typename T, typename InnerT>
Q_OUTOFLINE_TEMPLATE QGenericMatrix<N, M, T, InnerT> operator*(const QGenericMatrix<N, M, T, InnerT>& matrix, T factor)
{
    InnerT f(factor);
    QGenericMatrix<N, M, T, InnerT> result(1);
    for (int index = 0; index < N * M; ++index)
        result.m[0][index] = matrix.m[0][index] * f;
    return result;
}

template <int N, int M, typename T, typename InnerT>
Q_OUTOFLINE_TEMPLATE QGenericMatrix<N, M, T, InnerT> operator/(const QGenericMatrix<N, M, T, InnerT>& matrix, T divisor)
{
    InnerT d(divisor);
    QGenericMatrix<N, M, T, InnerT> result(1);
    for (int index = 0; index < N * M; ++index)
        result.m[0][index] = matrix.m[0][index] / d;
    return result;
}

template <int N, int M, typename T, typename InnerT>
Q_OUTOFLINE_TEMPLATE void QGenericMatrix<N, M, T, InnerT>::toValueArray(T *values)
{
    for (int col = 0; col < N; ++col)
        for (int row = 0; row < M; ++row)
            values[row * N + col] = T(m[col][row]);
}

// Define aliases for the useful variants of QGenericMatrix.
typedef QGenericMatrix<2, 2, qreal, float> QMatrix2x2;
typedef QGenericMatrix<2, 3, qreal, float> QMatrix2x3;
typedef QGenericMatrix<2, 4, qreal, float> QMatrix2x4;
typedef QGenericMatrix<3, 2, qreal, float> QMatrix3x2;
typedef QGenericMatrix<3, 3, qreal, float> QMatrix3x3;
typedef QGenericMatrix<3, 4, qreal, float> QMatrix3x4;
typedef QGenericMatrix<4, 2, qreal, float> QMatrix4x2;
typedef QGenericMatrix<4, 3, qreal, float> QMatrix4x3;

#ifndef QT_NO_DEBUG_STREAM

template <int N, int M, typename T, typename InnerT>
QDebug operator<<(QDebug dbg, const QGenericMatrix<N, M, T, InnerT> &m)
{
    dbg.nospace() << "QGenericMatrix<" << N << ", " << M
        << ", " << QTypeInfo<T>::name() << ", " << QTypeInfo<InnerT>::name()
        << ">(" << endl << qSetFieldWidth(10);
    for (int row = 0; row < M; ++row) {
        for (int col = 0; col < N; ++col)
            dbg << m(row, col);
        dbg << endl;
    }
    dbg << qSetFieldWidth(0) << ')';
    return dbg.space();
}

#endif

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QMatrix2x2)
Q_DECLARE_METATYPE(QMatrix2x3)
Q_DECLARE_METATYPE(QMatrix2x4)
Q_DECLARE_METATYPE(QMatrix3x2)
Q_DECLARE_METATYPE(QMatrix3x3)
Q_DECLARE_METATYPE(QMatrix3x4)
Q_DECLARE_METATYPE(QMatrix4x2)
Q_DECLARE_METATYPE(QMatrix4x3)

QT_END_HEADER

#endif

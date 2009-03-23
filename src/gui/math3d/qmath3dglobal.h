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

#ifndef QMATH3DGLOBAL_H
#define QMATH3DGLOBAL_H

#include <QtCore/qglobal.h>
#include <QtGui/qfixedpt.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

// Detect the presence of a fixed-point OpenGL implementation.
#if defined(QT_OPENGL_ES_1_CL) || defined(QT_NO_GL_FLOAT)
#ifndef QT_GL_FIXED_PREFERRED
#define QT_GL_FIXED_PREFERRED 1
#endif
#endif

// QT_GL_FIXED_PREFERRED indicates that fixed-point should be
// preferred over floating-point for operations requiring high performance.
//
// qreal is the floating-point type that should be used in
// user-visible functions.  qrealinner is used internally where
// values may be stored as either floating-point or fixed-point.
// qrealinner will typically be the same size as GLfloat or GLfixed.
#if defined(QT_GL_FIXED_PREFERRED)
typedef QFixedPt<16> qrealinner;
#else
typedef float qrealinner;
#endif

// Explicit conversion operator, primarily for converting from
// fixed point back to floating-point.  This is safer than
// declaring conversion operators in the QFixedPt class.
template <typename T1, typename T2>
T1 qt_math3d_convert(T2 v)
{
    return T1(v);
}
template <>
inline float qt_math3d_convert< float, QFixedPt<16> >(QFixedPt<16> v)
{
    return float(v.toReal());
}
template <>
inline double qt_math3d_convert< double, QFixedPt<16> >(QFixedPt<16> v)
{
    return double(v.toReal());
}
template <>
inline int qt_math3d_convert< int, QFixedPt<16> >(QFixedPt<16> v)
{
    return v.toInt();
}

QT_END_NAMESPACE

QT_END_HEADER

#endif

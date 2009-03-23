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

#ifndef QMATH3DUTIL_P_H
#define QMATH3DUTIL_P_H

#include <QtGui/qmath3dglobal.h>
#include <QtCore/qmath.h>

QT_BEGIN_NAMESPACE

#ifdef QT_GL_FIXED_PREFERRED
#define qvtsqrt(x)  ((x).sqrtF())
#else
#define qvtsqrt(x)  qSqrt((x))
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void qt_math3d_sincos(qreal degrees, qrealinner *s, qrealinner *c);

#ifdef QT_GL_FIXED_PREFERRED

inline qrealinner qf2vt_round(qreal x)
{
    QFixedPt<16> result;
    if (x >= 0.0f)
        result.setBits(int(x * 65536.0f + 0.5f));
    else
        result.setBits(int(x * 65536.0f - 0.5f));
    return result;
}

// Helper macros for computing dot products without losing precision.
// In fixed-point mode, a 64-bit intermediate result is used.
#define qvtmul64(x,y)   ((qint64((x).bits())) * (qint64((y).bits())))
#define qvtsqrt64(x)    \
    (qt_math3d_fixed_sqrt((x) << 16) / (qreal)(1 << 24))
#define qvtdot64(x)     ((x) / (qreal)(((qint64)1) << 32))

#else

inline qrealinner qf2vt_round(qreal x)
{
    return x;
}

#define qvtmul64(x,y)   ((x) * (y))
#define qvtsqrt64(x)    (qvtsqrt((x)))
#define qvtdot64(x)     ((x))

#endif

QT_END_NAMESPACE

#endif

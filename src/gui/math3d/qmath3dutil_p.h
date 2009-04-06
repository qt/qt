/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
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
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
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

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

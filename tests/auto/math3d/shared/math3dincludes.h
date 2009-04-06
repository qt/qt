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

#ifndef MATH3DINCLUDES_H
#define MATH3DINCLUDES_H

#if defined(FIXED_POINT_TESTS)

// Rename the classes we want to test in fixed-point mode so that
// they don't conflict with the ones that are built into Qt.
#define QT_NO_GL_FLOAT 1
#define QVector2D           tst_QVector2D
#define QVector3D           tst_QVector3D
#define QVector4D           tst_QVector4D
#define QQuaternion         tst_QQuaternionX
#define QMatrix2x2          tst_QMatrix2x2
#define QMatrix3x3          tst_QMatrix3x3
#define QMatrix4x4          tst_QMatrix4x4
#define QMatrix2x3          tst_QMatrix2x3
#define QMatrix2x4          tst_QMatrix2x4
#define QMatrix3x2          tst_QMatrix3x2
#define QMatrix3x4          tst_QMatrix3x4
#define QMatrix4x2          tst_QMatrix4x2
#define QMatrix4x3          tst_QMatrix4x3
#define QGenericMatrix      tst_QGenericMatrix
#define qt_math3d_sincos    tst_qt_math3d_sincos
#define qt_math3d_convert   tst_qt_math3d_convert
#define qrealinner          tst_qrealinner

// We need to re-include the headers with the changed class names.
#undef QGENERICMATRIX_H
#undef QMATH3DGLOBAL_H
#undef QMATH3DUTIL_P_H
#undef QMATRIX4X4_H
#undef QQUATERNION_H
#undef QVECTOR2D_H
#undef QVECTOR3D_H
#undef QVECTOR4D_H

#endif

#include <QtGui/qmatrix4x4.h>
#include <QtGui/qgenericmatrix.h>
#include <QtGui/qvector2d.h>
#include <QtGui/qvector3d.h>
#include <QtGui/qvector4d.h>
#include <QtGui/qquaternion.h>

#endif

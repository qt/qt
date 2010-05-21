/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

import Test 1.0

MyTypeObject {
    property real v_m11: matrix.m11
    property real v_m12: matrix.m12
    property real v_m13: matrix.m13
    property real v_m14: matrix.m14
    property real v_m21: matrix.m21
    property real v_m22: matrix.m22
    property real v_m23: matrix.m23
    property real v_m24: matrix.m24
    property real v_m31: matrix.m31
    property real v_m32: matrix.m32
    property real v_m33: matrix.m33
    property real v_m34: matrix.m34
    property real v_m41: matrix.m41
    property real v_m42: matrix.m42
    property real v_m43: matrix.m43
    property real v_m44: matrix.m44
    property variant copy: matrix
}


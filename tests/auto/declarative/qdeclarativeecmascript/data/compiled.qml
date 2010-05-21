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

import Qt 4.7

QtObject {
    //real
    property real test1: a + b
    property real test2: a - b
    property bool test3: (a < b)
    property bool test4: (a > b)
    property bool test5: (a == b)
    property bool test6: (a != b)

    //int
    property int test7: c + d
    property int test8: d - c
    property bool test9: (c < d)
    property bool test10: (c > d)
    property bool test11: (c == d)
    property bool test12: (c != d)

    //string
    property string test13: e + f
    property string test14: e + " " + f
    property bool test15: (e == f)
    property bool test16: (e != f)

    //type conversion
    property int test17: a
    property real test18: d
    property int test19: g
    property real test20: g
    property string test21: g
    property string test22: h
    property bool test23: i
    property color test24: j
    property color test25: k

    property real a: 4.5
    property real b: 11.2
    property int c: 9
    property int d: 176
    property string e: "Hello"
    property string f: "World"
    property variant g: 6.7
    property variant h: "!"
    property variant i: true
    property string j: "#112233"
    property string k: "#aa112233"
}

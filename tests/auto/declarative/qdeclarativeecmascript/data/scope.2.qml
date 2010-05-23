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

Item {
    property int a: 0
    property int b: 0

    function b() { return 11; }
    function c() { return 33; }

    QtObject {
        id: a
        property int value: 19
    }

    QtObject {
        id: c
        property int value: 24
    }

    QtObject {
        id: nested
        property int a: 1
        property int test: a.value
        property int test2: b()
        property int test3: c.value
    }


    // id takes precedence over local, and root properties
    property int test1: a.value 
    property alias test2: nested.test

    // methods takes precedence over local, and root properties
    property int test3: b()
    property alias test4: nested.test2

    // id takes precedence over methods
    property int test5: c.value
    property alias test6: nested.test3
}

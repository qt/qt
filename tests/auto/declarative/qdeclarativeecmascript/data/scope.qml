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
    id: root

    property int a: 1
    property int binding: a
    property string binding2: a + "Test"
    property int binding3: myFunction()
    property int binding4: nestedObject.myNestedFunction()

    function myFunction() {
        return a;
    }

    Item {
        id: nestedObject

        function myNestedFunction() {
            return a;
        }

        property int a: 2
        property int binding: a
        property string binding2: a + "Test"
        property int binding3: myFunction()
        property int binding4: myNestedFunction()
    }

    ScopeObject {
        id: compObject
    }

    property alias test1: root.binding
    property alias test2: nestedObject.binding
    property alias test3: root.binding2
    property alias test4: nestedObject.binding2
    property alias test5: root.binding3
    property alias test6: nestedObject.binding3
    property alias test7: root.binding4
    property alias test8: nestedObject.binding4
    property alias test9: compObject.binding
    property alias test10: compObject.binding2
}

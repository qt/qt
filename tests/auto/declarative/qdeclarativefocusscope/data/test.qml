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

Rectangle {
    color: "white"
    width: 800
    height: 600

    Keys.onDigit9Pressed: console.log("Error - Root")

    FocusScope {
        id: myScope
        focus: true

        Keys.onDigit9Pressed: console.log("Error - FocusScope")

        Rectangle {
            objectName: "item0"
            height: 120
            width: 420

            color: "transparent"
            border.width: 5
            border.color: myScope.wantsFocus?"blue":"black"

            Rectangle {
                id: item1; objectName: "item1"
                x: 10; y: 10
                width: 100; height: 100; color: "green"
                border.width: 5
                border.color: wantsFocus?"blue":"black"
                Keys.onDigit9Pressed: console.debug("Top Left");
                KeyNavigation.right: item2
                focus: true

                Rectangle {
                    width: 50; height: 50; anchors.centerIn: parent
                    color: parent.focus?"red":"transparent"
                }
            }

            Rectangle {
                id: item2; objectName: "item2"
                x: 310; y: 10
                width: 100; height: 100; color: "green"
                border.width: 5
                border.color: wantsFocus?"blue":"black"
                KeyNavigation.left: item1
                Keys.onDigit9Pressed: console.log("Top Right");

                Rectangle {
                    width: 50; height: 50; anchors.centerIn: parent
                    color: parent.focus?"red":"transparent"
                }
            }
        }
        KeyNavigation.down: item3
    }

    Text { x:100; y:170; text: "Blue border indicates scoped focus\nBlack border indicates NOT scoped focus\nRed box indicates active focus\nUse arrow keys to navigate\nPress \"9\" to print currently focused item" }

    Rectangle {
        id: item3; objectName: "item3"
        x: 10; y: 300
        width: 100; height: 100; color: "green"
        border.width: 5
        border.color: wantsFocus?"blue":"black"

        Keys.onDigit9Pressed: console.log("Bottom Left");
        KeyNavigation.up: myScope

        Rectangle {
            width: 50; height: 50; anchors.centerIn: parent
            color: parent.focus?"red":"transparent"
        }
    }

}

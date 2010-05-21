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
    width: 240
    height: 320
    color: "#ffffff"
    function checkProperties() {
        testObject.error = false;
        if (list.model != testModel) {
            console.log("model property incorrect");
            testObject.error = true;
        }
        if (!testObject.animate && list.delegate != myDelegate) {
            console.log("delegate property incorrect - expected myDelegate");
            testObject.error = true;
        }
        if (testObject.animate && list.delegate != animatedDelegate) {
            console.log("delegate property incorrect - expected animatedDelegate");
            testObject.error = true;
        }
        if (testObject.invalidHighlight && list.highlight != invalidHl) {
            console.log("highlight property incorrect - expected invalidHl");
            testObject.error = true;
        }
        if (!testObject.invalidHighlight && list.highlight != myHighlight) {
            console.log("highlight property incorrect - expected myHighlight");
            testObject.error = true;
        }
    }
    resources: [
        Component {
            id: myDelegate
            Rectangle {
                id: wrapper
                objectName: "wrapper"
                height: 20
                width: 240
                Text {
                    text: index
                }
                Text {
                    x: 30
                    id: textName
                    objectName: "textName"
                    text: name
                }
                Text {
                    x: 120
                    id: textNumber
                    objectName: "textNumber"
                    text: number
                }
                Text {
                    x: 200
                    text: wrapper.y
                }
                color: ListView.isCurrentItem ? "lightsteelblue" : "white"
            }
        },
        Component {
            id: animatedDelegate
            Rectangle {
                id: wrapper
                objectName: "wrapper"
                height: 20
                width: 240
                Text {
                    text: index
                }
                Text {
                    x: 30
                    id: textName
                    objectName: "textName"
                    text: name
                }
                Text {
                    x: 120
                    id: textNumber
                    objectName: "textNumber"
                    text: number
                }
                Text {
                    x: 200
                    text: wrapper.y
                }
                color: ListView.isCurrentItem ? "lightsteelblue" : "white"
                ListView.onRemove: SequentialAnimation {
                    PropertyAction { target: wrapper; property: "ListView.delayRemove"; value: true }
                    NumberAnimation { target: wrapper; property: "scale"; to: 0; duration: 250; easing.type: "InOutQuad" }
                    PropertyAction { target: wrapper; property: "ListView.delayRemove"; value: false }

                }
            }
        },
        Component {
            id: myHighlight
            Rectangle { color: "green" }
        },
        Component {
            id: invalidHl
            SmoothedAnimation {}
        }
    ]
    ListView {
        id: list
        objectName: "list"
        focus: true
        width: 240
        height: 320
        model: testModel
        delegate: testObject.animate ? animatedDelegate : myDelegate
        highlight: testObject.invalidHighlight ? invalidHl : myHighlight
        highlightMoveSpeed: 1000
        highlightResizeSpeed: 1000
        cacheBuffer: testObject.cacheBuffer
    }
}

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

/*
This test displays 6 red rects -- 4 in the top row, 2 in the bottom.

Sequence:
1. the bottom-left rect turns blue, then green
2. the second rect in the top row turns blue
3. the third rect in the top row turns blue
4. the last rect in the top row quickly turns blue then back to red
5. the bottom-left rect turns blue, then green
*/

Rectangle {
    id: root
    width: 400
    height: 100

    // Left click on me
    Rectangle {
        width: 98; height: 48
        color: "red"
        MouseArea {
            id: mr1
            anchors.fill: parent
            enabled: false
            onClicked: { parent.color = "blue"; root.error = "mr1 should ignore presses"; }
        }
    }

    // Left click, then right click
    Rectangle {
        x: 100
        width: 98; height: 48
        color: "red"
        MouseArea {
            id: mr2
            anchors.fill: parent
            acceptedButtons: Qt.RightButton
            onClicked: {
                if (mouse.button == Qt.RightButton) {
                    parent.color = "blue";
                } else {
                    parent.color = "green";
                    root.error = "mr1 should ignore presses";
                }
            }
        }
    }

    // press and hold me
    Rectangle {
        x: 200
        width: 98; height: 48
        color: "red"
        MouseArea {
            id: mr3
            anchors.fill: parent
            onPressAndHold: {
                parent.color = "blue";
            }
        }
    }

    // click me
    Rectangle {
        x: 300
        width: 98; height: 48
        color: "red"
        MouseArea {
            id: mr4
            anchors.fill: parent
            onPressed: {
                parent.color = "blue";
            }
            onReleased: {
                parent.color = "red";
            }
        }
    }

    // move into and out of me
    Rectangle {
        x: 0
        y: 50
        width: 98; height: 48
        color: "red"
        MouseArea {
            id: mr5
            anchors.fill: parent
            hoverEnabled: true
            onEntered: {
                parent.color = "blue";
            }
            onExited: {
                parent.color = "green";
            }
        }
    }

    // click, then double click me
    Rectangle {
        x: 100
        y: 50
        width: 98; height: 48
        color: "red"
        MouseArea {
            id: mr6
            anchors.fill: parent
            onClicked: {
                parent.color = "blue";
            }
            onDoubleClicked: {
                parent.color = "green";
            }
        }
    }

    // click, then double click me - nothing should happen
    Rectangle {
        x: 100
        y: 50
        width: 98; height: 48
        color: "red"
        MouseArea {
            id: mr7
            anchors.fill: parent
            enabled: false
            onClicked: { parent.color = "blue" }
            onPressed: { parent.color = "yellow" }
            onReleased: { parent.color = "cyan" }
            onDoubleClicked: { parent.color = "green" }
        }
    }
}

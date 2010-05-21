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
    color: "#ffffff"
    width: 320; height: 240
    Rectangle {
        id: rect
        color: "#00ff00"
        y: 200; width: 60; height: 20
        SequentialAnimation on y {
            loops: Animation.Infinite
            NumberAnimation {
                to: 20; duration: 500
                easing.type: "InOutQuad"
            }
            NumberAnimation {
                to: 200; duration: 2000
                easing.type: "OutBounce"
            }
            PauseAnimation { duration: 1000 }
        }
    }

    // Velocity
    Rectangle {
        color: "#ff0000"
        x: rect.width; width: rect.width; height: 20
        y: 200
        SpringFollow on y { to: rect.y; velocity: 200 }
    }

    // Spring
    Rectangle {
        color: "#ff0000"
        x: rect.width * 2; width: rect.width/2; height: 20
        y: 200
        SpringFollow on y { to: rect.y; spring: 1.0; damping: 0.2 }
    }
    Rectangle {
        color: "#880000"
        x: rect.width * 2.5; width: rect.width/2; height: 20
        y: 200
        SpringFollow on y { to: rect.y; spring: 1.0; damping: 0.2; mass: 3.0 } // "heavier" object
    }

    // Follow mouse
    MouseArea {
        id: mouseRegion
        anchors.fill: parent
        Rectangle {
            id: ball
            width: 20; height: 20
            radius: 10
            color: "#0000ff"
            SpringFollow on x { id: f1; to: mouseRegion.mouseX-10; spring: 1.0; damping: 0.05; epsilon: 0.25 }
            SpringFollow on y { id: f2; to: mouseRegion.mouseY-10; spring: 1.0; damping: 0.05; epsilon: 0.25 }
            states: [
                State {
                    name: "following"
                    when: !f1.inSync || !f2.inSync
                    PropertyChanges { target: ball; color: "#ff0000" }
                }
            ]
            transitions: [
                Transition {
                    ColorAnimation { duration: 200 }
                }
            ]
        }
    }
}

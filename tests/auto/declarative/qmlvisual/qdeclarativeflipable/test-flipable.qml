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
    width: 400; height: 240
    color: "white"

    Timer {
        interval: 3000; running: true; repeat: true; triggeredOnStart: true
        onTriggered: {
            if (flipable.state == '') flipable.state = 'back'; else flipable.state = ''
            if (flipable2.state == '') flipable2.state = 'back'; else flipable2.state = ''
        }
    }

    Flipable {
        id: flipable
        width: 200; height: 200

        transform: Rotation {
            id: rotation; angle: 0
            origin.x: 100; origin.y: 100
            axis.x: 0; axis.y: 1; axis.z: 0
        }

        front: Rectangle {
            color: "steelblue"; width: 200; height: 200
        }

        back: Rectangle {
            color: "deeppink"; width: 200; height: 200
        }

        states: State {
            name: "back"
            PropertyChanges { target: rotation; angle: 180 }
        }

        transitions: Transition {
            NumberAnimation { easing.type: "InOutQuad"; properties: "angle"; duration: 3000 }
        }
    }

    Flipable {
        id: flipable2
        x: 200; width: 200; height: 200

        transform: Rotation {
            id: rotation2; angle: 0
            origin.x: 100; origin.y: 100
            axis.x: 1; axis.z: 0
        }

        front: Rectangle {
            color: "deeppink"; width: 200; height: 200
        }

        back: Rectangle {
            color: "steelblue"; width: 200; height: 200
        }

        states: State {
            name: "back"
            PropertyChanges { target: rotation2; angle: 180 }
        }

        transitions: Transition {
            NumberAnimation { easing.type: "InOutQuad"; properties: "angle"; duration: 3000 }
        }
    }

    Rectangle {
        x: 25; width: 150; y: 210; height: 20; color: "black"
        visible: flipable.side == Flipable.Front
    }
    Rectangle {
        x: 225; width: 150; y: 210; height: 20; color: "black"
        visible: flipable2.side == Flipable.Back
    }
}

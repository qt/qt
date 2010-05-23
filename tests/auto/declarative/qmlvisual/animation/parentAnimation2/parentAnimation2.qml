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
Blue rect fills (with 10px margin) screen, then red, then green, then screen again.
*/

Rectangle {
    id: whiteRect
    width: 640; height: 480;

    Rectangle {
        id: redRect
        x: 400; y: 50
        width: 100; height: 100
        color: "red"
    }

    Rectangle {
        id: greenRect
        x: 100; y: 150
        width: 200; height: 300
        color: "green"
    }

    Rectangle {
        id: blueRect
        x: 5; y: 5
        width: parent.width-10
        height: parent.height-10
        color: "lightblue"

        //Text { text: "Click me!"; anchors.centerIn: parent }

        MouseArea {
            anchors.fill: parent
            onClicked: {
            switch(blueRect.state) {
                case "": blueRect.state = "inRed"; break;
                case "inRed": blueRect.state = "inGreen"; break;
                case "inGreen": blueRect.state = ""; break;
                }
            }
        }

        states: [
            State {
                name: "inRed"
                ParentChange { target: blueRect; parent: redRect; x: 5; y: 5; width: parent.width-10; height: parent.height-10 }
                PropertyChanges { target: redRect; z: 1 }
            },
            State {
                name: "inGreen"
                ParentChange { target: blueRect; parent: greenRect; x: 5; y: 5; width: parent.width-10; height: parent.height-10 }
                PropertyChanges { target: greenRect; z: 1 }
            }
        ]

        transitions: Transition {
            ParentAnimation { target: blueRect; //via: whiteRect;
                NumberAnimation { properties: "x, y, width, height"; duration: 500 }
            }
        }
    }
}

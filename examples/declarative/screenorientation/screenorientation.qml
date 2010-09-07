/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

import Qt 4.7
import "Core"
import "Core/screenorientation.js" as ScreenOrientation

Rectangle {
    id: window
    width: 360  
    height: 640
    color: "white"

    Rectangle {
        id: main
        clip: true
        property variant selectedOrientation: Orientation.UnknownOrientation
        property variant activeOrientation: selectedOrientation == Orientation.UnknownOrientation ? runtime.orientation : selectedOrientation
        state: "orientation " + activeOrientation
        property bool inPortrait: (activeOrientation == Orientation.Portrait || activeOrientation == Orientation.PortraitInverted);

        // rotation correction for landscape devices like N900
        property bool landscapeWindow: window.width > window.height 
        property variant rotationDelta: landscapeWindow ? -90 : 0
        rotation: rotationDelta

        // initial state is portrait
        property real baseWidth: landscapeWindow ? window.height-10 : window.width-10
        property real baseHeight: landscapeWindow ? window.width-10 : window.height-10

        width: baseWidth
        height: baseHeight
        anchors.centerIn: parent

        color: "black"
        gradient: Gradient {
            GradientStop { position: 0.0; color: Qt.rgba(0.5,0.5,0.5,0.5) }
            GradientStop { position: 0.8; color: "black" }
            GradientStop { position: 1.0; color: "black" }
        }
        Item {
            id: bubbles
            property bool rising: false
            anchors.fill: parent
            property variant gravityPoint: ScreenOrientation.calculateGravityPoint(main.activeOrientation, runtime.orientation)
            Repeater {
                model: 24
                Bubble {
                    rising: bubbles.rising
                    verticalRise: ScreenOrientation.parallel(main.activeOrientation, runtime.orientation)
                    xAttractor: parent.gravityPoint.x
                    yAttractor: parent.gravityPoint.y
                }
            }
            Component.onCompleted: bubbles.rising = true;
        }

        Column {
            width: centeredText.width
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter 
            anchors.verticalCenterOffset: 30
            Text {
                text: "Orientation"
                color: "white"
                font.pixelSize: 22
                anchors.horizontalCenter: parent.horizontalCenter
            }
            Text {
                id: centeredText
                text: ScreenOrientation.printOrientation(main.activeOrientation)
                color: "white"
                font.pixelSize: 40
                anchors.horizontalCenter: parent.horizontalCenter
            }
            Text {
                text: "sensor: " + ScreenOrientation.printOrientation(runtime.orientation)
                color: "white"
                font.pixelSize: 14
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
        Flow {
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 10
            spacing: 4
            Button {
                width: main.inPortrait ? (parent.width-4)/2 : (parent.width-8)/3
                text: "Portrait"
                onClicked: main.selectedOrientation = Orientation.Portrait
                toggled: main.selectedOrientation == Orientation.Portrait
            }
            Button {
                width: main.inPortrait ? (parent.width-4)/2 : (parent.width-8)/3
                text: "Portrait inverted"
                onClicked: main.selectedOrientation = Orientation.PortraitInverted
                toggled: main.selectedOrientation == Orientation.PortraitInverted
            }
            Button {
                width: main.inPortrait ? (parent.width-4)/2 : (parent.width-8)/3
                text: "Landscape"
                onClicked: main.selectedOrientation = Orientation.Landscape
                toggled: main.selectedOrientation == Orientation.Landscape
            }
            Button {
                width: main.inPortrait ? (parent.width-4)/2 : (parent.width-8)/3
                text: "Landscape inverted"
                onClicked: main.selectedOrientation = Orientation.LandscapeInverted
                toggled: main.selectedOrientation == Orientation.LandscapeInverted
            }
            Button {
                width: main.inPortrait ? parent.width : 2*(parent.width-2)/3
                text: "From runtime.orientation"
                onClicked: main.selectedOrientation = Orientation.UnknownOrientation
                toggled: main.selectedOrientation == Orientation.UnknownOrientation
            }
        }
        states: [
            State {
                name: "orientation " + Orientation.Landscape
                PropertyChanges { 
                    target: main
                    rotation: ScreenOrientation.getAngle(Orientation.Landscape)+rotationDelta
                    width: baseHeight
                    height: baseWidth
                }
            },
            State {
                name: "orientation " + Orientation.PortraitInverted
                PropertyChanges { 
                    target: main
                    rotation: ScreenOrientation.getAngle(Orientation.PortraitInverted)+rotationDelta
                    width: baseWidth
                    height: baseHeight
                }
            },
            State {
                name: "orientation " + Orientation.LandscapeInverted
                PropertyChanges { 
                    target: main
                    rotation: ScreenOrientation.getAngle(Orientation.LandscapeInverted)+rotationDelta
                    width: baseHeight
                    height: baseWidth
                }
            }
        ]
        transitions: Transition {
            ParallelAnimation {
                RotationAnimation { 
                    direction: RotationAnimation.Shortest
                    duration: 300
                    easing.type: Easing.InOutQuint 
                }
                NumberAnimation { 
                    properties: "x,y,width,height"
                    duration: 300
                    easing.type: Easing.InOutQuint 
                }
            }
        }
    }
}

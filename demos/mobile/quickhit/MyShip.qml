/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 1.0
import "Game.js" as GameScript

Item {
    id: myShip
    objectName: "myShip"

    property variant myShipSize
    property int originalY

    // Fires missile if exists
    function fire() {
        if (myShip.opacity==1) {
            GameScript.fireMissile(myShip.x+myShip.width/2,myShip.y,myShip.height*-1)
            if (myShip.y+myShip.height < gameArea.height+5) {
                goDownAnim.restart()
            }
        }
    }

    function createGraphicsForLevel() {
        myShip.myShipSize = LevelPlugin.graphSize(LevelPlugin.pathToMyShipPic())
        myShip.height = myShipSize.height
        myShip.width = myShipSize.width
        image.source = "file:/"+LevelPlugin.pathToMyShipPic()
        myShip.y = gameArea.height - myShip.height - 10
        originalY = myShip.y
        myShip.x = (gameArea.width - myShip.width)/ 2
    }

    Image {
        id: image
        smooth: true
    }

    Keys.onSpacePressed: { fire() }
    Keys.onSelectPressed: { fire() }
    Keys.onRightPressed: {
        if (myShip.x < (gameArea.width - myShip.width - 20)) {
            toRightAnim.restart()
        }
    }
    Keys.onLeftPressed: {
        if (myShip.x > 20) {
            toLeftAnim.restart()
        }
    }

    // To right animation
    PropertyAnimation { id: toRightAnim; target: myShip; easing.type: Easing.OutQuint;
        properties: "x"; from: myShip.x; to: myShip.x + 20; duration: 500 }

    // To left animation
    PropertyAnimation { id: toLeftAnim; target: myShip; easing.type: Easing.OutQuint;
        properties: "x"; from: myShip.x; to: myShip.x - 20; duration: 500 }

    // Go down on fire animation
    SequentialAnimation {
        id: goDownAnim
        NumberAnimation { target: myShip; property:"y"; from: myShip.y; to: myShip.y+5;
            easing.type: Easing.Linear; duration: 200 }
        NumberAnimation { target: myShip; property:"y"; from: myShip.y; to: originalY;
            easing.type: Easing.Linear; duration: 200 }
    }


    /*
    MouseArea {
        anchors.fill: parent
        drag.target: myShip
        drag.axis: Drag.XAxis
        drag.minimumX: 0
        drag.maximumX: gameArea.width - myShip.width
        onReleased: {
            fire()
        }
    }
    */


    /*
    property bool isAutoRepeat: false
    Keys.onPressed: {
             if (event.key == Qt.Key_Right) {
                 if (event.isAutoRepeat) {
                     isAutoRepeat = true
                     toRightAnim.restart()
                 } else {
                     isAutoRepeat = false
                     toRotateRightAndCenterAnim.restart()
                 }
                 event.accepted = true
             }
             else if (event.key == Qt.Key_Left) {
                 if (event.isAutoRepeat) {
                     isAutoRepeat = true
                     toLeftAnim.restart()
                 } else {
                     isAutoRepeat = false
                     toRotateLeftAndCenterAnim.restart()
                 }
                 event.accepted = true
             }
         }

    Keys.onReleased: {
        if (isAutoRepeat) {
            toRotateRightAndCenterAnim.stop()
            toRotateLeftAndCenterAnim.stop()
        } else {
            toRotateCenter.restart()
        }
        event.accepted = true
    }

    // Rotation on movement animation
    transform: Rotation { id:rotationId; origin.x: width/2; origin.y: height/2; axis { x: 0; y: 1; z: 0 } angle: 0 }
    SequentialAnimation {
        id: toRotateRightAndCenterAnim
        PropertyAnimation { target: rotationId; easing.type: Easing.Linear;
            properties: "angle"; from: 0; to: 30; duration: 100 }
        PropertyAnimation { target: myShip; easing.type: Easing.OutQuint;
            properties: "x"; from: myShip.x; to: myShip.x + 20; duration: 500 }
        PropertyAnimation { target: rotationId; easing.type: Easing.Linear;
            properties: "angle"; to: 0; duration: 100 }
    }
    SequentialAnimation {
        id: toRotateLeftAndCenterAnim
        PropertyAnimation { target: rotationId; easing.type: Easing.Linear;
            properties: "angle"; from: 0; to: -30; duration: 100 }
        PropertyAnimation { target: myShip; easing.type: Easing.OutQuint;
            properties: "x"; from: myShip.x; to: myShip.x - 20; duration: 500 }
        PropertyAnimation { target: rotationId; easing.type: Easing.Linear;
            properties: "angle"; to: 0; duration: 100 }
    }
    PropertyAnimation { id: toRotateRight; target: rotationId; easing.type: Easing.Linear;
        properties: "angle"; from: 0; to: 30; duration: 100 }
    PropertyAnimation { id: toRotateCenter; target: rotationId; easing.type: Easing.Linear;
        properties: "angle"; to: 0; duration: 100 }
    PropertyAnimation { id: toRotateLeft; target: rotationId; easing.type: Easing.Linear;
        properties: "angle"; from: 0; to: -30; duration: 100 }
*/

}

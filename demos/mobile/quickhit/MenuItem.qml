/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
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
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 1.0

Text {
    id: menuItem
    text: name
    color: "white"
    opacity: 1
    font.family: "Calibri"
    font.pixelSize:  menu.width / 8
    x: (parent.width - width )/ 2

    property bool selectedItem: false
    property int type

    signal itemSelected()

    transform: Rotation { id:rotationId; origin.x:width/2; origin.y:height/2; axis { x: 1; y: 0; z: 0 } angle: 0 }
    SequentialAnimation {
        id: toRotateRightAndCenterAnim
        PropertyAnimation { target: rotationId; easing.type: Easing.Linear;
            properties: "angle"; from: 0; to: 180; duration: 300 }
        PropertyAction  { target: menuItem; property: "color"; value:"red"}
        PropertyAnimation { target: rotationId; easing.type: Easing.Linear;
            properties: "angle"; from: 180; to: 360; duration: 300 }
        PropertyAction  { target: menuItem; property: "color"; value:"white"}
    }

    function selectItem(doSelect) {
        if (!toRotateRightAndCenterAnim.running) {
            GameEngine.playInternalSound(0)
            toRotateRightAndCenterAnim.restart()
            if (doSelect) {
                GameEngine.playInternalSound(1)
                selectionTimer.restart()
            }
        }
    }

    Timer {
        id: selectionTimer
        interval: 1000; running: false; repeat: false
        onTriggered: menuItem.itemSelected()
    }

    MouseArea {
        id: mouseArea;
        anchors.fill: parent
        onClicked: {
            selectItem(true)
        }
    }

    Keys.onSpacePressed: { selectItem(true) }
    Keys.onSelectPressed: { selectItem(true) }
    Keys.onEnterPressed: { selectItem(true) }
    Keys.onReleased: {
        if (event.key == Qt.Key_Down || event.key == Qt.Key_Up) {
            selectItem(false)
        }
        event.accepted = false
    }
}


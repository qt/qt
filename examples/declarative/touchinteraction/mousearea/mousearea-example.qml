/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

import Qt 4.7

Rectangle {
    id: box
    width: 350; height: 250

    function showInfo(text) {
        statusText.text = text
    }

    Rectangle {
        width: 80; height: 80
        color: "red"

        Text { text: "Click"; font.pixelSize: 16; anchors.centerIn: parent }

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            acceptedButtons: Qt.LeftButton | Qt.RightButton

            onPressed: box.showInfo('Pressed (x=' + mouse.x + ' y=' + mouse.y + ' button=' 
                    + (mouse.button == Qt.RightButton ? 'right' : 'left') 
                    + ' Shift=' + (mouse.modifiers & Qt.ShiftModifier ? 'true' : 'false') + ')') 
            onReleased: box.showInfo('Released (x=' + mouse.x + ' y=' + mouse.y 
                    + ' isClick=' + mouse.isClick + ' wasHeld=' + mouse.wasHeld + ')') 
            onClicked: box.showInfo('Clicked (x=' + mouse.x + ' y=' + mouse.y + ' wasHeld=' + mouse.wasHeld + ')')
            onDoubleClicked: box.showInfo('Double clicked (x=' + mouse.x + ' y=' + mouse.y + ')')
            onPressAndHold: box.showInfo('Press and hold')
            onEntered: box.showInfo('Entered (pressed=' + pressed + ')')
            onExited: box.showInfo('Exited (pressed=' + pressed + ')')
        }
    }

    Rectangle {
        width: 80; height: 80; anchors.right: parent.right
        color: "blue"

        Text { text: "Drag"; font.pixelSize: 16; color: "white"; anchors.centerIn: parent }

        MouseArea {
            anchors.fill: parent
            drag.target: parent
            drag.axis: Drag.XAxis
            drag.minimumX: 0
            drag.maximumX: 150

            onPressed: box.showInfo('Pressed')
            onReleased: box.showInfo('Released (isClick=' + mouse.isClick + ' wasHeld=' + mouse.wasHeld + ')')
            onClicked: box.showInfo('Clicked' + ' (wasHeld=' + mouse.wasHeld + ')')
            onDoubleClicked: box.showInfo('Double clicked')
            onPressAndHold: box.showInfo('Press and hold')
        }
    }

    Text {
        id: statusText
        anchors.bottom: parent.bottom; anchors.horizontalCenter: parent.horizontalCenter; anchors.margins: 30

        onTextChanged: console.log(text)
    }
}

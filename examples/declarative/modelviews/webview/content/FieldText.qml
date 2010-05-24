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

Item {
    id: fieldText
    height: 30
    property string text: ""
    property string label: ""
    property bool mouseGrabbed: false
    signal confirmed
    signal cancelled
    signal startEdit

    function edit() {
        if (!mouseGrabbed) {
            fieldText.startEdit();
            fieldText.state='editing';
            mouseGrabbed=true;
        }
    }

    function confirm() {
        fieldText.state='';
        fieldText.text = textEdit.text;
        mouseGrabbed=false;
        fieldText.confirmed();
    }

    function reset() {
        textEdit.text = fieldText.text;
        fieldText.state='';
        mouseGrabbed=false;
        fieldText.cancelled();
    }

    Image {
        id: cancelIcon
        width: 22
        height: 22
        anchors.right: parent.right
        anchors.rightMargin: 4
        anchors.verticalCenter: parent.verticalCenter
        source: "pics/cancel.png"
        opacity: 0
    }

    Image {
        id: confirmIcon
        width: 22
        height: 22
        anchors.left: parent.left
        anchors.leftMargin: 4
        anchors.verticalCenter: parent.verticalCenter
        source: "pics/ok.png"
        opacity: 0
    }

    TextInput {
        id: textEdit
        text: fieldText.text
        focus: false
        anchors.left: parent.left
        anchors.leftMargin: 0
        anchors.right: parent.right
        anchors.rightMargin: 0
        anchors.verticalCenter: parent.verticalCenter
        color: "black"
        font.bold: true
        readOnly: true
        onAccepted: confirm()
        Keys.onEscapePressed: reset()
    }

    Text {
        id: textLabel
        x: 5
        width: parent.width-10
        anchors.verticalCenter: parent.verticalCenter
        horizontalAlignment: Text.AlignHCenter
        color: fieldText.state == "editing" ? "#505050" : "#AAAAAA"
        font.italic: true
        font.bold: true
        text: label
        opacity: textEdit.text == '' ? 1 : 0
        Behavior on opacity {
            NumberAnimation {
                property: "opacity"
                duration: 250
            }
        }
    }

    MouseArea {
        anchors.fill: cancelIcon
        onClicked: { reset() }
    }

    MouseArea {
        anchors.fill: confirmIcon
        onClicked: { confirm() }
    }

    MouseArea {
        id: editRegion
        anchors.fill: textEdit
        onClicked: { edit() }
    }

    states: [
    State {
        name: "editing"
        PropertyChanges {
            target: confirmIcon
            opacity: 1
        }
        PropertyChanges {
            target: cancelIcon
            opacity: 1
        }
        PropertyChanges {
            target: textEdit
            color: "black"
            readOnly: false
            focus: true
            selectionStart: 0
            selectionEnd: -1
        }
        PropertyChanges {
            target: editRegion
            opacity: 0
        }
        PropertyChanges {
            target: textEdit.anchors
            leftMargin: 34
        }
        PropertyChanges {
            target: textEdit.anchors
            rightMargin: 34
        }
    }
    ]

    transitions: [
    Transition {
        from: ""
        to: "*"
        reversible: true
        NumberAnimation {
            properties: "opacity,leftMargin,rightMargin"
            duration: 200
        }
        ColorAnimation {
            property: "color"
            duration: 150
        }
    }
    ]
}

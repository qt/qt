/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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

import QtQuick 2.0
import Qt.labs.inputcontext 1.0 as InputContext


Rectangle {
    id: keyboard

    radius: 5
    height: 122
    width: 324

    property bool shift: false

    gradient: Gradient {
        GradientStop { position: 0.0; color: "lightgrey" }
        GradientStop { position: 1.0; color: "white" }
    }

    Column {
        anchors.left: parent.left; anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter

        spacing: 2

        Row {
            spacing: 2
            anchors.horizontalCenter: parent.horizontalCenter

            Key { key: Qt.Key_Q; text: "q"; }
            Key { key: Qt.Key_W; text: "w"; }
            Key { key: Qt.Key_E; text: "e"; }
            Key { key: Qt.Key_R; text: "r"; }
            Key { key: Qt.Key_T; text: "t"; }
            Key { key: Qt.Key_Y; text: "y"; }
            Key { key: Qt.Key_U; text: "u"; }
            Key { key: Qt.Key_I; text: "i"; }
            Key { key: Qt.Key_O; text: "o"; }
            Key { key: Qt.Key_P; text: "p"; }
        }
        Row {
            spacing: 2
            anchors.horizontalCenter: parent.horizontalCenter

            Key { key: Qt.Key_A; text: "a"; }
            Key { key: Qt.Key_S; text: "s"; }
            Key { key: Qt.Key_D; text: "d"; }
            Key { key: Qt.Key_F; text: "f"; }
            Key { key: Qt.Key_G; text: "g"; }
            Key { key: Qt.Key_H; text: "h"; }
            Key { key: Qt.Key_J; text: "j"; }
            Key { key: Qt.Key_K; text: "k"; }
            Key { key: Qt.Key_L; text: "l"; }
        }
        Row {
            spacing: 2
            anchors.horizontalCenter: parent.horizontalCenter

            Key { key: Qt.Key_Shift; displayText: "shift"; width: 50 }
            Key { key: Qt.Key_Z; text: "z"; }
            Key { key: Qt.Key_X; text: "x"; }
            Key { key: Qt.Key_C; text: "c"; }
            Key { key: Qt.Key_V; text: "v"; }
            Key { key: Qt.Key_B; text: "b"; }
            Key { key: Qt.Key_N; text: "n"; }
            Key { key: Qt.Key_M; text: "m"; }
            Key { key: Qt.Key_Comma; text: ","; }
            Key { key: Qt.Key_Period; text: "."; }
        }

        Row {
            spacing: 2
            anchors.horizontalCenter: parent.horizontalCenter

            Key { key: Qt.Key_Enter; text: "\n"; displayText: "enter"; width: 90 }
            Key { key: Qt.Key_Space; text: " "; displayText: "space"; width: 138}
            Key { key: Qt.Key_Backspace; displayText: "backspace"; width: 90 }
        }
    }

    function keyPress(key, text)
    {
        if (key == Qt.Key_Shift)
            keyboard.shift = !keyboard.shift
        else if (keyboard.shift)
            InputContext.sendKeyPress(key, text.toUpperCase(), Qt.ShiftModifier)
        else
            InputContext.sendKeyPress(key, text)
    }

    function keyRelease(key, text)
    {
        if (key != Qt.Key_Shift) {
            if (keyboard.shift) {
                InputContext.sendKeyRelease(key, text.toUpperCase(), Qt.ShiftModifier)
                keyboard.shift = false
            } else {
                InputContext.sendKeyRelease(key, text)
            }
        }
    }
}

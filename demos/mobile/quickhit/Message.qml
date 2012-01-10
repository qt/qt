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

Rectangle {
    id: message
    border.color: "black"
    border.width: 1
    radius: 8
    smooth: true
    opacity: 0 // transparent = 0 by default
    color: "black"

    property string txt
    property int animSpeed

    function showMessage(text,speed) {
        message.border.color = "black"
        message.y = 50
        message.width = gameArea.width / 5 * 4
        message.height = gameArea.height / 8 * 1
        txt = text
        animSpeed = speed
        message.opacity = 1
        messageText.font.pixelSize = parent.width / 14
        messageAnim.restart()
    }

    function showErrorMessage(text,speed) {
        message.border.color = "red"
        message.y = 50
        message.width = gameArea.width / 5 * 4
        message.height = gameArea.height / 8 * 1
        txt = text
        animSpeed = speed
        message.opacity = 1
        messageText.font.pixelSize = parent.width / 14
        messageAnim.restart()
    }

    function showInfoMessage() {
        message.border.color = "white"
        txt = "<a href=\"https://projects.forum.nokia.com/quickhit\">QuickHit</a>" +
              "<p>" +
              "Forum Nokia <br>" +
              "Qt Quick Game Example <br>" +
              "</p>" +
              "<p>" +
              "Sounds were loaded from freesound.org under Creative Commons Sampling Plus 1.0 license.<br>" +
              "</p>" +
              "<p>" +
              "Sounds were created by these nicknames:<br>" +
              "HardPCM<br>" +
              "Matt_G<br>" +
              "klankbeeld<br>" +
              "steveygos93<br>" +
              "joe93barlow<br>" +
              "ljudman<br>" +
              "Jovica<br>" +
              "patchen<br>" +
              "nthompson<br>" +
              "</p>"

        animSpeed = 9000
        message.opacity = 1
        message.y = 10
        message.height = gameArea.height - message.y - 10
        message.width = gameArea.width - 20

        messageText.font.pixelSize = parent.width / 16
        messageAnim.restart()
    }

    //gradient: Gradient {
    //    GradientStop { position: 0.0; color: "white" }
    //    GradientStop { position: 1.0; color: "black" }
    //}

    SequentialAnimation {
        id: messageAnim
        PropertyAnimation {
            target: message; properties: "x";
            from: message.width*-1; to: gameArea.width; duration: animSpeed; easing.type: Easing.OutInExpo}
        PropertyAction { target: message; properties: "opacity"; value: 0 }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            messageAnim.stop()
            message.opacity = 0
        }
    }

    Text {
        id: messageText
        anchors.fill: parent
        anchors.margins: 10
        text: txt
        wrapMode: Text.WordWrap
        color: "white"
        font.family: "Calibri"
        font.pixelSize:  parent.width / 14
        font.bold: true
        onLinkActivated: {
            GameEngine.openLink(link)
        }
    }
}

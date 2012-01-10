/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
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

import QtQuick 1.0
import QtWebKit 1.0

WebView {
    id: webView
    width: 200
    height: 150
    url: "alerts.html"

    onAlert: popup.show(message)

    Rectangle {
        id: popup

        color: "red"
        border.color: "black"; border.width: 2
        radius: 4

        y: parent.height // off "screen"
        anchors.horizontalCenter: parent.horizontalCenter
        width: label.width + 5
        height: label.height + 5

        opacity: 0

        function show(text) {
            label.text = text
            popup.state = "visible"
            timer.start()
        }
        states: State {
            name: "visible"
            PropertyChanges { target: popup; opacity: 1 }
            PropertyChanges { target: popup; y: (webView.height-popup.height)/2 }
        }

        transitions: [
            Transition { from: ""; PropertyAnimation { properties: "opacity,y"; duration: 65 } },
            Transition { from: "visible"; PropertyAnimation { properties: "opacity,y"; duration: 500 } }
        ]

        Timer {
            id: timer
            interval: 1000

            onTriggered: popup.state = ""
        }

        Text {
            id: label
            anchors.centerIn: parent
            width: webView.width  *0.75

            color: "white"
            font.pixelSize: 20
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            smooth: true
        }
    }
}

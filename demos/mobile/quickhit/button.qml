/******************************************************************************
 *
 * Copyright (c) 2011 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * Contact: Nokia Corporation (qt-info@nokia.com)
 *
 * $QT_BEGIN_LICENSE:BSD$
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the author organization nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $QT_END_LICENSE$
 *
 *****************************************************************************/

import Qt 4.7

Item {
    width: 30
    height: 30
    smooth: true

    property string buttonPath // for example "qrc:/gfx/exit.png"
    property int buttonId: 0

    signal btnClicked(int buttonId)

    property bool animationEnabled: true

    Image {
        id: image
        anchors.fill: parent
        source: buttonPath
        fillMode: Image.PreserveAspectFit
        smooth: true
    }

    SequentialAnimation {
        id:anim
        PropertyAction  { target: image; property: "scale"; value:"0.7"}
        PauseAnimation { duration: 200 }
        PropertyAction  { target: image; property: "scale"; value:"1.0"}
    }

    Timer {
        id: buttonPressedTimer
        interval: 300; running: false; repeat: false
        onTriggered: btnClicked(buttonId)
    }

    MouseArea {
        anchors.fill: parent
        onPressed: {
            if (animationEnabled && !anim.running) {
                anim.restart()
            }
            buttonPressedTimer.restart()
        }
    }

}

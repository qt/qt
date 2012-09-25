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
import "mycomponents"

/* The base canvas for all QML drawing. */
Rectangle {
    id: application

    property int targetNoteIndex: 0
    property alias frequency: noteChooser.currentFrequency
    //Data provided to C++.
    property bool isInput: true
    property bool isMuted: false
    property bool isAuto: true
    property alias maxVoiceDifference: voiceDifferenceMeter.maxValue
    property real volume: 0.5
    property real sensitivity: 0.5

    //Signals to C++.
    signal volumeChanged(real volume)
    signal microphoneSensitivityChanged(real sensitivity)
    signal targetFrequencyChanged(real frequency)
    signal modeChanged(bool isInput)
    signal muteStateChanged(bool isMuted)

    //Slots for signals coming from C++.
    function voiceDifferenceChanged(difference) {
        if (isAuto) timer.running = true;
        voiceDifferenceMeter.valueChanged(difference);
        noteImage.glowing = false
    }
    function correctFrequencyObtained() {
        noteImage.glowing = true
    }
    function lowVoice() {
        noteImage.glowing = false
    }

    //Private function for changing the target frequency automatically.
    function calculateTargetFrequency(difference) {
        var tempDifference = Math.abs(difference);
        var tempIndex = targetNoteIndex
        while (!(difference < 0 && tempIndex == 0) &&
               tempDifference >= notes.get(tempIndex-(difference<0)).interval/2) {
            tempDifference -= notes.get(tempIndex-(difference<0)).interval;
            tempIndex += difference/Math.abs(difference);
        }
        if (tempIndex != targetNoteIndex) {
            targetNoteIndex = tempIndex
            noteChooser.currentFrequency = notes.get(targetNoteIndex).frequency;
            targetFrequencyChanged(frequency);
        }
    }

    width: 360; height: 640
    color: "black"

    //Provides data for functions.
    NotesModel {id: notes}

    /* A timer for changing the target frequency automatically.
     * This is needed for avoiding recursion. */
    Timer {
        id: timer

        interval: 1
        onTriggered: calculateTargetFrequency(voiceDifferenceMeter.value)
    }

    //A meter for showing the difference between current and target frequency.
    Meter {
        id: voiceDifferenceMeter

        maxValue: 12
        minValue: -maxValue
        height: imageSize.height/background.sourceSize.height*parent.height
        width: imageSize.width/background.sourceSize.width*parent.width
        anchors {
            topMargin: 100/background.sourceSize.height*parent.height
            horizontalCenter: parent.horizontalCenter
            top: parent.top
        }
    }

    Image {
        id: background

        anchors.fill: parent
        smooth: true
        source: "./mycomponents/images/guitartuner_skin.png"
    }

    //A button for quitting the application.
    Image {
        id: quitButton

        width: sourceSize.width/background.sourceSize.width*parent.width
        height: sourceSize.height/background.sourceSize.height*parent.height
        source: "./mycomponents/images/power.png"
        smooth: true
        KeyNavigation.up: volumeAdjuster
        KeyNavigation.down: modeButton
        Keys.onEnterPressed: Qt.quit()
        anchors{
            leftMargin: 297/background.sourceSize.width*parent.width
            left: parent.left;
            topMargin: 17/background.sourceSize.height*parent.height
            top: parent.top
        }

        MouseArea {
            anchors.fill: parent
            onClicked: Qt.quit()
        }
    }

    //An image for showing the target note.
    Image {
        id: noteImage

        property bool glowing: false

        width: sourceSize.width/background.sourceSize.width*parent.width
        height: sourceSize.height/background.sourceSize.height*parent.height
        source: glowing ? notes.get(targetNoteIndex).glowSource : notes.get(targetNoteIndex).bigSource

        anchors {
            topMargin: 273/background.sourceSize.height*parent.height
            top: parent.top
            horizontalCenter: parent.horizontalCenter
        }
    }

    //A button for choosing the input/output mode.
    Image {
        id: modeButton

        function buttonPressed() {
            isInput = !isInput
            modeChanged(isInput)
            if (isInput) {
                soundIcons.source = "./mycomponents/images/sensitivity.png"
                source = "./mycomponents/images/voicemode_off.png"
                volumeAdjuster.setValue(sensitivity)
            }
            else {
                //Change off from "auto" mode
                if (isAuto) {
                    noteChooser.pushButton(targetNoteIndex)
                }
                if (isMuted) {
                    soundIcons.source = "./mycomponents/images/volume_off.png";
                }
                else
                    soundIcons.source = "./mycomponents/images/volume.png"
                source = "./mycomponents/images/voicemode_on.png"
                volumeAdjuster.setValue(volume)
            }
        }

        width: sourceSize.width/background.sourceSize.width*parent.width
        height: sourceSize.height/background.sourceSize.height*parent.height
        smooth: true
        source: "./mycomponents/images/voicemode_off.png"
        KeyNavigation.up: quitButton
        KeyNavigation.down: noteChooser
        Keys.onEnterPressed: buttonPressed()
        anchors {
            leftMargin: 16/background.sourceSize.width*parent.width
            left: parent.left
            topMargin: 353/background.sourceSize.height*parent.height
            top: parent.top
        }

        MouseArea {
            anchors.fill: parent
            onPressed: {
                parent.focus = true
                parent.scale = 0.95
            }
            onReleased: {
                parent.scale = 1/0.95
            }
            onClicked: parent.buttonPressed()
        }
    }

    //Buttons for choosing the target note.
    NoteButtonView {
        id: noteChooser

        width: parent.width*0.95; height: width/model.count
        onNoteSelected: {
            if (note == "Auto") {
                if (!isAuto) {
                    isAuto = true
                }
                if (!isInput) {
                    modeButton.buttonPressed()
                }
            }
            else {
                timer.running = false;
                isAuto = false
                targetNoteIndex = index
                targetFrequencyChanged(frequency)
            }
            focus = true
        }
        KeyNavigation.up: modeButton
        KeyNavigation.down: soundIcons
        anchors {
            horizontalCenter: parent.horizontalCenter
            topMargin: 454/background.sourceSize.height*parent.height
            top: parent.top
        }
    }

    //An element for showing the mode and changing the mute state.
    Image {
        id: soundIcons

        function stateChanged() {
            isMuted = !isMuted
            muteStateChanged(isMuted)
            if (isMuted) {
                source = "qrc:/src/mycomponents/images/volume_off.png"
            }
            else {
                source = "qrc:/src/mycomponents/images/volume.png"
            }
        }

        width: sourceSize.width/background.sourceSize.width*parent.width
        height: sourceSize.height/background.sourceSize.height*parent.height
        smooth: true
        source: "./mycomponents/images/sensitivity.png"
        Keys.onEnterPressed: stateChanged()
        KeyNavigation.up: noteChooser
        KeyNavigation.down: quitButton
        KeyNavigation.left: volumeAdjuster
        KeyNavigation.right: volumeAdjuster
        anchors {
            leftMargin: 42/background.sourceSize.width*parent.width
            left: parent.left
            topMargin: 565/background.sourceSize.height*parent.height
            top: parent.top
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                if (!isInput) {
                    parent.stateChanged()
                }
                parent.focus = true
            }
        }
    }

    //An element for adjusting volume.
    Adjuster {
        id: volumeAdjuster

        max: 1
        value: 0.5
        width: 222/background.sourceSize.width*parent.width
        height: parent.height*0.1
        onFocusChangedByClick: focus = true
        onArrowPressedWhenValueOverLimits: soundIcons.focus = true
        KeyNavigation.up: modeButton
        KeyNavigation.down: quitButton
        anchors {
            leftMargin: 98/background.sourceSize.width*parent.width
            left: parent.left
            verticalCenter: soundIcons.verticalCenter
        }
        onValueChanged: {
            if (isInput) {
                sensitivity = value;
                microphoneSensitivityChanged(1-sensitivity)
            }
            else {
                volume = value
                volumeChanged(volume)
            }
        }
    }
}

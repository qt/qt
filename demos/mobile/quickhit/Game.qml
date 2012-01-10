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
    id: gameArea
    width: 360; height: 640 // NOTE: have to exists some default sizes
    anchors.fill: parent
    objectName: "gameArea"


    // Who win, you or computer
    property bool gameOverProperty: false

    // for info message to user
    property variant messageBox

    // Level activated signal for Qt side to load activated level
    signal levelActivated(int index)

    // Pause whole game, if level is loaded
    function pauseGame() {
        if (GameScript.levelPlugin) {
            GameScript.pauseGame(true)
            myShip.focus = false
            btnPause.opacity = 0
            menu.showPauseMenu()
        }
    }

    // Qt side call this for QML take new level into use
    function levelReadyForCreation() {
        // Hide menu
        menu.hideMenu()
        // Show message
        message.showMessage("Loading Level...",2000)
        // Wait before create level
        levelCreationTimer.restart()
    }

    // Fires ememy missile. Function is called from Qt side
    function fireEnemyMissile(aXpox, aFromYpos, aToYpos) {
        GameScript.fireEnemyMissile(aXpox, aFromYpos, aToYpos)
    }

    // Game Over handling. Function is called from Qt side
    function gameOver(youWin) {
        // Stop GameEngine timers
        GameEngine.enableEngineTimer(false)
        gameOverProperty = youWin
        GameScript.hideMissiles()
        gameOverTimer.restart()
        //GameEngine.vibra()
    }

    // End game and show menu
    function endGame(showMessage) {
        backgroundPic.opacity = 1
        // Stop GameEngine timers
        GameEngine.enableEngineTimer(false)
        // Do game over
        GameScript.gameOver()
        GameScript.hideMissiles()
        // Clear GameEngine QML objects
        GameEngine.clearQmlObjects()
        myShip.opacity = 0
        myShip.focus = false
        btnPause.opacity = 0
        idMainLogo.opacity = 1
        menu.showLevelMenu()
        if (showMessage) {
            message.showMessage("Game End",2000)
        }
    }

    // Timer for level creationing
    Timer {
        id: levelCreationTimer
        interval: 1000; running: false; repeat: false
        onTriggered: {
            btnPause.opacity = 1
            myShip.opacity = 1
            myShip.focus = true
            // Create new level
            var ret = GameScript.createLevel()
            if (ret==0) {
                backgroundPic.opacity = 0
                // Find QML object for GameEngine
                GameEngine.findQmlObjects()
                // Enable GameEngine timer
                GameEngine.enableEngineTimer(true)
            } else {
                // Level cannot be created!
                endGame(false)
            }
        }
    }

    // Timer for game over
    Timer {
        id: gameOverTimer
        interval: 1500; running: false; repeat: false
        onTriggered: {
            backgroundPic.opacity = 1
            idMainLogo.opacity = 1
            // Do game over
            GameScript.gameOver()
            // Clear GameEngine QML objects
            GameEngine.clearQmlObjects()
            menu.showLevelMenu()
            if (gameOverProperty) {
                GameEngine.playInternalSound(4)
                message.showMessage("Game Over, You win!",2000)
            } else {
                GameEngine.playInternalSounds(3,3)
                message.showMessage("Game Over, You lose",2000)
            }
        }
    }

    // Menu signal to slot connections
    Connections {
        target: menu
        // Level selected
        onLevelSelected: {
            // Stop bigship animation
            bigShipAnim.stop()
            bigShip.opacity = 0
            // Hide logo
            idMainLogo.opacity = 0
            // Signal level activated
            gameArea.levelActivated(levelIndex)
        }
        // Level resumed selected
        onResumeSelected: {
            menu.hideMenu()
            GameScript.pauseGame(false)
            myShip.focus = true
        }
    }

    // This component creation completed
    Component.onCompleted: {
        // Show main Level selection menu
        menu.showLevelMenu()

        // Start big ship animation
        bigShipAnim.restart()

        // Set variable
        messageBox = message

        // Play game start sound
        GameEngine.gameStartSound()

        // Fade splash screen
        fadeAnim.restart()
        // Rotate splash logo
        rotAnim.restart()
    }


    // Background image for the game
    Image {
        id: backgroundPic
        z:1
        source: "qrc:/gfx/background2.png"
        fillMode: Image.PreserveAspectCrop
        smooth: true
        anchors.fill: parent
    }

    Image {
        id: bigShip
        source: "qrc:/gfx/bigship.png"
        z:1.5
        smooth: true
        x: width * -1
        y: parent.height * 0.55
    }
    SequentialAnimation {
        id: bigShipAnim;
        NumberAnimation {target:bigShip; property:"x"; to:gameArea.width; easing.type: Easing.Linear; duration: 80000 }
        PropertyAction {target:bigShip; properties: "opacity"; value: 0}
    }


    // Enemies grid
    Item {
        // This is general level QML plaseholder
        // Into this is level QML created in createLevel()
        id:levelId
        anchors.fill: parent
        z:2
    }

    Image {
        id: idMainLogo
        z:19
        source: "qrc:/gfx/quickhit_logo.png"
        smooth: true
        anchors.horizontalCenter: gameArea.horizontalCenter
        y:gameArea.height / 10 * 1
    }

    // Game menu
    Menu {
        z:20
        id:menu
        width: gameArea.width / 5 * 4
        height: gameArea.height / 10 * 4
        anchors.verticalCenter: gameArea.verticalCenter
        anchors.horizontalCenter: gameArea.horizontalCenter
    }

    // My ship
    MyShip {
        z:10
        id: myShip
        opacity: 0
    }

    // Mouse area of your ship
    MouseArea {
        //anchors.fill: parent
        width: parent.width
        height: myShip.height
        x:0
        y:gameArea.height - myShip.height

        drag.target: myShip
        drag.axis: Drag.XAxis
        drag.minimumX: 0
        drag.maximumX: gameArea.width - myShip.width

        property int startPos: 0
        property int dragCount: 0

        onPressed: {
            startPos = mouseX
            dragCount = 0
        }
        onPositionChanged: {
            //dragCount = Math.abs(mouseX-startPos)
        }
        onReleased: {
            //if (dragCount<20) {
            myShip.fire()
            //}
        }
    }

    // Pause button
    Button {
        id: btnSound
        z:12
        //animationEnabled: false
        anchors.top: parent.top
        anchors.topMargin: 10
        buttonPath: "qrc:/gfx/soundOn.png"
        buttonId: 4
        width: gameArea.width / 10
        height: gameArea.width / 10
        x: gameArea.width - width - 15
        opacity: 1
        Connections {
            target: btnSound
            onBtnClicked: {
                if (btnSound.buttonId==4) {
                    // Sound off
                    btnSound.buttonPath = "qrc:/gfx/soundOff.png"
                    btnSound.buttonId = 5
                    GameEngine.enableSounds(false)
                } else {
                    // Sound on
                    btnSound.buttonPath = "qrc:/gfx/soundOn.png"
                    btnSound.buttonId = 4
                    GameEngine.enableSounds(true)
                }
            }
        }
    }

    // Pause button
    Button {
        id: btnPause
        z:13
        anchors.top: parent.top
        anchors.topMargin: 10
        buttonPath: "qrc:/gfx/pause.png"
        buttonId: 3
        width: gameArea.width / 10
        height: gameArea.width / 10
        x: gameArea.width - width - btnPause.width - 15*2
        opacity: 0
        Connections {
            target: btnPause
            onBtnClicked: {
                GameScript.pauseGame(true)
                myShip.focus = false
                btnPause.opacity = 0
                menu.showPauseMenu()
            }
        }
    }


    // Hidden missiles ready for to be launched
    Missile {
        z:3
        id: missile_1
        x:0
        y:10
    }
    Missile {
        z:4
        id: missile_2
        x: 20
        y:10
    }
    Missile {
        z:5
        id: missile_3
        x: 40
        y:10
    }
    Missile {
        z:6
        id: missile_4
        x: 60
        y:10
    }
    Missile {
        z:7
        id: missile_5
        x: 80
        y:10
    }
    Missile {
        z:8
        objectName: "enemy_missile"
        id: enemy_missile_1
        enemyMissile: true
    }
    Missile {
        z:9
        objectName: "enemy_missile"
        id: enemy_missile_2
        enemyMissile: true
    }

    // Messages to the user
    Message {
        id: message
        z:21
    }


    // Splach screen
    Rectangle {
        id: blackFace
        x:-2
        y:0
        width: parent.width + 2
        height: parent.height
        z:100
        color: "black"
        opacity: 1

        MouseArea {
            anchors.fill: parent
            onPressed: {
                mouse.accepted = true
                fadeAnim.stop()
                blackFace.opacity = 0
            }
        }

        Image {
            id: idLogo
            source: "qrc:/gfx/quickhit_logo.png"
            smooth: true
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            opacity: 0
        }
        NumberAnimation { id: rotAnim; target: idLogo; property: "rotation"; to: 20; duration: 9000 }

    }
    // Splach screen animation
    SequentialAnimation {
        id: fadeAnim
        PauseAnimation {duration: 1000 }
        NumberAnimation {target: idLogo; property: "opacity"; from: 0; to: 1; duration: 2000 }
        PauseAnimation {duration: 2000 }
        NumberAnimation {target: idLogo; property: "opacity"; from: 1; to: 0; duration: 2000 }
        NumberAnimation {target: blackFace; property: "opacity"; from: 1; to: 0; duration: 2000 }
    }

}

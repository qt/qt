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
import Qt.labs.particles 1.0
import "Level.js" as LevelScript

Item {
    id: enemies
    anchors.fill: parent

    property int enemiesCreated: 0
    property int enemiesEnabled: 0

    property int backOneTop
    property int backTwoTop

    // Long background 2
    Image {
        id: longBackground_2
        source: "file:/"+LevelPlugin.pictureRootPath()+"background_loop.png"
        fillMode: Image.Stretch
        smooth: false
        width: gameArea.width
        height: gameArea.height
    }

    // Long background 1
    Image {
        id: longBackground_1
        source: "file:/"+LevelPlugin.pictureRootPath()+"background_loop.png"
        fillMode: Image.Stretch
        smooth: false
        width: gameArea.width
        height: gameArea.height
    }

    // Long background Up to down animation
     ParallelAnimation {
        id: backgroundAnim;
        loops: Animation.Infinite
        PropertyAnimation { target:longBackground_1; property:"y";
        from:backOneTop; to:gameArea.height; easing.type:Easing.Linear; duration:10000 }
        PropertyAnimation { target:longBackground_2; property:"y";
        from:backTwoTop; to:backOneTop; easing.type:Easing.Linear; duration:10000 }
     }



    // ***************************************************
    // *** NOTE: This is mandatory for all level QML files
    function pause(doPause) {
        if (doPause) {
            enemyCreationTimer.stop()
            backgroundAnim.stop()
        } else {
            enemyCreationTimer.restart()
            backgroundAnim.restart()
        }
        for (var i=0;i<LevelScript.hiddenEnemies.length;i++) {
            var enemy = LevelScript.hiddenEnemies[i]
            enemy.pause(doPause)
        }
    }


    // ***************************************************
    // *** NOTE: This is mandatory for all level QML files
    objectName: "level"


    // Enemy creation timer
    Timer {
        id: enemyCreationTimer
        interval: 1000; running: false; repeat: true
        onTriggered: {
            enableEnemy()
        }
    }

    function enableEnemy() {
        if (enemiesEnabled < enemiesCreated) {
            var enemy = LevelScript.hiddenEnemies[enemiesEnabled]
            enemy.opacity = 1
            enemy.x = GameEngine.randInt(0,gameArea.width-enemy.width-50)
            enemy.startMovingAnim()
            enemyCreationTimer.stop()
            enemyCreationTimer.interval = GameEngine.randInt(500,2000)
            enemyCreationTimer.restart()
            enemiesEnabled = enemiesEnabled + 1
            GameEngine.playSound(4) // NOTE: Enable enemy sound
        } else {
            enemyCreationTimer.stop()
        }
    }

    // Hidden enemies group
    function createHiddenEnemiesGroup() {
        for (var i=0;i<LevelPlugin.enemyCount();i++) {
            var enemyComponent = Qt.createComponent("file:/"+LevelPlugin.qmlRootPath()+"Enemy.qml")
            var enemyObject = enemyComponent.createObject(enemies)
            LevelScript.hiddenEnemies.push(enemyObject)
            enemiesCreated = enemiesCreated + 1
        }
    }

    Component.onCompleted: {
        // Create hidden enemies
        createHiddenEnemiesGroup()
        // Flying moons animation
        starBurstTimer.restart()
        // Create enemies timer
        enemyCreationTimer.restart()

        // Start Long background loop animation
        backOneTop = 0
        longBackground_1.y = backOneTop;
        backTwoTop = gameArea.height * -1
        longBackground_2.y = backTwoTop;
        backgroundAnim.restart()

        GameEngine.playSounds(5,3)  //NOTE: Level starts sound, 3 times
    }

    Timer {
        id: starBurstTimer
        interval: starParticles.lifeSpan+GameEngine.randInt(100,2000); running: false; repeat: true
        onTriggered: {
        starParticles.burst(GameEngine.randInt(4,6))
        }
     }

    // ***************************************************
    // *** NOTE: This is mandatory for all level QML files
    // Execute explode particle effect
    function explode(x,y) {
        explodeParticles.x = x
        explodeParticles.y = y
        explodeParticles.burst(20)
        explodeParticles.opacity = 1
    }
    // ***************************************************
    // *** NOTE: This is mandatory for all level QML files
    // Explode particle effect
    Particles {
        id: explodeParticles
        opacity: 0
        width: 1
        height: 1
        emissionRate: 0
        source: "file:/"+LevelPlugin.pictureRootPath()+"blue_fire.png"
        lifeSpan: 500
        lifeSpanDeviation: 2000
        count: -1
        angle: 270
        angleDeviation: 65
        velocity: 100
        velocityDeviation: 100
        ParticleMotionGravity {
            yattractor: 1000
            xattractor: 300
            acceleration: 50
        }
    }

  Particles {
         id: starParticles
         y: -50
         x: 0
         width: parent.width
         height: 50
         emissionRate: 0
         source: "file:/"+LevelPlugin.pictureRootPath()+"star3.png"
         lifeSpan: 5000
         count: -1
         angle: 90
         velocity: 100
         opacity: 0.7
        ParticleMotionGravity {
            yattractor: 1000
            xattractor: 500
        }
     }


}






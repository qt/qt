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
import Qt.labs.particles 1.0

Rectangle {
    id: enemies
    anchors.fill: parent
    color: "black"


    // Background image for the level
    Image {
        id: background1
        source: "file:/"+LevelPlugin.pictureRootPath()+"background3.png"
        fillMode: Image.PreserveAspectCrop
        smooth: true
        //anchors.fill: parent
        width: parent.width
        height: parent.height
    }


    // ***************************************************
    // *** NOTE: This is mandatory for all level QML files
    function pause(doPause) {
        if (doPause) {
        rightLeftAnim.pause()
        upToDownAnim.pause()
        } else {
        rightLeftAnim.resume()
        upToDownAnim.resume()
        }
    }


    // ***************************************************
    // *** NOTE: This is mandatory for all level QML files
    objectName: "level"

    property int enemySpeed: LevelPlugin.enemySpeed()
    property int enemyMaxWidth

    // Create enemies dynamically
    function createEnemies(amount) {
        var b = false;
        for (var i=0;i<amount;i++) {
            if (b){
                Qt.createQmlObject('import QtQuick 1.0; Image { property bool hit:false; width:50; objectName:"enemy"; smooth:true; fillMode:Image.PreserveAspectFit; source:"file:/"+LevelPlugin.pictureRootPath()+"enemy1.png";}',enemiesGrid);
            } else {
                Qt.createQmlObject('import QtQuick 1.0; Image { property bool hit:false; width:50; objectName:"enemy"; smooth:true; fillMode:Image.PreserveAspectFit; source:"file:/"+LevelPlugin.pictureRootPath()+"enemy2.png";}',enemiesGrid);
            }
            b = !b;
        }
    }

    // Calculate enemy max width
    function calEnemyMaxWidth() {
        var enemyCountInCol = LevelPlugin.enemyCount() / LevelPlugin.enemyRowCount();
        enemyCountInCol++; // add some extra space needs for width
        if (enemyCountInCol*LevelPlugin.graphSize(LevelPlugin.pictureRootPath()+"enemy1.png").width > gameArea.width) {
            enemyMaxWidth = gameArea.width / (enemyCountInCol + 1);
        }
        else {
            enemyMaxWidth = LevelPlugin.graphSize(LevelPlugin.pictureRootPath()+"enemy1.png").width;
        }
    }


    Component.onCompleted: {
        // Calculate enemy max width
        calEnemyMaxWidth()

        // Create enemies dynamically after component created
        createEnemies(LevelPlugin.enemyCount())

        // Start animations
        enemiesGrid.y = enemiesGrid.height * -1
        rightLeftAnim.restart()
        upToDownAnim.restart()

        // Flying stars animation, not in Maemo
        if (!GameEngine.isMaemo()) {
            starBurstTimer.restart()
        }

        GameEngine.playSound(4) // NOTE: Level start sound
    }


    Timer {
        id: starBurstTimer
        interval: starParticles.lifeSpan+GameEngine.randInt(100,2000); running: false; repeat: true
        onTriggered: {
        starParticles.burst(GameEngine.randInt(4,10))
        }
     }


    // Right-left animation
    SequentialAnimation {
        id: rightLeftAnim
        loops: Animation.Infinite
        NumberAnimation { target:enemiesGrid; property:"x"; from: 0; to:gameArea.width - enemiesGrid.width; easing.type: Easing.OutQuad; duration: 3000 }
        NumberAnimation { target:enemiesGrid; property:"x"; to: 0; easing.type: Easing.OutQuad; duration: 3000 }
    }

    // Up to down animation
    PropertyAnimation { id: upToDownAnim; target:enemiesGrid; property:"y"; to: gameArea.height;
        easing.type: Easing.Linear; duration: enemies.enemySpeed }

    Grid {
        id: enemiesGrid
        objectName: "enemiesGrid"
        columns: LevelPlugin.enemyCount() / LevelPlugin.enemyRowCount()
        spacing: 8
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
        source: "file:/"+LevelPlugin.pictureRootPath()+"red_fire.png"
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
         lifeSpan: 3000
         count: -1
         angle: 90
         velocity: 100
        ParticleMotionGravity {
            yattractor: 1000
            xattractor: 500
        }
     }


}






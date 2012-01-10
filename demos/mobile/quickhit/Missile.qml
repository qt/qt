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

Item {
    id: missile
    objectName: "missile"
    opacity: 0 // transparent by default

    property int fromYpos
    property int toYpos
    property variant myMissileSize
    property bool enemyMissile: false
    property int defaultX: 0
    property int defaultY: 0

    function storeDefaultPos() {
        defaultX = missile.x
        defaultY = missile.y
    }

    function setToDefaultPos() {
        missile.x = defaultX
        missile.y = defaultY
        opacity = 0.5
    }

    function createGraphicsForLevel() {
        missile.myMissileSize = LevelPlugin.graphSize(LevelPlugin.pathToMissilePic())
        missile.height = myMissileSize.height;
        missile.width = myMissileSize.width;
        if (missile.enemyMissile) {
            missileImage.source = "file:/"+LevelPlugin.pathToEnemyMissilePic()
        } else {
            missileImage.source = "file:/"+LevelPlugin.pathToMissilePic()
        }
    }

    // Execute fire!
    function fire(aXpox, aFromYpos, aToYpos) {
        missile.x = aXpox - missile.width / 2
        missile.y = aFromYpos
        missile.fromYpos = aFromYpos
        missile.toYpos = aToYpos
        missile.opacity = 1
        GameEngine.playSound(2) // NOTE: 3 for your missile sound
        flying.restart()
    }

    // Enemy fires!
    function enemyFire(aXpox, aFromYpos, aToYpos) {
        missile.x = aXpox - missile.width / 2
        missile.y = aFromYpos
        missile.fromYpos = aFromYpos
        missile.toYpos = aToYpos
        missile.opacity = 1
        GameEngine.playSound(3) // NOTE: 3 for enemy missile sound
        flyingEnemy.restart()
    }

    // Stop missile
    function stop() {
        flying.stop()
    }

    // Pause missile
    function pause(doPause) {
        if (doPause) {
            flying.pause()
            flyingEnemy.pause()
        } else {
            flying.resume()
            flyingEnemy.resume()
        }
    }

    Component.onCompleted: {
        if (!enemyMissile) {
            storeDefaultPos()
            setToDefaultPos()
        }
    }

    // Animates missile flying to the target
    SequentialAnimation {
        id: flying
        PropertyAnimation {target: missile; properties: "y";
            from: fromYpos; to: toYpos; duration: 2000; easing.type: Easing.InCubic }
        //PropertyAction {target: missile; properties: "opacity"; value: 0}
        ScriptAction { script: setToDefaultPos() }
    }

    SequentialAnimation {
        id: flyingEnemy
        PropertyAnimation {target: missile; properties: "y";
            from: fromYpos; to: toYpos; duration: 2000; easing.type: Easing.InCubic }
        PropertyAction {target: missile; properties: "opacity"; value: 0}
    }

    Image {
        id: missileImage
        smooth: true
    }
}


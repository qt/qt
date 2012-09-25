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

var levelPlugin = null

// ---------------------------------------------------------------------------
// hideMissiles
function hideMissiles() {
    missile_1.stop()
    missile_1.opacity = 0;

    missile_2.stop()
    missile_2.opacity = 0;

    missile_3.stop()
    missile_3.opacity = 0;

    missile_4.stop()
    missile_4.opacity = 0;

    missile_5.stop()
    missile_5.opacity = 0;

    enemy_missile_1.opacity = 0;
    enemy_missile_2.opacity = 0;
}

// ---------------------------------------------------------------------------
// GameOver
function gameOver() {

    // Stop GameEngine timers
    GameEngine.enableEngineTimer(false)

    // Delete loaded plugin level
    deleteLevel()

    // Hiding
    myShip.opacity = 0
    btnPause.opacity = 0
}

// ---------------------------------------------------------------------------
// Pause game
function pauseGame(doPause) {
    missile_1.pause(doPause)
    missile_2.pause(doPause)
    missile_3.pause(doPause)
    missile_4.pause(doPause)
    missile_5.pause(doPause)
    enemy_missile_1.pause(doPause)
    enemy_missile_2.pause(doPause)

    if (doPause) {
        btnPause.opacity = 0
    } else {
        btnPause.opacity = 1
    }

    GameEngine.pauseLevel(doPause)
}

// ---------------------------------------------------------------------------
// Fire your missile
function fireMissile(aXpox, aFromYpos, aToYpos) {
    if (missile_1.opacity<1) {
        missile_1.fire(aXpox,aFromYpos,aToYpos)
    }
    else if (missile_2.opacity<1) {
        missile_2.fire(aXpox,aFromYpos,aToYpos)
    }
    else if (missile_3.opacity<1) {
        missile_3.fire(aXpox,aFromYpos,aToYpos)
    }
    else if (missile_4.opacity<1) {
        missile_4.fire(aXpox,aFromYpos,aToYpos)
    }
    else if (missile_5.opacity<1) {
        missile_5.fire(aXpox,aFromYpos,aToYpos)
    }
}

// ---------------------------------------------------------------------------
// Fire enemy missile
function fireEnemyMissile(aXpox, aFromYpos, aToYpos) {
    if (enemy_missile_1.opacity==0) {
        enemy_missile_1.enemyFire(aXpox,aFromYpos,aToYpos)
    }
    else if (enemy_missile_2.opacity==0) {
        enemy_missile_2.enemyFire(aXpox,aFromYpos,aToYpos)
    }
}

// ---------------------------------------------------------------------------
// Create level QML component
function createLevel() {
    console.log("Creating level "+LevelPlugin.qmlRootPath()+"Level.qml")
    var levelComponent = Qt.createComponent("file:/"+LevelPlugin.qmlRootPath()+"Level.qml");
    if (levelComponent.status == Component.Ready) {
        if (levelPlugin) {
            deleteLevel()
        }
        levelPlugin = levelComponent.createObject(levelId);
        if (levelPlugin != null) {
            console.log("Level created")
        } else {
            console.log("Can not create level")
            messageBox.showErrorMessage("ERROR: Can not create level!",6000)
            return 1
        }
    } else {
        console.log("Can not find level, error:"+levelComponent.errorString())
        messageBox.showErrorMessage("ERROR: Can not find level!",6000)
        return 1
    }

    missile_1.createGraphicsForLevel()
    missile_1.setToDefaultPos()
    missile_2.createGraphicsForLevel()
    missile_2.setToDefaultPos()
    missile_3.createGraphicsForLevel()
    missile_3.setToDefaultPos()
    missile_4.createGraphicsForLevel()
    missile_4.setToDefaultPos()
    missile_5.createGraphicsForLevel()
    missile_5.setToDefaultPos()
    enemy_missile_1.createGraphicsForLevel()
    enemy_missile_2.createGraphicsForLevel()
    myShip.createGraphicsForLevel()

    return 0
}

// ---------------------------------------------------------------------------
// Delete level
function deleteLevel() {
    if (levelPlugin) {
        GameEngine.clearQmlObjects()
        levelPlugin.destroy()

    }
    levelPlugin = null;
}

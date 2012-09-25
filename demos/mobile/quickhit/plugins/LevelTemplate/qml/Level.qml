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

// Level own JavaScript file
import "Level.js" as LevelScript

Rectangle {
    id: enemies
    anchors.fill: parent
    property int enemiesCreated: 0
    property int enemiesEnabled: 0
    color: "black"

    // ***************************************************
    // *** NOTE: This "pause(bool)" function is mandatory for all level QML files
    function pause(doPause) {
    }

    // ***************************************************
    // *** NOTE: Level object name "level" is mandatory for all level QML files
    objectName: "level"

    // Enemy placeholder. Have to exist at least one enemy for prevent to level be stopped
    Enemy {
        x: parent.width / 2
        y: parent.height / 2
        opacity: 1
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
}

/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

import QtQuick 2.0
import Qt.labs.particles 2.0

Rectangle {
    id: root
    width: 360
    height: 540
    color: "black"

    ParticleSystem{
        id: particles
    }

    /*
    ColoredParticle{
        id: fireball
        anchors.fill: parent
        particles: ["E"]
        system: particles
        image: "content/particleA.png"
        colorVariation: 0.2
        color: "#00ff400f"
    }
    */
    ColoredParticle{
        id: smoke
        system: particles
        anchors.fill: parent
        particles: ["A", "B"]
        image: "content/particle.png"
        colorVariation: 0
        color: "#00111111"
    }
    ColoredParticle{
        id: flame
        anchors.fill: parent
        system: particles
        particles: ["C", "D"]
        image: "content/particle.png"
        colorVariation: 0.1
        color: "#00ff400f"
    }
    TrailEmitter{
        id: fire
        system: particles
        particle: "C"

        y: parent.height
        width: parent.width

        particlesPerSecond: 350
        particleDuration: 3500

        acceleration: PointVector{ y: -17; xVariation: 3 }
        speed: PointVector{xVariation: 3}

        particleSize: 24
        particleSizeVariation: 8
        particleEndSize: 4
    }
    FollowEmitter{
        id: fireSmoke
        particle: "B"
        system: particles
        follow: "C"
        width: root.width
        height: root.height - 68

        particlesPerParticlePerSecond: 1
        particleDuration: 2000

        speed: PointVector{y:-17*6; yVariation: -17; xVariation: 3}
        acceleration: PointVector{xVariation: 3}

        particleSize: 36
        particleSizeVariation: 8
        particleEndSize: 16
    }
    FollowEmitter{
        id: fireballFlame
        anchors.fill: parent
        system: particles
        particle: "D"
        follow: "E"

        particlesPerParticlePerSecond: 120
        particleDuration: 180
        emissionWidth: 8
        emissionHeight: 8

        particleSize: 16
        particleSizeVariation: 4
        particleEndSize: 4
    }
    
    FollowEmitter{
        id: fireballSmoke
        anchors.fill: parent
        system: particles
        particle: "A"
        follow: "E"

        particlesPerParticlePerSecond: 128
        particleDuration: 2400
        emissionWidth: 16
        emissionHeight: 16

        speed: PointVector{yVariation: 16; xVariation: 16}
        acceleration: PointVector{y: -16}

        particleSize: 24
        particleSizeVariation: 8
        particleEndSize: 8
    }
    TrailEmitter{
        id: balls
        system: particles
        particle: "E"

        y: parent.height
        width: parent.width

        particlesPerSecond: 2
        particleDuration: 7000

        speed: PointVector{y:-17*4*2; xVariation: 6*6}
        acceleration: PointVector{y: 17*2; xVariation: 6*6}

        particleSize: 12
        particleSizeVariation: 4
    }

}


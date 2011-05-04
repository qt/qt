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

import Qt.labs.particles 2.0
import Qt.labs.particles 2.0 as Qlp
import QtQuick 2.0

Item {
    id: root
    width: 360
    height: 540
    MouseArea{
        id: ma
        anchors.fill: parent
    }

    ParticleSystem { id: sys }
    Image {
        source: "content/finalfrontier.png"
        transformOrigin: Item.Center
        anchors.centerIn: parent
        smooth: true
        NumberAnimation on rotation {
            from: 0
            to: 360
            duration: 200000
            loops: Animation.Infinite
        }

    }
    ColoredParticle {
        system: sys
        particles: ["starfield"]
        image: "content/star.png"
        colorVariation: 0.3
        color: "white"
    }
    TrailEmitter {
        id: starField
        system: sys
        particle: "starfield"

        particlesPerSecond: 80
        particleDuration: 2500

        anchors.centerIn: parent

        //acceleration: AngleVector{angleVariation: 360; magnitude: 200}//Is this a better effect, more consistent speed?
        acceleration: PointVector{ xVariation: 200; yVariation: 200; }

        particleSize: 0
        particleEndSize: 80
        particleSizeVariation: 10
    }
    TrailEmitter{
        system: sys
        particle: "meteor"
        particlesPerSecond: 12
        particleDuration: 5000
        emitting: true
        acceleration: PointVector{ xVariation: 80; yVariation: 80; }
        particleSize: 15
        particleEndSize: 300
        anchors.centerIn: parent
     }
    SpriteParticle{
        system: sys
        particles: ["meteor"]
        sprites:[Sprite{
                id: spinState
                name: "spinning"
                source: "content/meteor.png"
                frames: 35
                duration: 40
                speedModifiesDuration: -0.1
                to: {"explode":0, "spinning":1}
            },Sprite{
                name: "explode"
                source: "content/_explo.png"
                frames: 22
                duration: 40
                speedModifiesDuration: -0.1
                to: {"nullFrame":1}
            },Sprite{//Not sure if this is needed, but seemed easiest
                name: "nullFrame"
                source: "content/nullRock.png"
                frames: 1
                duration: 1000
            }
        ]
    }
    SpriteGoal{
        particles: ["meteor"]
        system: sys
        goalState: "explode"
        jump: true
        anchors.centerIn: holder//A bug in affectors currently isn't compensating for relative x,y. when that's fixed this can just anchors.fill: rocketShip
        width: 60
        height: 60
    }
    Image {
        id: rocketShip
        source: "content/rocket.png"
        smooth: true
        anchors.centerIn: holder
        rotation: (circle.percent+0.25) * 360
        z: 2
    }
    Item {
        id: holder
        x: circle.x - Math.sin(circle.percent * 6.28316530714)*200
        y: circle.y + Math.cos(circle.percent * 6.28316530714)*200
        z: 1
    }

    Item {
        id: circle
        x: root.width / 1.2 
        y: root.height / 1.7
        property real percent: 0

        SequentialAnimation on percent {
            id: circleAnim1
            loops: Animation.Infinite
            running: true
            NumberAnimation {
            duration: 4000
            from: 1
            to: 0
            }

        }
    }
    ColoredParticle{
        z:0 
        system: sys
        particles: ["exhaust"]
        image: "content/particle4.png"

        color: "orange"
        SequentialAnimation on color {
            loops: Animation.Infinite
            ColorAnimation {
                from: "red"
                to: "cyan"
                duration: 1000
            }
            ColorAnimation {
                from: "cyan"
                to: "red"
                duration: 1000
            }
        }

        colorVariation: 0.2
    }
    TrailEmitter{
        id: trailsNormal2
        system: sys
        particle: "exhaust"

        particlesPerSecond: 300
        particleDuration: 500

        y: holder.y
        x: holder.x 

        speed: PointVector{ xVariation: 40; yVariation: 40; }
        speedFromMovement: 16

        acceleration: PointVector{ xVariation: 10; yVariation: 10; }

        particleSize: 4
        particleSizeVariation: 4
    }
}

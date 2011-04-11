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

Rectangle{
    id: root
    width: 360
    height: 540
    color: "black"
    Image{
        anchors.fill: parent
        source: "content/portal_bg.png"
    }
    ParticleSystem{ 
        id: particles 
        startTime: 2000
    }
    ColoredParticle{
        particles: ["center","edge"]
        anchors.fill: parent
        system: particles
        image: "content/particle.png"
        colorVariation: 0.1
        color: "#009999FF"
    }
    TrailEmitter{
        anchors.fill: parent
        particle: "center"
        system: particles
        particlesPerSecond: 200
        particleDuration: 2000
        emitting: true
        particleSize: 20
        particleSizeVariation: 2
        particleEndSize: 0
        shape: Ellipse{fill: false}
        speed: DirectedVector{
            targetX: root.width/2 
            targetY: root.height/2
            proportionalMagnitude: true
            magnitude: 0.5
        }
    }
    TrailEmitter{
        anchors.fill: parent
        particle: "edge"
        system: particles
        particlesPerSecond: 4000
        particleDuration: 2000
        emitting: true
        particleSize: 20
        particleSizeVariation: 2
        particleEndSize: 0
        shape: Ellipse{fill: false}
        speed: DirectedVector{
            targetX: root.width/2 
            targetY: root.height/2
            proportionalMagnitude: true
            magnitude: 0.1
            magnitudeVariation: 0.1
        }
        acceleration: DirectedVector{
            targetX: root.width/2 
            targetY: root.height/2
            targetVariation: 200
            proportionalMagnitude: true
            magnitude: 0.1
            magnitudeVariation: 0.1
        }
    }
}

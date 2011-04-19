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
    color: "goldenrod"
    width: 400
    height: 400
    ParticleSystem{id:sys}
    DeformableParticle{
        system: sys
        particles: ["goingLeft", "goingRight"]
        image: "content/singlesmile.png"
        rotation: 90
        autoRotation: true
    }
    DeformableParticle{
        system: sys
        particles: ["goingDown"]
        image: "content/squarefacespriteXX.png"
        rotation: 180
        yVector: PointVector{ y: 0.5; yVariation: 0.25; xVariation: 0.25; }
    }
    Timer{
        running: true
        repeat: false
        interval: 100
        onTriggered: emitA.emitting = true;
    }
    Timer{
        running: true
        repeat: false
        interval: 4200
        onTriggered: emitB.emitting = true;
    }
    Timer{
        running: true
        repeat: false
        interval: 8400
        onTriggered: emitC.emitting = true;
    }
    TrailEmitter{
        id: emitA
        x: 0
        y: 120
        system: sys
        emitting: false
        particle: "goingRight"
        speed: PointVector{ x: 100 }
        particleDuration: 4000
        particlesPerSecond: 2
        particleSize: 32
    }
    TrailEmitter{
        id: emitB
        x: 400
        y: 240
        system: sys
        emitting: false
        particle: "goingLeft"
        speed: PointVector{ x: -100 }
        particleDuration: 4000
        particlesPerSecond: 2
        particleSize: 32
    }
    TrailEmitter{
        id: emitC
        x: 0
        y: 360
        system: sys
        emitting: false
        particle: "goingDown"
        speed: PointVector{ x: 100 }
        particleDuration: 4000
        particlesPerSecond: 2
        particleSize: 32
    }
}

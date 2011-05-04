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
    width: 800
    height: 800
    id: root
    SpriteImage{
        sprites: [Sprite{
            name: "happy"
            source: "content/squarefacesprite2.png"
            frames: 6
            duration: 120
            to: {"silly": 0.4, "sad": 0.2, "cyclops":0.1, "boggled":0.3, "dying":0.0}
        }, Sprite{
            name: "silly"
            source: "content/squarefacesprite.png"
            frames: 6
            duration: 120
            to: {"love": 0.4, "happy": 0.1, "evil": 0.2, "cyclops":0.1, "boggled":0.2}
        }, Sprite{
            name: "sad"
            source: "content/squarefacesprite3.png"
            frames: 6
            duration: 120
            to: {"love" : 0.2, "evil": 0.2, "silly": 0.2, "cyclops":0.2, "boggled":0.2}
        }, Sprite{
            name: "cyclops"
            source: "content/squarefacesprite4.png"
            frames: 6
            duration: 120
            to: {"love": 0.1, "evil": 0.1, "silly":0.1, "boggled":0.1, "cyclops" : 1.0}
        }, Sprite{
            name: "evil"
            source: "content/squarefacesprite5.png"
            frames: 6
            duration: 120
            to: {"happy": 1.0}
        }, Sprite{
            name: "love"
            source: "content/squarefacesprite6.png"
            frames: 6
            duration: 120
            to: {"sad": 0.6, "evil":0.4, "boggled":0.2}
        }, Sprite{
            name: "boggled"
            source: "content/squarefacesprite7.png"
            frames: 6
            duration: 120
            to: {"love" : 0.2, "evil": 0.2, "silly": 0.2, "cyclops":0.1, "sad":0.2}
        }, Sprite{
            name: "dying"
            source: "content/squarefacespriteX.png"
            frames: 4
            duration: 120
            to: {"dead":1.0}
        }, Sprite{
            name: "dead"
            source: "content/squarefacespriteXX.png"
            frames: 1
            duration: 10000
        }]

        width: 100
        height: 100
        x: 20
        y: 20
        z:4
    }
    ParticleSystem{ id: sys }
    SpriteParticle{
        anchors.fill: parent
        id: particles
        system: sys
        sprites: [Sprite{
            name: "happy"
            source: "content/squarefacesprite2.png"
            frames: 6
            duration: 120
            to: {"silly": 0.4, "sad": 0.2, "cyclops":0.1, "boggled":0.3, "dying":0.0}
        }, Sprite{
            name: "silly"
            source: "content/squarefacesprite.png"
            frames: 6
            duration: 120
            to: {"love": 0.4, "happy": 0.1, "evil": 0.2, "cyclops":0.1, "boggled":0.2}
        }, Sprite{
            name: "sad"
            source: "content/squarefacesprite3.png"
            frames: 6
            duration: 120
            to: {"love" : 0.2, "evil": 0.2, "silly": 0.2, "cyclops":0.2, "boggled":0.2}
        }, Sprite{
            name: "cyclops"
            source: "content/squarefacesprite4.png"
            frames: 6
            duration: 120
            to: {"love": 0.1, "evil": 0.1, "silly":0.1, "boggled":0.1, "cyclops" : 1.0}
        }, Sprite{
            name: "evil"
            source: "content/squarefacesprite5.png"
            frames: 6
            duration: 120
            to: {"happy": 1.0}
        }, Sprite{
            name: "love"
            source: "content/squarefacesprite6.png"
            frames: 6
            duration: 120
            to: {"sad": 0.6, "evil":0.4, "boggled":0.2}
        }, Sprite{
            name: "boggled"
            source: "content/squarefacesprite7.png"
            frames: 6
            duration: 120
            to: {"love" : 0.2, "evil": 0.2, "silly": 0.2, "cyclops":0.1, "sad":0.2}
        }, Sprite{
            name: "dying"
            source: "content/squarefacespriteX.png"
            frames: 4
            duration: 120
            to: {"dead":1.0}
        }, Sprite{
            name: "dead"
            source: "content/squarefacespriteXX.png"
            frames: 1
            duration: 10000
        }]
    }
    TrailEmitter{
        system: sys
        particlesPerSecond: 16
        particleDuration: 10000
        emitting: true
        speed: AngleVector{angle: 90; magnitude: 60; angleVariation: 5}
        acceleration: PointVector{ y: 10 }
        particleSize: 30
        particleSizeVariation: 10
        width: parent.width
        height: 100
    }
    SpriteGoal{
        system: sys
        width: root.width;
        height: root.height/2;
        y: root.height/2;
        goalState:"dead"
    }
}

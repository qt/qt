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
    color: "black"
    width: 640
    height: 480
    ParticleSystem{
        id: sys
    }
    ColoredParticle{
        system: sys
        image: "content/particle.png"
        color: "white"
        colorVariation: 1.0
        alpha: 0.1
    }
    Component{
        id: emitterComp
        TrailEmitter{
            id: container
            TrailEmitter{
                id: emitMore
                system: sys
                emitting: true
                particlesPerSecond: 128
                particleDuration: 600
                particleSize: 16
                particleEndSize: 8
                speed: AngleVector{angleVariation:360; magnitude: 60}
            }

            property int life: 2600
            property real targetX: 0
            property real targetY: 0
            function go(){
                xAnim.start();
                yAnim.start();
                container.emitting = true
            }
            system: sys
            emitting: true
            particlesPerSecond: 64
            particleDuration: 600
            particleSize: 24
            particleEndSize: 8
            NumberAnimation on x{
                id: xAnim;
                to: targetX
                duration: life
                running: false
            }
            NumberAnimation on y{
                id: yAnim;
                to: targetY
                duration: life
                running: false
            }
            Timer{
                interval: life
                running: true
                onTriggered: container.destroy();
            }
        }
    }
    MouseArea{
        anchors.fill: parent
        onClicked:{
            for(var i=0; i<16; i++){
                var obj = emitterComp.createObject(root);
                obj.x = mouse.x
                obj.y = mouse.y
                obj.targetX = Math.random() * 640
                obj.targetY = Math.random() * 480
                obj.life = Math.round(Math.random() * 2400) + 200
                obj.go();
            }
        }
    }
}

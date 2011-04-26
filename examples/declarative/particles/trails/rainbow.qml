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
import QtQuick 2.0

Rectangle {
    id: root
    width: 360
    height: 540
    color: "black"

    ParticleSystem{ id: particles }
    ColoredParticle{
        system: particles
        colorVariation: 0.5
        alpha: 0

        image: "content/particle.png"
        colorTable: "content/colortable.png"
        sizeTable: "content/colortable.png"
    }
    TrailEmitter{
        system: particles
        particlesPerSecond: 500
        particleDuration: 2000

        y: root.height / 2 + Math.sin(t * 2) * root.height * 0.3
        x: root.width / 2 + Math.cos(t) * root.width * 0.3
        property real t;

        NumberAnimation on t {
            from: 0; to: Math.PI * 2; duration: 10000; loops: Animation.Infinite
        }

        speedFromMovement: 20

        speed: PointVector{ xVariation: 5; yVariation: 5;}
        acceleration: PointVector{ xVariation: 5; yVariation: 5;}

        particleSize: 16
        //particleEndSize: 8
        //particleSizeVariation: 8
    }
}

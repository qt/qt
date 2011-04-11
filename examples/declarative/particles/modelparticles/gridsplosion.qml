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
import "content"

Item{
    id: root
    width: 240
    height: 240
    property bool inGrid: false
    ParticleSystem{ id: sys }
    TrailEmitter{
        system: sys
        id: burster;
        emitting: false
        particlesPerSecond: 1000
        particleDuration: 500
        speed: PointVector{xVariation: 400; yVariation: 400}
        anchors.centerIn: parent
        Timer{
            interval: 1000
            running: true
            repeat: false
            onTriggered: burster.pulse(0.1);
        }
        Timer{
            interval: 2000
            running: true
            repeat: false
            onTriggered: {inGrid = true;}// sys.running = false;}
        }
    }
    ColoredParticle{
        system: sys
        image: "../trails/content/particle.png"
        color: "black"
        colorVariation: 0.0
    }
    GridView{ id: grid; cellWidth: 40; cellHeight: 40
        model: theModel.parts.grid
        width: 120
        height: 120
    }
    ModelParticle{
        system: sys
        model: theModel.parts.particles
    }
    Friction{
        system: sys
        factor: 1
    }
    Stasis{
        system: sys
        targetLife: 400
    }
    VisualDataModel{
        id: theModel
        delegate: Delegate2{}
        model: ListModel{
            ListElement{
                w: 40
                h: 20
                col: "forestgreen"
            }
            ListElement{
                w: 20
                h: 40
                col: "salmon"
            }
            ListElement{
                w: 20
                h: 20
                col: "lightsteelblue"
            }
            ListElement{
                w: 40
                h: 40
                col: "goldenrod"
            }
            ListElement{
                w: 40
                h: 20
                col: "forestgreen"
            }
            ListElement{
                w: 20
                h: 40
                col: "salmon"
            }
            ListElement{
                w: 20
                h: 20
                col: "lightsteelblue"
            }
            ListElement{
                w: 40
                h: 40
                col: "goldenrod"
            }
            ListElement{
                w: 0
                h: 0
                col: "white"//Hack because add isn't working well with old stuff
            }
        }
    }
}

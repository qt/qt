/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 1.0
import Qt.labs.particles 1.0

Item { id:link
    property bool dying: false
    property bool spawned: false
    property int type: 0
    property int row: 0
    property int column: 0
    property int rotation;

    width: 40;
    height: 40

    x: margin - 3 + gridSize * column
    y: margin - 3 + gridSize * row
    Behavior on x { NumberAnimation { duration: spawned ? heartbeatInterval : 0} }
    Behavior on y { NumberAnimation { duration: spawned ? heartbeatInterval : 0 } }


    Item {
        id: img
        anchors.fill: parent
        Image {
            source: {
                if(type == 1) {
                    "pics/blueStone.png";
                } else if (type == 2) {
                    "pics/head.png";
                } else {
                    "pics/redStone.png";
                }
            }

            transform: Rotation {
                id: actualImageRotation
                origin.x: width/2; origin.y: height/2;
                angle: rotation * 90
                Behavior on angle {
                    RotationAnimation{
                        direction: RotationAnimation.Shortest
                        duration: spawned ? 200 : 0
                    }
                }
            }
        }

        Image {
            source: "pics/stoneShadow.png"
        }

        opacity: 0
    }


    Particles { id: particles
        width:1; height:1; anchors.centerIn: parent;
        emissionRate: 0;
        lifeSpan: 700; lifeSpanDeviation: 600;
        angle: 0; angleDeviation: 360;
        velocity: 100; velocityDeviation:30;
        source: {
            if(type == 1){
                "pics/blueStar.png";
            } else {
                "pics/redStar.png";
            }
        }
    }

    states: [
        State{ name: "AliveState"; when: spawned == true && dying == false
            PropertyChanges { target: img; opacity: 1 }
        },
        State{ name: "DeathState"; when: dying == true
            StateChangeScript { script: particles.burst(50); }
            PropertyChanges { target: img; opacity: 0 }
        }
    ]

    transitions: [
        Transition {
            NumberAnimation { target: img; property: "opacity"; duration: 200 }
        }
    ]

}

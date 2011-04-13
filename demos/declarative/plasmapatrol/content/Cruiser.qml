/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
import QtQuick 2.0
import Qt.labs.particles 2.0

Item {
    id: container
    property int maxHP: 100
    property int hp: maxHP
    property real initialDodge: 0.01
    property real dodge: initialDodge
    onHpChanged: if(hp <= 0) target = container;
    property ParticleSystem system//TODO: Ship abstraction
    property Item target: container
    property string shipParticle: "default"//Per team colors?
    property int gunType: 0
    width: 128
    height: 128
    TrailEmitter{
        //TODO: Cooler would be an 'orbiting' affector
        //TODO: On the subject, opacity and size should be grouped type 'overLife' if we can cram that in the particles
        system: container.system
        particle: container.shipParticle
        anchors.centerIn: parent
        width: 64
        height: 64
        shape: Ellipse{}

        particlesPerSecond: hp > 0 ?  hp * 1 + 20 : 0 
        particleDuration: 2400
        maxParticles: (maxHP * 1 + 20)*2.4

        particleSize: 48
        particleSizeVariation: 16
        particleEndSize: 16

        speed: AngleVector{angleVariation:360; magnitudeVariation: 32}
    }
    TrailEmitter{
        system: container.system
        particle: "cruiserArmor"
        anchors.fill: parent
        shape: Ellipse{ fill: false }
        emitting: hp>0
        
        particlesPerSecond: 16
        particleDuration: 2000

        particleSize: 48
        particleSizeVariation: 24

        SpriteGoal{
            id: destructor
            system: container.system
            active: container.hp <=0
            anchors.fill: parent
            particles: ["cruiserArmor"]
            goalState: "death"
//            jump: true
            onceOff: true
        }
    }

    Timer{
        id: fireControl
        property int next: Math.floor(Math.random() * 3) + 1
        interval: 800
        running: root.readySetGo
        repeat: true
        onTriggered:{
            if(next == 1){
                gun1.fireAt(container.target);
                next = Math.floor(Math.random() * 3) + 1;
            }else if(next == 2){
                gun2.fireAt(container.target);
                next = Math.floor(Math.random() * 3) + 1;
            }else if(next == 3){
                gun3.fireAt(container.target);
                next = Math.floor(Math.random() * 3) + 1;
            }
        }
    }

    Hardpoint{//TODO: Hardpoint abstraction
        x: 112 - 12 - 8*2
        y: 128 - 12 - 12*2
        id: gun1
        system: container.system
        show: hp > 0
        hardpointType: gunType
    }
    Hardpoint{
        x: 64 - 12
        y: 0 - 12 + 12*2
        id: gun2
        system: container.system
        show: hp > 0
        hardpointType: gunType
    }
    Hardpoint{
        x: 16 - 12 + 8*2
        y: 128 - 12 - 12*2
        id: gun3
        system: container.system
        show: hp > 0
        hardpointType: gunType
    }
}

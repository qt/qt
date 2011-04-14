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
    property variant target: {"y": -90, "x":12}
    property Item targetObj: container
    property Item hardpoint: container
    property ParticleSystem system
    property int blasts: 16
    property int bonusBlasts: 12
    property bool show: true

    width: 24
    height: 24
    TrailEmitter{
        id: visualization
        particle: "blaster"
        system: container.system
        emitting: show
        anchors.fill: parent
        shape: Ellipse{}
        speed: DirectedVector{ targetX: width/2; targetY: width/2; magnitude: -1; proportionalMagnitude: true}
        particleDuration: 1000
        particlesPerSecond: 64 

        particleSize: 24
        particleSizeVariation: 24
        particleEndSize: 0
    }

    property int blastsLeft: 0
    function fireAt(targetArg, container){
        target = container.mapFromItem(targetArg, targetArg.width/2, targetArg.height/2);
        targetObj = targetArg;
        hardpoint = container;
        blastsLeft = blasts;
        rofTimer.repeat = true;
        rofTimer.start();
    }
    Timer{
        id: rofTimer
        interval: 30;//Has to be greater than 1 frame or they stack up
        running: false
        repeat: false
        onTriggered:{
            if(targetObj.hp <= 0)
                return;
            //TODO: calculate hit and damage at target, which must be a Ship
            var hit;
            if(blastsLeft >= bonusBlasts)
                hit = Math.random() > targetObj.dodge;
            else
                hit = false; //purely aesthetic shots, because the damage isn't that fine grained
            if(hit == true){
                switch(targetObj.shipType){
                case 1: hardpoint.damageDealt += 4; break;
                case 2: hardpoint.damageDealt += 5; break;
                case 3: hardpoint.damageDealt += 1; break;
                default: hardpoint.damageDealt += 100;
                }
            }
            blastVector.targetX = target.x;
            blastVector.targetY = target.y;
            if(!hit){//TODO: Actual targetVariation
                blastVector.targetX += (128 * Math.random() - 64);
                blastVector.targetY += (128 * Math.random() - 64);
            }
            emitter.burst(1);
            blastsLeft--;
            if(!blastsLeft)
                rofTimer.repeat = false;
        }
    }
    TrailEmitter{
        id: emitter
        particle: "blaster"
        emitting: false
        system: container.system
        anchors.centerIn: parent

        particleDuration: 1000
        particlesPerSecond: 16
        maxParticles: blasts
        particleSize: 24
        particleEndSize:16
        particleSizeVariation: 8
        speed: DirectedVector{
            id: blastVector
            targetX: target.x; targetY: target.y; magnitude: 1.1; proportionalMagnitude: true
        }
    }
}

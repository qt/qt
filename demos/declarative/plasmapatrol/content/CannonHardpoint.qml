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
    property ParticleSystem system
    property bool show: true

    width: 24
    height: 24
    TrailEmitter{
        id: visualization
        particle: "cannon"
        emitting: container.show
        system: container.system
        anchors.centerIn: parent
        particleDuration: 2000
        particlesPerSecond: 1

        particleSize: 4
        particleEndSize: 0
    }

    function fireAt(targetArg, hardpoint){
        target = container.mapFromItem(targetArg, targetArg.width/2, targetArg.height/2);
        if(container.hp <= 0 || targetArg.hp <= 0)
            return;
        //TODO: calculate hit and damage at target, which must be a Ship
        var hit = Math.random() > targetArg.dodge
        if(hit){
            switch(targetArg.shipType){
            case 1: hardpoint.damageDealt += 8; break;
            case 2: hardpoint.damageDealt += 10; break;
            case 3: hardpoint.damageDealt += 16; break;
            default: hardpoint.damageDealt += 1000;
            }
        }
        emitter.burst(1);
    }
    TrailEmitter{
        id: emitter
        particle: "cannon"
        emitting: false
        system: container.system
        anchors.centerIn: parent

        particleDuration: 1000
        particlesPerSecond: 1
        particleSize: 8
        particleEndSize: 4
        speed: DirectedVector{
            id: blastVector
            targetX: target.x; targetY: target.y; magnitude: 1.1; proportionalMagnitude: true
        }
    }
}

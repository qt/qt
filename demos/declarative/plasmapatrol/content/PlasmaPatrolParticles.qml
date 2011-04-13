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

Item{
    property ParticleSystem sys
    ColoredParticle{
        system: sys
        particles: ["default"]
        image: "pics/blur-circle3.png"
        color: "#003A3A3A"
        colorVariation: 0.1
        z: 0
    }
    ColoredParticle{
        system: sys
        particles: ["redTeam"]
        image: "pics/blur-circle3.png"
        color: "#0028060A"
        colorVariation: 0.1
        z: 0
    }
    ColoredParticle{
        system: sys
        particles: ["greenTeam"]
        image: "pics/blur-circle3.png"
        color: "#0006280A"
        colorVariation: 0.1
       z: 0
    }
    ColoredParticle{
        system: sys
        particles: ["blaster"]
        image: "pics/star2.png"
        //color: "#0F282406"
        color: "#0F484416"
        colorVariation: 0.2
        z: 2
    }
    ColoredParticle{
        system: sys
        particles: ["laser"]
        image: "pics/star3.png"
        //color: "#00123F68"
        color: "#00428FF8"
        colorVariation: 0.2
        z: 2
    }
    ColoredParticle{
        system: sys
        particles: ["cannon"]
        image: "pics/particle.png"
        color: "#80FFAAFF"
        colorVariation: 0.1
        z: 2
    }
    ColoredParticle{
        system: sys
        particles: ["cannonCore"]
        image: "pics/particle.png"
        color: "#00666666"
        colorVariation: 0.8
        z: 1
    }
    ColoredParticle{
        system: sys
        particles: ["cannonWake"]
        image: "pics/star.png"
        color: "#00CCCCCC"
        colorVariation: 0.2
        z: 1
    }
    ColoredParticle{
        system: sys
        particles: ["frigateShield"]
        image: "pics/blur-circle2.png"
        color: "#00000000"
        colorVariation: 0.05
        blueVariation: 0.5
        greenVariation: 0.1
        z: 3
    }
    SpriteParticle{
        system: sys
        particles: ["cruiserArmor"]
        z: 1
        sprites:[Sprite{
                id: spinState
                name: "spinning"
                source: "pics/meteor.png"
                frames: 35
                duration: 40
                to: {"death":0, "spinning":1}
            },Sprite{
                name: "death"
                source: "pics/meteor_explo.png"
                frames: 22
                duration: 40
                to: {"null":1}
            }, Sprite{
                name: "null"
                source: "pics/nullRock.png"
                frames: 1
                duration: 1000
            }
        ]
    }
    FollowEmitter{
        system: sys
        particle: "cannonWake"
        follow: "cannon"
        particlesPerParticlePerSecond: 64
        particleDuration: 600
        speed: AngleVector{ angleVariation: 360; magnitude: 48}
        particleSize: 16
        particleEndSize: 8
        particleSizeVariation: 2
        emitting: true
        width: 1000//XXX: Terrible hack
        height: 1000
    }
    FollowEmitter{
        system: sys
        particle: "cannonCore"
        follow: "cannon"
        particlesPerParticlePerSecond: 256
        particleDuration: 128
        particleSize: 24
        particleEndSize: 8
        emitting: true
        width: 1000//XXX: Terrible hack
        height: 1000
    }
}

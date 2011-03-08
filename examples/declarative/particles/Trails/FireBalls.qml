import QtQuick 2.0
import Qt.labs.particles 2.0

Rectangle {
    id: root
    width: 360
    height: 600
    color: "black"

    ParticleSystem{
        id: particles
        ColoredParticle{
            id: smoke
            image: "content/particle.png"
            colorVariation: 0
            color: "#333333"
            additive: 1
        }
        ColoredParticle{
            id: flame
            image: "content/particle.png"
            colorVariation: 0.1
            color: "#ff400f"
            additive: 1
        }
        ColoredParticle{
            id: flame2
            image: "content/particle.png"
            colorVariation: 0.1
            color: "#ff400f"
            additive: 1
        }
        ColoredParticle{
            id: fireball
            image: "content/particleA.png"
            colorVariation: 0.1
            color: "#ff400f"
            additive: 1
        }
        affectors:[
            Follow{ emitter: fireballSmoke },
            Follow{ emitter: fireballFlame }
        ]
    }

    TrailEmitter{
        id: fire
        system: particles
        particle: flame

        y: parent.height
        width: parent.width

        particlesPerSecond: 250
        particleDuration: 3500

        yAccel: -17
        xAccelVariation: 3

        xSpeedVariation: 3

        particleSize: 24
        particleSizeVariation: 8
        particleEndSize: 4
    }
    FollowEmitter{
        id: fireballFlame
        system: particles
        particle: flame2
        follow: fireball

        particlesPerParticlePerSecond: 32
        particleDuration: 320

        particleSize: 24
        particleSizeVariation: 8
        particleEndSize: 4
    }
    FollowEmitter{
        id: fireballSmoke
        system: particles
        particle: smoke
        follow: fireball

        particlesPerParticlePerSecond: 16
        particleDuration: 1200

        ySpeed: -17
        xAccelVariation: 3
        xSpeedVariation: 6

        particleSize: 24
        particleSizeVariation: 8
        particleEndSize: 4
    }
    TrailEmitter{
        id: balls
        system: particles
        particle: fireball

        y: parent.height
        width: parent.width

        particlesPerSecond: 2
        particleDuration: 7000

        ySpeed: -17*4*2
        yAccel: 17* 2
        xAccelVariation: 6 * 6
        xSpeedVariation: 6 * 6

        particleSize: 16
        particleSizeVariation: 16
        particleEndSize: 8
    }

}


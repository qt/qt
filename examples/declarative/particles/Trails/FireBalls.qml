import QtQuick 2.0
import Qt.labs.particles 2.0

Rectangle {
    id: root
    width: 360
    height: 540
    color: "black"

    ParticleSystem{
        id: particles
        particles: [smoke, smoke2, flame, flame2, fireball]
        affectors:[
            Follow{ emitter: fireballSmoke },
            Zone{
                width: root.width
                height: 168
                y: root.height - 196
                Follow{ emitter: fireSmoke 
                }
                },
            Follow{ emitter: fireballFlame }
        ]
    }

    ColoredParticle{
        id: smoke
        image: "content/particle.png"
        colorVariation: 0
        color: "#111111"
        additive: 1
    }
    ColoredParticle{
        id: smoke2
        image: "content/particle.png"
        colorVariation: 0
        color: "#111111"
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
        colorVariation: 0.2
        color: "#ff400f"
        additive: 1
    }
    TrailEmitter{
        id: fire
        system: particles
        particle: flame

        y: parent.height
        width: parent.width

        particlesPerSecond: 350
        particleDuration: 3500

        yAccel: -17
        xAccelVariation: 3

        xSpeedVariation: 3

        particleSize: 24
        particleSizeVariation: 8
        particleEndSize: 4
    }
    FollowEmitter{
        id: fireSmoke
        system: particles
        particle: smoke2
        follow: flame

        particlesPerParticlePerSecond: 1
        particleDuration: 2000

        ySpeed: -17 * 6
        ySpeedVariation: -17
        xAccelVariation: 3
        xSpeedVariation: 3

        particleSize: 36
        particleSizeVariation: 8
        particleEndSize: 16
    }
    FollowEmitter{
        id: fireballFlame
        system: particles
        particle: flame2
        follow: fireball

        particlesPerParticlePerSecond: 48
        particleDuration: 360

        particleSize: 16
        particleSizeVariation: 4
        particleEndSize: 4
    }
    FollowEmitter{
        id: fireballSmoke
        system: particles
        particle: smoke
        follow: fireball

        particlesPerParticlePerSecond: 128
        particleDuration: 2400

        yAccel: -16
        ySpeedVariation: 16
        xSpeedVariation: 16

        particleSize: 24
        particleSizeVariation: 8
        particleEndSize: 8
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

        particleSize: 8
        particleSizeVariation: 8
    }

}


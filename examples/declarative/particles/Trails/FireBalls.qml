import QtQuick 2.0
import Qt.labs.particles 2.0

Rectangle {
    id: root
    width: 360
    height: 540
    color: "black"

    ParticleSystem{
        id: particles
    }

    ColoredParticle{
        id: fireball
        anchors.fill: parent
        particles: ["E"]
        system: particles
        image: "content/particleA.png"
        colorVariation: 0.2
        color: "#ff400f"
        additive: 1
    }
    ColoredParticle{
        id: smoke
        system: particles
        anchors.fill: parent
        particles: ["A", "B"]
        image: "content/particle.png"
        colorVariation: 0
        color: "#111111"
        additive: 1
    }
    ColoredParticle{
        id: flame
        anchors.fill: parent
        system: particles
        particles: ["C", "D"]
        image: "content/particle.png"
        colorVariation: 0.1
        color: "#ff400f"
        additive: 1
    }
    TrailEmitter{
        id: fire
        system: particles
        particle: "C"

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
        particle: "B"
        system: particles
        follow: "C"
        width: root.width
        height: root.height - 68

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
        anchors.fill: parent
        system: particles
        particle: "D"
        follow: "E"

        particlesPerParticlePerSecond: 48
        particleDuration: 360

        particleSize: 16
        particleSizeVariation: 4
        particleEndSize: 4
    }
    
    FollowEmitter{
        id: fireballSmoke
        anchors.fill: parent
        system: particles
        particle: "A"
        follow: "E"

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
        particle: "E"

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


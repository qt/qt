import QtQuick 2.0
import Qt.labs.particles 2.0

Rectangle{
    width: 360
    height: 540
    color: "#222222"
    id: root
    Image{
        source: "content/candle.png"
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottomMargin: -8
        anchors.horizontalCenterOffset: 2
    }
    ParticleSystem{
        id: ps
        anchors.fill: parent
        clip: true
        ColoredParticle{
            id: smoke
            image: "content/particle.png"
            color: "#111111"
            colorVariation: 0
            additive: 1
        }
        ColoredParticle{
            id: flame
            image: "content/particle.png"
            color: "#ff400f"
            colorVariation: 0.1
            additive: 1
        }
        affectors: [
            Turbulence{ 
                strength: 32;
            }, Zone{
                width: root.width
                height: 232
                Follow{ emitter: smoke1 }
            }, Zone{
                width: root.width
                height: 248
                Follow{ emitter: smoke2 }
            }]

    }
    TrailEmitter{
        anchors.centerIn: parent
        system: ps
        particle: flame
        particlesPerSecond: 120
        particleDuration: 1200
        particleSize: 20
        particleEndSize: 10
        particleSizeVariation: 10
        yAccel: -40
        ySpeed: -20
        ySpeedVariation: 10
        xSpeedVariation: 20
    }
    FollowEmitter{
        id: smoke1
        anchors.centerIn: parent
        anchors.verticalCenterOffset: - 10
        system: ps
        particle: smoke
        follow: flame
        particlesPerParticlePerSecond: 4
        particleDuration: 2400
        particleSize: 16
        particleEndSize: 8
        particleSizeVariation: 8
        yAccel: -40
        ySpeed: -40
        ySpeedVariation: 10
        xSpeedVariation: 20
    }
    FollowEmitter{
        id: smoke2
        anchors.centerIn: parent
        anchors.verticalCenterOffset: - 10
        system: ps
        particle: smoke
        follow: flame
        particlesPerParticlePerSecond: 1
        particleDuration: 2400
        particleSize: 36
        particleEndSize: 24
        particleSizeVariation: 8
        yAccel: -40
        ySpeed: -40
        ySpeedVariation: 10
        xSpeedVariation: 20
    }
}

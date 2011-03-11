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
    }
    Turbulence{
        system: ps
        height: (parent.height / 2)
        width: parent.width / 2
        x: parent. width / 4
        anchors.fill: parent
        strength: 16
        frequency: 64
        gridSize: 16
    }
    ColoredParticle{
        particles: ["smoke"]
        system: ps
        image: "content/particle.png"
        color: "#111111"
        colorVariation: 0
        additive: 1
    }
    ColoredParticle{
        particles: ["flame"]
        system: ps
        image: "content/particle.png"
        color: "#ff400f"
        colorVariation: 0.1
        additive: 1
        }
    TrailEmitter{
        anchors.centerIn: parent
        system: ps
        particle: "flame"
        
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
        width: root.width
        height: 232
        system: ps
        particle: "smoke"
        follow: "flame"

        particlesPerParticlePerSecond: 4
        particleDuration: 2400
        particleDurationVariation: 400
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
        width: root.width
        height: 232
        system: ps
        particle: "smoke"
        follow: "flame"
        
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

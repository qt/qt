import QtQuick 2.0
import Qt.labs.particles 2.0

Rectangle{
    color: "black"
    width: 360
    height: 540
    ParticleSystem{
        anchors.fill: parent
        particles: ColoredParticle{
            id: cp
            image: "content/particle.png"
            colorVariation: 0.4
            additive: 1
        }
        emitters: TrailEmitter{
            speedFromMovement: 4.0
            emitting: ma.pressed
            emitterX: ma.mouseX
            emitterY: ma.mouseY
            particlesPerSecond: 400
            particleDuration: 2000
            yAccel: 32
            yAccelVariation: 16
            xAccel: 0
            xAccelVariation: 16
            particleSize: 8
            particleEndSize: 16
            particleSizeVariation: 8
        }
    }
    MouseArea{
        id: ma
        anchors.fill: parent
    }
}

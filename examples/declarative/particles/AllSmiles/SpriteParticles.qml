import QtQuick 2.0
import Qt.labs.particles 2.0

Rectangle{
    color: "goldenrod"
    width: 400
    height: 400
    ColoredParticle{
        id: test
        particles: ["Test"]
        image: "particle.png"
        system: sys
        z: 2
        anchors.fill: parent
        color: "lightsteelblue"
        colorVariation: 0.0
        additive: 0.6
    }
    SpriteParticle{
        id: single
        particles: ["Face"]
        system: sys
        z: 2
        anchors.fill: parent
        Sprite{
            source: "squarefacesprite.png"
            frames: 6
            duration: 120
        }
    }
    Mask{
        id: mask
        source: "smileMask.png"
    }
    TrailEmitter{
        system: sys
        particle: "Test"
        anchors.fill: parent
        id: particles2
        particlesPerSecond: 6000
        particleDuration: 720
        emitting: true
        xSpeed: 0
        ySpeed: 0
        particleSize: 10
        extruder: mask
    }
    TrailEmitter{
        system: sys
        particle: "Face"
        anchors.fill: parent
        id: particles
        particlesPerSecond: 60
        particleDuration: 1440
        emitting: true
        xSpeed: 0
        ySpeed: 0
        xSpeedVariation: 10
        ySpeedVariation: 10
        particleSize: 30
        particleSizeVariation: 10
        extruder: mask
    }
    ParticleSystem{
        id: sys
        anchors.fill: parent
    }

}

import QtQuick 2.0
import Qt.labs.particles 2.0

Rectangle{
    width: 360
    height: 540
    color: "black"
    Image{
        anchors.fill: parent
        source: "content/portal_bg.png"
    }
    ParticleSystem{ 
        id: particles 
        startTime: 2000
    }
    ColoredParticle{
        anchors.fill: parent
        system: particles
        image: "content/particle.png"
        colorVariation: 0.1
        color: "lightsteelblue"
    }
    DirectedEmitter{
        anchors.fill: parent
        system: particles
        particlesPerSecond: 3000
        particleDuration: 3000
        emitting: true
        particleSize: 16
        particleSizeVariation: 8
        particleEndSize: 0
        extruder: Ellipse{fill: false}
        targetX: parent.width/2
        targetY: parent.height/2
        speed: 40
        speedVariation: 40
        acceleration: 20
        accelerationVariation: 20
    }
}

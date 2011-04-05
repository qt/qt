import QtQuick 2.0
import Qt.labs.particles 2.0

Rectangle{
    width: 360
    height: 540
    color: "black"
    MouseArea{
        anchors.fill: parent
        onClicked: particles.running = !particles.running
    }
    ParticleSystem{ 
        id: particles 
        running: false
    }
    ColoredParticle{
        anchors.fill: parent
        system: particles
        image: "content/star.png"
        colorVariation: 0.2
    }
    TrailEmitter{
        anchors.fill: parent
        system: particles
        particlesPerSecond: 2000
        particleDuration: 2000
        emitting: true
        particleSize: 40
        particleSizeVariation: 10
    }
}

import QtQuick 2.0
import Qt.labs.particles 2.0

Rectangle{
    color: "lightsteelblue"
    width: 800
    height: 800
    SpriteParticles{
        id: particles
        anchors.fill: parent
        states: SpriteState{
            name: "snow"
            source: "flake-01.png"
            frames: 20
            duration: 100
        }
        particlesPerSecond: 100
        particleDuration: 8000
        emitting: true
        xSpeed: 0
        ySpeed: 200
        xSpeedVariation: 20
        ySpeedVariation: 100
        xAccel: 0
        yAccel: 4
        particleSize: 30
        particleSizeVariation: 10
        emitterX: width/2
        emitterY: 50
        emitterXVariation: width/2
        emitterYVariation: 50
    }
    MouseArea{
        id: ma
        anchors.fill: parent
    }
}

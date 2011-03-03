import QtQuick 2.0
import Qt.labs.particles 2.0

Rectangle{
    color: "goldenrod"
    width: 400
    height: 400
    ParticleSystem{
        anchors.fill: parent
        particles:[ SpriteParticle{
            id: single
            Sprite{
                source: "squarefacesprite.png"
                frames: 6
                duration: 120
            }
        }]
        z: 2
        emitters: particles
    }
    Rectangle{
        id: rect
        width: 40
        height: 40
        x: 180
        y: 180
        color: "lightsteelblue"
        TrailEmitter{
            anchors.centerIn: parent
            id: particles
                particlesPerSecond: 6
                particleDuration: 5000
                emitting: true
                xSpeed: 0
                ySpeed: 0
                xSpeedVariation: 100
                ySpeedVariation: 100
                particleSize: 30
                particleSizeVariation: 10
        }
        z: 1
        MouseArea{
            id: ma
            anchors.fill: parent
            drag.target: rect
            drag.axis: Drag.XandYAxis
        }
    }

}

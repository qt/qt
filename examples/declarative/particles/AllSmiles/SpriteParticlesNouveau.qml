import QtQuick 2.0
import Qt.labs.particles 2.1

Rectangle{
    color: "lightsteelblue"
    width: 600
    height: 600
    ParticleSystem{
        anchors.fill: parent
        emitters: [
        SpriteEmitter{
            id: particles
            states: SpriteState{
                source: "squarefacesprite.png"
                frames: 6
                duration: 120
            }

            particlesPerSecond: 16
            particleDuration: 5000
            emitting: true
            xSpeed: 0
            ySpeed: 0
            xSpeedVariation: 100
            ySpeedVariation: 100
            particleSize: 30
            particleSizeVariation: 10
            emitterX: width/2
            emitterY: height/2
        }
        ,
        TrailsEmitter{
            id: alsoParticles
            image: "particle.png"
            emitterX: particles.emitterX
            emitterY: particles.emitterY

            emitting: true
            particlesPerSecond: 160
            particleDuration: 2500
            xSpeedVariation: 100
            ySpeedVariation: 100
            particleSize: 10
            particleSizeVariation: 5
            colorVariation: 0.4
        }
        ]
        affectors: [Wander{
            xVariance: 100
            yVariance: 100
            pace: 100
        }, Rocking{
            initialAngle: -45
            angle: 45
            angleVariance: 360
        }
        ]
    }
    Binding{
        target: particles
        property: "emitterY"
        value: ma.mouseY
        when: ma.mouseX !=0 || ma.mouseY!=0
    }
    Binding{
        target: particles
        property: "emitterX"
        value: ma.mouseX
        when: ma.mouseX !=0 || ma.mouseY!=0
    }
    MouseArea{
        id: ma
        anchors.fill: parent
    }
}

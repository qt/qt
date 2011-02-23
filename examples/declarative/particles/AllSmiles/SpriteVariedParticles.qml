import QtQuick 2.0
import Qt.labs.particles 2.0

Rectangle{
    color: "goldenrod"
    width: 800
    height: 800
    SpriteParticles{
        id: particles
        anchors.fill: parent
        states: [SpriteState{
            name: "initial"
            source: "squarefacesprite.png"
            frames: 6
            duration: 0
            to: {"happy":0.2, "silly":0.2, "sad":0.2, "cyclops":0.1, "evil":0.1, "love":0.1, "boggled":0.1}
        }, SpriteState{
            name: "silly"
            source: "squarefacesprite.png"
            frames: 6
            duration: 120
        }, SpriteState{
            name: "happy"
            source: "squarefacesprite2.png"
            frames: 6
            duration: 120
        }, SpriteState{
            name: "sad"
            source: "squarefacesprite3.png"
            frames: 6
            duration: 120
        }, SpriteState{
            name: "cyclops"
            source: "squarefacesprite4.png"
            frames: 6
            duration: 120
        }, SpriteState{
            name: "evil"
            source: "squarefacesprite5.png"
            frames: 6
            duration: 120
        }, SpriteState{
            name: "love"
            source: "squarefacesprite6.png"
            frames: 6
            duration: 120
        }, SpriteState{
            name: "boggled"
            source: "squarefacesprite7.png"
            frames: 6
            duration: 120
        }]

        particlesPerSecond: 16
        particleDuration: 8000
        emitting: true
        xSpeed: 0
        ySpeed: 100
        xSpeedVariation: 10
        ySpeedVariation: 100
        xAccel: 0
        yAccel: 10
        particleSize: 30
        particleSizeVariation: 10
        emitterX: width/2
        emitterY: 50
        emitterXVariation: width/2
        emitterYVariation: 50
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

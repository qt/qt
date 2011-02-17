import QtQuick 2.0
import Qt.labs.particles 2.0

Rectangle{
    color: "goldenrod"
    width: 800
    height: 800
    SpriteParticles{
        id: particles
        anchors.fill: parent
        goalState:ma.pressed?"dead":""
        states: [SpriteState{
            name: "happy"
            source: "squarefacesprite2.png"
            frames: 6
            duration: 120
            to: {"silly": 0.4, "sad": 0.2, "cyclops":0.1, "boggled":0.3, "dying":0.0}
        }, SpriteState{
            name: "silly"
            source: "squarefacesprite.png"
            frames: 6
            duration: 120
            to: {"love": 0.4, "happy": 0.1, "evil": 0.2, "cyclops":0.1, "boggled":0.2}
        }, SpriteState{
            name: "sad"
            source: "squarefacesprite3.png"
            frames: 6
            duration: 120
            to: {"love" : 0.2, "evil": 0.2, "silly": 0.2, "cyclops":0.2, "boggled":0.2}
        }, SpriteState{
            name: "cyclops"
            source: "squarefacesprite4.png"
            frames: 6
            duration: 120
            to: {"love": 0.1, "evil": 0.1, "silly":0.1, "boggled":0.1, "cyclops" : 1.0}
        }, SpriteState{
            name: "evil"
            source: "squarefacesprite5.png"
            frames: 6
            duration: 120
            to: {"happy": 1.0}
        }, SpriteState{
            name: "love"
            source: "squarefacesprite6.png"
            frames: 6
            duration: 120
            to: {"sad": 0.6, "evil":0.4, "boggled":0.2}
        }, SpriteState{
            name: "boggled"
            source: "squarefacesprite7.png"
            frames: 6
            duration: 120
            to: {"love" : 0.2, "evil": 0.2, "silly": 0.2, "cyclops":0.1, "sad":0.2}
        }, SpriteState{
            name: "dying"
            source: "squarefacespriteX.png"
            frames: 4
            duration: 120
            to: {"dead":1.0}
        }, SpriteState{
            name: "dead"
            source: "squarefacespriteXX.png"
            frames: 1
            duration: 10000
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
    MouseArea{
        id: ma
        anchors.fill: parent
    }
}

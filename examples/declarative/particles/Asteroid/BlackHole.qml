import QtQuick 2.0
import Qt.labs.particles 2.0

Rectangle{
    id: root
    width: 360
    height: 540
    color: "black"
    property bool spacePressed: false
    focus: true
    Keys.onPressed: {
        if (event.key == Qt.Key_Space) {
            spacePressed = true;
            event.accepted = true;
        }
    }
    Keys.onReleased: {
        if (event.key == Qt.Key_Space) {
            spacePressed = false;
            event.accepted = true;
        }
    }

    TrailEmitter{
        particle: stars//TODO: Merge stars and roids
        system: particles
        particlesPerSecond: 40
        particleDuration: 4000
        emitting: true
        particleSize: 30
        particleSizeVariation: 10
        xSpeed: 220
        xSpeedVariation: 40
        height: parent.height
    }
    TrailEmitter{
        particle: roids//TODO: Merge stars and roids
        system: particles
        particlesPerSecond: 10
        particleDuration: 4000
        emitting: true
        particleSize: 30
        particleSizeVariation: 10
        xSpeed: 220
        xSpeedVariation: 40
        height: parent.height
    }
    ParticleSystem{
        id: particles
        anchors.fill: parent
        particles: [ColoredParticle{
            id: stars
            image: "star.png"
            color: "white"
            colorVariation: 0.1
            additive: 1
        },SpriteParticle{
            id: roids
            sprites: Sprite{
                id: spinState
                name: "spinning"
                source: "meteor.png"
                frames: 35
                duration: 60
                speedModifiesDuration: -0.1
            }
        },ColoredParticle{
            id: shot
            image: "star.png"

            color: "orange"
            colorVariation: 0.3
        },ColoredParticle{
            id: engine
            image: "particle4.png"

            color: "orange"
            SequentialAnimation on color {
                loops: Animation.Infinite
                ColorAnimation {
                    from: "red"
                    to: "cyan"
                    duration: 1000
                }
                ColorAnimation {
                    from: "cyan"
                    to: "red"
                    duration: 1000
                }
            }

            colorVariation: 0.2
            additive: 1
        }]
        affectors: [
            GravitationalSingularity{id: gs; x: root.width/2; y: root.height/2; strength: 4000000;},
            Zone{
                x: gs.x - 8;
                y: gs.y - 8;
                width: 16
                height: 16
                affector: Kill{}
            }
        ]
    }
    Image{
        source:"rocket2.png"
        id: ship
        width: 45
        height: 22
        MouseArea{
            id: ma
            anchors.fill: parent;
            drag.axis: Drag.XandYAxis
            drag.target: ship
        }
        TrailEmitter{
            particle: engine
            system: particles
            particlesPerSecond: 200
            particleDuration: 1000
            emitting: true
            particleSize: 10
            particleEndSize: 4
            particleSizeVariation: 4
            xSpeed: -128
            xSpeedVariation: 32
            height: parent.height
            width: 20
        }
        TrailEmitter{
            particle: shot
            system: particles
            particlesPerSecond: 32
            particleDuration: 2000
            emitting: spacePressed
            particleSize: 40
            xSpeed: 256
            x: parent.width
            y: parent.height/2
        }
    }
    Text{
        color: "white"
        anchors.bottom: parent.bottom
        text:"Drag the ship, Spacebar to fire."
    }
}


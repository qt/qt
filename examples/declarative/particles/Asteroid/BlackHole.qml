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

    ParticleSystem{
        id: shipStuff
        anchors.fill: parent
        particles: [ColoredParticle{
            id: stars
            image: "star.png"
            color: "white"
            colorVariation: 0.1
            additive: 1
        },SpriteParticle{
            id: roids
            sprite: Sprite{
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
        emitters: [TrailEmitter{
            particle: engine
            particlesPerSecond: 200
            particleDuration: 1000
            emitting: true
            particleSize: 10
            particleEndSize: 4
            particleSizeVariation: 4
            xSpeed: -128
            xSpeedVariation: 32
            emitterX: ship.x + 10
            emitterY: ship.y + 11
            emitterXVariation: 10
            emitterYVariation: 10
        }, TrailEmitter{
            particle: shot
            particlesPerSecond: 32
            particleDuration: 2000
            emitting: spacePressed
            particleSize: 40
            xSpeed: 256
            emitterX: ship.x + 15
            emitterY: ship.y + 11
        }, TrailEmitter{
            particle: stars//TODO: Merge stars and roids
            particlesPerSecond: 40
            particleDuration: 4000
            emitting: true
            particleSize: 30
            particleSizeVariation: 10
            xSpeed: 220
            xSpeedVariation: 40
            emitterX: -40
            emitterY: height/2
            emitterYVariation: height/2
        }, TrailEmitter{
            particle: roids//TODO: Merge stars and roids
            particlesPerSecond: 10
            particleDuration: 4000
            emitting: true
            particleSize: 30
            particleSizeVariation: 10
            xSpeed: 220
            xSpeedVariation: 40
            emitterX: -40
            emitterY: height/2
            emitterYVariation: height/2
        }]
        affectors: [
            GravitationalSingularity{id: gs; x: root.width/4; y: root.height/4; strength: 4000000;},
/*            Zone{
                x: gs.x - 8;
                y: gs.y - 8;
                width: 16
                height: 16
                affector: Kill{}
            },*/
            GravitationalSingularity{id: g2; x: 3*root.width/4; y: root.height/2; strength: 4000000;},
           /* Zone{
                x: g2.x - 8;
                y: g2.y - 8;
                width: 16
                height: 16
                affector: Kill{}
            },*/
            GravitationalSingularity{id: s3; x: root.width/4; y: 3*root.height/4; strength: 4000000;},
            Zone{
                x: s3.x - 8;
                y: s3.y - 8;
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
    }
    ParticleSystem{
        id: background
        anchors.fill: parent
        particles: [ColoredParticle{
            id: stars2
            image: "star.png"
            color: "white"
            colorVariation: 0.1
            additive: 1
        },SpriteParticle{
            id: roids2
            sprite: Sprite{
                name: "spinning"
                source: "meteor.png"
                frames: 35
                duration: 80
                speedModifiesDuration: -0.1
            }
        }]
        emitters: [TrailEmitter{
            particle: stars2//TODO: Merge stars and roids
            particlesPerSecond: 160
            particleDuration: 4000
            emitting: true
            particleSize: 30
            particleSizeVariation: 10
            xSpeed: 220
            xSpeedVariation: 40
            emitterX: -40
            emitterY: height/2
            emitterYVariation: height/2
        }, TrailEmitter{
            particle: roids2//TODO: Merge stars and roids
            particlesPerSecond: 60
            particleDuration: 4000
            emitting: true
            particleSize: 30
            particleSizeVariation: 10
            xSpeed: 220
            xSpeedVariation: 40
            emitterX: -40
            emitterY: height/2
            emitterYVariation: height/2
        }]
        affectors: [
            GravitationalSingularity{id: gs2; x: gs.x; y: gs.y; strength: gs.strength/40;}
        ]
    }
    Text{
        color: "white"
        anchors.bottom: parent.bottom
        text:"Drag the ship, Spacebar to fire."
    }
}


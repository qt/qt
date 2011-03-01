import Qt.labs.particles 2.1
import Qt.labs.particles 2.1 as Qlp
import QtQuick 2.0

Item {
    id: root
    width: 360
    height: 540
    MouseArea{
        id: ma
        anchors.fill: parent
    }

    SequentialAnimation {
        running: true
        PropertyAction { target: circleAnim1; property: "running"; value: true }
        //PauseAnimation { duration: 70 }
        //PropertyAction { target: circleAnim2; property: "running"; value: true }
    }

    Image {
        source: "finalfrontier.png"
        transformOrigin: Item.Center
        anchors.centerIn: parent
        smooth: true
        NumberAnimation on rotation {
            from: 0
            to: 360
            duration: 200000
            loops: Animation.Infinite
        }

    }
    ParticleSystem{
        particles: ColoredParticle {
            image: "star.png"
            colorVariation: 0.3
            color: "white"
            additive: 1
        }
        emitters: TrailEmitter {
            id: starField

            particlesPerSecond: 80
            particleDuration: 2500

            emitterY: root.height / 2
            emitterX: root.width / 2

            yAccelVariation: 100
            xAccelVariation: 100;

            particleSize: 0
            particleEndSize: 80
            particleSizeVariation: 10
        }
    }

    ParticleSystem{
        anchors.fill: parent
        particles: SpriteParticle{
            id: particles
            spriteEngine: SpriteEngine{
                sprites:[Sprite{
                    id: spinState
                    name: "spinning"
                    source: "meteor.png"
                    frames: 35
                    duration: 30
                    speedModifiesDuration: -0.1
                    to: {"explode":0, "spinning":1}
                },Sprite{
                    name: "explode"
                    source: "_explo.png"
                    frames: 22
                    duration: 40
                    speedModifiesDuration: -0.1
                    to: {"nullFrame":1}
                },Sprite{//Not sure if this is needed, but seemed easiest
                    name: "nullFrame"
                    source: "nullRock.png"
                    frames: 1
                    duration: 1000
                }
                ]
            }
        }
        emitters: TrailEmitter{
            particlesPerSecond: 12
            particleDuration: 5000
            emitting: true
            yAccelVariation: 80
            xAccelVariation: 80;
            particleSize: 15
            particleEndSize: 300
            emitterX: width/2
            emitterY: height/2
        }
        affectors: Toggle{
            affecting: ma.pressed;
            Zone{
                x: ma.mouseX - 16
                y: ma.mouseY - 16
                width: 32
                height:32
                SpriteGoal{
                    goalState: "nullFrame"
                }
            }
        }
    }
    Image {
        id: rocketShip
        source: "rocket.png"
        smooth: true
        anchors.centerIn: holder
        rotation: (circle.percent+0.25) * 360
        z: 2
    }
    Item {
        id: holder
        x: circle.x - Math.sin(circle.percent * 6.28316530714)*250
        y: circle.y + Math.cos(circle.percent * 6.28316530714)*250
        z: 1
    }

    Item {
        id: circle
        x: root.width / 1.2 
        y: root.height / 1.7
        property real percent: 0

        SequentialAnimation on percent {
            id: circleAnim1
            loops: Animation.Infinite
            running: false
            NumberAnimation {
            duration: 4000
            from: 1
            to: 0
            }

        }
    }
    ParticleSystem {
        z: 0
        particles: ColoredParticle{
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
        }
        emitters: TrailEmitter{
            id: trailsNormal2

            particlesPerSecond: 300
            particleDuration: 500

            emitterY: holder.y
            emitterX: holder.x 

            xSpeedVariation: 40
            ySpeedVariation: 40
            speedFromMovement: 16

            yAccelVariation: 10
            xAccelVariation: 10

            particleSize: 4
            particleSizeVariation: 4
        }
    }
}

import Qt.labs.particles 2.1
import Qt.labs.particles 2.1 as Qlp
import QtQuick 2.0

Item {
    id: root
    width: 360
    height: 540
    Timer{
        id: particleResetTimer
        interval: spinState.duration * spinState.frames
        repeat: false
        running: false
        onTriggered: particles.goalState = ""
    }
    MouseArea{
        anchors.fill: parent
        onClicked: {particles.goalState="nullFrame"; particleResetTimer.start();}
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
        emitters: TrailsEmitter {
            id: starField
            image: "star.png"

            particlesPerSecond: 80
            particleDuration: 2500

            colorVariation: 0.3
            color: "white"

            emitterY: root.height / 2
            emitterX: root.width / 2

            yAccelVariation: 100
            xAccelVariation: 100;

            particleSize: 0
            particleEndSize: 80
            particleSizeVariation: 10
            additive: 1
        }
    }

    ParticleSystem{
        anchors.fill: parent
        emitters: SpriteEmitter{
            id: particles
            states: [SpriteState{
                id: spinState
                name: "spinning"
                source: "meteor.png"
                frames: 35
                duration: 40
                speedModifiesDuration: -0.1
                to: {"explode":0, "spinning":1}
            },SpriteState{
                name: "explode"
                source: "_explo.png"
                frames: 22
                duration: 40
                speedModifiesDuration: -0.1
                to: {"nullFrame":1}
            },SpriteState{//Not sure if this is needed, but seemed easiest
                name: "nullFrame"
                source: "nullRock.png"
                frames: 1
                duration: 1000
            }
            ]

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
        affectors: [Qlp.Scale{}]
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
        emitters: TrailsEmitter{
            id: trailsNormal2
            image: "particle4.png"

            particlesPerSecond: 300
            particleDuration: 500

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

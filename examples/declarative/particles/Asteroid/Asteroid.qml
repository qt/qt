import Qt.labs.particles 2.0
import Qt.labs.particles 2.0 as Qlp
import QtQuick 2.0

Item {
    id: root
    width: 360
    height: 540
    MouseArea{
        id: ma
        anchors.fill: parent
    }

    ParticleSystem { id: sys }
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
    ColoredParticle {
        system: sys
        particles: ["starfield"]
        image: "star.png"
        colorVariation: 0.3
        color: "white"
        additive: 1
    }
    TrailEmitter {
        id: starField
        system: sys
        particle: "starfield"

        particlesPerSecond: 80
        particleDuration: 2500

        anchors.centerIn: parent

        yAccelVariation: 100
        xAccelVariation: 100;

        particleSize: 0
        particleEndSize: 80
        particleSizeVariation: 10
    }
    TrailEmitter{
        system: sys
        particle: "meteor"
        particlesPerSecond: 12
        particleDuration: 5000
        emitting: true
        yAccelVariation: 80
        xAccelVariation: 80;
        particleSize: 15
        particleEndSize: 300
        anchors.centerIn: parent
     }
    SpriteParticle{
        system: sys
        particles: ["meteor"]
        sprites:[Sprite{
                id: spinState
                name: "spinning"
                source: "meteor.png"
                frames: 35
                duration: 40
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
    SpriteGoal{
        particles: ["meteor"]
        system: sys
        goalState: "explode"
        jump: true
        anchors.centerIn: holder//A bug in affectors currently isn't compensating for relative x,y. when that's fixed this can just anchors.fill: rocketShip
        width: 60
        height: 60
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
        x: circle.x - Math.sin(circle.percent * 6.28316530714)*200
        y: circle.y + Math.cos(circle.percent * 6.28316530714)*200
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
            running: true
            NumberAnimation {
            duration: 4000
            from: 1
            to: 0
            }

        }
    }
    ColoredParticle{
        z:0 
        system: sys
        particles: ["exhaust"]
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
    TrailEmitter{
        id: trailsNormal2
        system: sys
        particle: "exhaust"

        particlesPerSecond: 300
        particleDuration: 500

        y: holder.y
        x: holder.x 

        xSpeedVariation: 40
        ySpeedVariation: 40
        speedFromMovement: 16

        yAccelVariation: 10
        xAccelVariation: 10

        particleSize: 4
        particleSizeVariation: 4
    }
}

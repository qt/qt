import QtQuick 2.0
import Qt.labs.particles 2.0

Rectangle {

    id: root

    height: 540
    width: 360

    gradient: Gradient {
        GradientStop { position: 0; color: "#000020" }
        GradientStop { position: 1; color: "#000000" }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: root

        onPressed: stopAndStart()
        onReleased: stopAndStart()
        function stopAndStart() {
            trailsNormal.emitting = false;
            trailsNormal.emitting = true;
            trailsStars.emitting = false;
            trailsStars.emitting = true;
            trailsNormal2.emitting = false;
            trailsNormal2.emitting = true;
            trailsStars2.emitting = false;
            trailsStars2.emitting = true;
            print("stop and start")
        }
    }

    ParticleSystem{ id: sys1 }
    ColoredParticle{
        system: sys1
        image: "content/particle.png"
        color: "cyan"
        SequentialAnimation on color {
            loops: Animation.Infinite
            ColorAnimation {
                from: "cyan"
                to: "magenta"
                duration: 1000
            }
            ColorAnimation {
                from: "magenta"
                to: "blue"
                duration: 2000
            }
            ColorAnimation {
                from: "blue"
                to: "violet"
                duration: 2000
            }
            ColorAnimation {
                from: "violet"
                to: "cyan"
                duration: 2000
            }
        }
        colorVariation: 0.3
    }
    TrailEmitter{
        id: trailsNormal
        system: sys1

        particlesPerSecond: 500
        particleDuration: 2000


        emitterY: mouseArea.pressed ? mouseArea.mouseY : circle.cy
        emitterX: mouseArea.pressed ? mouseArea.mouseX : circle.cx

        xSpeedVariation: 4
        ySpeedVariation: 4
        speedFromMovement: 8

        yAccelVariation: 10
        xAccelVariation: 10

        particleSize: 8
        particleSizeVariation: 4
    }
    ParticleSystem { id: sys2 }
    ColoredParticle{
        color: "cyan"
        system: sys2
        SequentialAnimation on color {
            loops: Animation.Infinite
            ColorAnimation {
                from: "magenta"
                to: "cyan"
                duration: 1000
            }
            ColorAnimation {
                from: "cyan"
                to: "magenta"
                duration: 2000
            }
        }
        colorVariation: 0.5
        image: "content/star.png"
    }
    TrailEmitter{
        id: trailsStars
        system: sys2

        particlesPerSecond: 100
        particleDuration: 2200


        emitterY: mouseArea.pressed ? mouseArea.mouseY : circle.cy
        emitterX: mouseArea.pressed ? mouseArea.mouseX : circle.cx

        xSpeedVariation: 4
        ySpeedVariation: 4
        speedFromMovement: 8

        yAccelVariation: 10
        xAccelVariation: 10

        particleSize: 22
        particleSizeVariation: 4
    }
    ParticleSystem { id: sys3; }
    ColoredParticle{
        image: "content/particle.png"
        system: sys3
        color: "orange"
        SequentialAnimation on color {
            loops: Animation.Infinite
            ColorAnimation {
                from: "red"
                to: "green"
                duration: 2000
            }
            ColorAnimation {
                from: "green"
                to: "red"
                duration: 2000
            }
        }

        colorVariation: 0.2

    }
    TrailEmitter{
        id: trailsNormal2
        system: sys3

        particlesPerSecond: 300
        particleDuration: 2000

        emitterY: mouseArea.pressed ? mouseArea.mouseY : circle2.cy
        emitterX: mouseArea.pressed ? mouseArea.mouseX : circle2.cx

        xSpeedVariation: 4
        ySpeedVariation: 4
        speedFromMovement: 16

        yAccelVariation: 10
        xAccelVariation: 10

        particleSize: 12
        particleSizeVariation: 4
    }
    ParticleSystem { id: sys4; }
    ColoredParticle{
        system: sys4
        image: "content/star.png"
        color: "green"
        SequentialAnimation on color {
            loops: Animation.Infinite
            ColorAnimation {
                from: "green"
                to: "red"
                duration: 2000
            }
            ColorAnimation {
                from: "red"
                to: "green"
                duration: 2000
            }
        }

        colorVariation: 0.5
    }
    TrailEmitter{
        id: trailsStars2
        system: sys4

        particlesPerSecond: 50
        particleDuration: 2200


        emitterY: mouseArea.pressed ? mouseArea.mouseY : circle2.cy
        emitterX: mouseArea.pressed ? mouseArea.mouseX : circle2.cx

        xSpeedVariation: 2
        ySpeedVariation: 2
        speedFromMovement: 16

        yAccelVariation: 10
        xAccelVariation: 10

        particleSize: 22
        particleSizeVariation: 4
    }



    color: "white"

    Item {
        id: circle
        //anchors.fill: parent
        property real radius: 0
        property real dx: root.width / 2
        property real dy: root.height / 2
        property real cx: radius * Math.sin(percent*6.283185307179) + dx
        property real cy: radius * Math.cos(percent*6.283185307179) + dy
        property real percent: 0

        SequentialAnimation on percent {
            loops: Animation.Infinite
            running: true
            NumberAnimation {
            duration: 1000
            from: 1
            to: 0
            loops: 8
            }
            NumberAnimation {
            duration: 1000
            from: 0
            to: 1
            loops: 8
            }

        }

        SequentialAnimation on radius {
            loops: Animation.Infinite
            running: true
            NumberAnimation {
                duration: 4000
                from: 0
                to: 100
            }
            NumberAnimation {
                duration: 4000
                from: 100
                to: 0
            }
        }
    }

    Item {
        id: circle3
        property real radius: 100
        property real dx: root.width / 2
        property real dy: root.height / 2
        property real cx: radius * Math.sin(percent*6.283185307179) + dx
        property real cy: radius * Math.cos(percent*6.283185307179) + dy
        property real percent: 0

        SequentialAnimation on percent {
            loops: Animation.Infinite
            running: true
            NumberAnimation { from: 0.0; to: 1 ; duration: 10000;  }
        }
    }

    Item {
        id: circle2
        property real radius: 30
        property real dx: circle3.cx
        property real dy: circle3.cy
        property real cx: radius * Math.sin(percent*6.283185307179) + dx
        property real cy: radius * Math.cos(percent*6.283185307179) + dy
        property real percent: 0

        SequentialAnimation on percent {
            loops: Animation.Infinite
            running: true
            NumberAnimation { from: 0.0; to: 1 ; duration: 1000; }
        }
    }

}

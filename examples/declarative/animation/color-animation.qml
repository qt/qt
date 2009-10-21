import Qt 4.6

Item {
    id: window
    width: 640; height: 480

    // Let's draw the sky...
    Rectangle {
        anchors { left: parent.left; top: parent.top; right: parent.right; bottom: parent.verticalCenter }
        gradient: Gradient {
            GradientStop {
                position: 0.0
                color: SequentialAnimation {
                    running: true; repeat: true
                    ColorAnimation { from: "DeepSkyBlue"; to: "#141450"; duration: 5000 }
                    ColorAnimation { from: "#141450"; to: "DeepSkyBlue"; duration: 5000 }
                }
            }
            GradientStop {
                position: 1.0
                color: SequentialAnimation {
                    running: true; repeat: true
                    ColorAnimation { from: "SkyBlue"; to: "#437284"; duration: 5000 }
                    ColorAnimation { from: "#437284"; to: "SkyBlue"; duration: 5000 }
                }
            }
        }
        Particles {
            anchors.fill:parent; source: "images/star.png"; angleDeviation: 360; velocity: 0
            velocityDeviation: 0; count: parent.width / 10; fadeInDuration: 2800
            opacity: SequentialAnimation {
                running: true; repeat: true
                NumberAnimation { from: 0; to: 1; duration: 5000 }
                NumberAnimation { from: 1; to: 0; duration: 5000 }
            }
        }
    }

    Item {
        id: space
        width: parent.width; height: 2 * parent.height
        transformOrigin: "Center"
        rotation: SequentialAnimation {
            running: true; repeat: true
            NumberAnimation { from: 0; to: 360; duration: 10000 }
        }
        Image {
            source: "images/sun.png"; y: 10; anchors.horizontalCenter: parent.horizontalCenter
            transformOrigin: "Center"; rotation: -space.rotation
        }
        Image {
            source: "images/moon.png"; y: parent.height - 74; anchors.horizontalCenter: parent.horizontalCenter
            transformOrigin: "Center"; rotation: -space.rotation
        }
    }

    // ...and the ground.
    Rectangle {
        anchors { left: parent.left; top: parent.verticalCenter; right: parent.right; bottom: parent.bottom }
        gradient: Gradient {
            GradientStop {
                position: 0.0
                color: SequentialAnimation {
                    running: true; repeat: true
                    ColorAnimation { from: "ForestGreen"; to: "#001600"; duration: 5000 }
                    ColorAnimation { from: "#001600"; to: "ForestGreen"; duration: 5000 }
                }
            }
            GradientStop { position: 1.0; color: "DarkGreen" }
        }
    }
}

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
                    ColorAnimation { from: "DeepSkyBlue"; to: "#0E1533"; duration: 5000 }
                    ColorAnimation { from: "#0E1533"; to: "DeepSkyBlue"; duration: 5000 }
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
    }

    // the sun, moon, and stars
    Item {
        width: parent.width; height: 2 * parent.height
        transformOrigin: Item.Center
        rotation: NumberAnimation { from: 0; to: 360; duration: 10000; running: true; repeat: true }
        Image {
            source: "images/sun.png"; y: 10; anchors.horizontalCenter: parent.horizontalCenter
            transformOrigin: Item.Center; rotation: -3 * parent.rotation
        }
        Image {
            source: "images/moon.png"; y: parent.height - 74; anchors.horizontalCenter: parent.horizontalCenter
            transformOrigin: Item.Center; rotation: -parent.rotation
        }
        Particles {
            x: 0; y: parent.height/2; width: parent.width; height: parent.height/2
            source: "images/star.png"; angleDeviation: 360; velocity: 0
            velocityDeviation: 0; count: parent.width / 10; fadeInDuration: 2800
            opacity: SequentialAnimation {
                running: true; repeat: true
                NumberAnimation { from: 0; to: 1; duration: 5000 }
                NumberAnimation { from: 1; to: 0; duration: 5000 }
            }
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

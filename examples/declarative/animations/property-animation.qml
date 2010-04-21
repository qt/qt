import Qt 4.7

Item {
    id: window
    width: 320; height: 480

    // Let's draw the sky...
    Rectangle {
        anchors { left: parent.left; top: parent.top; right: parent.right; bottom: parent.verticalCenter }
        gradient: Gradient {
            GradientStop { position: 0.0; color: "DeepSkyBlue" }
            GradientStop { position: 1.0; color: "LightSkyBlue" }
        }
    }

    // ...and the ground.
    Rectangle {
        anchors { left: parent.left; top: parent.verticalCenter; right: parent.right; bottom: parent.bottom }
        gradient: Gradient {
            GradientStop { position: 0.0; color: "ForestGreen" }
            GradientStop { position: 1.0; color: "DarkGreen" }
        }
    }

    // The shadow for the smiley face
    Image {
        anchors.horizontalCenter: parent.horizontalCenter
        source: "images/shadow.png"; y: smiley.minHeight + 58
        transformOrigin: Item.Center

        // The scale property depends on the y position of the smiley face.
        scale: smiley.y * 0.5 / (smiley.minHeight - smiley.maxHeight)
    }

    Image {
        id: smiley
        property int maxHeight: window.height / 3
        property int minHeight: 2 * window.height / 3

        anchors.horizontalCenter: parent.horizontalCenter
        source: "images/face-smile.png"; y: minHeight

        // Animate the y property. Setting loops to Animation.Infinite makes the
        // animation repeat indefinitely, otherwise it would only run once.
        SequentialAnimation on y {
            loops: Animation.Infinite

            // Move from minHeight to maxHeight in 300ms, using the OutExpo easing function
            NumberAnimation {
                from: smiley.minHeight; to: smiley.maxHeight
                easing.type: "OutExpo"; duration: 300
            }

            // Then move back to minHeight in 1 second, using the OutBounce easing function
            NumberAnimation {
                from: smiley.maxHeight; to: smiley.minHeight
                easing.type: "OutBounce"; duration: 1000
            }

            // Then pause for 500ms
            PauseAnimation { duration: 500 }
        }
    }
}

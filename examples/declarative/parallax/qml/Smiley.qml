import Qt 4.6

Item {
    id: window
    width: 320; height: 480

    // The shadow for the smiley face
    Image {
        anchors.horizontalCenter: parent.horizontalCenter
        source: "../pics/shadow.png"; y: smiley.minHeight + 58
        transformOrigin: Item.Center

        // The scale property depends on the y position of the smiley face.
        scale: smiley.y * 0.5 / (smiley.minHeight - smiley.maxHeight)
    }

    Image {
        id: smiley
        property int maxHeight: window.height / 3
        property int minHeight: 2 * window.height / 3

        anchors.horizontalCenter: parent.horizontalCenter
        source: "../pics/face-smile.png"; y: minHeight

        // Animate the y property. Setting repeat to true makes the
        // animation repeat indefinitely, otherwise it would only run once.
        y: SequentialAnimation {
            running: true; repeat: true

            // Move from minHeight to maxHeight in 300ms, using the easeOutExpo easing function
            NumberAnimation {
                from: smiley.minHeight; to: smiley.maxHeight
                easing: "easeOutExpo"; duration: 300
            }
            
            // Then move back to minHeight in 1 second, using the easeOutBounce easing function
            NumberAnimation {
                from: smiley.maxHeight; to: smiley.minHeight
                easing: "easeOutBounce"; duration: 1000
            }

            // Then pause for 500ms
            PauseAnimation { duration: 500 }
        }
    }
}


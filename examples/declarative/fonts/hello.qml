import Qt 4.6

Rectangle {
    id: screen; width: 800; height: 480; color: "black"

    Item {
        id: container; x: screen.width / 2; y: screen.height / 2
        Text {
            id: text; color: "white"; anchors.centerIn: parent
            text: "Hello world!"; font.pixelSize: 60

            font.letterSpacing: SequentialAnimation {
                repeat: true; running: true
                NumberAnimation { from: 100; to: 300; easing: "easeInQuad"; duration: 3000 }
                ScriptAction { script: {
                    container.y = (screen.height / 4) + (Math.random() * screen.height / 2)
                    container.x = (screen.width / 4) + (Math.random() * screen.width / 2)
                } }
            }
            opacity: SequentialAnimation {
                repeat: true; running: true
                NumberAnimation { from: 1; to: 0; duration: 2600 }
                PauseAnimation { duration: 400 }
            }
        }
    }
}

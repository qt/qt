import Qt 4.6

Rectangle {
    id: rect
    width: 120
    height: 200
    color: "white"
    Image {
        id: img
        source: "pics/qtlogo.png"
        x: 60-width/2
        y: 200-height
        y: SequentialAnimation {
            repeat: true
            NumberAnimation {
                to: 0; duration: 500
                easing: "easeInOutQuad"
            }
            NumberAnimation {
                to: 200-img.height
                easing: "easeOutBounce"
                duration: 2000
            }
            PauseAnimation {
                duration: 1000
            }
        }
    }
}

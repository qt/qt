Rect {
    color: "#ffffff"
    width: 320; height: 240
    Rect {
        id: Rect
        color: "#00ff00"
        y: 200; width: 60; height: 20
        y: SequentialAnimation {
            running: true; repeat: true
            NumberAnimation {
                to: 20; duration: 500
                easing: "easeInOutQuad"
            }
            NumberAnimation {
                to: 200; duration: 2000
                easing: "easeOutBounce"
            }
            PauseAnimation { duration: 1000 }
        }
    }

    // Velocity
    Rect {
        color: "#ff0000"
        x: Rect.width; width: Rect.width; height: 20
        y: Follow { source: Rect.y; velocity: 200 }
    }
    Text { x: Rect.width; y: 220; text: "Velocity" }

    // Spring
    Rect {
        color: "#ff0000"
        x: Rect.width * 2; width: Rect.width; height: 20
        y: Follow { source: Rect.y; spring: 1.2; damping: 0.1 }
    }
    Text { x: Rect.width * 2; y: 220; text: "Spring" }

    // Follow mouse
    MouseRegion {
        id: Mouse
        anchors.fill: parent
        Rect {
            width: 20; height: 20
            radius: 10
            color: "#0000ff"
            x: Follow { source: Mouse.mouseX-10; spring: 1.0; damping: 0.05 }
            y: Follow { source: Mouse.mouseY-10; spring: 1.0; damping: 0.05 }
        }
    }
}

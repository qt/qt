import Qt 4.6

Rectangle {
    color: "#ffffff"
    width: 320; height: 240
    Rectangle {
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
    Rectangle {
        color: "#ff0000"
        x: Rect.width; width: Rect.width; height: 20
        y: 200
        y: Follow { source: Rect.y; velocity: 200 }
    }
    Text { x: Rect.width; y: 220; text: "Velocity" }

    // Spring
    Rectangle {
        color: "#ff0000"
        x: Rect.width * 2; width: Rect.width/2; height: 20
        y: 200
        y: Follow { source: Rect.y; spring: 1.0; damping: 0.2 }
    }
    Rectangle {
        color: "#880000"
        x: Rect.width * 2.5; width: Rect.width/2; height: 20
        y: 200
        y: Follow { source: Rect.y; spring: 1.0; damping: 0.2; mass: 3.0 } // "heavier" object
    }
    Text { x: Rect.width * 2; y: 220; text: "Spring" }

    // Follow mouse
    MouseRegion {
        id: Mouse
        anchors.fill: parent
        Rectangle {
            id: "Ball"
            width: 20; height: 20
            radius: 10
            color: "#0000ff"
            x: Follow { id: "F1"; source: Mouse.mouseX-10; spring: 1.0; damping: 0.05; epsilon: 0.25 }
            y: Follow { id: "F2"; source: Mouse.mouseY-10; spring: 1.0; damping: 0.05; epsilon: 0.25 }
            states: [
                State {
                    name: "following"
                    when: !F1.inSync || !F2.inSync
                    PropertyChanges { target: Ball; color: "#ff0000" }
                }
            ]
            transitions: [
                Transition {
                    ColorAnimation { duration: 200 }
                }
            ]
        }
    }
}

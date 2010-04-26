import Qt 4.7

Rectangle {
    width: 800; height: 240; color: "gray"

    Rectangle {
        id: rect
        width: 50; height: 20; y: 30; color: "black"
        SequentialAnimation on x {
            loops: Animation.Infinite
            NumberAnimation { from: 50; to: 700; duration: 2000 }
            NumberAnimation { from: 700; to: 50; duration: 2000 }
        }
    }

    Rectangle {
        width: 50; height: 20; y: 60; color: "red"
        x: rect.x
        Behavior on x { SmoothedAnimation { velocity: 400 } }
    }

    Rectangle {
        width: 50; height: 20; y: 90; color: "yellow"
        x: rect.x
        Behavior on x { SmoothedAnimation { velocity: 300; reversingMode: SmoothedAnimation.Immediate } }
    }

    Rectangle {
        width: 50; height: 20; y: 120; color: "green"
        x: rect.x
        Behavior on x { SmoothedAnimation { reversingMode: SmoothedAnimation.Sync } }
    }

    Rectangle {
        width: 50; height: 20; y: 150; color: "purple"
        x: rect.x
        Behavior on x { SmoothedAnimation { maximumEasingTime: 200 } }
    }

    Rectangle {
        width: 50; height: 20; y: 180; color: "blue"
        x: rect.x
        Behavior on x { SmoothedAnimation { duration: 300 } }
    }
}

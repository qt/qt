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
        width: 50; height: 20; x: rect.x; y: 60; color: "red"
        Behavior on x { SmoothedAnimation { velocity: 400 } }
    }

    Rectangle {
        width: 50; height: 20; x: rect.x; y: 90; color: "yellow"
        Behavior on x { SmoothedAnimation { velocity: 300; reversingMode: SmoothedAnimation.Immediate } }
    }

    Rectangle {
        width: 50; height: 20; x: rect.x; y: 120; color: "green"
        Behavior on x { SmoothedAnimation { reversingMode: SmoothedAnimation.Sync } }
    }

    Rectangle {
        width: 50; height: 20; x: rect.x; y: 150; color: "purple"
        Behavior on x { SmoothedAnimation { maximumEasingTime: 200 } }
    }

    Rectangle {
        width: 50; height: 20; x: rect.x; y: 180; color: "blue"
        Behavior on x { SmoothedAnimation { duration: 300 } }
    }
}

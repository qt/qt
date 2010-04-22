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
        SmoothedFollow on x { to: rect.x; velocity: 400; enabled: true }
    }

    Rectangle {
        width: 50; height: 20; y: 90; color: "yellow"
        SmoothedFollow on x { to: rect.x; velocity: 300; reversingMode: SmoothedAnimation.Immediate; enabled: true }
    }

    Rectangle {
        width: 50; height: 20; y: 120; color: "green"
        SmoothedFollow on x { to: rect.x; reversingMode: SmoothedAnimation.Sync; enabled: true }
    }

    Rectangle {
        width: 50; height: 20; y: 150; color: "purple"
        SmoothedFollow on x { to: rect.x; maximumEasingTime: 200; enabled: true }
    }

    Rectangle {
        width: 50; height: 20; y: 180; color: "blue"
        SmoothedFollow on x { to: rect.x; duration: 300; enabled: true }
    }
}

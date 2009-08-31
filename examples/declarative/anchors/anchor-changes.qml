import Qt 4.6

Item {
    id: Window
    width: 200; height: 450

    Rectangle {
        id: Red; color: "Red"
        anchors.top: parent.top; height: 150
        width: parent.width
    }

    Rectangle {
        id: Orange; color: "Orange"
        anchors.top: Red.bottom; anchors.bottom: Green.top
        width: parent.width
    }

    Rectangle {
        id: Green; color: "Green"
        height: 150; anchors.bottom: parent.bottom
        width: parent.width
    }

    MouseRegion {
        anchors.fill: parent
        onPressed: Window.state = "Pressed"
        onReleased: Window.state = ""
    }

    states : State {
        name: "Pressed"
        PropertyChanges { target: Orange; color: "Yellow" }
        AnchorChanges { target: Orange; top: Window.top; bottom: Window.bottom }
    }
}

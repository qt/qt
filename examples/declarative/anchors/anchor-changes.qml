import Qt 4.6

Item {
    id: Window
    width: 200; height: 450

    Rectangle {
        id: TitleBar; color: "Gray"
        anchors.top: parent.top; height: 50
        width: parent.width
    }

    Rectangle {
        id: StatusBar; color: "Gray"
        height: 50; anchors.bottom: parent.bottom
        width: parent.width
    }

    Rectangle {
        id: Content
        anchors.top: TitleBar.bottom; anchors.bottom: StatusBar.top
        width: parent.width

        Text { text: "Top"; anchors.top: parent.top }
        Text { text: "Bottom"; anchors.bottom: parent.bottom }
    }

    MouseRegion {
        anchors.fill: parent
        onPressed: Window.state = "FullScreen"
        onReleased: Window.state = ""
    }

    states : State {
        name: "FullScreen"
        AnchorChanges { target: Content; top: Window.top; bottom: Window.bottom }
    }

    transitions : Transition {
        NumberAnimation { properties: "y,height" }
    }
}

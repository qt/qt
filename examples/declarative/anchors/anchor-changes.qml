import Qt 4.6

Item {
    id: window
    width: 200; height: 450

    Rectangle {
        id: titleBar; color: "Gray"
        anchors.top: parent.top; height: 50
        width: parent.width
    }

    Rectangle {
        id: statusBar; color: "Gray"
        height: 50; anchors.bottom: parent.bottom
        width: parent.width
    }

    Rectangle {
        id: content
        anchors.top: titleBar.bottom; anchors.bottom: statusBar.top
        width: parent.width

        Text { text: "Top"; anchors.top: parent.top }
        Text { text: "Bottom"; anchors.bottom: parent.bottom }
    }

    MouseRegion {
        anchors.fill: content
        onPressed: window.state = "FullScreen"
        onReleased: window.state = ""
    }

    states : State {
        name: "FullScreen"
    //! [0]
        AnchorChanges {
            target: content; top: window.top; bottom: window.bottom
        }
    //! [0]
    }

    transitions : Transition {
        NumberAnimation { matchProperties: "y,height" }
    }
}

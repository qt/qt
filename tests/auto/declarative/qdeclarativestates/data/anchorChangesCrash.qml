import Qt 4.7

Rectangle {
    id: container
    width: 400
    height: 400

    states: State {
        name: "reanchored"
        AnchorChanges {
            anchors.top: container.top
        }
    }
}

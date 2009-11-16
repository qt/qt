import Qt 4.6

Rectangle {
    id: root
    width: 400
    height: 100

    // Left click on me
    Rectangle {
        width: 98; height: 48
        color: "red"
        MouseRegion {
            id: mr1
            anchors.fill: parent
            enabled: false
            onClicked: { parent.color = "blue"; root.error = "mr1 should ignore presses"; }
        }
    }

    // Left click, then right click
    Rectangle {
        x: 100
        width: 98; height: 48
        color: "red"
        MouseRegion {
            id: mr2
            anchors.fill: parent
            acceptedButtons: Qt.RightButton
            onClicked: {
                if (mouse.button == Qt.RightButton) {
                    parent.color = "blue";
                } else {
                    parent.color = "green";
                    root.error = "mr1 should ignore presses";
                }
            }
        }
    }

    // press and hold me
    Rectangle {
        x: 200
        width: 98; height: 48
        color: "red"
        MouseRegion {
            id: mr3
            anchors.fill: parent
            onPressAndHold: {
                parent.color = "blue";
            }
        }
    }

    // click me
    Rectangle {
        x: 300
        width: 98; height: 48
        color: "red"
        MouseRegion {
            id: mr4
            anchors.fill: parent
            onPressed: {
                parent.color = "blue";
            }
            onReleased: {
                parent.color = "red";
            }
        }
    }

    // move into and out of me
    Rectangle {
        x: 0
        y: 50
        width: 98; height: 48
        color: "red"
        MouseRegion {
            id: mr5
            anchors.fill: parent
            hoverEnabled: true
            onEntered: {
                parent.color = "blue";
            }
            onExited: {
                parent.color = "green";
            }
        }
    }

    // click, then double click me
    Rectangle {
        x: 100
        y: 50
        width: 98; height: 48
        color: "red"
        MouseRegion {
            id: mr6
            anchors.fill: parent
            onClicked: {
                parent.color = "blue";
            }
            onDoubleClicked: {
                parent.color = "green";
            }
        }
    }

    // click, then double click me - nothing should happen
    Rectangle {
        x: 100
        y: 50
        width: 98; height: 48
        color: "red"
        MouseRegion {
            id: mr7
            anchors.fill: parent
            enabled: false
            onClicked: { parent.color = "blue" }
            onPressed: { parent.color = "yellow" }
            onReleased: { parent.color = "cyan" }
            onDoubleClicked: { parent.color = "green" }
        }
    }
}

import Qt 4.6

//! [all]
Rectangle {
    width: 100
    height: 100
    color: "blue"
    resources: [
        Component {
            id: redRectangle
            Rectangle {
                width: 30
                height: 30
                color: "red"
                radius: 5
            }
        }
    ]
    Loader {
        sourceComponent: redRectangle
        anchors.right: parent.right
        anchors.top: parent.top
    }
    Loader {
        sourceComponent: redRectangle
        anchors.left: parent.left
        anchors.top: parent.top
    }
    Loader {
        sourceComponent: redRectangle
        anchors.left: parent.left
        anchors.bottom: parent.bottom
    }
}
//! [all]

import Qt 4.7

Rectangle {
    width: 300
    height: 200
    color: "blue"
    Loader {
        objectName: "loader"
        anchors.fill: parent
        sourceComponent: Component {
            Rectangle { color: "red"; objectName: "sourceElement" }
        }
    }
}

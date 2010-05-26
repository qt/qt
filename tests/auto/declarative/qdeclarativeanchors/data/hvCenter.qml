import Qt 4.7

Rectangle {
    width: 77; height: 95
    Rectangle {
        objectName: "centered"
        width: 57; height: 57; color: "blue"
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
    }
}

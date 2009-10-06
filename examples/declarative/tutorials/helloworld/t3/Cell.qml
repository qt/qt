import Qt 4.6

Item {
    property var color

    id: cellContainer
    width: 40
    height: 25
    Rectangle {
        anchors.fill: parent
        color: cellContainer.color
    }
    MouseRegion {
        anchors.fill: parent
        onClicked: { helloText.color = cellContainer.color }
    }
}

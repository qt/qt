import Qt 4.6

Item {
    property var color

    id: CellContainer
    width: 40
    height: 25

    Rect {
        anchors.fill: parent
        color: CellContainer.color
    }
    MouseRegion {
        anchors.fill: parent
        onClicked: { HelloText.color = CellContainer.color }
    }
}

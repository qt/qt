Item {
    id: CellContainer
    width: 40
    height: 25
    properties: Property {
        name: "color"
    }
    Rect {
        anchors.fill: parent
        color: CellContainer.color
    }
    MouseRegion {
        anchors.fill: parent
        onClicked: { HelloText.color = CellContainer.color }
    }
}

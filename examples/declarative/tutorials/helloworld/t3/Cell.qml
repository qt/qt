import Qt 4.6

Item {
    id: container
    property alias color: rectangle.color
    signal clicked(string color)

    width: 40; height: 25

    Rectangle {
        id: rectangle
        border.color: "white"
        anchors.fill: parent
    }

    MouseRegion {
        anchors.fill: parent
        onClicked: container.clicked(container.color)
    }
}

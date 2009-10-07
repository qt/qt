import Qt 4.6

Rectangle {
    id: container

    property var text
    signal clicked

    height: text.height + 10
    width: text.width + 20
    border.width: 1
    radius: 4
    color: "grey"
    MouseRegion { anchors.fill: parent; onClicked: container.clicked() }
    Text { id: text; anchors.centerIn:parent; font.pointSize: 10; text: parent.text }
}

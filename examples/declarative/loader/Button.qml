Rect {
    id: Container

    property var text
    signal clicked

    height: Text.height + 10
    width: Text.width + 20
    pen.width: 1
    radius: 4
    color: "grey"
    MouseRegion { anchors.fill: parent; onClicked: Container.clicked() }
    Text { id: Text; anchors.centeredIn:parent; font.size: 10; text: parent.text }
}

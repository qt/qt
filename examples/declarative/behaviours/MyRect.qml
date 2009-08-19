import Qt 4.6

Rectangle {
    radius: 15
    border.color: "black"
    width: 100
    height: 100
    id: Page
    MouseRegion {
        anchors.fill: parent
        onClicked: { bluerect.parent = Page; bluerect.x=0 }
    }
}

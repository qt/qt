import Qt 4.6

Rectangle {
    radius: 15
    border.color: "black"
    width: 100
    height: 100
    id: page
    MouseArea {
        anchors.fill: parent
        onClicked: { bluerect.parent = page; bluerect.x=0 }
    }
}

import Qt 4.7

Rectangle {
    width: txt.width + 16
    height: txt.height + 8
    radius: 4
    border.color: "black"
    property alias caption: txt.text
    signal triggered
    Text{
        id: txt
        text: "Button"
        anchors.centerIn: parent
    }
    MouseArea{
        anchors.fill: parent
        onClicked: parent.triggered()
    }
}

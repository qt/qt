import Qt 4.6

Rectangle {
    id: myRect

    property string text

    color: "black"
    width: 75; height: 50
    radius: 5
    border.width: 10; border.color: "white";
    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onEntered: { focusRect.x = myRect.x; focusRect.y = myRect.y; focusRect.text = myRect.text }
    }
}

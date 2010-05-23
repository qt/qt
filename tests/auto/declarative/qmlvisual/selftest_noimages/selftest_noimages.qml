import Qt 4.7
Text {
    property string error: "not pressed"
    text: (new Date()).valueOf()
    MouseArea {
        anchors.fill: parent
        onPressed: error=""
    }
}

import Qt 4.6
Text {
    property string error: "not pressed"
    text: (new Date()).valueOf()
    MouseRegion {
        anchors.fill: parent
        onPressed: error=""
    }
}

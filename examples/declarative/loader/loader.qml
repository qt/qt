import Qt 4.6

Rect {
    width: 300
    height: 400
    Loader {
        anchors.fill: parent
        source: "Browser.qml"
    }
}

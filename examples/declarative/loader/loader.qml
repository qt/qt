import Qt 4.6

Rectangle {
    width: 300
    height: 400
    Loader {
        anchors.fill: parent
        source: "Browser.qml"
    }
}

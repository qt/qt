import Qt 4.6

Rectangle {
    width: 100; height: 100
    color: "green"
    Component.onCompleted: myLoader.source = "BlueRect.qml"
}

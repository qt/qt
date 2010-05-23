import Qt 4.7

Rectangle {
    width: 200
    height: 400
    Flickable {
        id: flick
        anchors.fill: parent
        contentWidth: 1000; contentHeight: parent.height
        Rectangle {
            border.color: "black"
            border.width: 10
            width: 1000; height: 1000
            rotation: 90
            gradient: Gradient {
                GradientStop { position: 0; color: "black" }
                GradientStop { position: 1; color: "white" }
            }
        }
    }
    Rectangle {
        color: "red"
        width: 100; height: 100
        y: flick.contentX < 10 ? 300 : 0
        Behavior on y { NumberAnimation {} }
    }
}

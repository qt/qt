import Qt 4.6
Rectangle {
    width: 400
    height: 400
    property real basex : 0
    property real movedx: 200
    Rectangle {
        id: rect
        objectName: "MyRect"
        width: 100; height: 100; color: "green"
        x: basex
        x: Behavior { NumberAnimation { duration: 500; } }
    }
    MouseRegion {
        id: clicker
        anchors.fill: parent
    }
    states: State {
        name: "moved"
        when: clicker.pressed
        PropertyChanges {
            target: rect
            x: movedx
        }
    }
}

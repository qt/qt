import Qt 4.6
Rectangle {
    width: 400
    height: 400
    Rectangle {
        id: rect
        objectName: "MyRect"
        width: 100; height: 100;
        color: "green"
        color: Behavior { ColorAnimation { duration: 200; } }
    }
    MouseRegion {
        id: clicker
        anchors.fill: parent
    }
    states: State {
        name: "red"
        when: clicker.pressed
        PropertyChanges {
            target: rect
            color: "red"
        }
    }
}

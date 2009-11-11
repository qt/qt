import Qt 4.6

Rectangle {
    width: 400; height: 400
    Rectangle {
        id: MyRect
        width: 100; height: 100
        color: "red"
    }
    MouseRegion {
        id: Clickable
        anchors.fill: parent
    }

    states: State {
        name: "state1"
        when: Clickable.pressed
        PropertyChanges {
            target: MyRect
            x: 50; y: 50
            color: "blue"
        }
    }

    transitions: Transition {
        to: "state1"
        reversible: true
        SequentialAnimation {
            ColorAnimation {}
            PropertyAction { matchProperties: "x" }
            NumberAnimation { matchProperties: "y"; easing: "InOutQuad" }
        }
    }
}

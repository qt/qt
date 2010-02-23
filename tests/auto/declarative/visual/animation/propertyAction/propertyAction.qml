import Qt 4.6

Rectangle {
    width: 400; height: 400
    Rectangle {
        id: myRect
        width: 100; height: 100
        color: "red"
    }
    MouseArea {
        id: clickable
        anchors.fill: parent
    }

    states: State {
        name: "state1"
        when: clickable.pressed
        PropertyChanges {
            target: myRect
            x: 50; y: 50
            color: "blue"
        }
    }

    transitions: Transition {
        to: "state1"
        reversible: true
        SequentialAnimation {
            ColorAnimation {}
            PropertyAction { properties: "x" }
            NumberAnimation { properties: "y"; easing: "InOutQuad" }
        }
    }
}

import Qt 4.7

/*
This test starts with a red rectangle at 0,0. It should animate a color change to blue,
then jump 50 pixels right, and then animate moving 50 pixels down. Afer this it should
do an exact visual reversal (animate up 50 pixels, jump left 50 pixels, and then animate
a change back to red).
*/

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
            NumberAnimation { properties: "y"; easing.type: "InOutQuad" }
        }
    }
}

import QtQuick 1.0

/*
    This test verifies that a single animation animating two properties is visually the same as two
    animations in a parallel group animating the same properties. Visually, you should see a red
    rectangle at 0,0 stretching from the top of the window to the bottom. This rect will be moved to
    the right side of the window while turning purple. If the bottom half is visually different
    than the top half, there is a problem.
*/

Rectangle {
    width: 400; height: 200
    Rectangle {
        id: redRect
        width: 100; height: 100
        color: "red"
    }
    Rectangle {
        id: redRect2
        width: 100; height: 100
        y: 100
        color: "red"
    }

    MouseArea {
        anchors.fill: parent
        onClicked: parent.state = "state1"
    }

    states: State {
        name: "state1"
        PropertyChanges {
            target: redRect
            x: 300
            color: "purple"
        }
        PropertyChanges {
            target: redRect2
            x: 300
            color: "purple"
        }
    }

    transitions: Transition {
        PropertyAnimation { targets: redRect; properties: "x,color"; duration: 300 }
        ParallelAnimation {
            NumberAnimation { targets: redRect2; properties: "x"; duration: 300 }
            ColorAnimation { targets: redRect2; properties: "color"; duration: 300 }
        }
    }
}

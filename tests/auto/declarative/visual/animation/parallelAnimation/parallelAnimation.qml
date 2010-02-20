import Qt 4.6

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

    MouseRegion {
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

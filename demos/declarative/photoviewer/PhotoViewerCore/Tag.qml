import Qt 4.7

Flipable {
    id: flipable

    property alias frontLabel: frontButton.label
    property alias backLabel: backButton.label

    property int angle: 0
    property int randomAngle: Math.random() * (2 * 6 + 1) - 6
    property bool flipped: false

    signal frontClicked
    signal backClicked
    signal tagChanged(string tag)

    front: EditableButton {
        id: frontButton; rotation: flipable.randomAngle
        anchors { centerIn: parent; verticalCenterOffset: -20 }
        onClicked: flipable.frontClicked()
        onLabelChanged: flipable.tagChanged(label)
    }

    back: Button {
        id: backButton; tint: "red"; rotation: flipable.randomAngle
        anchors { centerIn: parent; verticalCenterOffset: -20 }
        onClicked: flipable.backClicked()
    }

    transform: Rotation {
        origin.x: flipable.width / 2; origin.y: flipable.height / 2
        axis.x: 0; axis.y: 1; axis.z: 0
        angle: flipable.angle
    }

    states: State {
        name: "back"; when: flipable.flipped
        PropertyChanges { target: flipable; angle: 180 }
    }

    transitions: Transition {
        ParallelAnimation {
            NumberAnimation { properties: "angle"; duration: 400 }
            SequentialAnimation {
                NumberAnimation { target: flipable; property: "scale"; to: 0.8; duration: 200 }
                NumberAnimation { target: flipable; property: "scale"; to: 1.0; duration: 200 }
            }
        }
    }
}

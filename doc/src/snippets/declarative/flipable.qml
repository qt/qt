//! [0]
import Qt 4.7

Flipable {
    id: flipable
    width: 240
    height: 240

    property int angle: 0
    property bool flipped: false

    front: Image { source: "front.png" }
    back: Image { source: "back.png" }

    transform: Rotation {
        origin.x: flipable.width/2; origin.y: flipable.height/2
        axis.x: 0; axis.y: 1; axis.z: 0     // rotate around y-axis
        angle: flipable.angle
    }

    states: State {
        name: "back"
        PropertyChanges { target: flipable; angle: 180 }
        when: flipable.flipped
    }

    transitions: Transition {
        NumberAnimation { properties: "angle"; duration: 1000 }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: flipable.flipped = !flipable.flipped
    }
}
//! [0]


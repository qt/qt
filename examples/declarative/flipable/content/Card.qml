import Qt 4.7

Flipable {
    id: container

    property alias image: frontImage.source
    property bool flipped: true
    property int xAxis: 0
    property int yAxis: 0
    property int angle: 0

    width: front.width; height: front.height; state: "back"

    front: Image { id: frontImage; smooth: true }
    back: Image { source: "back.png"; smooth: true }

    MouseArea { anchors.fill: parent; onClicked: container.flipped = !container.flipped }

    transform: Rotation {
        id: rotation; origin.x: container.width / 2; origin.y: container.height / 2
        axis.x: container.xAxis; axis.y: container.yAxis; axis.z: 0
    }

    states: State {
        name: "back"; when: container.flipped
        PropertyChanges { target: rotation; angle: container.angle }
    }

    transitions: Transition {
        ParallelAnimation {
            NumberAnimation { target: rotation; properties: "angle"; duration: 600 }
            SequentialAnimation {
                NumberAnimation { target: container; property: "scale"; to: 0.75; duration: 300 }
                NumberAnimation { target: container; property: "scale"; to: 1.0; duration: 300 }
            }
        }
    }
}

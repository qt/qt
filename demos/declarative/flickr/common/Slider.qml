import Qt 4.7

Item {
    id: slider; width: 400; height: 16

    // value is read/write.
    property real value
    onValueChanged: { handle.x = 2 + (value - minimum) * slider.xMax / (maximum - minimum); }
    property real maximum: 1
    property real minimum: 1
    property int xMax: slider.width - handle.width - 4

    Rectangle {
        anchors.fill: parent
        border.color: "white"; border.width: 0; radius: 8
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#66343434" }
            GradientStop { position: 1.0; color: "#66000000" }
        }
    }

    Rectangle {
        id: handle; smooth: true
        x: slider.width / 2 - handle.width / 2; y: 2; width: 30; height: slider.height-4; radius: 6
        gradient: Gradient {
            GradientStop { position: 0.0; color: "lightgray" }
            GradientStop { position: 1.0; color: "gray" }
        }

        MouseArea {
            anchors.fill: parent; drag.target: parent
            drag.axis: "XAxis"; drag.minimumX: 2; drag.maximumX: slider.xMax+2
            onPositionChanged: { value = (maximum - minimum) * (handle.x-2) / slider.xMax + minimum; }
        }
    }
}

import Qt 4.6

Item {
    id: Slider; width: 400; height: 16

    // value is read/write.
    property real value
    onValueChanged: { Handle.x = 2 + (value - minimum) * Slider.xMax / (maximum - minimum); }
    property real maximum: 1
    property real minimum: 1
    property int xMax: Slider.width - Handle.width - 4

    Rect {
        id: Container; anchors.fill: parent
        border.color: "white"; border.width: 0; radius: 8
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#66343434" }
            GradientStop { position: 1.0; color: "#66000000" }
        }
    }

    Rect {
        id: Handle; smooth: true
        x: Slider.width / 2 - Handle.width / 2; y: 2; width: 30; height: Slider.height-4; radius: 6
        gradient: Gradient {
            GradientStop { position: 0.0; color: "lightgray" }
            GradientStop { position: 1.0; color: "gray" }
        }

        MouseRegion {
            anchors.fill: parent; drag.target: parent
            drag.axis: "XAxis"; drag.minimumX: 2; drag.maximumX: Slider.xMax+2
            onPositionChanged: { value = (maximum - minimum) * (Handle.x-2) / Slider.xMax + minimum; }
        }
    }
}

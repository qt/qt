Item {
    id: Slider; width: 400; height: 16

    // value is read/write.
    property real value
    onValueChanged: { Handle.x = (value - minimum) * Slider.xMax / (maximum - minimum); }
    property real maximum: 1
    property real minimum: 1
    property int xMax: Slider.width - Handle.width - 2

    Rect {
        id: Container; anchors.fill: parent; gradientColor: "#66000000";
        pen.color: "white"; pen.width: 1;  color: "#66343434"; radius: 8
    }

    Rect {
        id: Handle
        x: Slider.width / 2 - Handle.width / 2; y: 2; width: 30; height: 12
        color: "lightgray"; gradientColor: "gray"; radius: 6

        MouseRegion {
            anchors.fill: parent; drag.target: parent
            drag.axis: "x"; drag.xmin: 2; drag.xmax: Slider.xMax
            onPositionChanged: { value = (maximum - minimum) * Handle.x / Slider.xMax + minimum; }
        }
    }
}

import "DialLibrary"
Rect {
    color: "white"
    width: 210
    height: 240
    // Dial with a slider to adjust it
    Dial {
        id: Dial
        value: Slider.x-2
    }
    Rect {
        anchors.top: Dial.bottom
        x: 20
        width: 160
        height: 16
        color: "steelblue"
        gradientColor: "lightsteelblue"
        radius: 8
        opacity: 0.7
        Rect {
            id: Slider
            x: 2
            y: 2
            width: 30
            height: 12
            color: "lightgray"
            gradientColor: "gray"
            radius: 6
            MouseRegion {
                anchors.fill: parent
                drag.target: parent
                drag.axis: "x"
                drag.xmin: 2
                drag.xmax: 128
            }
        }
    }
}

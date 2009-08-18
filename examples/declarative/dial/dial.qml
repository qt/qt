import Qt 4.6

import "DialLibrary"
Rect {
    color: "white"
    width: 210; height: 240

    // Dial with a slider to adjust it
    Dial { id: Dial; value: Slider.x-2 }

    Rect {
        anchors.top: Dial.bottom
        x: 20; width: 160; height: 16
        gradient: Gradient {
            GradientStop { position: 0.0; color: "steelblue" }
            GradientStop { position: 1.0; color: "lightsteelblue" }
        }
        radius: 8; opacity: 0.7; smooth: true
        Rect {
            id: Slider
            x: 2; y: 2; width: 30; height: 12
            radius: 6; smooth: true
            gradient: Gradient {
                GradientStop { position: 0.0; color: "lightgray" }
                GradientStop { position: 1.0; color: "gray" }
            }
            MouseRegion {
                anchors.fill: parent
                drag.target: parent; drag.axis: "x"; drag.minimumX: 2; drag.maximumX: 128
            }
        }
    }
}

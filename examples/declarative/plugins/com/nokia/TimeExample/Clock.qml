import Qt 4.7

Rectangle {
    id: clock
    width: 200; height: 200; color: "gray"

    property alias city: cityLabel.text
    property variant hours
    property variant minutes
    property variant shift : 0

    Image { id: background; source: "clock.png" }

    Image {
        x: 92.5; y: 27
        source: "hour.png"
        smooth: true
        transform: Rotation {
            id: hourRotation
            origin.x: 7.5; origin.y: 73; angle: 0
            SpringFollow on angle {
                spring: 2; damping: 0.2; modulus: 360
                to: (clock.hours * 30) + (clock.minutes * 0.5)
            }
        }
    }

    Image {
        x: 93.5; y: 17
        source: "minute.png"
        smooth: true
        transform: Rotation {
            id: minuteRotation
            origin.x: 6.5; origin.y: 83; angle: 0
            SpringFollow on angle {
                spring: 2; damping: 0.2; modulus: 360
                to: clock.minutes * 6
            }
        }
    }

    Image {
        anchors.centerIn: background; source: "center.png"
    }

    Text {
        id: cityLabel; font.bold: true; font.pixelSize: 14; y:200; color: "white"
        anchors.horizontalCenter: parent.horizontalCenter
    }
}

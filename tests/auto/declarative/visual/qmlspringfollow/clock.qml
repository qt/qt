import Qt 4.6

Rectangle {
    id: clock
    color: "gray"
    width: 200; height: 200

    property var hours: 10
    property var minutes: 28
    property var seconds: 0

    Timer {
        interval: 1000; running: true; repeat: true; triggeredOnStart: true
        onTriggered: seconds++
    }

    Image { id: background; source: "content/clock.png" }

    Image {
        x: 92.5; y: 27
        source: "content/hour.png"
        smooth: true
        transform: Rotation {
            id: hourRotation
            origin.x: 7.5; origin.y: 73; angle: 0
            angle: SpringFollow {
                spring: 2; damping: 0.2; modulus: 360
                source: (clock.hours * 30) + (clock.minutes * 0.5)
            }
        }
    }

    Image {
        x: 93.5; y: 17
        source: "content/minute.png"
        smooth: true
        transform: Rotation {
            id: minuteRotation
            origin.x: 6.5; origin.y: 83; angle: 0
            angle: SpringFollow {
                spring: 2; damping: 0.2; modulus: 360
                source: clock.minutes * 6
            }
        }
    }

    Image {
        x: 97.5; y: 20
        source: "content/second.png"
        smooth: true
        transform: Rotation {
            id: secondRotation
            origin.x: 2.5; origin.y: 80; angle: 0
            angle: SpringFollow {
                spring: 5; damping: 0.25; modulus: 360
                source: clock.seconds * 6
            }
        }
    }

    Image {
        anchors.centerIn: background; source: "content/center.png"
    }
}

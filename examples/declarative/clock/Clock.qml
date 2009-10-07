import Qt 4.6

Item {
    id: clock
    width: 200; height: 200
    property var time
    property var hours
    property var minutes
    property var seconds
    onTimeChanged: {
        var date = new Date;
        hours = date.getHours();
        minutes = date.getMinutes();
        seconds = date.getSeconds();
    }
    Timer {
        interval: 100; running: true; repeat: true; triggeredOnStart: true
        onTriggered: clock.time = new Date()
    }

    Image { source: "background.png" }
    Image {
        x: 95
        y: 54
        source: "hour.png"
        smooth: true
        transform: Rotation {
            id: hourRotation
            origin.x: 4; origin.y: 45
            angle: 0
            angle: SpringFollow {
                spring: 2
                damping: .2
                source: clock.hours * 50 * 3 + clock.minutes / 2
            }
        }
    }
    Image {
        x: 95
        y: 30
        source: "minute.png"
        smooth: true
        transform: Rotation {
            id: minuteRotation
            origin.x: 4; origin.y: 70
            angle: 0
            angle: SpringFollow {
                spring: 2
                damping: .2
                source: clock.minutes * 6
            }
        }
    }
    Image {
        x: 96
        y: 40
        source: "second.png"
        smooth: true
        transform: Rotation {
            id: secondRotation
            origin.x: 2; origin.y: 60
            angle: 0
            angle: SpringFollow {
                spring: 5
                damping: .25
                modulus: 360
                source: clock.seconds * 6
            }
        }
    }

    Rectangle {
        x: 93
        y: 94
        width: 11
        height: 11
        radius: 5
        color: "black"
    }
}

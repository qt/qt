Item {
    id: Clock
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
        interval: 500; running: true; repeat: true; triggeredOnStart: true
        onTriggered: Clock.time = new Date()
    }

    Image { source: "background.png" }
    Image {
        x: 95
        y: 54
        source: "hour.png"
        smooth: true
        transform: Rotation {
            id: HourRotation
            originX: 4; originY: 45
            angle: 0
            angle: Follow {
                spring: 2
                damping: .2
                source: Clock.hours * 50 * 3 + Clock.minutes / 2
            }
        }
    }
    Image {
        x: 95
        y: 30
        source: "minute.png"
        smooth: true
        transform: Rotation {
            id: MinuteRotation
            originX: 4; originY: 70
            angle: 0
            angle: Follow {
                spring: 2
                damping: .2
                source: Clock.minutes * 6
            }
        }
    }
    Image {
        x: 96
        y: 40
        source: "second.png"
        smooth: true
        transform: Rotation {
            id: SecondRotation
            originX: 2; originY: 60
            angle: 0
            angle: Follow {
                spring: 5
                damping: .25
                modulus: 360
                source: Clock.seconds * 6
            }
        }
    }

    Rect {
        x: 93
        y: 94
        width: 11
        height: 11
        radius: 5
        color: "black"
    }
}

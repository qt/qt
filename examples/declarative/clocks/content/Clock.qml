import Qt 4.6

Item {
    id: clock
    width: 200; height: 230

    property alias city: cityLabel.text
    property var hours
    property var minutes
    property var seconds
    property var shift : 0
    property bool night: false

    function timeChanged() {
        var date = new Date;
        hours = shift ? date.getUTCHours() + Math.floor(clock.shift) : date.getHours()
        if ( hours < 7 || hours > 19 ) night = true; else night = false
        minutes = shift ? date.getUTCMinutes() + ((clock.shift % 1) * 60) : date.getMinutes()
        seconds = date.getUTCSeconds();
    }

    Timer {
        interval: 100; running: true; repeat: true; triggeredOnStart: true
        onTriggered: clock.timeChanged()
    }

    Image { id: background; source: "clock.png"; visible: clock.night == false }
    Image { source: "clock-night.png"; visible: clock.night == true }

    Image {
        x: 92.5; y: 27
        source: "hour.png"
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
        source: "minute.png"
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
        source: "second.png"
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
        anchors.centerIn: background; source: "center.png"
    }

    Text {
        id: cityLabel; font.bold: true; font.pixelSize: 14; y:200; color: "white"
        anchors.horizontalCenter: parent.horizontalCenter
    }
}

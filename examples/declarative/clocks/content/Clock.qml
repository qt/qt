import Qt 4.7

Item {
    id: clock
    width: 200; height: 230

    property alias city: cityLabel.text
    property variant hours
    property variant minutes
    property variant seconds
    property variant shift : 0
    property bool night: false

    function timeChanged() {
        var date = new Date;
        hours = shift ? date.getUTCHours() + Math.floor(clock.shift) : date.getHours()
        night = ( hours < 7 || hours > 19 )
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
        x: 97.5; y: 20
        source: "second.png"
        smooth: true
        transform: Rotation {
            id: secondRotation
            origin.x: 2.5; origin.y: 80; angle: 0
            SpringFollow on angle {
                spring: 5; damping: 0.25; modulus: 360
                to: clock.seconds * 6
            }
        }
    }

    Image {
        anchors.centerIn: background; source: "center.png"
    }

    Text {
        id: cityLabel
        y: 200; anchors.horizontalCenter: parent.horizontalCenter
        color: "white"
        font.bold: true; font.pixelSize: 14
        style: Text.Raised; styleColor: "black"
    }
}

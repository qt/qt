import Qt 4.6

Rectangle {
    color: "lightSteelBlue"
    width: 300; height: 600

    ListModel {
        id: list
        ListElement { dayColor: "steelblue" }
        ListElement { dayColor: "blue" }
        ListElement { dayColor: "yellow" }
        ListElement { dayColor: "purple" }
        ListElement { dayColor: "red" }
        ListElement { dayColor: "green" }
        ListElement { dayColor: "orange" }
    }

    Flickable {
        id: Flick
        height: parent.height-50
        width: parent.width; viewportHeight: column.height

        Column {
            id: column
            Repeater {
                model: list
                Rectangle { width: 300; height: 200; color: mr.pressed ? "black" : dayColor
                    MouseRegion {
                        id: mr
                        anchors.fill: parent
                    }
                }
            }
        }
        clip: true
        reportedVelocitySmoothing: 1000
    }
    Rectangle {
        radius: 3
        x: Flick.width-8
        width: 8
        y: Flick.visibleArea.yPosition * Flick.height
        height: Flick.visibleArea.heightRatio * Flick.height
    }

    // click to toggle interactive flag
    Rectangle {
        width: 64
        height: 48
        y: parent.height - 50
        color: "red"
        MouseRegion {
            anchors.fill: parent
            onClicked: Flick.interactive = Flick.interactive ? false : true
        }
    }

    // click to toggle click delay
    Rectangle {
        width: 64
        height: 48
        x: 66
        y: parent.height - 50
        color: "green"
        MouseRegion {
            anchors.fill: parent
            onClicked: Flick.pressDelay = Flick.pressDelay > 0 ? 0 : 500
        }
    }

    // click to toggle overshoot
    Rectangle {
        width: 64
        height: 48
        x: 130
        y: parent.height - 50
        color: "yellow"
        MouseRegion {
            anchors.fill: parent
            onClicked: Flick.overShoot = Flick.overShoot > 0 ? 0 : 30
        }
    }

    Rectangle {
        width: Math.abs(Flick.verticalVelocity)/100
        height: 50
        x: 200
        y: parent.height - 50
        color: blue
    }
}

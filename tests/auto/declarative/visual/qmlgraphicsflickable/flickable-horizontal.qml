import Qt 4.6

Rectangle {
    color: "lightSteelBlue"
    width: 600; height: 300

    ListModel {
        id: List
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
        anchors.fill: parent; viewportWidth: row.width

        Row {
            id: row
            Repeater {
                model: List
                Rectangle { width: 200; height: 300; color: dayColor }
            }
        }
    }
    Rectangle {
        radius: 3
        y: Flick.height-8
        height: 8
        x: Flick.visibleArea.xPosition * Flick.width
        width: Flick.visibleArea.widthRatio * Flick.width
    }
}

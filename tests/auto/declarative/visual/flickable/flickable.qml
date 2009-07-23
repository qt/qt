import Qt 4.6

Rect {
    color: "lightSteelBlue"
    width: 600
    height: 300
    ListModel {
        id: List
        ListElement {
            name: "Sunday"
            dayColor: "#808080"
        }
        ListElement {
            name: "Monday"
            dayColor: "blue"
        }
        ListElement {
            name: "Tuesday"
            dayColor: "yellow"
        }
        ListElement {
            name: "Wednesday"
            dayColor: "purple"
        }
        ListElement {
            name: "Thursday"
            dayColor: "blue"
        }
        ListElement {
            name: "Friday"
            dayColor: "green"
        }
        ListElement {
            name: "Saturday"
            dayColor: "orange"
        }
    }
    Flickable {
        id: Flick
        anchors.fill: parent
        viewportWidth: Lay.width
        HorizontalLayout {
            id: Lay
            Repeater {
                dataSource: List
                Component {
                    Day {
                        day: name
                        color: dayColor
                    }
                }
            }
        }
    }
}

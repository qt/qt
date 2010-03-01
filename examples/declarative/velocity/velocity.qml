import Qt 4.6

Rectangle {
    color: "lightSteelBlue"
    width: 800; height: 600

    ListModel {
        id: list
        ListElement {
            name: "Sunday"
            dayColor: "#808080"
            notes: [
                ListElement {
                    noteText: "Lunch"
                    },
                    ListElement {
                        noteText: "Party"
                    }
            ]
        }
        ListElement {
            name: "Monday"
            dayColor: "blue"
            notes: [
                ListElement {
                    noteText: "Pickup kids"
                    },
                    ListElement {
                        noteText: "Checkout kinetic"
                        },
                        ListElement {
                            noteText: "Read email"
                        }
            ]
        }
        ListElement {
            name: "Tuesday"
            dayColor: "yellow"
            notes: [
                ListElement {
                    noteText: "Walk dog"
                    },
                    ListElement {
                        noteText: "Buy newspaper"
                    }
            ]
        }
        ListElement {
            name: "Wednesday"
            dayColor: "purple"
            notes: [
                ListElement {
                    noteText: "Cook dinner"
                    },
                    ListElement {
                        noteText: "Eat dinner"
                    }
            ]
        }
        ListElement {
            name: "Thursday"
            dayColor: "blue"
            notes: [
                ListElement {
                    noteText: "5:30pm Meeting"
                    },
                    ListElement {
                        noteText: "Weed garden"
                    }
            ]
        }
        ListElement {
            name: "Friday"
            dayColor: "green"
            notes: [
                ListElement {
                    noteText: "Still work"
                    },
                    ListElement {
                        noteText: "Drink"
                    }
            ]
        }
        ListElement {
            name: "Saturday"
            dayColor: "orange"
            notes: [
                ListElement {
                    noteText: "Drink"
                    },
                    ListElement {
                        noteText: "Drink"
                    }
            ]
        }
    }
    Flickable {
        id: flickable
        anchors.fill: parent; contentWidth: lay.width
        Row {
            id: lay
            Repeater {
                model: list
                Component { Day { day: name; color: dayColor; stickies: notes } }
            }
        }
    }
}

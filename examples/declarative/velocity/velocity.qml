import Qt 4.7

Rectangle {
    width: 800; height: 480
    color: "#464646"

    ListModel {
        id: list

        ListElement {
            name: "Sunday"
            notes: [ 
                ListElement { noteText: "Lunch" },
                ListElement { noteText: "Birthday Party" }
            ]
        }
        
        ListElement {
            name: "Monday"
            notes: [
                ListElement { noteText: "Pickup kids from\nschool\n4.30pm" },
                ListElement { noteText: "Checkout Qt" }, ListElement { noteText: "Read email" }
            ]
        }

        ListElement {
            name: "Tuesday"
            notes: [
                ListElement { noteText: "Walk dog" },
                ListElement { noteText: "Buy newspaper" }
            ]
        }

        ListElement {
            name: "Wednesday"
            notes: [ ListElement { noteText: "Cook dinner" } ]
        }

        ListElement {
            name: "Thursday"
            notes: [
                ListElement { noteText: "Meeting\n5.30pm" },
                ListElement { noteText: "Weed garden" }
            ]
        }

        ListElement {
            name: "Friday"
            notes: [
                ListElement { noteText: "More work" },
                ListElement { noteText: "Grocery shopping" }
            ]
        }

        ListElement {
            name: "Saturday"
            notes: [
                ListElement { noteText: "Drink" },
                ListElement { noteText: "Download Qt\nPlay with QML" }
            ]
        }
    }

    ListView {
        id: flickable

        anchors.fill: parent
        focus: true
        highlightRangeMode: ListView.StrictlyEnforceRange
        orientation: ListView.Horizontal
        snapMode: ListView.SnapOneItem
        model: list
        delegate: Day { }
    }
}

import Qt 4.6

Rectangle {
    color: "white"
    width: 800
    height: 600

    ListModel {
        id: Model
        ListElement { name: "1" }
        ListElement { name: "2" }
        ListElement { name: "3" }
        ListElement { name: "4" }
        ListElement { name: "5" }
        ListElement { name: "6" }
        ListElement { name: "6" }
        ListElement { name: "8" }
        ListElement { name: "9" }
    }

    Component {
        id: VerticalDelegate
        FocusScope {
            id: Root
            width: 50; height: 50; 
            Keys.onDigit9Pressed: print("Error - " + name)
            Rectangle { 
                focus: true
                Keys.onDigit9Pressed: print(name)
                width: 50; height: 50; 
                color: Root.ListView.isCurrentItem?"red":"green"
                Text { text: name; anchors.centerIn: parent }
            }
        }
    }

    ListView {
        width: 800; height: 50; orientation: "Horizontal"
        focus: true
        model: Model
        delegate: VerticalDelegate
        currentItemPositioning: "SnapAuto"
    }


    Text {
        y: 100; x: 50
        text: "Currently selected element should be red\nPressing \"9\" should print the number of the currently selected item\nBe sure to scroll all the way to the right, pause, and then all the way to the left."
    }
}

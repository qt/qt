import Qt 4.7

Rectangle {
    width: 400; height: 300

    // MyPets model is defined in dummydata/MyPetsModel.qml
    // The viewer automatically loads files in dummydata/* to assist
    // development without a real data source.
    // This one contains my pets.

    // Define a delegate component.  A component will be
    // instantiated for each visible item in the list.
    Component {
        id: petDelegate
        Item {
            id: wrapper
            width: 200; height: 50
            Column {
                Text { text: 'Name: ' + name }
                Text { text: 'Type: ' + type }
                Text { text: 'Age: ' + age }
            }
            // Use the ListView.isCurrentItem attached property to
            // indent the item if it is the current item.
            states: State {
                name: "Current"
                when: wrapper.ListView.isCurrentItem
                PropertyChanges { target: wrapper; x: 10 }
            }
            transitions: Transition {
                NumberAnimation { properties: "x"; duration: 200 } 
            }
        }
    }
    // Specify a highlight with custom movement.  Note that highlightFollowsCurrentItem
    // is set to false in the ListView so that we can control how the
    // highlight moves to the current item.
    Component {
        id: petHighlight
        Rectangle {
            width: 200; height: 50
            color: "#FFFF88"
            SpringFollow on y { to: list1.currentItem.y; spring: 3; damping: 0.1 }
        }
    }

    ListView {
        id: list1
        width: 200; height: parent.height
        model: MyPetsModel
        delegate: petDelegate
        highlight: petHighlight; highlightFollowsCurrentItem: false
        focus: true
    }
}

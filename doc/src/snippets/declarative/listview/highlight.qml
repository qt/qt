import Qt 4.7

Rectangle {
    width: 180; height: 200; color: "white"

    // ContactModel model is defined in dummydata/ContactModel.qml
    // The viewer automatically loads files in dummydata/* to assist
    // development without a real data source.

    // Define a delegate component.  A component will be
    // instantiated for each visible item in the list.
//! [0]
    Component {
        id: delegate
        Item {
            id: wrapper
            width: 180; height: 40
            Column {
                x: 5; y: 5
                Text { text: '<b>Name:</b> ' + name }
                Text { text: '<b>Number:</b> ' + number }
            }
            // Use the ListView.isCurrentItem attached property to
            // indent the item if it is the current item.
            states: [
                State {
                    name: "Current"
                    when: wrapper.ListView.isCurrentItem
                    PropertyChanges { target: wrapper; x: 10 }
                }
            ]
            transitions: [
                Transition { NumberAnimation { properties: "x"; duration: 200 } }
            ]
        }
    }
//! [0]
    // Specify a highlight with custom movement.  Note that autoHighlight
    // is set to false in the ListView so that we can control how the
    // highlight moves to the current item.
//! [1]
    Component {
        id: highlight
        Rectangle {
            width: 180; height: 40
            color: "lightsteelblue"; radius: 5
            SpringFollow on y {
                to: list.currentItem.y
                spring: 3
                damping: 0.2
            }
        }
    }
    ListView {
        id: list
        width: parent.height; height: parent.height
        model: ContactModel; delegate: delegate
        highlight: highlight
        highlightFollowsCurrentItem: false
        focus: true
    }
//! [1]
}

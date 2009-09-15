import Qt 4.6

Rectangle {
    width: 180; height: 200; color: "white"

    // ContactModel model is defined in dummydata/ContactModel.qml
    // The viewer automatically loads files in dummydata/* to assist
    // development without a real data source.

    // Define a delegate component.  A component will be
    // instantiated for each visible item in the list.
//! [0]
    Component {
        id: Delegate
        Item {
            id: Wrapper
            width: 180; height: 40
            Column {
                x: 5; y: 5
                Text { text: '<b>Name:</b> ' + name }
                Text { text: '<b>Number:</b> ' + number }
            }
        }
    }
//! [0]
    // Specify a highlight with custom movement.  Note that autoHighlight
    // is set to false in the ListView so that we can control how the
    // highlight moves to the current item.
//! [1]
    Component {
        id: Highlight
        Rectangle {
            width: 180; height: 40
            color: "lightsteelblue"; radius: 5
            y: SpringFollow {
                source: List.currentItem.y
                spring: 3
                damping: 0.2
            }
        }
    }
    ListView {
        id: List
        width: parent.height; height: parent.height
        model: ContactModel; delegate: Delegate
        highlight: Highlight
        highlightFollowsCurrentItem: false
        focus: true
    }
//! [1]
}

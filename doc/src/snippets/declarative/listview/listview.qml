import Qt 4.6

//! [3]
Rect {
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
            VerticalLayout {
                x: 5; y: 5
                Text { text: '<b>Name:</b> ' + name }
                Text { text: '<b>Number:</b> ' + number }
            }
        }
    }
//! [0]
    // Define a highlight component.  Just one of these will be instantiated
    // by each ListView and placed behind the current item.
//! [1]
    Component {
        id: Highlight
        Rect {
            color: "lightsteelblue"
            radius: 5
        }
    }
//! [1]
    // The actual list
//! [2]
    ListView {
        width: parent.width; height: parent.height
        model: ContactModel
        delegate: Delegate
        highlight: Highlight
        focus: true
    }
//! [2]
}
//! [3]

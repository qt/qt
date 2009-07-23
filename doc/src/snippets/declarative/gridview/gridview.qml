import Qt 4.6

//! [3]
Rect {
    width: 240; height: 180; color: "white"
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
            width: 80; height: 78
            VerticalLayout {
                Image { source: portrait; anchors.horizontalCenter: parent.horizontalCenter }
                Text { text: name; anchors.horizontalCenter: parent.horizontalCenter }
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
    // The actual grid
//! [2]
    GridView {
        width: parent.width; height: parent.height
        model: ContactModel; delegate: Delegate
        cellWidth: 80; cellHeight: 80
        highlight: Highlight
        focus: true
    }
//! [2]
}
//! [3]

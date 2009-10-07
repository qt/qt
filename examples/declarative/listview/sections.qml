import Qt 4.6

//! [0]
Rectangle {
    width: 200
    height: 240
    color: "white"
    // MyPets model is defined in dummydata/MyPetsModel.qml
    // The viewer automatically loads files in dummydata/* to assist
    // development without a real data source.
    // This one contains my pets.

    // Define a delegate component that includes a separator for sections.
    Component {
        id: petDelegate
        Item {
            id: wrapper
            width: 200
            // My height is the combined height of the description and the section separator
            height: separator.height + desc.height
            Rectangle {
                id: separator
                color: "lightsteelblue"
                width: parent.width
                // Only show the section separator when we are the beginning of a new section
                // Note that for this to work nicely, the list must be ordered by section.
                height: wrapper.ListView.prevSection != wrapper.ListView.section ? 20 : 0
                opacity: wrapper.ListView.prevSection != wrapper.ListView.section ? 1 : 0
                Text {
                    text: wrapper.ListView.section; font.bold: true
                    x: 2; height: parent.height; verticalAlignment: 'AlignVCenter'
                }
            }
            Item {
                id: desc
                x: 5
                height: layout.height + 4
                anchors.top: separator.bottom
                Column {
                    id: layout
                    y: 2
                    Text { text: 'Name: ' + name }
                    Text { text: 'Type: ' + type }
                    Text { text: 'Age: ' + age }
                }
            }
        }
    }
    // Define a highlight component.  Just one of these will be instantiated
    // by each ListView and placed behind the current item.
    Component {
        id: petHighlight
        Rectangle {
            color: "#FFFF88"
        }
    }
    // The list
    ListView {
        id: List
        width: 200
        height: parent.height
        model: MyPetsModel
        delegate: petDelegate
        highlight: petHighlight
        // The sectionExpression is simply the size of the pet.
        // We use this to determine which section we are in above.
        sectionExpression: "size"
        focus: true
    }
}
//! [0]

import Qt 4.6

//! [0]
Rect {
    width: 200
    height: 240
    color: "white"
    // MyPets model is defined in dummydata/MyPetsModel.qml
    // The viewer automatically loads files in dummydata/* to assist
    // development without a real data source.
    // This one contains my pets.

    // Define a delegate component that includes a separator for sections.
    Component {
        id: PetDelegate
        Item {
            id: Wrapper
            width: 200
            // My height is the combined height of the description and the section separator
            height: Separator.height + Desc.height
            Rect {
                id: Separator
                color: "lightsteelblue"
                width: parent.width
                // Only show the section separator when we are the beginning of a new section
                // Note that for this to work nicely, the list must be ordered by section.
                height: Wrapper.ListView.prevSection != Wrapper.ListView.section ? 20 : 0
                opacity: Wrapper.ListView.prevSection != Wrapper.ListView.section ? 1 : 0
                Text {
                    text: Wrapper.ListView.section; font.bold: true
                    x: 2; height: parent.height; vAlign: 'AlignVCenter'
                }
            }
            Item {
                id: Desc
                x: 5
                height: Layout.height + 4
                anchors.top: Separator.bottom
                VerticalPositioner {
                    id: Layout
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
        id: PetHighlight
        Rect {
            color: "#FFFF88"
        }
    }
    // The list
    ListView {
        id: List
        width: 200
        height: parent.height
        model: MyPetsModel
        delegate: PetDelegate
        highlight: PetHighlight
        // The sectionExpression is simply the size of the pet.
        // We use this to determine which section we are in above.
        sectionExpression: "size"
        focus: true
    }
}
//! [0]

import Qt 4.7

//! [0]
Rectangle {
    width: 200
    height: 240

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
            height: desc.height // height is the combined height of the description and the section separator

            Item {
                id: desc
                x: 5; height: layout.height + 4

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
        Rectangle { color: "#FFFF88" }
    }

    // The list
    ListView {
        id: myList

        width: 200; height: parent.height
        model: MyPetsModel
        delegate: petDelegate
        highlight: petHighlight
        focus: true

        // The sectionExpression is simply the size of the pet.
        // We use this to determine which section we are in above.
        section.property: "size"
        section.criteria: ViewSection.FullString
        section.delegate: Rectangle {
            color: "lightsteelblue"
            width: 200
            height: 20
            Text {
                x: 2; height: parent.height
                verticalAlignment: 'AlignVCenter'
                text: section
                font.bold: true
            }
        }
    }
}
//! [0]

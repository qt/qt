Rect {
    width: 400; height: 300; color: "white"

    // MyPets model is defined in dummydata/MyPetsModel.qml
    // The viewer automatically loads files in dummydata/* to assist
    // development without a real data source.
    // This one contains my pets.
    // Define a delegate component.  A component will be
    // instantiated for each visible item in the list.
    Component {
        id: PetDelegate
        Item {
            id: Wrapper
            width: 200; height: 50
            VerticalLayout {
                Text { text: 'Name: ' + name }
                Text { text: 'Type: ' + type }
                Text { text: 'Age: ' + age }
            }
            // Use the ListView.isCurrentItem attached property to
            // indent the item if it is the current item.
            states: [
                State {
                    name: "Current"
                    when: Wrapper.ListView.isCurrentItem
                    SetProperties { target: Wrapper; x: 10 }
                }
            ]
            transitions: [
                Transition {
                    NumericAnimation {
                        properties: "x"; duration: 200
                    }
                }
            ]
        }
    }
    // Specify a highlight with custom movement.  Note that autoHighlight
    // is set to false in the ListView so that we can control how the
    // highlight moves to the current item.
    Component {
        id: PetHighlight
        Rect {
            width: 200; height: 50; color: "#FFFF88"
            y: Follow { source: List1.current.y; spring: 3; damping: 0.1 }
        }
    }
    ListView {
        id: List1
        width: 200; height: parent.height
        model: MyPetsModel; delegate: PetDelegate
        highlight: PetHighlight; autoHighlight: false
        focus: true
    }
}

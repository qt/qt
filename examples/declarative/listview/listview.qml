import Qt 4.6

Rect {
    width: 600; height: 300; color: "white"

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
        }
    }

    // Define a highlight component.  Just one of these will be instantiated
    // by each ListView and placed behind the current item.
    Component {
        id: PetHighlight
        Rect { color: "#FFFF88" }
    }

    // Show the model in three lists, with different currentItemPositioning.
    // currentItemPositioning determines how the list behaves when the
    // current item changes.  Note that the second and third ListView
    // set their currentIndex to be the same as the first, and that
    // the first ListView is given keyboard focus.
    // The default mode, Free, allows the currentItem to move freely
    // within the visible area.  If it would move outside the visible
    // area, the view is scrolled to keep it visible.
    // Snap currentItemPositioning attempts to keep the current item
    // aligned with the snapPosition by scrolling the view, however the
    // items will not scroll beyond the beginning or end of the view.
    // SnapAuto currentItemPositioning always keeps the current item on
    // the snapPosition by scrolling the view.  It also automatically
    // sets the current item as is scrolled with the mouse.  Note
    // that the first ListView sets its currentIndex to be equal to
    // the third ListView's currentIndex.  By flicking List3 with
    // the mouse, the current index of List1 will be changed.
    ListView {
        id: List1
        width: 200; height: parent.height
        model: MyPetsModel; delegate: PetDelegate
        highlight: PetHighlight; currentIndex: List3.currentIndex
        focus: true
    }
    ListView {
        id: List2
        x: 200; width: 200; height: parent.height
        model: MyPetsModel; delegate: PetDelegate; highlight: PetHighlight
        currentItemPositioning: "Snap"; snapPosition: 125
        currentIndex: List1.currentIndex
    }
    ListView {
        id: List3
        x: 400; width: 200; height: parent.height
        model: MyPetsModel; delegate: PetDelegate
        currentItemPositioning: "SnapAuto"; snapPosition: 125
        currentIndex: List1.currentIndex
        children: [
            // Position a static highlight rather than a normal highlight so that
            // when the view is flicked, the highlight does not move.
            // By positioning the highlight at the same position as the snapPosition
            // the item under the highlight will always be the current item.
            // Note that we specify the 'children' property.  This is because
            // the default property of a ListView is 'delegate'.
            Rect {
                y: 125; width: 200; height: 50
                color: "#FFFF88"; z: -1
            }
        ]
    }
}

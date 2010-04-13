import Qt 4.7

Rectangle {
    width: 600; height: 300

    // MyPets model is defined in dummydata/MyPetsModel.qml
    // The viewer automatically loads files in dummydata/* to assist
    // development without a real data source.
    // This one contains my pets.

    // Define a delegate component.  A component will be
    // instantiated for each visible item in the list.
    Component {
        id: petDelegate
        Item {
            width: 200; height: 50
            Column {
                Text { text: 'Name: ' + name }
                Text { text: 'Type: ' + type }
                Text { text: 'Age: ' + age }
            }
        }
    }

    // Define a highlight component.  Just one of these will be instantiated
    // by each ListView and placed behind the current item.
    Component {
        id: petHighlight
        Rectangle { color: "#FFFF88" }
    }

    // Show the model in three lists, with different highlight ranges.
    // preferredHighlightBegin and preferredHighlightEnd set the
    // range in which to attempt to maintain the highlight.
    //
    // Note that the second and third ListView
    // set their currentIndex to be the same as the first, and that
    // the first ListView is given keyboard focus.
    //
    // The default mode allows the currentItem to move freely
    // within the visible area.  If it would move outside the visible
    // area, the view is scrolled to keep it visible.
    //
    // The second list sets a highlight range which attempts to keep the
    // current item within the the bounds of the range, however
    // items will not scroll beyond the beginning or end of the view,
    // forcing the highlight to move outside the range at the ends.
    //
    // The third list sets the highlightRangeMode to StrictlyEnforceRange
    // and sets a range smaller than the height of an item.  This
    // forces the current item to change when the view is flicked,
    // since the highlight is unable to move.
    //
    // Note that the first ListView sets its currentIndex to be equal to
    // the third ListView's currentIndex.  By flicking List3 with
    // the mouse, the current index of List1 will be changed.

    ListView {
        id: list1
        width: 200; height: parent.height
        model: MyPetsModel
        delegate: petDelegate

        highlight: petHighlight
        currentIndex: list3.currentIndex
        focus: true
    }

    ListView {
        id: list2
        x: 200; width: 200; height: parent.height
        model: MyPetsModel
        delegate: petDelegate

        highlight: petHighlight
        currentIndex: list1.currentIndex
        preferredHighlightBegin: 80; preferredHighlightEnd: 220
        highlightRangeMode: "ApplyRange"
    }

    ListView {
        id: list3
        x: 400; width: 200; height: parent.height
        model: MyPetsModel
        delegate: petDelegate

        highlight: Rectangle { color: "lightsteelblue" }
        currentIndex: list1.currentIndex
        preferredHighlightBegin: 125; preferredHighlightEnd: 125
        highlightRangeMode: "StrictlyEnforceRange"
        flickDeceleration: 1000
    }
}

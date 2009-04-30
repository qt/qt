Rect {
    id: page
    width: layout.width
    height: layout.height
    color: "white"
    Bind {
        id: currentItem
        value: true
    }
    // relies on the current focus behavior whereby setting focus=true on a
    // component removes focus from any previous element
    GridLayout {
        id: layout
        width: contents.width
        height: contents.height
        GroupBox {
            contents: "1/ContactView.qml"
            label: "something"
        }
        GroupBox {
            contents: "2/ContactView.qml"
            label: "something"
        }
        GroupBox {
            contents: "3/ContactView.qml"
            label: "something"
        }
    }
}

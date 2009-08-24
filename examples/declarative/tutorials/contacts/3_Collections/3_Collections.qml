import Qt 4.6

Rectangle {
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
    Grid {
        id: layout
        width: childrenRect.width
        height: childrenRect.height
        GroupBox {
            contents: "1/ContactView.qml"
            label: "list only"
        }
        GroupBox {
            contents: "2/ContactView.qml"
            label: "dynamic delegate"
        }
        GroupBox {
            contents: "3/ContactView.qml"
            label: "delayed loading"
        }
    }
}

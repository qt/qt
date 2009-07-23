import Qt 4.6

Rect {
    id: page
    width: layout.width
    height: layout.height
    color: "white"
    VerticalLayout {
        id: layout
        width: contents.width
        GroupBox {
            contents: "1/RemoveButton.qml"
            label: "Rectangle Component"
        }
        GroupBox {
            contents: "2/RemoveButton.qml"
            label: "Closed Remove Item Button"
        }
        GroupBox {
            contents: "2a/RemoveButton.qml"
            label: "Alternative Closed Button"
        }
        GroupBox {
            contents: "3/RemoveButton.qml"
            label: "Open Remove Item Button"
        }
        GroupBox {
            contents: "4/RemoveButton.qml"
            label: "State Based Button"
        }
        GroupBox {
            contents: "5/RemoveButton.qml"
            label: "Animated Button"
        }
    }
}

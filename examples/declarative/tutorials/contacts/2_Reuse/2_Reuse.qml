import Qt 4.6

Rect {
    id: page
    width: layout.width
    height: layout.height
    color: "white"
    GridLayout {
        id: layout
        columns: 2
        rows: 4
        width: contents.width
        GroupBox {
            contents: "1/ContactField.qml"
            label: "Loading: simple"
        }
        GroupBox {
            contents: "1a/ContactField.qml"
            label: "Loading: qml property"
        }
        GroupBox {
            contents: "2/ContactField.qml"
            label: "Using properties"
        }
        GroupBox {
            id: prev
            contents: "3/ContactField.qml"
            label: "Defining signals"
        }
        GroupBox {
            contents: "3/Contact.qml"
            label: "Multiple Items"
        }
        GroupBox {
            contents: "4/Contact.qml"
            label: "Mouse Grabbing"
        }
    }
}

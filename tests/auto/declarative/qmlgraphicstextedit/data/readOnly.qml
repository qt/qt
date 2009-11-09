import Qt 4.6

Rectangle {
    property var myInput: Input

    width: 800; height: 600; color: "blue"

    TextEdit { id: Input; focus: true
        readOnly: true
        text: "I am the very model of a modern major general.\n"
    }
}

import Qt 4.7

Rectangle {
    width: 300
    height: 300

    Rectangle {
        height: childrenRect.height

        Repeater {
            model: 1
            Rectangle {	}
        }
    }
}

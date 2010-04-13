import Qt 4.6

Rectangle {
    Flipable {
        id: flipable
    }
    Rectangle {
        visible: flipable.side == Flipable.Front
    }
}

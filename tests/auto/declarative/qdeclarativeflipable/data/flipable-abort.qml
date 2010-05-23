import Qt 4.7

Rectangle {
    Flipable {
        id: flipable
    }
    Rectangle {
        visible: flipable.side == Flipable.Front
    }
}

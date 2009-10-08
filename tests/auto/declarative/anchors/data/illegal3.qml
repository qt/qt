import Qt 4.6

Rectangle {
    id: rect
    width: 120; height: 200; color: "white"
    Item {
        Rectangle { id: TheRect; width: 100; height: 100 }
    }
    Rectangle {
        anchors.left: TheRect.left
    }
}

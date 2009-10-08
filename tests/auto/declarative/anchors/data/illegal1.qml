import Qt 4.6

Rectangle {
    id: rect
    width: 120; height: 200; color: "white"
    Rectangle { id: TheRect; width: 100; height: 100 }
    Rectangle {
        anchors.left: TheRect.left
        anchors.right: TheRect.right
        anchors.horizontalCenter: TheRect.horizontalCenter
    }
}

import Qt 4.6

Rect {
    id: rect
    width: 120; height: 200; color: "white"
    Rect { id: TheRect; width: 100; height: 100 }
    Rect {
        anchors.left: TheRect.left
        anchors.right: TheRect.right
        anchors.horizontalCenter: TheRect.horizontalCenter
    }
}

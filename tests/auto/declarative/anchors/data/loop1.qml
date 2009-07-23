import Qt 4.6

Rect {
    id: rect
    width: 120; height: 200; color: "white"
    Text { id: Text1; anchors.right: Text2.right; text: "Hello" }
    Text { id: Text2; anchors.right: Text1.right; anchors.rightMargin: 10; text: "World" }
}

import Qt 4.6

Rect {
    id: rect
    width: 120; height: 200; color: "white"
    Text { id: Text1; text: "Hello" }
    Text {
        id: Text2;
        anchors.baseline: Text1.baseline;
        anchors.top: Text1.top;
        text: "World"
    }
}

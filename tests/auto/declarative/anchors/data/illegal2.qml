import Qt 4.6

Rectangle {
    id: rect
    width: 120; height: 200; color: "white"
    Text { id: text1; text: "Hello" }
    Text {
        id: text2;
        anchors.baseline: text1.baseline;
        anchors.top: text1.top;
        text: "World"
    }
}

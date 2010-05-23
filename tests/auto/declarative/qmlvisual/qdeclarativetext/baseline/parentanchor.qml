import Qt 4.7

Rectangle {
    id: s; width: 600; height: 100; color: "lightsteelblue"
    property string text: "The quick brown fox jumps over the lazy dog."
    Text {
        text: s.text
        anchors.verticalCenter: s.verticalCenter
    }
    Text {
        text: s.text
        anchors.baseline: s.verticalCenter
    }
}

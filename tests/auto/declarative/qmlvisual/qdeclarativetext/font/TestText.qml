import QtQuick 1.0

Text {
    id: testText

    property color bcolor: "blue"

    text: "The quick brown fox\njumps over\nthe lazy dog."
    font.family: "Helvetica"
    font.pointSize: 16

    Rectangle { id: borderr; color: "transparent"; border.color: bcolor; anchors.fill: parent; opacity: 0.2 }
}

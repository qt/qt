import Qt 4.7

Item {

//! [0]
Rectangle {
    anchors.left: parent.left; anchors.top: parent.top; anchors.right: parent.right; anchors.leftMargin: 20
}

Text {
    text: "hello"
    font.bold: true; font.italic: true; font.pixelSize: 20; font.capitalization: Font.AllUppercase
}

//! [0]

//! [1]
Rectangle {
    anchors { left: parent.left; top: parent.top; right: parent.right; leftMargin: 20 }
}

Text {
    text: "hello"
    font { bold: true; italic: true; pixelSize: 20; capitalization: Font.AllUppercase }
}
//! [1]

}

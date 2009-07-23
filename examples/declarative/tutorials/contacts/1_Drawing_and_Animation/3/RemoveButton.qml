import Qt 4.6

//! [0]
Rect {
    id: removeButton
    width: 230
    height: 30
    color: "red"
    radius: 5
    Image {
        id: cancelIcon
        width: 22
        height: 22
        anchors.right: parent.right
        anchors.rightMargin: 4
        anchors.verticalCenter: parent.verticalCenter
        source: "../../shared/pics/cancel.png"
    }
    Image {
        id: confirmIcon
        width: 22
        height: 22
        anchors.left: parent.left
        anchors.leftMargin: 4
        anchors.verticalCenter: parent.verticalCenter
        source: "../../shared/pics/ok.png"
    }
    Text {
        id: text
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: confirmIcon.right
        anchors.leftMargin: 4
        anchors.right: cancelIcon.left
        anchors.rightMargin: 4
        font.bold: true
        color: "white"
        hAlign: "AlignHCenter"
        text: "Remove"
    }
}
//! [0]

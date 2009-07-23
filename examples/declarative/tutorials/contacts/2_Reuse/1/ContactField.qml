import Qt 4.6

//! [load]
Item {
    id: contactField
    clip: true
    width: 230
    height: 30
    RemoveButton {
        id: removeButton
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
    }
//! [load]
    Text {
        id: fieldText
        width: contactField.width-80
        anchors.right: removeButton.left
        anchors.rightMargin: 10
        anchors.verticalCenter: parent.verticalCenter
        font.bold: true
        color: "black"
        text: 123123
    }
    Image {
        source: "../../shared/pics/phone.png"
        anchors.right: fieldText.left
        anchors.rightMargin: 10
        anchors.verticalCenter: parent.verticalCenter
    }
}

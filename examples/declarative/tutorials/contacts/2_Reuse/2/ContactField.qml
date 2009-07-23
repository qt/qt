import Qt 4.6

Item {
    id: contactField
    clip: true
    width: 230
    height: 30
//! [use properties and signals]
    RemoveButton {
        id: removeButton
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        expandedWidth: contactField.width
        onConfirmed: { fieldText.text='' }
    }
//! [use properties and signals]
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

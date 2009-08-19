import Qt 4.6

//! [0]
Rectangle {
    id: removeButton
    width: 30
    height: 30
    color: "red"
    radius: 5
    Image {
        id: trashIcon
        width: 22
        height: 22
        anchors.right: parent.right
        anchors.rightMargin: 4
        anchors.verticalCenter: parent.verticalCenter
        source: "../../shared/pics/trash.png"
    }
}
//! [0]

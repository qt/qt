import Qt 4.6

Rect {
    id: searchBar
    color: "white"
    property var text: searchEdit.text
    Image {
        id: searchIcon
        anchors.left: parent.left
        anchors.leftMargin: 5
        anchors.verticalCenter: parent.verticalCenter
        source: "pics/search.png"
    }
    TextEdit {
        id: searchEdit
        anchors.left: searchIcon.right
        anchors.right: parent.right
        anchors.leftMargin: 5
        anchors.rightMargin: 5
        anchors.verticalCenter: parent.verticalCenter
        readOnly: false
        wrap: false
        focus: true
        text: ""
    }
}

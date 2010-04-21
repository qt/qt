import Qt 4.7

FocusScope {
    id: container

    property bool open: false

    Item {
        anchors.fill: parent

        Rectangle {
            anchors.fill: parent
            color: "#D1DBBD"
            focus: true
            Keys.onRightPressed: mainView.focus = true
        }
    }
}

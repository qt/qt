import Qt 4.7

Item {
    property alias image: icon.source
    property variant action

    width: 40; height: parent.height

    Image {
        id: icon; anchors.centerIn: parent
        opacity: action.enabled ? 1.0 : 0.4
    }

    MouseArea {
        anchors { fill: parent; topMargin: -10; bottomMargin: -10 }
        onClicked: action.trigger()
    }
}

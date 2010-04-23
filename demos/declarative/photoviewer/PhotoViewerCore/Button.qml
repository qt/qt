import Qt 4.7

Item {
    id: container

    property alias label: labelText.text
    property color tint: "#FFFFFFFF"
    signal clicked

    width: labelText.width + 70 ; height: labelText.height + 18

    BorderImage {
        anchors { fill: container; leftMargin: -6; topMargin: -6; rightMargin: -8; bottomMargin: -8 }
        source: 'images/box-shadow.png'; smooth: true
        border.left: 10; border.top: 10; border.right: 10; border.bottom: 10
    }

    Image { anchors.fill: parent; source: "images/cardboard.png"; smooth: true }

    Rectangle {
        anchors.fill: container; color: container.tint; visible: container.tint != ""
        opacity: 0.1; smooth: true
    }

    Text { id: labelText; font.pixelSize: 15; anchors.centerIn: parent; smooth: true }

    MouseArea {
        anchors { fill: parent; leftMargin: -20; topMargin: -20; rightMargin: -20; bottomMargin: -20 }
        onClicked: container.clicked()
    }
}

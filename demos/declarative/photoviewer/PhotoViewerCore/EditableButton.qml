import Qt 4.7

Item {
    id: container

    property string label
    signal clicked
    signal labelChanged(string label)

    width: textInput.width + 70 ; height: textInput.height + 18

    BorderImage {
        anchors { fill: container; leftMargin: -6; topMargin: -6; rightMargin: -8; bottomMargin: -8 }
        source: 'images/box-shadow.png'; smooth: true
        border.left: 10; border.top: 10; border.right: 10; border.bottom: 10
    }

    Image { anchors.fill: parent; source: "images/cardboard.png"; smooth: true }

    TextInput {
        id: textInput; text: label; font.pixelSize: 15; anchors.centerIn: parent; smooth: true
        Keys.onReturnPressed: {
            container.labelChanged(textInput.text)
            container.focus = true
        }
        Keys.onEscapePressed: {
            textInput.text = container.label
            container.focus = true
        }
    }

    Rectangle {
        anchors.fill: container; border.color: "steelblue"; border.width: 4
        color: "transparent"; visible: textInput.focus; smooth: true
    }

    MouseArea {
        anchors { fill: parent; leftMargin: -20; topMargin: -20; rightMargin: -20; bottomMargin: -20 }
        onClicked: textInput.forceFocus()
    }
}

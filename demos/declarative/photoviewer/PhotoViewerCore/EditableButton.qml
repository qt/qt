import Qt 4.7

Item {
    id: container

    property string label
    property string tint: ""
    signal clicked
    signal labelChanged(string label)

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

    Text { id: labelText; text: label; font.pixelSize: 15; anchors.centerIn: parent; smooth: true }

    TextInput {
        id: textInput; text: label; font.pixelSize: 15; anchors.centerIn: parent; smooth: true; visible: false
        Keys.onReturnPressed: container.labelChanged(textInput.text)
        Keys.onEscapePressed: {
            textInput.text = labelText.text
            container.state = ''
        }
    }

    MouseArea {
        anchors { fill: parent; leftMargin: -20; topMargin: -20; rightMargin: -20; bottomMargin: -20 }
        onClicked: container.state = "editMode"
    }

    states: State {
        name: "editMode"
        PropertyChanges { target: container; width: textInput.width + 70; height: textInput.height + 17 }
        PropertyChanges { target: textInput; visible: true; focus: true }
        PropertyChanges { target: labelText; visible: false }
    }

    onLabelChanged: {
        labelText.text = label
        container.state = ''
    }
}

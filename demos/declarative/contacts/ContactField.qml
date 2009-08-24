import Qt 4.6

Item {
    id: contactField
    clip: true
    height: 30
    property var label: "Name"
    property var icon: ""
    property var value: ""
    onValueChanged: { fieldText.text = contactField.value }
    RemoveButton {
        id: removeButton
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        expandedWidth: contactField.width
        onConfirmed: { print('Clear field text'); fieldText.text='' }
    }
    FieldText {
        id: fieldText
        width: contactField.width-70
        anchors.right: removeButton.left
        anchors.rightMargin: 5
        anchors.verticalCenter: parent.verticalCenter
        label: contactField.label
        text: contactField.value
        onConfirmed: { contactField.value=fieldText.text }
    }
    Image {
        anchors.right: fieldText.left
        anchors.rightMargin: 5
        anchors.verticalCenter: parent.verticalCenter
        source: contactField.icon
    }
    states: [
        State {
            name: "editingText"
            when: fieldText.state == 'editing'
            PropertyChanges {
                target: removeButton.anchors
                rightMargin: -35
            }
            PropertyChanges {
                target: fieldText
                width: contactField.width
            }
        }
    ]
    transitions: [
        Transition {
            from: ""
            to: "*"
            reversible: true
            NumberAnimation {
                properties: "width,rightMargin"
                duration: 200
            }
        }
    ]
}

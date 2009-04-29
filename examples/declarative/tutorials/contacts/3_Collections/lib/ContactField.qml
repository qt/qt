Item {
    id: contactField
    clip: true
    height: 30
    properties: Property {
        name: "label"
    }
    properties: Property {
        name: "icon"
    }
    properties: Property {
        name: "value"
    }
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
    }
    Image {
        anchors.right: fieldText.left
        anchors.rightMargin: 5
        anchors.verticalCenter: parent.verticalCenter
        src: contactField.icon
    }
    states: [
        State {
            name: "editingText"
            when: fieldText.state == 'editing'
            SetProperty {
                target: removeButton.anchors
                property: "rightMargin"
                value: -35
            }
            SetProperty {
                target: fieldText
                property: "width"
                value: contactField.width
            }
        }
    ]
    transitions: [
        Transition {
            fromState: ""
            toState: "*"
            reversible: true
            NumericAnimation {
                properties: "width,rightMargin"
                duration: 200
            }
        }
    ]
}

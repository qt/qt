Item {
    id: contactDetails
    width: 230
    height: layout.height

    properties: Property {
        name: "contactid"
        value: ""
    }
    properties: Property {
        name: "label"
        onValueChanged: { labelField.value = label }
    }
    properties: Property {
        name: "phone"
        onValueChanged: { phoneField.value = phone }
    }
    properties: Property {
        name: "email"
        onValueChanged: { emailField.value = email }
    }
    VerticalLayout {
        id: layout
        anchors.fill: parent
        spacing: 5
        margin: 5
        ContactField {
            id: labelField
            anchors.left: layout.left
            anchors.leftMargin: 5
            anchors.right: layout.right
            anchors.rightMargin: 5
            label: "Name"
        }
        ContactField {
            id: phoneField
            anchors.left: layout.left
            anchors.leftMargin: 5
            anchors.right: layout.right
            anchors.rightMargin: 5
            icon: "../../shared/pics/phone.png"
            label: "Phone"
        }
        ContactField {
            id: emailField
            anchors.left: layout.left
            anchors.leftMargin: 5
            anchors.right: layout.right
            anchors.rightMargin: 5
            icon: "../../shared/pics/email.png"
            label: "Email"
        }
    }
}

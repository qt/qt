import Qt 4.6

//! [grab property]
Item {
    id: contactDetails
    width: 230
    height: layout.height
    property var mouseGrabbed: false
//! [grab property]

    property var contactId: ""
    property var label: ""
    property var phone: ""
    property var email: ""

    onLabelChanged: { labelField.value = label }
    onEmailChanged: { emailField.value = email }
    onPhoneChanged: { phoneField.value = phone }

    VerticalPositioner {
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

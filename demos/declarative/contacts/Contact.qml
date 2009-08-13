import Qt 4.6

Item {
    id: contactDetails
    anchors.fill: parent

    property var contactId: ""
    property var label: ""
    property var phone: ""
    property var email: ""

    onLabelChanged: { labelField.value = label }
    onEmailChanged: { emailField.value = email }
    onPhoneChanged: { phoneField.value = phone }

    resources: [
        SqlQuery {
            id: updateContactQuery
            connection: contactDatabase
            query: "UPDATE contacts SET label = :l, email = :e, phone = :p WHERE recid = :r"
            bindings: [
                SqlBind {
                    name: ":r"
                    value: contactId
                },
                SqlBind {
                    name: ":l"
                    value: labelField.value
                },
                SqlBind {
                    name: ":e"
                    value: emailField.value
                },
                SqlBind {
                    name: ":p"
                    value: phoneField.value
                }
            ]
        },
        SqlQuery {
            id: insertContactQuery
            connection: contactDatabase
            query: "INSERT INTO contacts (label, email, phone) VALUES(:l, :e, :p)"
            bindings: [
                SqlBind {
                    name: ":l"
                    value: labelField.value
                },
                SqlBind {
                    name: ":e"
                    value: emailField.value
                },
                SqlBind {
                    name: ":p"
                    value: phoneField.value
                }
            ]
        },
        SqlQuery {
            id: removeContactQuery
            connection: contactDatabase
            query: "DELETE FROM contacts WHERE recid = :r"
            bindings: SqlBind {
                name: ":r"
                value: contactId
            }
        }
    ]
    function refresh() {
        labelField.value = label;
        emailField.value = email;
        phoneField.value = phone;
    }
    function update() {
        updateContactQuery.exec();
    }
    function insert() {
        insertContactQuery.exec();
    }
    function remove() {
        removeContactQuery.exec();
    }
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
            icon: "pics/phone.png"
            label: "Phone"
        }
        ContactField {
            id: emailField
            anchors.left: layout.left
            anchors.leftMargin: 5
            anchors.right: layout.right
            anchors.rightMargin: 5
            icon: "pics/email.png"
            label: "Email"
        }
    }
}

import "../lib"
Item {
    id: contacts
    width: 240
    height: 230
    property var mouseGrabbed: false
    resources: [
        SqlConnection {
            id: contactDatabase
            name: "qmlConnection"
            driver: "QSQLITE"
            databaseName: "../../shared/contacts.sqlite"
        },
        SqlQuery {
            id: contactList
            connection: contactDatabase
            query: "SELECT recid, label, email, phone FROM contacts ORDER BY label, recid"
        }
    ]
    Button {
        id: cancelEditButton
        anchors.top: parent.top
        anchors.topMargin: 5
        anchors.right: parent.right
        anchors.rightMargin: 5
        icon: "../../shared/pics/cancel.png"
        opacity: mouseGrabbed ? 0 : 1
    }
    ListView {
        id: contactListView
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: cancelEditButton.bottom
        anchors.bottom: parent.bottom
        clip: true
        model: contactList
        focus: true
        delegate: [
            Item {
                id: wrapper
                x: 0
                width: ListView.view.width
                height: 34
                Text {
                    id: label
                    x: 45
                    y: 12
                    width: parent.width-45
                    color: "black"
                    font.bold: true
                    text: model.label
                }
//! [setting qml]
                MouseRegion {
                    anchors.fill: label
                    onClicked: {
                        Details.qml = 'Contact.qml';
                        wrapper.state='opened';
                    }
                }
                Item {
                    id: Details
                    anchors.fill: parent
                    opacity: 0
//! [setting qml]
//! [binding]
                    Bind {
                        target: Details.qmlItem
                        property: "contactId"
                        value: model.recid
                    }
                    Bind {
                        target: Details.qmlItem
                        property: "label"
                        value: model.label
                    }
                    Bind {
                        target: Details.qmlItem
                        property: "phone"
                        value: model.phone
                    }
                    Bind {
                        target: Details.qmlItem
                        property: "email"
                        value: model.email
                    }
//! [binding]
                }
                states: [
                    State {
                        name: "opened"
                        SetProperties {
                            target: wrapper
                            height: contactListView.height
                        }
                        SetProperties {
                            target: contactListView
                            explicit: true
                            yPosition: wrapper.y
                        }
                        SetProperties {
                            target: contactListView
                            locked: 1
                        }
                        SetProperties {
                            target: label
                            opacity: 0
                        }
                        SetProperties {
                            target: Details
                            opacity: 1
                        }
                    }
                ]
                transitions: [
                    Transition {
                        NumberAnimation {
                            duration: 500
                            properties: "yPosition,height,opacity"
                        }
                    }
                ]
                Connection {
                    sender: cancelEditButton
                    signal: "clicked()"
                    script: {
                        if (wrapper.state == 'opened') {
                            wrapper.state = '';
                        }
                    }
                }
            }
        ]
    }
}

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
//! [button]
    Button {
        id: cancelEditButton
        anchors.top: parent.top
        anchors.topMargin: 5
        anchors.right: parent.right
        anchors.rightMargin: 5
        icon: "../../shared/pics/cancel.png"
        opacity: mouseGrabbed ? 0 : 1
    }
//! [button]
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
//! [components]
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
                MouseRegion {
                    anchors.fill: label
                    onClicked: { wrapper.state='opened'; }
                }
                Contact {
                    id: Details
                    anchors.fill: parent
                    contactId: model.recid
                    label: model.label
                    email: model.email
                    phone: model.phone
                    opacity: 0
                }
//! [components]
//! [states]
                states: [
                    State {
                        name: "opened"
                        SetProperty {
                            target: wrapper
                            property: "height"
                            value: contactListView.height
                        }
                        SetProperty {
                            target: contactListView
                            property: "yPosition"
                            value: wrapper.y
                        }
                        SetProperty {
                            target: contactListView
                            property: "locked"
                            value: 1
                        }
                        SetProperty {
                            target: label
                            property: "opacity"
                            value: 0
                        }
                        SetProperty {
                            target: Details
                            property: "opacity"
                            value: 1
                        }
                    }
                ]
//! [states]
//! [transitions]
                transitions: [
                    Transition {
                        NumberAnimation {
                            duration: 500
                            properties: "yPosition,height,opacity"
                        }
                    }
                ]
//! [transitions]
//! [connections]
                Connection {
                    sender: cancelEditButton
                    signal: "clicked()"
                    script: {
                        if (wrapper.state == 'opened') {
                            wrapper.state = '';
                        }
                    }
                }
//! [connections]
            }
        ]
    }
}

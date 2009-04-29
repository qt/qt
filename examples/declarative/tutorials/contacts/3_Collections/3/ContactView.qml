import "../lib"
Item {
    id: contacts
    width: 240
    height: 230
    properties: Property {
        name: "mouseGrabbed"
        value: false
    }
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
            query: "SELECT recid AS contactid, label, email, phone FROM contacts ORDER BY label, recid"
        },
        Component {
            id: contactDelegate
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
                    text: model.label
                    color: "black"
                    font.bold: true
                }
                MouseRegion {
                    anchors.fill: label
                    onClicked: { Details.qml = 'Contact.qml';
                        wrapper.state='opened'; }
                }
                Item {
                    id: Details
                    anchors.fill: wrapper
                    opacity: 0
                    Bind {
                        target: Details.qmlItem
                        property: "contactid"
                        value: model.contactid
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
                }
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
                transitions: [
                    Transition {
                        NumericAnimation {
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
        },
        Button {
            id: cancelEditButton
            anchors.top: parent.top
            anchors.topMargin: 5
            anchors.right: parent.right
            anchors.rightMargin: 5
            icon: "../../shared/pics/cancel.png"
            opacity: mouseGrabbed ? 0 : 1
        },
        ListView {
            id: contactListView
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: cancelEditButton.bottom
            anchors.bottom: parent.bottom
            clip: true
            model: contactList
            delegate: contactDelegate
            focus: true
        }
    ]
}

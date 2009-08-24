import Qt 4.6

Rectangle {
    id: contacts
    width: 240
    height: 320
    color: "black"
    property var mode: "list"
    property var mouseGrabbed: false
    resources: [
        SqlConnection {
            id: contactDatabase
            name: "qmlConnection"
            driver: "QSQLITE"
            databaseName: "contacts.sqlite"
        },
        SqlQuery {
            id: contactList
            connection: contactDatabase
            query: "SELECT recid, label, email, phone FROM contacts WHERE lower(label) LIKE lower(:searchTerm) ORDER BY label, recid"
            bindings: SqlBind {
                name: ":searchTerm"
                value: '%' + searchBar.text + '%' 
            }
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
                    x: 40
                    y: 12
                    width: parent.width-30
                    text: model.label
                    color: "white"
                    font.bold: true
                    children: [
                        MouseRegion {
                            anchors.fill: parent
                            onClicked: {
                                Details.source = 'Contact.qml';
                                wrapper.state ='opened';
                                contacts.mode = 'edit';
                            }
                        }
                    ]
                    states: [
                        State {
                            name: "currentItem"
                            when: wrapper.ListView.isCurrentItem
                            PropertyChanges {
                                target: label
                                color: "black"
                            }
                        }
                    ]
                    transitions: [
                        Transition {
                            ColorAnimation {
                                duration: 250
                                property: "color"
                            }
                        }
                    ]
                }
                Loader {
                    id: Details
                    anchors.fill: wrapper
                    opacity: 0
                    Binding {
                        target: Details.item
                        property: "contactId"
                        value: model.recid
                    }
                    Binding {
                        target: Details.item
                        property: "label"
                        value: model.label
                    }
                    Binding {
                        target: Details.item
                        property: "phone"
                        value: model.phone
                    }
                    Binding {
                        target: Details.item
                        property: "email"
                        value: model.email
                    }
                }
                states: [
                    State {
                        name: "opened"
                        PropertyChanges {
                            target: wrapper
                            height: contactListView.height
                        }
                        PropertyChanges {
                            target: contactListView
                            yPosition: wrapper.y
                        }
                        PropertyChanges {
                            target: contactListView
                            locked: 1
                        }
                        PropertyChanges {
                            target: label
                            opacity: 0
                        }
                        PropertyChanges {
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
                    sender: confirmEditButton
                    signal: "clicked()"
                    script: {
                    if (wrapper.state == 'opened' && !contacts.mouseGrabbed) {
                            Details.item.update();
                            wrapper.state = '';
                            contacts.mode = 'list';
                            contactList.exec();
                        }
                                    
                    }
                }
                Connection {
                    sender: cancelEditButton
                    signal: "clicked()"
                    script: {
                        if (wrapper.state == 'opened' && !contacts.mouseGrabbed) {
                            wrapper.state = '';
                            contacts.mode = 'list';
                        }
                                    
                    }
                }
                Connection {
                    sender: removeContactButton
                    signal: "confirmed()"
                    script: {
                        if (wrapper.state == 'opened' && !contacts.mouseGrabbed) {
                            Details.item.remove();
                            wrapper.state = '';
                            contacts.mode = 'list';
                            contactList.exec();
                        }
                                    
                    }
                }
            }
        }
    ]
    Button {
        id: newContactButton
        anchors.top: parent.top
        anchors.topMargin: 5
        anchors.right: parent.right
        anchors.rightMargin: 5
        icon: "pics/new.png"
        onClicked: { newContactItem.refresh(); contacts.mode = 'new' }
        opacity: contacts.mode == 'list' ? 1 : 0
    }
    Button {
        id: confirmEditButton
        anchors.top: parent.top
        anchors.topMargin: 5
        anchors.left: parent.left
        anchors.leftMargin: 5
        icon: "pics/ok.png"
        opacity: contacts.mode == 'list' || contacts.mouseGrabbed ? 0 : 1
    }
    Button {
        id: cancelEditButton
        anchors.top: parent.top
        anchors.topMargin: 5
        anchors.right: parent.right
        anchors.rightMargin: 5
        icon: "pics/cancel.png"
        opacity: contacts.mode == 'list' || contacts.mouseGrabbed ? 0 : 1
    }
    RemoveButton {
        id: removeContactButton
        anchors.top: parent.top
        anchors.topMargin: 5
        anchors.horizontalCenter: parent.horizontalCenter
        opacity: (contacts.mode == 'edit' && (!contacts.mouseGrabbed || removeContactButton.state =='opened')) ? 1 : 0
    }
    ListView {
        id: contactListView
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: cancelEditButton.bottom
        anchors.bottom: searchBarWrapper.top
        anchors.topMargin: 5
        clip: true
        model: contactList
        delegate: contactDelegate
        highlight: [
            Rectangle {
                id: contactHighlight
                border.width: 0
                color: 'white'
                opacity: contacts.mode == 'list' ? 1 : 0
                opacity: Behavior {
                    NumberAnimation {
                        property: "opacity"
                        duration: 250
                    }
                }
            }
        ]
        autoHighlight: true
        focus: false
    }
    FocusScope {
        id: newContactWrapper
        anchors.fill: contactListView
        opacity: 0
        focus: contacts.mode == 'new'
        Contact {
            id: newContactItem
            anchors.fill: parent
        }
    }
    Connection {
        sender: confirmEditButton
        signal: "clicked()"
        script: {
            if (contacts.mode == 'new' && contacts.mouseGrabbed != 'true') {
                newContactItem.insert();
                contacts.mode = 'list';
                contactList.exec();
            }
        }
    }
    Connection {
        sender: cancelEditButton
        signal: "clicked()"
        script: {
            if (contacts.mode == 'new' && contacts.mouseGrabbed != 'true') {
                contacts.mode = 'list';
            }
        }
    }
    FocusScope {
        id: searchBarWrapper
        height: 30
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottomMargin: 0
        focus: false
        SearchBar {
            id: searchBar
            anchors.fill: parent
        }
        states: [
            State {
                name: "searchHidden"
                when: searchBar.text == '' || contacts.mode != 'list'
                PropertyChanges {
                    target: searchBarWrapper.anchors
                    bottomMargin: -30
                }
            }
        ]
        transitions: [
            Transition {
                from: "*"
                to: "*"
                NumberAnimation {
                    property: "bottomMargin"
                    duration: 250
                }
            }
        ]
    }
    KeyProxy {
        focus: contacts.mode != 'new'
        targets: { contacts.mode == "list" ? [searchBarWrapper, contactListView] : [contactListView]}
    }
    states: [
        State {
            name: "editNewState"
            when: contacts.mode == 'new'
            PropertyChanges {
                target: contactListView
                opacity: 0
            }
            PropertyChanges {
                target: newContactWrapper
                opacity: 1
            }
        }
    ]
    transitions: [
        Transition {
            from: "*"
            to: "*"
            NumberAnimation {
                property: "opacity"
                duration: 500
            }
        }
    ]
}

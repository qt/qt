Rect {
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
                                Details.qml = 'Contact.qml';
                                wrapper.state='opened';
                                contacts.mode = 'edit';
                            }
                        }
                    ]
                }
                Item {
                    id: Details
                    anchors.fill: wrapper
                    opacity: 0
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
                    sender: confirmEditButton
                    signal: "clicked()"
                    script: {
                    if (wrapper.state == 'opened' && !contacts.mouseGrabbed) {
                            Details.qmlItem.update();
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
    ListView {
        id: contactListView
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: cancelEditButton.bottom
        anchors.bottom: searchBarWrapper.bottom
        clip: true
        model: contactList
        delegate: contactDelegate
        focus: false
    }
    FocusRealm {
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
    FocusRealm {
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
                SetProperty {
                    target: searchBarWrapper.anchors
                    property: "bottomMargin"
                    value: -30
                }
            }
        ]
        transitions: [
            Transition {
                fromState: "*"
                toState: "*"
                NumericAnimation {
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
            SetProperty {
                target: contactListView
                property: "opacity"
                value: 0
            }
            SetProperty {
                target: newContactWrapper
                property: "opacity"
                value: 1
            }
        }
    ]
    transitions: [
        Transition {
            fromState: "*"
            toState: "*"
            NumericAnimation {
                property: "opacity"
                duration: 500
            }
        }
    ]
}

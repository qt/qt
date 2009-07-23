import Qt 4.6

import "../lib"
Item {
    id: contacts
    width: 240
    height: 230
    property var mouseGrabbed: false
    resources: [
//! [model]
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
//! [model]
    ]
//! [view]
    ListView {
        id: contactListView
        anchors.fill: parent
        clip: true
        model: contactList
        focus: true
//! [delegate]
        delegate: [
            Text {
                x: 45
                y: 12
                width: contactListView.width-45
                height: 30
                color: "black"
                font.bold: true
                text: model.label
            }
        ]
//! [delegate]
    }
//! [view]
}

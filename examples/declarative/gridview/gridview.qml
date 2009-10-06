import Qt 4.6

Rectangle {
    width: 300; height: 400; color: "white"

    ListModel {
        id: AppModel
        ListElement {
            name: "Music"
            icon: "AudioPlayer_48.png"
        }
        ListElement {
            name: "Movies"
            icon: "VideoPlayer_48.png"
        }
        ListElement {
            name: "Camera"
            icon: "Camera_48.png"
        }
        ListElement {
            name: "Calendar"
            icon: "DateBook_48.png"
        }
        ListElement {
            name: "Messaging"
            icon: "EMail_48.png"
        }
        ListElement {
            name: "Todo List"
            icon: "TodoList_48.png"
        }
        ListElement {
            name: "Contacts"
            icon: "AddressBook_48.png"
        }
    }

    Component {
        id: AppDelegate
        Item {
            width: 100; height: 100
            Image { id: Icon; y: 20; anchors.horizontalCenter: parent.horizontalCenter; source: icon }
            Text { anchors.top: Icon.bottom; anchors.horizontalCenter: parent.horizontalCenter; text: name }
        }
    }

    Component {
        id: AppHighlight
        Rectangle { width: 80; height: 80; color: "#FFFF88" }
    }

    GridView {
        id: List1
        anchors.fill: parent
        cellWidth: 100; cellHeight: 100
        model: AppModel; delegate: AppDelegate
        highlight: AppHighlight
        focus: true
    }
}

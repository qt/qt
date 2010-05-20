//![import]
import Qt 4.7
//![import]

Item {

//![classdocs simple]
ListView {
    width: 180; height: 200

    model: ContactModel {}
    delegate: Text {
        text: name + ": " + number
    }
}
//![classdocs simple]

//![classdocs advanced]
Rectangle {
    width: 180; height: 200

    Component {
        id: contactDelegate
        Item {
            width: 180; height: 40
            Column {
                Text { text: '<b>Name:</b> ' + name }
                Text { text: '<b>Number:</b> ' + number }
            }
        }
    }

    ListView {
        anchors.fill: parent
        model: ContactModel {}
        delegate: contactDelegate
        highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
        focus: true
    }
}
//![classdocs advanced]

//![delayRemove]
Component {
    id: delegate
    Item {
        ListView.onRemove: SequentialAnimation {
            PropertyAction { target: wrapper; property: "ListView.delayRemove"; value: true }
            NumberAnimation { target: wrapper; property: "scale"; to: 0; duration: 250; easing.type: Easing.InOutQuad }
            PropertyAction { target: wrapper; property: "ListView.delayRemove"; value: false }
        }
    }
}
//![delayRemove]

//![highlightFollowsCurrentItem]
Component {
    id: highlight
    Rectangle {
        width: 180; height: 40
        color: "lightsteelblue"; radius: 5
        SpringFollow on y {
            to: list.currentItem.y
            spring: 3
            damping: 0.2
        }
    }
}

ListView {
    id: list
    width: 180; height: 200
    model: ContactModel {}
    delegate: Text { text: name }

    highlight: highlight
    highlightFollowsCurrentItem: false
    focus: true
}
//![highlightFollowsCurrentItem]

//![isCurrentItem]
ListView {
    width: 180; height: 200

    Component {
        id: contactsDelegate
        Text {
            id: contactInfo
            text: name + ": " + number
            color: contactInfo.ListView.isCurrentItem ? "red" : "black"
        }
    }

    model: ContactModel {}
    delegate: contactsDelegate 
    focus: true
}
//![isCurrentItem]

}

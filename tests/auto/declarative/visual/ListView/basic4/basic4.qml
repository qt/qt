import Qt 4.6

Rect {
    color: "blue"
    width: 200
    height: 300
    id: Page
    ListModel {
        id: Model
        ListElement {
            name: "January"
        }
        ListElement {
            name: "February"
        }
    }
    Component {
        id: Delegate
        Rect {
            color: "red"
            width: 100
            height: 100
            Text {
                text: name
            }
        }
    }
    ListView {
        anchors.fill: parent
        model: Model
        delegate: Delegate
    }
}

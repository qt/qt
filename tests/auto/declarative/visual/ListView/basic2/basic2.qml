import Qt 4.6

Rect {
    color: "blue"
    width: 200
    height: 300
    id: Page
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
        delegate: Delegate
        model: ListModel {
            ListElement {
                name: "January"
            }
            ListElement {
                name: "February"
            }
        }
    }
}

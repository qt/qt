import Qt 4.6

Rectangle {
    color: "blue"
    width: 300
    height: 200
    id: Page
    ListModel {
        id: DataSource
        ListElement {
            name: "January"
        }
        ListElement {
            name: "February"
        }
    }
    Component {
        id: Delegate
        Rectangle {
            color: "red"
            width: 100
            height: 100
            Text {
                text: name
            }
        }
    }
    Row {
        Repeater {
            model: DataSource
            delegate: Delegate
        }
    }
}

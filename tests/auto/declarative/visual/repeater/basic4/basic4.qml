import Qt 4.6

Rect {
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
        Rect {
            color: "red"
            width: 100
            height: 100
            Text {
                text: name
            }
        }
    }
    HorizontalLayout {
        Repeater {
            dataSource: DataSource
            component: Delegate
        }
    }
}

Rect {
    color: "blue"
    width: 800
    height: 600
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

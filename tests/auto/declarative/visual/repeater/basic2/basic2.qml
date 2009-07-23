import Qt 4.6

Rect {
    color: "blue"
    width: 300
    height: 200
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
    HorizontalLayout {
        Repeater {
            component: Delegate
            dataSource: ListModel {
                ListElement {
                    name: "January"
                }
                ListElement {
                    name: "February"
                }
            }
        }
    }
}

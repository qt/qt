Rect {
    color: "blue"
    width: 800
    height: 600
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

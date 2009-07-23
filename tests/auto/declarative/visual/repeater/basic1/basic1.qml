import Qt 4.6

Rect {
    color: "blue"
    width: 300
    height: 200
    id: Page
    HorizontalLayout {
        Repeater {
            component: Rect {
                color: "red"
                width: 100
                height: 100
                Text {
                    text: name
                }
            }
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

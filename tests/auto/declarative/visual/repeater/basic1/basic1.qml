Rect {
    color: "blue"
    width: 800
    height: 600
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

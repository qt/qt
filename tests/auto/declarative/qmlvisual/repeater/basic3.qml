import Qt 4.7

Rectangle {
    color: "blue"
    width: 300
    height: 200
    ListModel {
        id: dataSource
        ListElement {
            name: "January"
        }
        ListElement {
            name: "February"
        }
    }
    Row {
        Repeater {
            model: dataSource
            delegate: Rectangle {
                color: "red"
                width: 100
                height: 100
                Text {
                    text: name
                }
            }
        }
    }
}

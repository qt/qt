import Qt 4.7

Rectangle {
    color: "blue"
    width: 300
    height: 200
    Row {
        Repeater {
            delegate: Rectangle {
                color: "red"
                width: 100
                height: 100
                Text {
                    text: name
                }
            }
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
}

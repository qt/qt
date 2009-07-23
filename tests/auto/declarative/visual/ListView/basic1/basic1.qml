import Qt 4.6

Rect {
    color: "blue"
    width: 200
    height: 300
    id: Page
    ListView {
        anchors.fill: parent
        delegate: Rect {
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

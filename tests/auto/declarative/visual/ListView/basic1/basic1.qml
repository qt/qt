Rect {
    color: "blue"
    width: 300
    height: 200
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

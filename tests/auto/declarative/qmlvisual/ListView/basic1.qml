import Qt 4.7

Rectangle {
    color: "blue"
    width: 200
    height: 300
    id: page
    ListView {
        anchors.fill: parent
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

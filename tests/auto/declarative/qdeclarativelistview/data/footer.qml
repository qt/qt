import Qt 4.7

Rectangle {
    width: 240
    height: 320
    color: "#ffffff"
    Component {
        id: myDelegate
        Rectangle {
            id: wrapper
            objectName: "wrapper"
            height: 20
            width: 240
            Text {
                text: index
            }
            color: ListView.isCurrentItem ? "lightsteelblue" : "white"
        }
    }
    ListView {
        id: list
        objectName: "list"
        focus: true
        width: 240
        height: 320
        model: testModel
        delegate: myDelegate
        footer: Text { objectName: "footer"; text: "Footer"; height: 30 }
    }
}

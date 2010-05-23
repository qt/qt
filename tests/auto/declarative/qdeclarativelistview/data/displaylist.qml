import Qt 4.7

Rectangle {
    width: 240
    height: 320
    color: "#ffffff"
    resources: [
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
                Text {
                    x: 30
                    objectName: "displayText"
                    text: display
                }
                color: ListView.isCurrentItem ? "lightsteelblue" : "white"
            }
        },
        Component {
            id: myHighlight
            Rectangle { color: "green" }
        }
    ]
    ListView {
        id: list
        objectName: "list"
        focus: true
        width: 240
        height: 320
        model: testModel
        delegate: myDelegate
        highlight: myHighlight
        highlightMoveSpeed: 1000
        highlightResizeSpeed: 1000
    }
}

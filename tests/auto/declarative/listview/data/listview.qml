import Qt 4.6

Rectangle {
    width: 240
    height: 320
    color: "#ffffff"
    resources: [
        Component {
            id: Delegate
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
                    id: textName
                    objectName: "textName"
                    text: name
                }
                Text {
                    x: 120
                    id: textNumber
                    objectName: "textNumber"
                    text: number
                }
                Text {
                    x: 200
                    text: wrapper.y
                }
            }
        }
    ]
    ListView {
        id: list
        objectName: "list"
        width: 240
        height: 320
        model: testModel
        delegate: Delegate
    }
}

import Qt 4.6

Rect {
    width: 240
    height: 320
    color: "#ffffff"
    resources: [
        Component {
            id: Delegate
            Item {
                id: wrapper
                height: 20
                width: 240
                Text {
                    text: index
                }
                Text {
                    x: 30
                    id: textName
                    text: name
                }
                Text {
                    x: 120
                    id: textNumber
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
        width: 240
        height: 320
        model: testModel
        delegate: Delegate
    }
}

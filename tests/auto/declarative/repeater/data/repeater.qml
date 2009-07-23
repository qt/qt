import Qt 4.6

Rect {
    id: container
    width: 240
    height: 320
    color: "white"
    Repeater {
        id: repeater
        width: 240
        height: 320
        dataSource: testData
        Component {
            Text {
                y: index*20
                text: modelData
            }
        }
    }
}

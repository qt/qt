import Qt 4.7

Rectangle {
    width: 240
    height: 320
    color: "white"
    Component {
        id: myDelegate
        Item {
            objectName: "myDelegate"
            height: 20
            Text {
                y: index*20
                text: name
            }
            Text {
                y: index*20
                x: 100
                text: number
            }
        }
    }
    Column {
        id: container
        objectName: "container"
        Repeater {
            id: repeater
            objectName: "repeater"
            width: 240
            height: 320
            delegate: myDelegate
            model: testData
        }
    }
}

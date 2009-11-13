import Qt 4.6

Rectangle {
    id: container
    objectName: "container"
    width: 240
    height: 320
    color: "white"
    Repeater {
        id: repeater
        objectName: "repeater"
        model: testData
        Component {
            Item{}
        }
    }
}

import Qt 4.6

Rectangle {
    id: page
    width: 480
    height: 200
    color: "LightGrey"
    Text {
        id: helloText
        text: "Hello world!"
        font.pointSize: 24
        font.bold: true
        y: 30
        anchors.horizontalCenter: page.horizontalCenter
    }
}

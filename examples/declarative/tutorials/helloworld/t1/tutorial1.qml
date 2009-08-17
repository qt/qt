import Qt 4.6

Rect {
    id: Page
    width: 480
    height: 200
    color: "LightGrey"
    Text {
        id: HelloText
        text: "Hello world!"
        font.pointSize: 24
        font.bold: true
        y: 30
        anchors.horizontalCenter: Page.horizontalCenter
    }
}

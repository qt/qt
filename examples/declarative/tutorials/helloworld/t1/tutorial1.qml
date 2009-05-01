Rect {
    id: Page
    width: 480
    height: 200
    color: "white"
    Text {
        id: HelloText
        text: "Hello world!"
        font.size: 24
        font.bold: true
        y: 30
        anchors.horizontalCenter: Page.horizontalCenter
    }
}

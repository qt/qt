import Qt 4.6

Rectangle {
    id: screen
    width: 640; height: 320; color: "steelblue"

    property int pixelSize: screen.height * 1.25
    property color textColor: "lightsteelblue"
    property string text: "Hello world! "

    Row {
        y: -screen.height / 4.5
        NumberAnimation on x { from: 0; to: -text.width; duration: 6000; repeat: true }
        Text { id: text; font.pixelSize: screen.pixelSize; color: screen.textColor; text: screen.text }
        Text { font.pixelSize: screen.pixelSize; color: screen.textColor; text: screen.text }
        Text { font.pixelSize: screen.pixelSize; color: screen.textColor; text: screen.text }
    }
}

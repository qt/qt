import Qt 4.7

Rectangle {
    id: screen

    property int pixelSize: screen.height * 1.25
    property color textColor: "lightsteelblue"
    property string text: "Hello world! "

    width: 640; height: 320
    color: "steelblue"

    Row {
        y: -screen.height / 4.5

        NumberAnimation on x { from: 0; to: -text.width; duration: 6000; loops: Animation.Infinite }
        Text { id: text; font.pixelSize: screen.pixelSize; color: screen.textColor; text: screen.text }
        Text { font.pixelSize: screen.pixelSize; color: screen.textColor; text: screen.text }
        Text { font.pixelSize: screen.pixelSize; color: screen.textColor; text: screen.text }
    }
}

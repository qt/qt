import Qt 4.7
import "content"

Rectangle {
    width: 240; height: 320
    Column {
        y: 20; x: 20; spacing: 20
        Spinner {
            id: spinner
            width: 200; height: 240
            focus: true
            model: 20
            itemHeight: 30
            delegate: Text { font.pixelSize: 25; text: index; height: 30 }
        }
        Text { text: "Current item index: " + spinner.currentIndex }
    }
}

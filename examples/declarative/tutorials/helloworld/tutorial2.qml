//![0]
import Qt 4.6

Rectangle {
    id: page
    width: 500; height: 200
    color: "lightgray"

    Text {
        id: helloText
        text: "Hello world!"
        font.pointSize: 24; font.bold: true
        y: 30; anchors.horizontalCenter: page.horizontalCenter
    }

    Grid {
        id: colorPicker
        anchors.bottom: page.bottom
        rows: 2; columns: 3; spacing: 3

//![1]
        Cell { color: "red"; onClicked: helloText.color = color }
//![1]
        Cell { color: "green"; onClicked: helloText.color = color }
        Cell { color: "blue"; onClicked: helloText.color = color }
        Cell { color: "yellow"; onClicked: helloText.color = color }
        Cell { color: "steelblue"; onClicked: helloText.color = color }
        Cell { color: "black"; onClicked: helloText.color = color }
    }
}
//![0]

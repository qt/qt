import Qt 4.6

Rectangle {
    property string day

    width: 200
    height: 300
    radius: 7
    border.color: "black"
    id: Page
    Image {
        x: 10
        y: 10
        source: "cork.jpg"
    }
    Text {
        x: 20
        y: 20
        height: 40
        font.pixelSize: 14
        font.bold: true
        text: day
        style: "Outline"
        styleColor: "#dedede"
    }
}

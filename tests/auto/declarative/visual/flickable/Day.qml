Rect {
    property string day

    width: 400
    height: 500
    radius: 7
    pen.color: "black"
    id: Page
    Image {
        x: 10
        y: 10
        source: "cork.jpg"
        opaque: true
    }
    Text {
        x: 20
        y: 20
        height: 40
        font.size: 14
        font.bold: true
        width: 370
        text: day
        style: "Outline"
        styleColor: "#dedede"
    }
}

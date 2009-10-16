import Qt 4.6

Rectangle {
    id: main
    width: 800; height: 580; color: "#edecec"

    Flickable {
        anchors.fill: parent; viewportHeight: column.height
    Column {
        id: column; spacing: 4
        Repeater {
            model: 50
            ProgressBar {
                property int r: Math.floor(Math.random() * 4000 + 1000)
                width: main.width
                value: NumberAnimation { duration: r; from: 0; to: 100; running: true; repeat: true }
                color: ColorAnimation { duration: r; from: "lightsteelblue"; to: "thistle"; running: true; repeat: true }
                secondColor: ColorAnimation { duration: r; from: "steelblue"; to: "#CD96CD"; running: true; repeat: true }
            }
        }
    }
    }
}

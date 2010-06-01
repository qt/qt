import Qt 4.7

//![0]
Rectangle {
    width: 100; height: 100
    color: "yellow"

    Rectangle {
        anchors.fill: parent
        anchors.margins: 10
        clip: true

        Rectangle {
            anchors.fill: parent
            border.width: 1
        }
    }
}
//![0]

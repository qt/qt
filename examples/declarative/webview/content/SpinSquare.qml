import Qt 4.6

Item {
    property var period : 250
    property var color : "black"
    id: root

    Item {
        x: root.width/2
        y: root.height/2
        Rectangle {
            color: root.color
            x: -width/2
            y: -height/2
            width: root.width
            height: width
        }
        NumberAnimation on rotation {
            from: 0
            to: 360
            loops: Qt.Infinite
            duration: root.period
        }
    }
}

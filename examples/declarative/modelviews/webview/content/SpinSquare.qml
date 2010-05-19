import Qt 4.7

Item {
    property variant period : 250
    property variant color : "black"
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
            loops: Animation.Infinite
            duration: root.period
        }
    }
}

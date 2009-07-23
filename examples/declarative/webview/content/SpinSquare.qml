import Qt 4.6

Item {
    properties var period : 250
    properties var color : "black"
    id: Root

    Item {
        x: Root.width/2
        y: Root.height/2
        Rect {
            color: Root.color
            x: -width/2
            y: -height/2
            width: Root.width
            height: width
        }
        rotation: NumberAnimation {
            from: 0
            to: 360
            repeat: true
            running: true
            duration: Root.period
        }
    }
}

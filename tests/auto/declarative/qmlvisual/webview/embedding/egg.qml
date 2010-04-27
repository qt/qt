import Qt 4.7

Item {
    property variant period : 250
    property variant color : "black"
    id: root

    Item {
        x: root.width/2
        y: root.height/2
        Rectangle {
            radius: width/2
            color: root.color
            x: -width/2
            y: -height/2
            width: root.width*1.5
            height: root.height*1.5
        }
        rotation: NumberAnimation {
            from: 0
            to: 360
            repeat: true
            duration: root.period
        }
    }
}

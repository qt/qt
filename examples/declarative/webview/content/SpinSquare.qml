Item {
    id: Root
    properties: Property {
        name: "period"
        value: 250
    }
    properties: Property {
        name: "color"
        value: "black"
    }
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
        rotation: NumericAnimation {
            from: 0
            to: 360
            repeat: true
            running: true
            duration: Root.period
        }
    }
}

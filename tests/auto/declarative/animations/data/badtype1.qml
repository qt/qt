import Qt 4.6

Rect {
    width: 240
    height: 320
    Rect {
        color: "red"
        width: 50; height: 50
        x: 100; y: 100
        x: PropertyAnimation { from: "blue"; to: "green"; running: true; }
    }
}

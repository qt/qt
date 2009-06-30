Rect {
    width: 240
    height: 320
    Rect {
        color: "red"
        color: PropertyAnimation { to: "green"; running: true }
        width: 50; height: 50
        x: 100; y: 100
    }
}

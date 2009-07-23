import Qt 4.6

Rect {
    width: 240
    height: 320
    Rect {
        color: "red"
        color: ColorAnimation { from: 10; to: 15; running: true; }
        width: 50; height: 50
        x: 100; y: 100
    }
}

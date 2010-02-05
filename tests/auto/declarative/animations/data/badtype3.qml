import Qt 4.6

Rectangle {
    width: 240
    height: 320
    Rectangle {
        color: "red"
        color: ColorAnimation { from: 10; to: 15; running: true; }
        width: 50; height: 50
        x: 100; y: 100
    }
}

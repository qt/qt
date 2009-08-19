import Qt 4.6

Item {
    width: 640
    height: 480
    HorizontalPositioner {
        Rect {
            id: one
            color: "red"
            width: 50
            height: 50
        }
        Rect {
            id: two
            color: "red"
            width: 20
            height: 10
        }
        Rect {
            id: three
            color: "red"
            width: 40
            height: 20
        }
    }
}

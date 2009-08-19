import Qt 4.6

Item {
    width: 640
    height: 480
    GridPositioner {
        columns: 3
        Rect {
            id: one
            color: "red"
            width: 50
            height: 50
        }
        Rect {
            id: two
            color: "green"
            width: 20
            height: 50
        }
        Rect {
            id: three
            color: "blue"
            width: 50
            height: 20
        }
        Rect {
            id: four
            color: "cyan"
            width: 50
            height: 50
        }
        Rect {
            id: five
            color: "magenta"
            width: 10
            height: 10
        }
    }
}

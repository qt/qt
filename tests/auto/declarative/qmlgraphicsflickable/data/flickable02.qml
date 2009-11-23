import Qt 4.6

Flickable {
    width: 100; height: 100
    viewportWidth: row.width; viewportHeight: row.height

    Row {
        id: row
        Repeater {
            model: 4
            Rectangle { width: 200; height: 300; color: "blue" }
        }
    }
}

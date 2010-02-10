import Qt 4.6

Flickable {
    width: 100; height: 100
    viewportWidth: column.width; viewportHeight: column.height

    Column {
        id: column
        Repeater {
            model: 4
            Rectangle { width: 200; height: 300; color: "blue" }
        }
    }
}

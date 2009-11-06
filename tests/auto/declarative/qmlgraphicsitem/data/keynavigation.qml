import Qt 4.6

Grid {
    columns: 2
    width: 100; height: 100
    Rectangle {
        id: item1
        objectName: "item1"
        focus: true
        width: 50; height: 50
        color: focus ? "red" : "lightgray"
        KeyNavigation.right: item2
        KeyNavigation.down: item3
    }
    Rectangle {
        id: item2
        objectName: "item2"
        width: 50; height: 50
        color: focus ? "red" : "lightgray"
        KeyNavigation.left: item1
        KeyNavigation.down: item4
    }
    Rectangle {
        id: item3
        objectName: "item3"
        width: 50; height: 50
        color: focus ? "red" : "lightgray"
        KeyNavigation.right: item4
        KeyNavigation.up: item1
    }
    Rectangle {
        id: item4
        objectName: "item4"
        width: 50; height: 50
        color: focus ? "red" : "lightgray"
        KeyNavigation.left: item3
        KeyNavigation.up: item2
    }
}

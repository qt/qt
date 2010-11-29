//![0]
import QtQuick 1.0

Grid {
    width: 100; height: 100
    columns: 2

    Rectangle {
        id: topLeft
        width: 50; height: 50
        color: focus ? "red" : "lightgray"
        focus: true

        KeyNavigation.right: topRight
        KeyNavigation.down: bottomLeft
    }

    Rectangle {
        id: topRight
        width: 50; height: 50
        color: focus ? "red" : "lightgray"

        KeyNavigation.left: topLeft
        KeyNavigation.down: bottomRight
    }

    Rectangle {
        id: bottomLeft
        width: 50; height: 50
        color: focus ? "red" : "lightgray"

        KeyNavigation.right: bottomRight
        KeyNavigation.up: topLeft
    }

    Rectangle {
        id: bottomRight
        width: 50; height: 50
        color: focus ? "red" : "lightgray"

        KeyNavigation.left: bottomLeft
        KeyNavigation.up: topRight
    }
}
//![0]

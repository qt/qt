import Qt 4.6

Object {
    property var test1: Qt.darker(Qt.rgba(1, 0.8, 0.3))
    property var test2: Qt.darker()
    property var test3: Qt.darker(Qt.rgba(1, 0.8, 0.3), 10)
}


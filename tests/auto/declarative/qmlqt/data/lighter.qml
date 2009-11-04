import Qt 4.6

Object {
    property var test1: Qt.lighter(Qt.rgba(1, 0.8, 0.3))
    property var test2: Qt.lighter()
    property var test3: Qt.lighter(Qt.rgba(1, 0.8, 0.3), 10)
}

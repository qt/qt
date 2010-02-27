import Qt 4.6

QtObject {
    property var test1: Qt.rect(10, 13, 100, 109)
    property var test2: Qt.rect(-10, 13, 100, 109.6)
    property var test3: Qt.rect(10, 13);
    property var test4: Qt.rect(10, 13, 100, 109, 10)
    property var test5: Qt.rect(10, 13, 100, -109)
}

import Qt 4.6

QtObject {
    property var test1: Qt.lighter(Qt.rgba(1, 0.8, 0.3))
    property var test2: Qt.lighter()
    property var test3: Qt.lighter(Qt.rgba(1, 0.8, 0.3), 10)
    property var test4: Qt.lighter("red");
    property var test5: Qt.lighter("perfectred"); // Non-existant color
    property var test6: Qt.lighter(10);
}

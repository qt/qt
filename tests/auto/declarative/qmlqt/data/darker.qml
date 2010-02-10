import Qt 4.6

QtObject {
    property var test1: Qt.darker(Qt.rgba(1, 0.8, 0.3))
    property var test2: Qt.darker()
    property var test3: Qt.darker(Qt.rgba(1, 0.8, 0.3), 10)
    property var test4: Qt.darker("red");
    property var test5: Qt.darker("perfectred"); // Non-existant color
    property var test6: Qt.darker(10);
}


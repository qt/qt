import Qt.VisualTest 4.6

VisualTest {
    Frame { msec: 0; image: "QT-669.png" }
    Key { key: Qt.Key_Right; count: 20 }
    Key { key: Qt.Key_Left; count: 20 }
    Frame { msec: 100; image: "QT-669.png" }
}

import Qt 4.7

QtObject {
    property string test1: Qt.md5()
    property string test2: Qt.md5("Hello World")
}

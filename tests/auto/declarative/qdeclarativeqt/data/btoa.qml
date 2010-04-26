import Qt 4.7

QtObject {
    property string test1: Qt.btoa()
    property string test2: Qt.btoa("Hello world!")
}

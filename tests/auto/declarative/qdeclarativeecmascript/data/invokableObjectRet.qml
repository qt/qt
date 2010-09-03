import Qt.test 1.0
import Qt 4.7

MyQmlObject {
    id: root
    property bool test: false
    Component.onCompleted: {
        test = (root.returnme() == root)
    }
}


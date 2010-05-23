import Qt 4.7
import Qt.test 1.0

MyQmlObject {
    Component.onCompleted:
        { throw(new Error("JS exception")) }
}


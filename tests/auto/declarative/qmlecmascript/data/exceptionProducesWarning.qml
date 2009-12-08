import Qt 4.6
import Qt.test 1.0

MyQmlObject {
    Component.onCompleted:
        { throw(123,"JS exception") }
}


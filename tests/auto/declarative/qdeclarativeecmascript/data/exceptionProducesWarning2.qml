import Qt 4.7
import Qt.test 1.0

MyQmlObject {
    value: { throw(new Error("JS exception")) }
}


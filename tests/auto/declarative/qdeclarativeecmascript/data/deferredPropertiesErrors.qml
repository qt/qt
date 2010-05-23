import Qt.test 1.0
import Qt 4.7

MyDeferredObject {
    value: undefined // error is resolved before complete
    objectProperty: undefined // immediate error
    objectProperty2: QtObject {
        Component.onCompleted: value = 10
    }
}

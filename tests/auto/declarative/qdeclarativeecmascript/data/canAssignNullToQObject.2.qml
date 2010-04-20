import Qt.test 1.0
import Qt 4.6

MyQmlObject {
    objectProperty: MyQmlObject {}

    Component.onCompleted: {
        objectProperty = null;
    }
}


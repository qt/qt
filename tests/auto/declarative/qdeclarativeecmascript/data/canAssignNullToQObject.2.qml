import Qt.test 1.0
import Qt 4.7

MyQmlObject {
    objectProperty: MyQmlObject {}

    Component.onCompleted: {
        objectProperty = null;
    }
}


import Qt.test 1.0
import Qt 4.7

MyQmlObject {
    objectProperty: if(1) otherObject

    property variant obj

    obj: QtObject {
        id: otherObject
    }
}


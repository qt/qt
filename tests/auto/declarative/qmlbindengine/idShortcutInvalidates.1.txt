import Qt.test 1.0
import Qt 4.6

MyQmlObject {
    objectProperty: if(1) OtherObject

    property var obj

    obj: Object {
        id: OtherObject
    }
}


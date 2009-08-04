import Qt.test 1.0
import Qt 4.6

MyQmlObject {
    objectProperty: OtherObject

    property var obj

    obj: Object {
        id: OtherObject
    }
}

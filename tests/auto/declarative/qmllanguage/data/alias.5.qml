import Qt 4.6
import Test 1.0

Object {
    property alias otherAlias: otherObject

    property var other 
    other: MyQmlObject {
        id: otherObject
        value: 10
    }
}


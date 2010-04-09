import Qt 4.6
import Test 1.0

QtObject {
    property alias otherAlias: otherObject

    property variant other 
    other: MyQmlObject {
        id: otherObject
        value: 10
    }
}


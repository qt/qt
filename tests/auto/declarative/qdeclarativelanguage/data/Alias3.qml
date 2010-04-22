import Test 1.0
import Qt 4.7

QtObject {
    property alias obj : otherObj
    property variant child
    child: QtObject {
        id: otherObj
        property int myValue: 10
    }
}


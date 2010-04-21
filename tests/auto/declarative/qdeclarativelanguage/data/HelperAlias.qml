import Test 1.0
import Qt 4.7

QtObject {
    property variant child
    child: QtObject { id: obj }
    property alias objAlias: obj;
}


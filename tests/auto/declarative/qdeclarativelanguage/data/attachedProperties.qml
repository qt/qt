import Test 1.0
import Test 1.0 as Namespace
import Qt 4.6

QtObject {
    property int value2: 8
    MyQmlObject.value: 10
    Namespace.MyQmlObject.value2: 13
    MyQmlObject.onValueChanged: value2 = MyQmlObject.value
}

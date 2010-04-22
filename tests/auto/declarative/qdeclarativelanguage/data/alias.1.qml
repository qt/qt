import Test 1.0
import Qt 4.7

QtObject {
    id: root
    property int value: 10
    property alias valueAlias: root.value
}

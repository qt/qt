import Test 1.0
import Qt 4.6

QtObject {
    id: Root
    property int value: 10
    property alias valueAlias: Root.value
}

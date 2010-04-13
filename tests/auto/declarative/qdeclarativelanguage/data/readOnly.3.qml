import Test 1.0
import Qt 4.6

QtObject {
    property variant child
    child: HelperAlias { objAlias: QtObject {} }
}


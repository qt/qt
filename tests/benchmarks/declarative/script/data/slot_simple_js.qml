import Qt.test 1.0
import "slot_simple_js.js" as Logic

TestObject {
    onMySignal: { for (var ii = 0; ii < 10000; ++ii) { Logic.myCustomFunction(); } }
}


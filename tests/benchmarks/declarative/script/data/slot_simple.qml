import Qt.test 1.0

TestObject {
    function myCustomFunction() {
        return 0;
    }

    onMySignal: { for (var ii = 0; ii < 10000; ++ii) { myCustomFunction(); } }
}

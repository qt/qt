import Qt.test 1.0

TestObject {

    Script {
        function myCustomFunction() {
            return 0;
        }
    }

    onMySignal: { for (var ii = 0; ii < 10000; ++ii) { myCustomFunction(); } }
}


import Qt.test 1.0

TestObject {
    Script {
        function myCustomFunction(n) {
            var a = 1;
            while (n > 0) {
                a = a * n;
                n--; 
            }
            return a;
        }
    }

    onMySignal: { for (var ii = 0; ii < 10000; ++ii) { myCustomFunction(10); } }
}



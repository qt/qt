import Qt 4.6

Item {
    Script {
        function method() {
            return 10;
        }
    }

    Script {
        source: "scriptAccess.js"
    }

    property int test1: method()
    property int test2: extMethod()
    property int test3: extVariable
}

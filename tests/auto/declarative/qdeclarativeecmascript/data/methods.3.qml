import Qt 4.7

QtObject {
    function testFunction() { return 19; }

    property int test: testFunction()
}

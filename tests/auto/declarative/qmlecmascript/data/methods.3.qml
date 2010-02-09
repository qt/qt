import Qt 4.6

QtObject {
    function testFunction() { return 19; }

    property int test: testFunction()
}

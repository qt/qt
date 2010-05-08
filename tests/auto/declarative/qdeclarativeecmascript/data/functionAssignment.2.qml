import Qt.test 1.0

MyQmlObject {
    property variant a
    property bool runTest: false
    onRunTestChanged: {
        function myFunction() {
            console.log("hello world");
        }
        a = myFunction;
    }

}

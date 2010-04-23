import Qt.test 1.0

MyQmlObject {
    property string result

    Script{
        function f() {
            result = b
        }
        
    }
    onArgumentSignal: f()
}

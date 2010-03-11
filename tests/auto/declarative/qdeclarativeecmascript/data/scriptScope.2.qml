import Qt.test 1.0

MyQmlObject {
    property string result
    property string aProp: "hello"

    Script{
        source: "scriptScope.js"
    }
    onBasicSignal: f()
}

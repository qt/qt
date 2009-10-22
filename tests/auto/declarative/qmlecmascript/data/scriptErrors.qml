import Qt.test 1.0

MyQmlObject {
    Script { source: "scriptErrors.js" }
    Script { function getValue() { a = 10; return 0; } }

    property int x: a.value
    property int y: getValue();

    onBasicSignal: { print(a.value); }
}


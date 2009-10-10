import Qt 4.6

Object {
    Script { source: "scriptErrors.js" }
    Script { function getValue() { a = 10; return 0; } }

    property int x: a.value
    property int y: getValue();
}


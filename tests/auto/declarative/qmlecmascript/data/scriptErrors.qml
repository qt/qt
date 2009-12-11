import Qt.test 1.0

MyQmlObject {
    Script { source: "scriptErrors.js" }
    Script { function getValue() { a = 10; return 0; } }

    property int t: a.value
    property int w: getValue();
    property int x: undefinedObject
    property int y: (a.value, undefinedObject)

    onBasicSignal: { console.log(a.value); }
    id: myObj
    onAnotherBasicSignal: myObj.trueProperty = false;
    onThirdBasicSignal: myObj.fakeProperty = "";
}


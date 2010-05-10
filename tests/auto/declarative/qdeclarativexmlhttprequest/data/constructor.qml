import Qt 4.7

QtObject {
    property bool calledAsConstructor
    property bool calledAsFunction

    Component.onCompleted: {
        var x1 = new XMLHttpRequest;
        var x2 = XMLHttpRequest();

        calledAsConstructor = (x1 != null && x1 instanceof XMLHttpRequest);
        calledAsFunction = (x2 == undefined);
    }
}

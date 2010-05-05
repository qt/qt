import Qt 4.7

QtObject {
    property bool exceptionThrown: false

    Component.onCompleted: {
        var x = new XMLHttpRequest;

        try {
            x.open("BLAH", "http://www.nokia.com");
        } catch (e) {
            if (e.code == DOMException.SYNTAX_ERR)
                exceptionThrown = true;
        }
    }
}

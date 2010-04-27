import Qt 4.7

QtObject {
    property bool exceptionThrown: false

    Component.onCompleted: {
        var x = new XMLHttpRequest;

        try {
            x.open("GET", "http://www.nokia.com", true, "user", "password", "extra");
        } catch (e) {
            if (e.code == DOMException.SYNTAX_ERR)
                exceptionThrown = true;
        }
    }
}



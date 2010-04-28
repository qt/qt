import Qt 4.7

QtObject {
    property bool exceptionThrown: false

    Component.onCompleted: {
        var x = new XMLHttpRequest;

        try {
            x.open("GET", "http://www.nokia.com", false);
        } catch (e) {
            if (e.code == DOMException.NOT_SUPPORTED_ERR)
                exceptionThrown = true;
        }
    }
}


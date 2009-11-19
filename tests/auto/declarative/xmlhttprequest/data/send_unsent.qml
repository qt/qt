import Qt 4.6

QtObject {
    property bool test: false

    Component.onCompleted: {
        var x = new XMLHttpRequest;

        try {
            x.send();
        } catch (e) {
            if (e.code == DOMException.INVALID_STATE_ERR)
                test = true;
        }
    }
}

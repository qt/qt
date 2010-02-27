import Qt 4.6

QtObject {
    property int test

    Component.onCompleted: {
        var o = new Object;
        o.test = 92;
        test = o.test;
    }
}


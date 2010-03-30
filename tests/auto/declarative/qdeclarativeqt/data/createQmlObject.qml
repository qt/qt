import Qt 4.6

Item {
    id: root

    property bool incorrectArgCount1: false
    property bool incorrectArgCount2: false
    property bool emptyArg: false
    property bool noParent: false
    property bool notAvailable: false
    property bool runtimeError: false
    property bool errors: false

    property bool success: false

    Component.onCompleted: {
        // errors
        incorrectArgCount1 = (createQmlObject() == null);
        incorrectArgCount2 = (createQmlObject("import Qt 4.6\nQtObject{}", root, "main.qml", 10) == null);
        emptyArg = (createQmlObject("", root) == null);
        errors = (createQmlObject("import Qt 4.6\nQtObject{\nproperty int test: 13\nproperty int test: 13\n}", root, "main.qml") == null);
        noParent = (createQmlObject("import Qt 4.6\nQtObject{\nproperty int test: 13}", 0) == null);
        notAvailable = (createQmlObject("import Qt 4.6\nQtObject{Blah{}}", root) == null);
        runtimeError = (createQmlObject("import Qt 4.6\nQtObject{property int test\nonTestChanged: QtObject{}\n}", root) == null);

        var o = createQmlObject("import Qt 4.6\nQtObject{\nproperty int test: 13\n}", root);
        success = (o.test == 13);

        createQmlObject("import Qt 4.6\nItem {}\n", root);
    }
}

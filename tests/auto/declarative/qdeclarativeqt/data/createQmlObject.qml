import Qt 4.6

Item {
    id: root

    // errors resulting in exceptions
    property QtObject incorrectArgCount1: createQmlObject()
    property QtObject incorrectArgCount2: createQmlObject("import Qt 4.6\nQtObject{}", root, "main.qml", 10)
    property QtObject noParent: createQmlObject("import Qt 4.6\nQtObject{\nproperty int test: 13}", 0)
    property QtObject notAvailable: createQmlObject("import Qt 4.6\nQtObject{Blah{}}", root)
    property QtObject errors: createQmlObject("import Qt 4.6\nQtObject{\nproperty int test: 13\nproperty int test: 13\n}", root, "main.qml")

    property bool emptyArg: false

    property bool success: false

    Component.onCompleted: {
        // errors resulting in nulls
        emptyArg = (createQmlObject("", root) == null);
        try {
            createQmlObject("import Qt 4.6\nQtObject{property int test\nonTestChanged: QtObject{}\n}", root)
        } catch (error) {
            console.log("RunTimeError: ",error.message);
        }

        var o = createQmlObject("import Qt 4.6\nQtObject{\nproperty int test: 13\n}", root);
        success = (o.test == 13);

        createQmlObject("import Qt 4.6\nItem {}\n", root);
    }
}

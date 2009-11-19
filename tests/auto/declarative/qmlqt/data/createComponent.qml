import Qt 4.6

QtObject {
    property bool incorrectArgCount1: false
    property bool incorrectArgCount2: false
    property bool emptyArg: false

    property string relativeUrl
    property string absoluteUrl

    Component.onCompleted: {
        // Test that using incorrect argument count returns a null object
        incorrectArgCount1 = (createComponent() == null);
        incorrectArgCount2 = (createComponent("main.qml", 10) == null);
        emptyArg = (createComponent("") == null);

        var r = createComponent("createComponentData.qml");
        relativeUrl = r.url;

        var a = createComponent("http://www.example.com/test.qml");
        absoluteUrl = a.url;
    }
}

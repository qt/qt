import Qt 4.6

QtObject {
    property bool emptyArg: false

    property string relativeUrl
    property string absoluteUrl

    property QtObject incorectArgCount1: createComponent()
    property QtObject incorectArgCount2: createComponent("main.qml", 10)

    Component.onCompleted: {
        emptyArg = (createComponent("") == null);
        var r = createComponent("createComponentData.qml");
        relativeUrl = r.url;

        var a = createComponent("http://www.example.com/test.qml");
        absoluteUrl = a.url;
    }
}

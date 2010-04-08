import Qt 4.6

QtObject {
    id: root

    property int a
    property var b

    Component.onCompleted: {
        try {
            root.a = undefined;
        } catch(e) {
            console.log (e.fileName + ":" + e.lineNumber + ":" + e);
        }

        try {
            root.b = [ 10, root ]
        } catch(e) {
            console.log (e.fileName + ":" + e.lineNumber + ":" + e);
        }

        try {
            root.a = "Hello";
        } catch(e) {
            console.log (e.fileName + ":" + e.lineNumber + ":" + e);
        }
    }
}

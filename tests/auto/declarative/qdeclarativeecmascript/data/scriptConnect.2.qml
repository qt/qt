import Qt.test 1.0
import Qt 4.7
import "scriptConnect.2.js" as Script

MyQmlObject { 
    property bool test: false

    id: root
    
    Component.onCompleted: {
        var a = new Object;
        a.b = 12;
        root.argumentSignal.connect(a, Script.testFunction);
    }
}


import Qt.test 1.0
import Qt 4.7
import "scriptConnect.1.js" as Script
MyQmlObject { 
    property bool test: false

    id: root
    
    Component.onCompleted: root.argumentSignal.connect(Script.testFunction);
}

import Qt.test 1.0
import Qt 4.7

MyQmlObject { 
    property bool test: false

    id: root
    
    function testFunction() {
        test = true;
    }

    Component.onCompleted: root.argumentSignal.connect(testFunction);
}


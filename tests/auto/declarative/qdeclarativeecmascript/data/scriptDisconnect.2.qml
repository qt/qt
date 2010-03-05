import Qt.test 1.0
import Qt 4.6

MyQmlObject { 
    property int test: 0

    id: root
    
    Script {
        function testFunction() {
            test++;
        }
    }

    Component.onCompleted: root.argumentSignal.connect(root, testFunction);

    onBasicSignal: root.argumentSignal.disconnect(root, testFunction);
}


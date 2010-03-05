import Qt.test 1.0
import Qt 4.6

MyQmlObject { 
    property int test: 0

    id: root
    
    Script {
        function testFunction() {
            test++;
        }
        function otherFunction() {
        }
    }

    Component.onCompleted: root.argumentSignal.connect(testFunction);

    onBasicSignal: root.argumentSignal.disconnect(otherFunction);
}

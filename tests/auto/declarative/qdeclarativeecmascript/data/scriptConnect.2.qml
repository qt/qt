import Qt.test 1.0
import Qt 4.6

MyQmlObject { 
    property bool test: false

    id: root
    
    Script {
        function testFunction() {
            if (this.b == 12)
                test = true;
        }
    }

    Component.onCompleted: {
        var a = new Object;
        a.b = 12;
        root.argumentSignal.connect(a, testFunction);
    }
}


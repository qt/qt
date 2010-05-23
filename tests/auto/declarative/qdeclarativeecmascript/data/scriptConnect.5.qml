import Qt.test 1.0
import Qt 4.7

MyQmlObject { 
    property bool test: false

    id: root

    Component.onCompleted: root.argumentSignal.connect(root, methodNoArgs);
}


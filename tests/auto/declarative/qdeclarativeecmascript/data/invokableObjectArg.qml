import Qt.test 1.0
import Qt 4.7

MyQmlObject {
    id: root
    Component.onCompleted: {
        root.myinvokable(root);
    }
}

import Qt.test 1.0
import Qt 4.6

MyQmlObject {
    id: root

    property bool test1
    property bool test2

    qlistProperty: root

    Component.onCompleted: {
        test1 = (qlistProperty.length == 1)
        test2 = (qlistProperty[0] == root)
    }
}

import Qt.test 1.0
import Qt 4.6

MyQmlObject {
    id: root

    property bool test1
    property bool test2
    property bool test3
    property bool test4

    objectProperty: QtObject { id: obj }
    qlistProperty: [ root, obj ]

    Component.onCompleted: {
        test1 = (qlistProperty.length == 2)
        test2 = (qlistProperty[0] == root)
        test3 = (qlistProperty[1] == obj)
        test4 = (qlistProperty[2] == null)
    }
}

import Qt.test 1.0
import Qt 4.6

MyQmlObject {
    id: root

    property bool test1
    property bool test2
    property bool test3
    property bool test4
    property bool test5
    property bool test6
    property bool test7
    property bool test8

    objectProperty: QtObject { id: obj }

    Component.onCompleted: {
        qlistProperty = [ root, obj ]

        test1 = (qlistProperty.length == 2)
        test2 = (qlistProperty[0] == root)
        test3 = (qlistProperty[1] == obj)
        test4 = (qlistProperty[2] == null)

        var a = qlistProperty;
        a.reverse();
        qlistProperty = a

        test5 = (qlistProperty.length == 2)
        test7 = (qlistProperty[0] == obj)
        test6 = (qlistProperty[1] == root)
        test8 = (qlistProperty[2] == null)
    }
}

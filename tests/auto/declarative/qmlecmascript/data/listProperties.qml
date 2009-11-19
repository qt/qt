import Qt.test 1.0
import Qt 4.6

MyQmlObject {
    id: root

    objectListProperty: [
        QtObject { property int a: 10 },
        QtObject { property int a: 11 }
    ]

    objectQmlListProperty: [
        QtObject { property int a: 10 },
        QtObject { property int a: 1 },
        QtObject { property int a: 39 }
    ]

    Script {
        function calcTest1() {
            var rv = 0;
            for (var ii = 0; ii < root.objectListProperty.length; ++ii) {
                rv += root.objectListProperty[ii].a;
            }
            return rv;
        }

        function calcTest2() {
            var rv = 0;
            for (var ii = 0; ii < root.objectQmlListProperty.length; ++ii) {
                rv += root.objectQmlListProperty[ii].a;
            }
            return rv;
        }
    }

    property int test1: calcTest1();
    property int test2: root.objectListProperty.length
    property int test3: calcTest2();
    property int test4: root.objectQmlListProperty.length
    property bool test5: root.objectQmlListProperty[1] != undefined
    property bool test6: root.objectQmlListProperty[100] == undefined
    property bool test7: root.objectListProperty[1] != undefined
    property bool test8: root.objectListProperty[100] == undefined
}

import Qt 4.6

Item {
    id: Root

    property int a: 1
    property int binding: a
    property string binding2: a + "Test"
    property int binding3: myFunction()
    property int binding4: myNestedFunction()

    Script {
        function myFunction() {
            return a;
        }
    }

    Item {
        id: NestedObject

        Script {
            function myNestedFunction() {
                return a;
            }
        }

        property int a: 2
        property int binding: a
        property string binding2: a + "Test"
        property int binding3: myFunction()
        property int binding4: myNestedFunction()
    }

    ScopeObject {
        id: CompObject
    }

    property alias test1: Root.binding
    property alias test2: NestedObject.binding
    property alias test3: Root.binding2
    property alias test4: NestedObject.binding2
    property alias test5: Root.binding3
    property alias test6: NestedObject.binding3
    property alias test7: Root.binding4
    property alias test8: NestedObject.binding4
    property alias test9: CompObject.binding
    property alias test10: CompObject.binding2
}

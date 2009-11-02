import Qt 4.6

Item {
    property int test: myObject.object.a

    Item {
        id: myObject
        property Object object;
        object: Object {
            property int a: 10
        }
    }
}

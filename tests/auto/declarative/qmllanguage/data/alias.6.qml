import Qt 4.6

Object {
    property Object o;
    property alias a: object.a
    o: NestedAlias { id: object }
}


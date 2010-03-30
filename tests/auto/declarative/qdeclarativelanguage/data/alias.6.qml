import Qt 4.6

QtObject {
    property QtObject o;
    property alias a: object.a
    o: NestedAlias { id: object }
}

